#include <StdAfx.h>

#include "EntityAwareness.h"
#include <GameObjects/GameObject.h>
#include <IActorSystem.h>
#include <IViewSystem.h>
#include <IMovementController.h>
#include <Actor/Player/Player.h>
#include <Actor/Character/Character.h>
#include <Camera/ICamera.h>


/** The distance forward of the actor we will ray-cast in search of entities. */
static const float forwardCastDistance = 300.0f;

/** Scales the proximity distance for what is considered 'close by' queries. */
static const float proximityCloseByFactor = 0.8f;

/** Allow previous ray-cast results to still be used for up to this long, in seconds. */
static const float maxRaycastStaleness = 0.05f;

#define PIERCE_GLASS (13)


CEntityAwareness::UpdateQueryFunction CEntityAwareness::m_updateQueryFunctions [] =
{
	&CEntityAwareness::UpdateRaycastQuery,
	&CEntityAwareness::UpdateProximityQuery,
	&CEntityAwareness::UpdateNearQuery,
	&CEntityAwareness::UpdateInFrontOfQuery,
};


CEntityAwareness::CEntityAwareness()
{
	m_rayHitPierceable.dist = -1.0f;
}


CEntityAwareness::~CEntityAwareness()
{
	for (int i = 0; i < maxQueuedRays; ++i)
	{
		m_queuedRays [i].Reset();
	}
}


// ***
// *** IGameObjectExtension
// ***

bool CEntityAwareness::Init(IGameObject * pGameObject)
{
	SetGameObject(pGameObject);

	// This extension is only valid for actors.
	m_pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(GetEntityId());
	if (!m_pActor)
	{
		GameWarning("EntityAwareness extension only available for actors");
		return false;
	}

	return true;
}


void CEntityAwareness::PostInit(IGameObject * pGameObject)
{
	pGameObject->EnableUpdateSlot(this, 0);
}


bool CEntityAwareness::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	ResetGameObject();

	CRY_ASSERT_MESSAGE(false, "CEntityAwareness::ReloadExtension not implemented");

	return false;
}


bool CEntityAwareness::GetEntityPoolSignature(TSerialize signature)
{
	CRY_ASSERT_MESSAGE(false, "CEntityAwareness::GetEntityPoolSignature not implemented");

	return true;
}


void CEntityAwareness::FullSerialize(TSerialize ser)
{
	if (ser.GetSerializationTarget() == eST_Network)
		return;

	ser.Value("proximityRadius", m_proximityRadius);
	m_validQueries = 0;

	if (GetISystem()->IsSerializingFile() == 1)
	{
		m_pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(GetEntityId());
		CRY_ASSERT(m_pActor);
	}
}


void CEntityAwareness::Update(SEntityUpdateContext& ctx, int slot)
{
	if (!m_pActor)
		return;

	// If this entity is the local player, and they are in a third person view, then we might want to use the camera
	// for entity selection, instead of the actor's eyes.
	auto localPlayer = CPlayer::GetLocalPlayer();
	if (m_pActor->GetEntityId() == localPlayer->GetAttachedEntityId () && localPlayer->IsThirdPerson())
	{
		auto camera = localPlayer->GetCamera();
		m_eyePosition = camera->GetPosition();
		m_eyeDirection = camera->GetRotation().v.GetNormalized();
	}
	else
	{
		// TODO: Movement controller doesn't update eye position unless the character is moving under their control.
		// For cases where they are falling or sliding, it fails. Need to correct the movement controller most likely.
		if (IMovementController* pMC = m_pActor->GetMovementController())
		{
			SMovementState s;
			pMC->GetMovementState(s);
			m_eyePosition = s.eyePosition;
			m_eyeDirection = s.eyeDirection.GetNormalized();
		}
	}
}


void CEntityAwareness::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->AddObject(this, sizeof(*this));
	pSizer->AddObject(m_entitiesInProximity);
	pSizer->AddObject(m_entitiesInFrontOf);
}


// ***
// *** CEntityAwareness
// ***


