#include <StdAfx.h>

#include "ActorStateSwimWaterTestProxy.h"
#include <Components/Actor/ActorControllerComponent.h>
#include <CryAction.h>
#include <CryActionPhysicQueues.h>


namespace Chrysalis
{
float CActorStateSwimWaterTestProxy::s_rayLength = 10.f;

CActorStateSwimWaterTestProxy::CActorStateSwimWaterTestProxy()
	: m_submergedFraction(0.0f)
	, m_shouldSwim(false)
	, m_lastWaterLevelCheckPosition(ZERO)
	, m_waterLevel(WATER_LEVEL_UNKNOWN)
	, m_bottomLevel(BOTTOM_LEVEL_UNKNOWN)
	, m_relativeBottomLevel(0.0f)
	, m_actorWaterLevel(-WATER_LEVEL_UNKNOWN)
	, m_bottomLevelRayID(0)
	, m_swimmingTimer(-1000.0f)
	, m_timeWaterLevelLastUpdated(0.0f)
	, m_headUnderwater(false)
	, m_headComingOutOfWater(false)
	, m_lastInternalState(eProxyInternalState_OutOfWater)
	, m_internalState(eProxyInternalState_OutOfWater)
	, m_lastRayCastResult(BOTTOM_LEVEL_UNKNOWN)
{}


CActorStateSwimWaterTestProxy::~CActorStateSwimWaterTestProxy()
{
	CancelPendingRays();
}


void CActorStateSwimWaterTestProxy::Reset(bool bCancelRays)
{
	if (bCancelRays)
	{
		CancelPendingRays();
		m_bottomLevelRayID = (0);
	}

	m_lastInternalState = m_internalState = eProxyInternalState_OutOfWater;
	m_submergedFraction = 0.0f;
	m_shouldSwim = false;
	m_lastWaterLevelCheckPosition = ZERO;
	m_waterLevel = WATER_LEVEL_UNKNOWN;
	m_bottomLevel = BOTTOM_LEVEL_UNKNOWN;
	m_lastRayCastResult = BOTTOM_LEVEL_UNKNOWN;
	m_relativeBottomLevel = 0.0f;
	m_actorWaterLevel = -WATER_LEVEL_UNKNOWN;
	m_swimmingTimer = 0.0f;
	m_headUnderwater = false;
	m_headComingOutOfWater = false;
}


void CActorStateSwimWaterTestProxy::OnEnterWater(const CActorControllerComponent& actorControllerComponent)
{
	// Force refresh water level (needed for serialization, when state is forced when loading a saved game).
	const Matrix34& CharacterWorldTM = actorControllerComponent.GetEntity()->GetWorldTM();
	const Vec3 CharacterWorldPos = CharacterWorldTM.GetTranslation();
	const Vec3 localReferencePos = GetLocalReferencePosition(actorControllerComponent);
	const Vec3 worldReferencePos = CharacterWorldPos + (Quat(CharacterWorldTM) * localReferencePos);

	m_waterLevel = gEnv->p3DEngine->GetWaterLevel(&worldReferencePos);
	m_internalState = eProxyInternalState_Swimming;
	m_swimmingTimer = 0.0f;

	CRY_ASSERT(m_waterLevel > WATER_LEVEL_UNKNOWN);
}


void CActorStateSwimWaterTestProxy::OnExitWater(const CActorControllerComponent& actorControllerComponent)
{
	Reset(true);
}


void CActorStateSwimWaterTestProxy::Update(const CActorControllerComponent& actorControllerComponent, const float frameTime)
{
	//CRY_PROFILE_FUNCTION(PROFILE_GAME);

	if (gEnv->IsEditor() && !gEnv->IsEditorGameMode())
	{
		m_lastInternalState = m_internalState;
		m_internalState = eProxyInternalState_OutOfWater;
		m_shouldSwim = false;
	}

	float newSwimmingTimer = 0.0f;
	switch (m_internalState)
	{
		case eProxyInternalState_OutOfWater:
		{
			if (m_lastInternalState != eProxyInternalState_OutOfWater)
			{
				Reset(true);
			}
			UpdateOutOfWater(actorControllerComponent, frameTime);
			newSwimmingTimer = m_swimmingTimer - frameTime;
			break;
		}

		case eProxyInternalState_PartiallySubmerged:
		{
			UpdateInWater(actorControllerComponent, frameTime);
			newSwimmingTimer = m_swimmingTimer - frameTime;
			break;
		}

		case eProxyInternalState_Swimming:
		{
			UpdateInWater(actorControllerComponent, frameTime);
			newSwimmingTimer = m_swimmingTimer + frameTime;
			break;
		}
	}

	m_swimmingTimer = clamp_tpl(newSwimmingTimer, -1000.0f, 1000.0f);
}


void CActorStateSwimWaterTestProxy::ForceUpdateBottomLevel(const CActorControllerComponent& actorControllerComponent)
{
	if (!IsWaitingForBottomLevelResults())
	{
		RayTestBottomLevel(actorControllerComponent, actorControllerComponent.GetEntity()->GetWorldPos(), CActorStateSwimWaterTestProxy::GetRayLength());
	}
}


void CActorStateSwimWaterTestProxy::PreUpdateNotSwimming(const CActorControllerComponent& actorControllerComponent, const float frameTime)
{
	const float submergedThreshold = 0.25f;
	m_lastInternalState = m_internalState;
	m_internalState = ((m_submergedFraction < submergedThreshold)) ? eProxyInternalState_OutOfWater : eProxyInternalState_PartiallySubmerged;
}


void CActorStateSwimWaterTestProxy::PreUpdateSwimming(const CActorControllerComponent& actorControllerComponent, const float frameTime)
{
	m_lastInternalState = m_internalState;
	m_internalState = eProxyInternalState_Swimming;
}


void CActorStateSwimWaterTestProxy::UpdateOutOfWater(const CActorControllerComponent& actorControllerComponent, const float frameTime)
{
	//CRY_PROFILE_FUNCTION(PROFILE_GAME);

	// Out of water, only query water level to figure out if Character / AI is in contact with a water volume.
	const Matrix34& CharacterWorldTM = actorControllerComponent.GetEntity()->GetWorldTM();
	const Vec3 CharacterWorldPos = CharacterWorldTM.GetTranslation();

	const Vec3 localReferencePos = GetLocalReferencePosition(actorControllerComponent);

	// Note: Try to tune value and set to higher value possible which works well.
	const bool lastCheckFarAwayEnough = ((m_lastWaterLevelCheckPosition - CharacterWorldPos).len2() >= sqr(0.6f));

	float worldWaterLevel = m_waterLevel;
	if (lastCheckFarAwayEnough)
	{
		const Vec3 worldReferencePos = CharacterWorldPos + (Quat(CharacterWorldTM) * localReferencePos);
		IPhysicalEntity* piPhysEntity = actorControllerComponent.GetEntity()->GetPhysics();

		UpdateWaterLevel(worldReferencePos, CharacterWorldPos, piPhysEntity);
	}

	// Update submerged fraction.
	UpdateSubmergedFraction(localReferencePos.z, CharacterWorldPos.z, worldWaterLevel);
}


void CActorStateSwimWaterTestProxy::UpdateInWater(const CActorControllerComponent& actorControllerComponent, const float frameTime)
{
	//CRY_PROFILE_FUNCTION(PROFILE_GAME);

	const Matrix34& CharacterWorldTM = actorControllerComponent.GetEntity()->GetWorldTM();
	const Vec3 CharacterWorldPos = CharacterWorldTM.GetTranslation();

	const Vec3 localReferencePos = GetLocalReferencePosition(actorControllerComponent);
	const Vec3 worldReferencePos = CharacterWorldPos + (Quat(CharacterWorldTM) * localReferencePos);

	const bool shouldUpdate =
		((gEnv->pTimer->GetCurrTime() - m_timeWaterLevelLastUpdated) > 0.3f) ||
		((m_lastWaterLevelCheckPosition - CharacterWorldPos).len2() >= sqr(0.35f)) ||
		(m_lastInternalState != m_internalState && m_internalState == eProxyInternalState_PartiallySubmerged); //Just entered partially emerged state

	if (shouldUpdate && !IsWaitingForBottomLevelResults())
	{
		RayTestBottomLevel(actorControllerComponent, worldReferencePos, s_rayLength);

		IPhysicalEntity* piPhysEntity = actorControllerComponent.GetEntity()->GetPhysics();

		UpdateWaterLevel(worldReferencePos, CharacterWorldPos, piPhysEntity);

		if (m_waterLevel > WATER_LEVEL_UNKNOWN)
		{
			m_actorWaterLevel = (worldReferencePos.z - m_waterLevel);
		}
		else
		{
			m_actorWaterLevel = -WATER_LEVEL_UNKNOWN;
			m_bottomLevel = BOTTOM_LEVEL_UNKNOWN;
			m_headUnderwater = false;
			m_headComingOutOfWater = false;
		}
	}

	m_relativeBottomLevel = (m_bottomLevel > BOTTOM_LEVEL_UNKNOWN) ? m_waterLevel - m_bottomLevel : 0.0f;

	const float localHeadZ = actorControllerComponent.GetLocalEyePos().z + 0.2f;
	const float worldHeadZ = CharacterWorldPos.z + localHeadZ;

	const bool headWasUnderWater = m_headUnderwater;
	m_headUnderwater = ((worldHeadZ - m_waterLevel) < 0.0f);

	m_headComingOutOfWater = headWasUnderWater && (!m_headUnderwater);

	if (m_internalState == eProxyInternalState_Swimming)
	{
		m_shouldSwim = ShouldSwim(max(localReferencePos.z, 1.3f));
	}
	else
	{
		UpdateSubmergedFraction(localReferencePos.z, CharacterWorldPos.z, m_waterLevel);

		m_shouldSwim = ShouldSwim(max(localReferencePos.z, 1.3f)) && (m_swimmingTimer < -0.5f);
	}
#ifdef STATE_DEBUG
	DebugDraw(actorControllerComponent, worldReferencePos);
#endif
}


void CActorStateSwimWaterTestProxy::UpdateSubmergedFraction(const float referenceHeight, const float CharacterHeight, const float waterLevel)
{
	const float referenceHeightFinal = max(referenceHeight, 1.3f);
	const float submergedTotal = CharacterHeight - waterLevel;
	const float submergedFraction = (float)__fsel(submergedTotal, 0.0f, clamp_tpl(-submergedTotal * __fres(referenceHeightFinal), 0.0f, 1.0f));

	SetSubmergedFraction(submergedFraction);
}


bool CActorStateSwimWaterTestProxy::ShouldSwim(const float referenceHeight) const
{
	if (m_waterLevel > WATER_LEVEL_UNKNOWN)
	{
		const float bottomDepth = m_waterLevel - m_bottomLevel;

		if (m_internalState == eProxyInternalState_PartiallySubmerged)
		{
			return ((bottomDepth > referenceHeight))
				&& ((m_submergedFraction > 0.4f) || m_headUnderwater)
				&& ((!IsWaitingForBottomLevelResults() && m_bottomLevel != BOTTOM_LEVEL_UNKNOWN) || m_headUnderwater); //Without this check, whilst waiting for the initial ray cast result to find m_bottomLevel this function can incorrectly return true in shallow water
		}
		else
		{
			//In water already
			return (bottomDepth > referenceHeight);
		}
	}

	return false;
}


Vec3 CActorStateSwimWaterTestProxy::GetLocalReferencePosition(const CActorControllerComponent& actorControllerComponent)
{
	/*const float CAMERA_SURFACE_OFFSET = -0.2f;

	Vec3 localReferencePos = ZERO;
	if (actorControllerComponent.IsViewFirstPerson())
	{
	// We get a smoother experience in FP if we work relative to the camera
	localReferencePos = actorControllerComponent.GetFPCameraPosition (false);
	localReferencePos.z += CAMERA_SURFACE_OFFSET;
	}
	else if (actorControllerComponent.HasBoneID (BONE_SPINE3))
	{
	localReferencePos.x = 0.0f;
	localReferencePos.y = 0.0f;
	localReferencePos.z = actorControllerComponent.GetBoneTransform (BONE_SPINE3).t.z;

	#if !defined(_RELEASE)
	bool bLocalRefPosValid = localReferencePos.IsValid ();
	CRY_ASSERT (bLocalRefPosValid);
	if (!bLocalRefPosValid)
	{
	localReferencePos = ZERO;
	}
	#endif
	}

	return localReferencePos;*/

	return Vec3(ZERO);
}


void CActorStateSwimWaterTestProxy::OnRayCastBottomLevelDataReceived(const QueuedRayID& rayID, const RayCastResult& result)
{
	CRY_ASSERT(m_bottomLevelRayID == rayID);

	m_bottomLevelRayID = 0;

	if (result.hitCount > 0)
	{
		m_bottomLevel = m_lastRayCastResult = result.hits [0].pt.z;
	}
	else
	{
		m_bottomLevel = BOTTOM_LEVEL_UNKNOWN;
	}
}


void CActorStateSwimWaterTestProxy::RayTestBottomLevel(const CActorControllerComponent& actorControllerComponent, const Vec3& referencePosition, float maxRelevantDepth)
{
	/*	CRY_PROFILE_FUNCTION(PROFILE_GAME);

		const float terrainWorldZ = gEnv->p3DEngine->GetTerrainElevation (referencePosition.x, referencePosition.y);

		int rayFlags = geom_colltype_Character << rwi_colltype_bit | rwi_stop_at_pierceable;
		int entityFlags = ent_terrain | ent_static | ent_sleeping_rigid | ent_rigid;
		const float padding = 0.2f;

		// NOTE: Terrain is above m_referencePos, so m_referencePos is probably inside a voxel or something.
		const float fPosWorldDiff = referencePosition.z - terrainWorldZ;
		float rayLength = (float) __fsel (fPosWorldDiff, min (maxRelevantDepth, fPosWorldDiff), maxRelevantDepth) + (padding * 2.0f);

		// We should not have entered this function if still waiting for the last result
		CRY_ASSERT (m_bottomLevelRayID == 0);

		m_bottomLevelRayID = CCryAction::GetCryAction()->GetPhysicQueues().GetRayCaster().Queue (
		actorControllerComponent.IsClient () ? RayCastRequest::HighPriority : RayCastRequest::MediumPriority,
		RayCastRequest (referencePosition + Vec3 (0, 0, padding), Vec3 (0, 0, -rayLength),
		entityFlags,
		rayFlags,
		0,
		0),
		functor (*this, &CActorStateSwimWaterTestProxy::OnRayCastBottomLevelDataReceived));*/
}


void CActorStateSwimWaterTestProxy::CancelPendingRays()
{
	/*	if (m_bottomLevelRayID != 0)
		{
			CCryAction::GetCryAction()->GetPhysicQueues().GetRayCaster().Cancel (m_bottomLevelRayID);
		}
		m_bottomLevelRayID = 0;*/
}


void CActorStateSwimWaterTestProxy::UpdateWaterLevel(const Vec3& worldReferencePos, const Vec3& CharacterWorldPos, IPhysicalEntity* piPhysEntity)
{
	m_waterLevel = gEnv->p3DEngine->GetWaterLevel(&worldReferencePos, piPhysEntity);
	m_timeWaterLevelLastUpdated = gEnv->pTimer->GetCurrTime();
	m_lastWaterLevelCheckPosition = CharacterWorldPos;
}


#ifdef STATE_DEBUG
void CActorStateSwimWaterTestProxy::DebugDraw(const CActorControllerComponent& actorControllerComponent, const Vec3& referencePosition)
{
	// DEBUG RENDERING
	/*const SActorStats& stats = *actorControllerComponent.GetActorState ();
	const bool debugSwimming = (g_pGameCVars->cl_debugSwimming != 0);

	if (debugSwimming && (m_actorWaterLevel > -10.0f) && (m_actorWaterLevel < 10.0f))
	{
	const Vec3 surfacePosition (referencePosition.x, referencePosition.y, m_waterLevel);

	const Vec3 vRight (actorControllerComponent.GetBaseQuat ().GetColumn0 ());

	const static ColorF referenceColor (1, 1, 1, 1);
	const static ColorF surfaceColor1 (0, 0.5f, 1, 1);
	const static ColorF surfaceColor0 (0, 0, 0.5f, 0);
	const static ColorF bottomColor (0, 0.5f, 0, 1);

	gEnv->pRenderer->GetIRenderAuxGeom ()->DrawSphere (referencePosition, 0.1f, referenceColor);
	gEnv->pRenderer->GetIRenderAuxGeom ()->DrawLine (referencePosition, surfaceColor1, surfacePosition, surfaceColor1, 2.0f);
	gEnv->pRenderer->GetIRenderAuxGeom ()->DrawSphere (surfacePosition, 0.2f, surfaceColor1);
	gEnv->pRenderer->DrawLabel (referencePosition + vRight * 0.5f, 1.5f, "WaterLevel %3.2f (Head underwater: %d)", m_actorWaterLevel, m_headUnderwater ? 1 : 0);

	const static int lines = 16;
	const static float radius0 = 0.5f;
	const static float radius1 = 1.0f;
	const static float radius2 = 2.0f;

	for (int i = 0; i < lines; ++i)
	{
	float radians = ((float) i / (float) lines) * gf_PI2;
	Vec3 offset0 (radius0 * cos_tpl (radians), radius0 * sin_tpl (radians), 0);
	Vec3 offset1 (radius1 * cos_tpl (radians), radius1 * sin_tpl (radians), 0);
	Vec3 offset2 (radius2 * cos_tpl (radians), radius2 * sin_tpl (radians), 0);
	gEnv->pRenderer->GetIRenderAuxGeom ()->DrawLine (surfacePosition + offset0, surfaceColor0, surfacePosition + offset1, surfaceColor1, 2.0f);
	gEnv->pRenderer->GetIRenderAuxGeom ()->DrawLine (surfacePosition + offset1, surfaceColor1, surfacePosition + offset2, surfaceColor0, 2.0f);
	}

	if (m_bottomLevel > 0.0f)
	{
	const Vec3 bottomPosition (referencePosition.x, referencePosition.y, m_bottomLevel);

	gEnv->pRenderer->GetIRenderAuxGeom ()->DrawLine (referencePosition, bottomColor, bottomPosition, bottomColor, 2.0f);
	gEnv->pRenderer->GetIRenderAuxGeom ()->DrawSphere (bottomPosition, 0.2f, bottomColor);
	gEnv->pRenderer->DrawLabel (bottomPosition + Vec3 (0, 0, 0.5f) - vRight * 0.5f, 1.5f, "BottomDepth %3.3f", m_waterLevel - m_bottomLevel);
	}
	}*/
}
#endif
}