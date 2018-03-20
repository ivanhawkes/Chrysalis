#include <StdAfx.h>

#include "EntityAwarenessComponent.h"
#include <GameObjects/GameObject.h>
#include <IActorSystem.h>
#include <IMovementController.h>
#include "Components/Player/PlayerComponent.h"
#include <Actor/Character/CharacterComponent.h>
#include <Actor/ActorComponent.h>
#include <Components/Player/Camera/ICameraComponent.h>
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Console/CVars.h>


namespace Chrysalis
{
/** The distance forward of the actor we will ray-cast in search of entities. */
static const float forwardCastDistance = 300.0f;

/** Scales the proximity distance for what is considered 'close by' queries. */
static const float proximityCloseByFactor = 0.5f;

/** Allow previous ray-cast results to still be used for up to this long, in seconds. */
static const float maxRaycastStaleness = 0.05f;

#define PIERCE_GLASS (13)


void CEntityAwarenessComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CEntityAwarenessComponent::ReflectType(Schematyc::CTypeDesc<CEntityAwarenessComponent>& desc)
{
	desc.SetGUID(CEntityAwarenessComponent::IID());
	desc.SetEditorCategory("Entities");
	desc.SetLabel("Entity Awareness");
	desc.SetDescription("Allow an entity to become aware of other entities around it.");
	desc.SetIcon("icons:ObjectTypes/light.ico");

	// TODO: Do we need a transform for this? Likely not.
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

	// Mark the actor component as a hard requirement.
	desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, CActorComponent::IID());
}