void CEntityAwareness::UpdateRaycastQuery()
{
	if (!m_pActor || m_pActor->GetEntity()->IsHidden())
		return;

	if (m_eyeDirection.IsValid())
	{
		// TODO: copy this method of skipping the actor over to the camera code.
		IEntity * pEntity = m_pActor->GetEntity();
		IPhysicalEntity * pPhysEnt = pEntity ? pEntity->GetPhysics() : nullptr;
		IPhysicalEntity *skipEntities [1];
		skipEntities [0] = pPhysEnt;

		int raySlot = GetRaySlot();
		CRY_ASSERT(raySlot != -1);

		m_queuedRays [raySlot].rayId = g_pGame->GetRayCaster().Queue(
			RayCastRequest::HighPriority,
			RayCastRequest(m_eyePosition, m_eyeDirection * forwardCastDistance,
				ent_all,
				rwi_pierceability(PIERCE_GLASS) | rwi_colltype_any,
				skipEntities,
				pPhysEnt ? 1 : 0,
				2),
			functor(*this, &CEntityAwareness::OnRayCastDataReceived));

		m_queuedRays [raySlot].counter = ++m_requestCounter;

		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(m_eyePosition, ColorB(0, 0, 128), m_eyePosition + (m_eyeDirection * forwardCastDistance),
			ColorB(0, 0, 255), 8.0f);
	}

	// We've queued up the ray-cast, but since it's asynchronous we might not get a result for a little while. To
	// counter this, we allow the old result to be valid for a short period of time, until the new result replaces it.
	const float staleness = gEnv->pTimer->GetCurrTime() - m_timeLastDeferredResult;
	if (staleness > maxRaycastStaleness)
	{
		m_rayHitAny = false;
		m_lookAtEntityId = 0;
	}
}


void CEntityAwareness::UpdateProximityQuery()
{
	// The bounding box is created by offseting m_proximityRadius in each direction.
	const Vec3 positionOffset(m_proximityRadius, m_proximityRadius, m_proximityRadius);

	// Retrieve the actor EntityId so we can exclude ourselves from the results.
	EntityId ownerActorId = m_pActor ? m_pActor->GetEntityId() : INVALID_ENTITYID;

	// Clear previous results. We could resize(0) at this point, but rather than thrash memory, I'll see how well
	// it works just growing when needing and remaining at that size.
	m_entitiesInProximity.clear();

	// Set up the bounding box query.
	SEntityProximityQuery qry;
	qry.box.min = m_eyePosition - positionOffset;
	qry.box.max = m_eyePosition + positionOffset;

	// DEBUG: Render the grid for debug purposes.
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(qry.box, false, ColorB(255, 0, 0), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);

	// Run the query and prepare to populate a list of the results, if they pass muster.
	gEnv->pEntitySystem->QueryProximity(qry);
	m_entitiesInProximity.reserve(qry.nCount);

	// Check each result.
	for (int i = 0; i < qry.nCount; i++)
	{
		IEntity* pEntity = qry.pEntities [i];
		EntityId entityId = pEntity ? pEntity->GetId() : INVALID_ENTITYID;

		// Skip this entity.
		if (!entityId || (entityId == ownerActorId))
			continue;

		// Add to the collection.
		m_entitiesInProximity.push_back(entityId);
	}
}


void CEntityAwareness::UpdateNearQuery()
{
	// We extend the grid out based on a scaling factor to give it extra reach.
	const float proximityRadius = m_proximityRadius * proximityCloseByFactor;
	const float flatDistanceSqr = proximityRadius * proximityRadius;
	//const float heightThreshold = proximityRadius * 0.6f;

	// DEBUG: Render the grid for debug purposes.
	const Vec3 positionOffset(proximityRadius, proximityRadius, proximityRadius);
	AABB aabb(m_eyePosition - positionOffset, m_eyePosition + positionOffset);
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(aabb, false, ColorB(196, 0, 0), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);

	// Clear previous results.
	m_entitiesNear.reserve(m_entitiesInProximity.size());
	m_entitiesNear.clear();

	// Retrieve the actor EntityId so we can exclude ourselves from the results.
	EntityId ownerActorId = m_pActor ? m_pActor->GetEntityId() : INVALID_ENTITYID;

	// We parse the results of the latest proximity query, just selecting the ones that match our strict criteria.
	for (auto entityId : m_entitiesInProximity)
	{
		// Skip this entity.
		if (!entityId || (entityId == ownerActorId))
			continue;

		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(entityId);

		// The entity height is compared to a pre-set threshold which flattens the AABB, and discarded if it's outside that range.
		//const Vec3 entityWorldPos = pEntity->GetWorldPos();
		//if (fabs_tpl(entityWorldPos.z - m_eyePosition.z) > heightThreshold)
		//	continue;

		// Seems to discard anything too far based on radius, turning the cube into a flat circle.
		AABB aabb;
		pEntity->GetWorldBounds(aabb);
		aabb.min.z = aabb.max.z = m_eyePosition.z;
		if (aabb.GetDistanceSqr(m_eyePosition) > flatDistanceSqr)
			continue;

		// DEBUG: Highlight the entity.
		AABB bbox;
		pEntity->GetWorldBounds(bbox);
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, true, ColorB(0, 196, 0), EBoundingBoxDrawStyle::eBBD_Faceted);

		// Acceptable result, add to the collection.
		m_entitiesNear.push_back(entityId);
	}
}


