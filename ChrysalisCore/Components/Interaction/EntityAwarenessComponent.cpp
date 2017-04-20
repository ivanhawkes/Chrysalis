#include <StdAfx.h>

#include "EntityAwarenessComponent.h"
#include <GameObjects/GameObject.h>
#include <IActorSystem.h>
#include <IMovementController.h>
#include <Player/Player.h>
#include <Actor/Character/Character.h>
#include <Player/Camera/ICameraComponent.h>
#include <Components/Interaction/EntityInteractionComponent.h>


CRYREGISTER_CLASS(CEntityAwarenessComponent)


class CEntityAwarenessRegistrator
	: public IEntityRegistrator
	, public CEntityAwarenessComponent::SExternalCVars
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CEntityAwarenessComponent>("EntityAwareness", "Entities");

		// This should make the entity class invisible in the editor.
		auto cls = gEnv->pEntitySystem->GetClassRegistry()->FindClass("EntityAwareness");
		cls->SetFlags(cls->GetFlags() | ECLF_INVISIBLE);

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_awareness_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CEntityAwarenessRegistrator g_entityAwarenessRegistrator;


/** The distance forward of the actor we will ray-cast in search of entities. */
static const float forwardCastDistance = 300.0f;

/** Scales the proximity distance for what is considered 'close by' queries. */
static const float proximityCloseByFactor = 0.5f;

/** Allow previous ray-cast results to still be used for up to this long, in seconds. */
static const float maxRaycastStaleness = 0.05f;

#define PIERCE_GLASS (13)


// ***
// *** IGameObjectExtension
// ***

void CEntityAwarenessComponent::Initialize()
{
	// Required for 5.3 to call update.
	GetEntity()->Activate(true);

	// This extension is only valid for actors.
	m_pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(GetEntityId());
	if (!m_pActor)
		GameWarning("EntityAwareness extension only available for actors");
}


void CEntityAwarenessComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			Update();
			break;
	}
}


void CEntityAwarenessComponent::SerializeProperties(Serialization::IArchive& archive)
{
	//if (archive.isInput())
	//{
	//	OnResetState();
	//}
}


//void CEntityAwarenessComponent::FullSerialize(TSerialize ser)
//{
//	if (ser.GetSerializationTarget() == eST_Network)
//		return;
//
//	ser.Value("proximityRadius", m_proximityRadius);
//	m_validQueries = 0;
//
//	if (GetISystem()->IsSerializingFile() == 1)
//	{
//		m_pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(GetEntityId());
//		CRY_ASSERT(m_pActor);
//	}
//}


void CEntityAwarenessComponent::Update()
{
	if (!m_pActor)
		return;

	m_eyePosition = m_pActor->GetEntity()->GetPos() + m_pActor->GetLocalEyePos();

	// Using the local player's camera is fine for now. Arguably, you should get it from the actor skeleton instead
	// but actor's don't allow you to query the eye direction, only it's position. Heisenberg much?
	// #TODO: make it work with the actor's eyes instead. That will work fine in FP, but might be tougher to control in TP - so test it.
	auto localPlayer = CPlayer::GetLocalPlayer();
	
	// #HACK: It should never come back null, but it does right now so we need this test.
	if (localPlayer)
	{
		auto camera = localPlayer->GetCamera();
		m_eyeDirection = camera->GetRotation();
	}

	// #HACK: For now, I am just going to force these queries to run each update tick. We can finesse it later by allowing
	// selection of which ones run.
	GetNearDotFiltered();
	RaycastQuery();
}


void CEntityAwarenessComponent::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->AddObject(this, sizeof(*this));
	pSizer->AddObject(m_entitiesInProximity);
	pSizer->AddObject(m_entitiesInFrontOf);
}


// ***
// *** CEntityAwarenessComponent
// ***


CEntityAwarenessComponent::UpdateQueryFunction CEntityAwarenessComponent::m_updateQueryFunctions [] =
{
	&CEntityAwarenessComponent::UpdateRaycastQuery,
	&CEntityAwarenessComponent::UpdateProximityQuery,
	&CEntityAwarenessComponent::UpdateNearQuery,
	&CEntityAwarenessComponent::UpdateInFrontOfQuery,
};