void CEntityAwarenessComponent::Initialize()
{
	m_pActor = GetEntity()->GetComponent<CActorComponent>();
	CRY_ASSERT_MESSAGE(m_pActor, "EntityAwareness component requires an actor component.");
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


void CEntityAwarenessComponent::Update()
{
	if (!m_pActor)
		return;

	m_eyePosition = m_pActor->GetEntity()->GetPos() + m_pActor->GetLocalEyePos();

	// Using the local player's camera is fine for now. Arguably, you should get it from the actor skeleton instead
	// but actor's don't allow you to query the eye direction, only it's position. Heisenberg much?
	// #TODO: make it work with the actor's eyes instead. That will work fine in FP, but might be tougher to control in TP - so test it.
	auto localPlayer = CPlayerComponent::GetLocalPlayer();

	// #HACK: It should never come back null, but it does right now so we need this test.
	if (localPlayer)
	{
		// HACK: We should get the actor instead, do this when patching for 5.4.
		if (auto pActorComponent = CPlayerComponent::GetLocalActor())
		{
			// Is this entity under local control?
			if (pActorComponent->GetEntityId() == GetEntityId())
			{
				// If we're playing as this character, we should use the camera as the eye direction for ray casting, etc.
				auto camera = localPlayer->GetCamera();
				m_eyeDirection = camera->GetRotation();
			}
			else
			{
				// HACK: we can't use the camera, but we also don't have access to the eye direction from the actor (if there even
				// is one). After 5.4 we should have refactored enough to get an eye direction when possible from all entities that
				// support it. For now, we will take the direction the entity is facing. 
				m_eyeDirection = Quat::CreateRotationVDir(GetEntity()->GetForwardDir());
			}
		}
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


void CEntityAwarenessComponent::UpdateRaycastQuery()
{
	if (!m_pActor || m_pActor->GetEntity()->IsHidden())
		return;

	if (m_eyeDirection.IsValid())
	{
		IEntity * pEntity = m_pActor->GetEntity();
		IPhysicalEntity * pPhysEnt = pEntity ? pEntity->GetPhysics() : nullptr;
		IPhysicalEntity *skipEntities [1];
		skipEntities [0] = pPhysEnt;

		int raySlot = RequestRaySlotId();
		CRY_ASSERT(raySlot != -1);

		ray_hit rayHit;

		// Use a synchronous ray-cast.
		auto hits = gEnv->pPhysicalWorld->RayWorldIntersection(
			m_eyePosition, m_eyeDirection * FORWARD_DIRECTION * forwardCastDistance,
			ent_all, rwi_pierceability(PIERCE_GLASS) | rwi_colltype_any,
			&rayHit, 1,
			skipEntities, pPhysEnt ? 1 : 0);
		if (hits > 0)
		{
			OnRayCast(rayHit);
		}

		// HACK: Removed asynchronous raycasts because they were using CryAction and that had link problems with 5.3 and
		// will be removed when 5.4 releases. 
		// Use an asynchronous ray-cast.
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

#if defined(_DEBUG)
		if (g_cvars.m_componentAwarenessDebug & eDB_RayCast)
		{
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(m_eyePosition, ColorB(0, 0, 128),
				m_eyePosition + (m_eyeDirection * FORWARD_DIRECTION * forwardCastDistance),
				ColorB(0, 0, 255), 8.0f);
		}
#endif
	}

	// We've queued up the ray-cast, but since it's asynchronous we might not get a result for a little while. To
	// counter this, we allow the old result to be valid for a short period of time, until the new result replaces it.
	const float staleness = gEnv->pTimer->GetCurrTime() - m_timeLastDeferredResult;
	if (staleness > maxRaycastStaleness)
	{
		m_rayHitAny = false;
		m_lookAtEntityId = INVALID_ENTITYID;
	}
}


void CEntityAwarenessComponent::OnRayCastDataReceived(const QueuedRayID& rayID, const RayCastResult& result)
{
	int raySlot = QueryRaySlotId(rayID);
	CRY_ASSERT(raySlot != -1);

	// Clear out the raycast slot if we used one.
	if (raySlot != -1)
		m_queuedRays [raySlot].Reset();

	// Force the distance to a negative value to invalidate the last result.
	m_rayHitPierceable.dist = -1.0f;

	const bool rayHitSucceed = (result.hitCount > 0);

	if (rayHitSucceed)
	{
		// Only because the raycast is being done with rwi_stop_at_pierceable can we rely on there being only 1 hit.
		// Otherwise hit[0] is always the solid hit (and thus the last), hit[1-n] are then first to last-1 in order of
		// distance. 
		OnRayCast(result.hits [0]);

		// Do we also have a pieceable result?
		if (result.hitCount > 1)
		{
			// We'll use the first pieceable result if there is one.
			m_rayHitPierceable = result.hits [1];
			m_rayHitPierceable.pCollider = nullptr;
			m_rayHitPierceable.next = nullptr;

#if defined(_DEBUG)
			if (g_cvars.m_componentAwarenessDebug & eDB_RayCast)
			{
				gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(result.hits [1].pt, 0.01f, ColorB(0, 0, 255));
			}
#endif
		}
	}

	// Book-keeping.
	m_rayHitAny = rayHitSucceed;
}


void CEntityAwarenessComponent::OnRayCast(const ray_hit & rayHit)
{
	EntityId entityIdHit = INVALID_ENTITYID;

	// Only because the raycast is being done with  rwi_stop_at_pierceable can we rely on there being only 1 hit. 
	// Otherwise hit[0] is always the solid hit (and thus the last), hit[1-n] are then first to last-1 in order of distance
	m_rayHitSolid = rayHit;

	// Find out what we can about the type of surface it intersected. This is useful for determining which of several
	// proxies it hit, etc.
	if (ISurfaceType* pSurfaceType = gEnv->p3DEngine->GetMaterialManager()->GetSurfaceType(m_rayHitSolid.surface_idx))
	{
		// TODO: Do something useful with this instead of it being a null op.
		// This should be exposed to code / schematyc / FG.
		auto surfaceId = pSurfaceType->GetId();
		string surfaceName = pSurfaceType->GetName();
		auto surfaceTypeName = pSurfaceType->GetType();

		//CryWatch("SurfaceId: %d", surfaceId);
		//CryWatch("surfaceName: %s", surfaceName);
		//CryWatch("surfaceTypeName: %s", surfaceTypeName);
	}

#if defined(_DEBUG)
	if (g_cvars.m_componentAwarenessDebug & eDB_RayCast)
	{
		// Highlight the hit point.
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(m_rayHitSolid.pt, 0.01f, ColorB(255, 0, 0));
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

	// Book-keeping.
	m_timeLastDeferredResult = gEnv->pTimer->GetCurrTime();
	m_lookAtEntityId = entityIdHit;
}


int CEntityAwarenessComponent::RequestRaySlotId()
{
	for (int i = 0; i < maxQueuedRays; ++i)
	{
		if (m_queuedRays [i].rayId == 0)
		{
			return i;
		}
	}

	// Find the oldest request and remove it.
	// HACK: This will lead to wrap-around issues, do a better job of getting a slot.
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


int CEntityAwarenessComponent::QueryRaySlotId(const QueuedRayID& rayId) const
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
	if (g_cvars.m_componentAwarenessDebug & eDB_ProximalEntities)
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
		if (g_cvars.m_componentAwarenessDebug & eDB_ProximalEntities)
		{
			if (strcmp(pEntity->GetName(), "TestMe") == 0)
				int a = 1;

			// DEBUG: Highlight each entity within the range.
			AABB bbox;
			pEntity->GetWorldBounds(bbox);
			bbox.Expand(Vec3(0.01f, 0.01f, 0.01f));
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, true, ColorB(0, 64, 0), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);
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
	if (g_cvars.m_componentAwarenessDebug & eDB_NearEntities)
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
		if (g_cvars.m_componentAwarenessDebug & eDB_NearEntities)
		{
			// DEBUG: Highlight the entity.
			AABB bbox;
			pEntity->GetWorldBounds(bbox);
			bbox.Expand(Vec3(0.02f, 0.02f, 0.02f));
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, true, ColorB(0, 196, 0), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);
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
	if (g_cvars.m_componentAwarenessDebug & eDB_InFront)
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
				if (g_cvars.m_componentAwarenessDebug & eDB_InFront)
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
			if (g_cvars.m_componentAwarenessDebug & eDB_DotFiltered)
			{
				// DEBUG: Highlight the entity.
				bbox.Expand(Vec3(0.03f, 0.03f, 0.03f));
				gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, true, ColorB(0, 0, 64), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);
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
	if (g_cvars.m_componentAwarenessDebug & eDB_DotFiltered)
	{
		IEntity *pEntity;
		if ((m_entitiesNearDotFiltered.size() > 0) && (pEntity = gEnv->pEntitySystem->GetEntity(m_entitiesNearDotFiltered [0])))
		{
			AABB bbox;
			pEntity->GetWorldBounds(bbox);
			bbox.Expand(Vec3(0.04f, 0.04f, 0.04f));
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, true, ColorB(192, 0, 0), EBoundingBoxDrawStyle::eBBD_Extremes_Color_Encoded);
		}
	}
#endif

	return m_entitiesNearDotFiltered;
}
}