void CEntityAwareness::UpdateInFrontOfQuery()
{
	// Clear previous results.
	m_entitiesInFrontOf.reserve(m_entitiesInProximity.size());
	m_entitiesInFrontOf.clear();

	// A line segment to represent where the actor is looking.
	Lineseg lineseg(m_eyePosition, m_eyePosition + m_proximityRadius * m_eyeDirection);

	// DEBUG: get the lineseg to show up.
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(lineseg.start, ColorB(64, 0, 0), lineseg.end, ColorB(128, 0, 0), 8.0f);

	// Retrieve the actor EntityId so we can exclude ourselves from the results.
	EntityId ownerActorId = m_pActor ? m_pActor->GetEntityId() : INVALID_ENTITYID;

	// We parse the results of the latest proximity query, just selecting the ones that match our strict criteria.
	for (auto entityId : m_entitiesInProximity)
	{
		// Skip this entity.
		if (!entityId || (entityId == ownerActorId))
			continue;

		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(entityId);

		AABB bbox;
		pEntity->GetLocalBounds(bbox);
		if (!bbox.IsEmpty())
		{
			OBB obb(OBB::CreateOBBfromAABB(Matrix33(pEntity->GetWorldTM()), bbox));

			if (Overlap::Lineseg_OBB(lineseg, pEntity->GetWorldPos(), obb))
			{
				// DEBUG: let's see those boxes.
				gEnv->pRenderer->GetIRenderAuxGeom()->DrawOBB(obb, pEntity->GetWorldTM(), true, ColorB(0, 0, 196), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);

				m_entitiesInFrontOf.push_back(pEntity->GetId());
			}
		}
	}
}


void CEntityAwareness::OnRayCastDataReceived(const QueuedRayID& rayID, const RayCastResult& result)
{
	int raySlot = GetSlotForRay(rayID);
	CRY_ASSERT(raySlot != -1);

	if (raySlot != -1)
	{
		m_queuedRays [raySlot].rayId = 0;
		m_queuedRays [raySlot].counter = 0;
	}

	const bool rayHitSucced = (result.hitCount > 0);
	EntityId entityIdHit = INVALID_ENTITYID;

	m_rayHitPierceable.dist = -1.0f;

	if (rayHitSucced)
	{
		// Only because the raycast is being done with  rwi_stop_at_pierceable can we rely on there being only 1 hit. 
		// Otherwise hit[0] is always the solid hit (and thus the last), hit[1-n] are then first to last-1 in order of distance
		m_rayHitSolid = result.hits [0];

		//#if defined(_DEBUG)
		//			if (m_bDebug)
		{
			// Initial position for our target.
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(m_rayHitSolid.pt, 0.05f, ColorB(255, 0, 0));
		}
		//#endif


		// Figure out who the entity is, if there is one.
		IEntity *pEntity = gEnv->pEntitySystem->GetEntityFromPhysics(m_rayHitSolid.pCollider);

		// Check if it's the child or not.
		int partId = m_rayHitSolid.partid;
		if (partId && pEntity)
			pEntity = pEntity->UnmapAttachedChild(partId);
		if (pEntity)
			entityIdHit = pEntity->GetId();

		// It is not safe to access this, since the result can be kept for multiple frames. Yet other data of the hit can be
		// useful, and the entity Id is also remembered.
		m_rayHitSolid.pCollider = nullptr;
		m_rayHitSolid.next = nullptr;

		if (result.hitCount > 1)
		{
			m_rayHitPierceable = result.hits [1];
			m_rayHitPierceable.pCollider = nullptr;
			m_rayHitPierceable.next = nullptr;

			// DEBUG
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(result.hits [1].pt, 0.16f, ColorB(0, 0, 255));
		}
	}

	m_timeLastDeferredResult = gEnv->pTimer->GetCurrTime();
	m_rayHitAny = rayHitSucced;
	m_lookAtEntityId = entityIdHit;
}


int CEntityAwareness::GetRaySlot()
{
	for (int i = 0; i < maxQueuedRays; ++i)
	{
		if (m_queuedRays [i].rayId == 0)
		{
			return i;
		}
	}

	// Find the oldest request and remove it.
	uint32 oldestCounter = m_requestCounter + 1;
	int oldestSlot = 0;
	for (int i = 0; i < maxQueuedRays; ++i)
	{
		if (m_queuedRays [i].counter < oldestCounter)
		{
			oldestCounter = m_queuedRays [i].counter;
			oldestSlot = i;
		}
	}

	m_queuedRays [oldestSlot].Reset();

	return oldestSlot;
}


int CEntityAwareness::GetSlotForRay(const QueuedRayID& rayId) const
{
	for (int i = 0; i < maxQueuedRays; ++i)
	{
		if (m_queuedRays [i].rayId == rayId)
		{
			return i;
		}
	}

	return -1;
}