CEntityAwarenessComponent::CEntityAwarenessComponent()
{
	m_rayHitPierceable.dist = -1.0f;
}


CEntityAwarenessComponent::~CEntityAwarenessComponent()
{
	for (int i = 0; i < maxQueuedRays; ++i)
	{
		m_queuedRays [i].Reset();
	}
}


const CEntityAwarenessComponent::SExternalCVars& CEntityAwarenessComponent::GetCVars() const
{
	return g_entityAwarenessRegistrator;
}


void CEntityAwarenessComponent::UpdateRaycastQuery()
{
	// #HACK: Really big hack, I've removed the ability to ray-cast due to link errors against 5.3


//	if (!m_pActor || m_pActor->GetEntity()->IsHidden())
//		return;
//
//	if (m_eyeDirection.IsValid())
//	{
//		IEntity * pEntity = m_pActor->GetEntity();
//		IPhysicalEntity * pPhysEnt = pEntity ? pEntity->GetPhysics() : nullptr;
//		IPhysicalEntity *skipEntities [1];
//		skipEntities [0] = pPhysEnt;
//
//		int raySlot = GetRaySlot();
//		CRY_ASSERT(raySlot != -1);
//
//		m_queuedRays [raySlot].rayId = static_cast<CCryAction*>(gEnv->pGameFramework)->GetPhysicQueues().GetRayCaster().Queue(
//			RayCastRequest::HighPriority,
//			RayCastRequest(m_eyePosition, m_eyeDirection * FORWARD_DIRECTION * forwardCastDistance,
//				ent_all,
//				rwi_pierceability(PIERCE_GLASS) | rwi_colltype_any,
//				skipEntities,
//				pPhysEnt ? 1 : 0,
//				2),
//			functor(*this, &CEntityAwarenessComponent::OnRayCastDataReceived));
//
//		m_queuedRays [raySlot].counter = ++m_requestCounter;
//
//#if defined(_DEBUG)
//		if (GetCVars().m_debug & eDB_RayCast)
//		{
//			gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(m_eyePosition, ColorB(0, 0, 128),
//				m_eyePosition + (m_eyeDirection * FORWARD_DIRECTION * forwardCastDistance), ColorB(0, 0, 255), 8.0f);
//		}
//#endif
//	}
//
//	// We've queued up the ray-cast, but since it's asynchronous we might not get a result for a little while. To
//	// counter this, we allow the old result to be valid for a short period of time, until the new result replaces it.
//	const float staleness = gEnv->pTimer->GetCurrTime() - m_timeLastDeferredResult;
//	if (staleness > maxRaycastStaleness)
//	{
//		m_rayHitAny = false;
//		m_lookAtEntityId = 0;
//	}
}


void CEntityAwarenessComponent::UpdateProximityQuery()
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

#if defined(_DEBUG)
	if (GetCVars().m_debug & eDB_ProximalEntities)
	{
		// DEBUG: Render the grid for debug purposes.
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(qry.box, false, ColorB(255, 0, 0), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);
	}
#endif

	// Run the query and prepare to populate a list of the results, if they pass muster.
	gEnv->pEntitySystem->QueryProximity(qry);
	m_entitiesInProximity.reserve(qry.nCount);

	// Check each result.
	for (int i = 0; i < qry.nCount; ++i)
	{
		IEntity* pEntity = qry.pEntities [i];
		EntityId entityId = pEntity ? pEntity->GetId() : INVALID_ENTITYID;

		// Skip this entity.
		if ((entityId == INVALID_ENTITYID) || (entityId == ownerActorId))
			continue;

#if defined(_DEBUG)
		if (GetCVars().m_debug & eDB_ProximalEntities)
		{
			// DEBUG: Highlight each entity within the range.
			AABB bbox;
			pEntity->GetWorldBounds(bbox);
			bbox.Expand(Vec3(0.01f, 0.01f, 0.01f));
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, true, ColorB(0, 64, 0), EBoundingBoxDrawStyle::eBBD_Faceted);
		}
#endif

		// Add to the collection.
		m_entitiesInProximity.push_back(entityId);
	}
}


void CEntityAwarenessComponent::UpdateNearQuery()
{
	// We extend the grid out based on a scaling factor to give it extra reach.
	const float proximityRadius = m_proximityRadius * proximityCloseByFactor;
	const float flatDistanceSqr = proximityRadius * proximityRadius;

#if defined(_DEBUG)
	if (GetCVars().m_debug & eDB_NearEntities)
	{
		// DEBUG: Render the grid for debug purposes.
		const Vec3 positionOffset(proximityRadius, proximityRadius, proximityRadius);
		AABB aabb(m_eyePosition - positionOffset, m_eyePosition + positionOffset);
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(aabb, false, ColorB(0, 196, 0), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);
	}
#endif

	// Clear previous results.
	m_entitiesNear.reserve(m_entitiesInProximity.size());
	m_entitiesNear.clear();

	// Retrieve the actor EntityId so we can exclude ourselves from the results.
	EntityId ownerActorId = m_pActor ? m_pActor->GetEntityId() : INVALID_ENTITYID;

	// We parse the results of the latest proximity query, just selecting the ones that match our strict criteria.
	for (auto& entityId : m_entitiesInProximity)
	{
		// Skip this entity.
		if ((entityId == INVALID_ENTITYID) || (entityId == ownerActorId))
			continue;

		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(entityId);

		// #TODO: The near query should really be based off distance from the actor, eye position is currently the camera in TP modes.

		// Seems to discard anything too far based on radius, turning the cube into a flat circle.
		AABB aabb;
		pEntity->GetWorldBounds(aabb);
		aabb.min.z = aabb.max.z = m_eyePosition.z;
		if (aabb.GetDistanceSqr(m_eyePosition) > flatDistanceSqr)
			continue;

#if defined(_DEBUG)
		if (GetCVars().m_debug & eDB_NearEntities)
		{
			// DEBUG: Highlight the entity.
			AABB bbox;
			pEntity->GetWorldBounds(bbox);
			bbox.Expand(Vec3(0.02f, 0.02f, 0.02f));
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, true, ColorB(0, 196, 0), EBoundingBoxDrawStyle::eBBD_Faceted);
		}
#endif

		// Acceptable result, add to the collection.
		m_entitiesNear.push_back(entityId);
	}
}


void CEntityAwarenessComponent::UpdateInFrontOfQuery()
{
	// Clear previous results.
	m_entitiesInFrontOf.reserve(m_entitiesInProximity.size());
	m_entitiesInFrontOf.clear();

	// A line segment to represent where the actor is looking.
	Lineseg lineseg(m_eyePosition, m_eyePosition + (m_eyeDirection * FORWARD_DIRECTION * m_proximityRadius));

#if defined(_DEBUG)
	if (GetCVars().m_debug & eDB_InFront)
	{
		// DEBUG: get the lineseg to show up.
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(lineseg.start, ColorB(64, 0, 0), lineseg.end, ColorB(128, 0, 0), 8.0f);
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(lineseg.start, 0.025f, ColorB(0, 0, 255, 255));
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(lineseg.end, 0.025f, ColorB(0, 0, 255, 255));
	}
#endif

	// Retrieve the actor EntityId so we can exclude ourselves from the results.
	EntityId ownerActorId = m_pActor ? m_pActor->GetEntityId() : INVALID_ENTITYID;

	// We parse the results of the latest proximity query, just selecting the ones that match our strict criteria.
	for (auto& entityId : m_entitiesInProximity)
	{
		// Skip this entity.
		if ((entityId == INVALID_ENTITYID) || (entityId == ownerActorId))
			continue;

		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(entityId);

		AABB bbox;
		pEntity->GetLocalBounds(bbox);
		if (!bbox.IsEmpty())
		{
			OBB obb(OBB::CreateOBBfromAABB(Matrix33(pEntity->GetWorldTM()), bbox));

			if (Overlap::Lineseg_OBB(lineseg, pEntity->GetWorldPos(), obb))
			{
#if defined(_DEBUG)
				if (GetCVars().m_debug & eDB_InFront)
				{
					// DEBUG: let's see those boxes.
					gEnv->pRenderer->GetIRenderAuxGeom()->DrawOBB(obb, pEntity->GetWorldTM(), true, ColorB(0, 0, 196), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);
				}
#endif

				// #HACK: TEST: wrong place for this test - move it to somewhere i can see it try all the entities near us.
				auto pInteractor = pEntity->GetComponent<CEntityInteractionComponent>();
				if (pInteractor)
				{
					// There's an interactor component, so this is an interactive entity.
					// 
					auto verbs = pInteractor->GetVerbs();
				}

				m_entitiesInFrontOf.push_back(pEntity->GetId());
			}
		}
	}
}


const Entities& CEntityAwarenessComponent::GetNearDotFiltered(float minDot, float maxDot)
{
	int resultIndex = 0;
	int bestResultIndex = 0;
	float bestScore = 10000.0f;

	// Clear previous results.
	m_entitiesNearDotFiltered.reserve(m_entitiesInProximity.size());
	m_entitiesNearDotFiltered.clear();

	// Refresh the list of near entities.
	auto entities = NearQuery();

	// Check each entity to see which is the best fit.
	for (auto& entityId : entities)
	{
		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
		if (!pEntity)
			continue;

		AABB bbox;
		pEntity->GetWorldBounds(bbox);

		Vec3 itemPos = bbox.GetCenter();
		Vec3 dirToItem = (itemPos - m_eyePosition).normalized();
		Vec3 dirLooking = (m_eyeDirection * FORWARD_DIRECTION).normalized();
		float dotToItem = dirToItem.dot(dirLooking);

		if ((dotToItem >= minDot) && (dotToItem <= maxDot))
		{
#if defined(_DEBUG)
			if (GetCVars().m_debug & eDB_DotFiltered)
			{
				// DEBUG: Highlight the entity.
				bbox.Expand(Vec3(0.03f, 0.03f, 0.03f));
				gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, true, ColorB(0, 0, 64), EBoundingBoxDrawStyle::eBBD_Faceted);
			}
#endif

			// Provide a score for this result to qualify how good a match it is.
			float distance = (itemPos - m_eyePosition).len();
			float score = (1.0f - dotToItem) * distance;
			if (score < bestScore)
			{
				bestResultIndex = resultIndex;
				bestScore = score;
			}

			// Acceptable result, add to the collection.
			m_entitiesNearDotFiltered.push_back(entityId);
			resultIndex++;
		}
	}

	// Before returning the result we should swap the best result into the first element.
	if ((m_entitiesNearDotFiltered.size() > 1) && (bestResultIndex != 0))
		std::swap(m_entitiesNearDotFiltered [0], m_entitiesNearDotFiltered [bestResultIndex]);

#if defined(_DEBUG)
	if (GetCVars().m_debug & eDB_DotFiltered)
	{
		IEntity *pEntity;
		if ((m_entitiesNearDotFiltered.size() > 0) && (pEntity = gEnv->pEntitySystem->GetEntity(m_entitiesNearDotFiltered [0])))
		{
			AABB bbox;
			pEntity->GetWorldBounds(bbox);
			bbox.Expand(Vec3(0.04f, 0.04f, 0.04f));
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, true, ColorB(192, 0, 0), EBoundingBoxDrawStyle::eBBD_Faceted);
		}
	}
#endif

	return m_entitiesNearDotFiltered;
}


void CEntityAwarenessComponent::OnRayCastDataReceived(const QueuedRayID& rayID, const RayCastResult& result)
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

#if defined(_DEBUG)
		if (GetCVars().m_debug & eDB_RayCast)
		{
			// Initial position for our target.
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(m_rayHitSolid.pt, 0.04f, ColorB(255, 0, 0));
		}
#endif

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

#if defined(_DEBUG)
			if (GetCVars().m_debug & eDB_RayCast)
			{
				gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(result.hits [1].pt, 0.04f, ColorB(0, 0, 255));
			}
#endif
		}
	}

	m_timeLastDeferredResult = gEnv->pTimer->GetCurrTime();
	m_rayHitAny = rayHitSucced;
	m_lookAtEntityId = entityIdHit;
}


int CEntityAwarenessComponent::GetRaySlot()
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


int CEntityAwarenessComponent::GetSlotForRay(const QueuedRayID& rayId) const
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
