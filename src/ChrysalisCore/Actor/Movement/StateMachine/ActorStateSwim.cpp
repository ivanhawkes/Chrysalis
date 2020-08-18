#include <StdAfx.h>

#include "ActorStateSwim.h"
#include <Components/Actor/ActorControllerComponent.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include "ActorStateEvents.h"
#include "ActorStateJump.h"
/*#include "Weapon.h"
#include "GameRules.h"
#include "StatsRecordingMgr.h"
#include "MovementAction.h"
#include "CharacterCamera.h"*/


namespace Chrysalis
{
static const int s_maxWaterVolumesToConsider = 2;
CActorStateSwim::CSwimmingParams CActorStateSwim::s_swimParams;


void CActorStateSwim::CSwimmingParams::SetParamsFromXml(const IItemParamsNode* pParams)
{
	/*const IItemParamsNode* pSwimParams = pParams->GetChild ("SwimmingParams");

	if (pSwimParams)
	{
	const IItemParamsNode* pSprintNode = pSwimParams->GetChild ("Sprint");
	if (pSprintNode)
	{
	pSprintNode->GetAttribute ("swimSpeedSprintSpeedMul", m_swimSpeedSprintSpeedMul);
	pSprintNode->GetAttribute ("swimUpSprintSpeedMul", m_swimUpSprintSpeedMul);
	pSprintNode->GetAttribute ("swimSprintSpeedMul", m_swimSprintSpeedMul);
	pSprintNode->GetAttribute ("stateSwim_animCameraFactor", m_stateSwim_animCameraFactor);
	}
	const IItemParamsNode* pDolphinNode = pSwimParams->GetChild ("Dolphin");
	if (pDolphinNode)
	{
	pDolphinNode->GetAttribute ("swimDolphinJumpDepth", m_swimDolphinJumpDepth);
	pDolphinNode->GetAttribute ("swimDolphinJumpThresholdSpeed", m_swimDolphinJumpThresholdSpeed);
	pDolphinNode->GetAttribute ("swimDolphinJumpSpeedModification", m_swimDolphinJumpSpeedModification);
	}
	}*/
}


CActorStateSwim::CActorStateSwim()
	:
	m_gravity(ZERO),
	m_headUnderWaterTimer(-1.f),
	m_lastWaterLevel(0.0f),
	m_lastWaterLevelTime(0.0f),
	m_verticalVelDueToSurfaceMovement(0.0f),
	m_onSurface(false),
	m_enduranceSwimSoundPlaying(false)
{}


bool CActorStateSwim::OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent, float frameTime)
{
	/*	const CActorStateSwimWaterTestProxy& waterProxy = actorControllerComponent.m_stateSwimWaterTestProxy;

		CActorStateUtility::PhySetFly (Character);

		const SActorStats& stats = actorControllerComponent.m_actorState;

		#ifdef STATE_DEBUG
		const bool debug = (g_pGameCVars->cl_debugSwimming != 0);
		#endif

		const Vec3 entityPos = actorControllerComponent.GetEntity ()->GetWorldPos ();
		const Quat baseQuat = actorControllerComponent.GetBaseQuat ();
		const Vec3 vRight (baseQuat.GetColumn0 ());

		Vec3 velocity = actorControllerComponent.GetActorPhysics()->velocity;

		// Underwater timer, sounds update and surface wave speed.
		if (waterProxy.IsHeadUnderWater ())
		{
		m_headUnderWaterTimer += frameTime;
		if (m_headUnderWaterTimer <= -0.0f && !m_onSurface)
		{
		actorControllerComponent.PlaySound (CActorControllerComponent::ESound_DiveIn, true, "speed", velocity.len ());
		m_headUnderWaterTimer = 0.0f;
		}

		actorControllerComponent.PlaySound (CActorControllerComponent::ESound_Underwater, true);
		}
		else
		{
		m_headUnderWaterTimer -= frameTime;
		if (m_headUnderWaterTimer >= 0.0f && (waterProxy.IsHeadComingOutOfWater () || m_onSurface))
		{
		actorControllerComponent.PlaySound (CActorControllerComponent::ESound_DiveOut, true, "speed", velocity.len ());
		m_headUnderWaterTimer = 0.0f;
		}

		actorControllerComponent.PlaySound (CActorControllerComponent::ESound_Underwater, false);
		}
		m_headUnderWaterTimer = clamp_tpl (m_headUnderWaterTimer, -0.2f, 0.2f);

		// Apply water flow velocity to the Character
		Vec3 gravity;
		pe_params_buoyancy buoyancy [s_maxWaterVolumesToConsider];
		if (int count = gEnv->pPhysicalWorld->CheckAreas (entityPos, gravity, &buoyancy [0], s_maxWaterVolumesToConsider))
		{
		for (int i = 0; i < count && i < s_maxWaterVolumesToConsider; ++i)
		{
		// 0 is water
		if (buoyancy [i].iMedium == 0)
		{
		velocity += (buoyancy [i].waterFlow * frameTime);
		}
		}
		}

		// Calculate desired acceleration (user input).
		Vec3 desiredWorldVelocity (ZERO);

		Vec3 acceleration (ZERO);
		{
		Vec3 desiredLocalNormalizedVelocity (ZERO);
		Vec3 desiredLocalVelocity (ZERO);

		const Quat viewQuat = actorControllerComponent.GetViewQuat ();

		const float backwardMultiplier = (float) __fsel (actorControllerComponent.GetVelocity().y, 1.0f, g_pGameCVars->pl_swimBackSpeedMul);
		desiredLocalNormalizedVelocity.x = actorControllerComponent.GetVelocity().x * g_pGameCVars->pl_swimSideSpeedMul;
		desiredLocalNormalizedVelocity.y = actorControllerComponent.GetVelocity().y * backwardMultiplier;

		float sprintMultiplier = 1.0f;
		if ((actorControllerComponent.IsSprinting ()) && !actorControllerComponent.IsCinematicFlagActive (SActorStats::eCinematicFlag_RestrictMovement))
		{
		sprintMultiplier = GetSwimParams ().m_swimSprintSpeedMul;

		// Higher speed multiplier when sprinting while looking up.
		const float upFraction = clamp_tpl (viewQuat.GetFwdZ (), 0.0f, 1.0f);
		sprintMultiplier *= LERP (1.0f, GetSwimParams ().m_swimUpSprintSpeedMul, upFraction);
		}

		const float baseSpeed = actorControllerComponent.GetStanceMaxSpeed (STANCE_SWIM);
		desiredLocalVelocity.x = desiredLocalNormalizedVelocity.x * sprintMultiplier * baseSpeed;
		desiredLocalVelocity.y = desiredLocalNormalizedVelocity.y * sprintMultiplier * baseSpeed;
		desiredLocalVelocity.z = desiredLocalNormalizedVelocity.z * g_pGameCVars->pl_swimVertSpeedMul * baseSpeed;

		// The desired movement is applied in view-space, not in entity-space, since entity does not necessarily pitch while swimming.
		desiredWorldVelocity += viewQuat.GetColumn0 () * desiredLocalVelocity.x;
		desiredWorldVelocity += viewQuat.GetColumn1 () * desiredLocalVelocity.y;

		// Apply up/down in world space.
		desiredWorldVelocity.z += desiredLocalVelocity.z;

		#ifdef STATE_DEBUG
		if (debug)
		{
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f + Vec3 (0, 0, 0.8f), 1.5f, "BaseSpeed %1.3f", baseSpeed);
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f + Vec3 (0, 0, 1.0f), 1.5f, "SprintMul %1.2f", sprintMultiplier);
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f + Vec3 (0, 0, 0.6f), 1.5f, "MoveN[%1.3f, %1.3f, %1.3f]", desiredLocalNormalizedVelocity.x, desiredLocalNormalizedVelocity.y, desiredLocalNormalizedVelocity.z);
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f + Vec3 (0, 0, 0.5f), 1.5f, "VeloL[%1.3f, %1.3f, %1.3f]", desiredLocalVelocity.x, desiredLocalVelocity.y, desiredLocalVelocity.z);
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f + Vec3 (0, 0, 0.4f), 1.5f, "VeloW[%1.3f, %1.3f, %1.3f]", desiredWorldVelocity.x, desiredWorldVelocity.y, desiredWorldVelocity.z);
		}
		#endif

		// Remove a bit of control when entering the water.
		const float userControlFraction = (float) __fsel (0.3f - waterProxy.GetSwimmingTimer (), 0.2f, 1.0f);
		acceleration += desiredWorldVelocity * userControlFraction;
		}

		// Apply acceleration (frame-rate independent)
		const float accelerateDelayInv = 3.333f;
		velocity += acceleration * (frameTime * accelerateDelayInv);

		#ifdef STATE_DEBUG
		if (debug)
		{
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f - Vec3 (0, 0, 0.2f), 1.5f, " Axx[%1.3f, %1.3f, %1.3f]", acceleration.x, acceleration.y, acceleration.z);
		}
		#endif

		const float relativeWaterLevel = waterProxy.GetRelativeWaterLevel () + 0.1f;
		const float surfaceDistanceFraction = clamp_tpl (fabsf (relativeWaterLevel), 0.0f, 1.0f);
		float surfaceProximityInfluence = 1.0f - surfaceDistanceFraction;
		const float verticalVelocityFraction = clamp_tpl ((fabsf (desiredWorldVelocity.z) - 0.3f) * 2.5f, 0.0f, 1.0f);
		surfaceProximityInfluence = surfaceProximityInfluence * (1.0f - verticalVelocityFraction);

		// Apply velocity dampening (frame-rate independent)
		Vec3 damping (ZERO);

		const float zSpeedPreDamping = velocity.z;

		{
		damping.x = fabsf (velocity.x);
		damping.y = fabsf (velocity.y);

		// Vertical damping is special, to allow jumping out of water with higher speed,
		// and also not sink too deep when falling down ito the water after jump or such.
		float zDamp = 1.0f + (6.0f * clamp_tpl ((-velocity.z - 1.0f) * 0.333f, 0.0f, 1.0f));
		zDamp *= 1.0f - surfaceProximityInfluence;

		damping.z = fabsf (velocity.z) * zDamp;

		const float stopDelayInv = 3.333f;
		damping *= (frameTime * stopDelayInv);
		velocity.x = (float) __fsel ((fabsf (velocity.x) - damping.x), (velocity.x - fsgnf (velocity.x) * damping.x), 0.0f);
		velocity.y = (float) __fsel ((fabsf (velocity.y) - damping.y), (velocity.y - fsgnf (velocity.y) * damping.y), 0.0f);
		velocity.z = (float) __fsel ((fabsf (velocity.z) - damping.z), (velocity.z - fsgnf (velocity.z) * damping.z), 0.0f);

		// Make sure you can not swim above the surface.
		if ((relativeWaterLevel >= 0.0f) && (velocity.z > 0.0f))
		{
		velocity.z = 0.0f;
		}
		}

		// Decide if we're on the surface and therefore need to be kept there.
		if (relativeWaterLevel > -0.2f && relativeWaterLevel < 1.0f && fabs_tpl (zSpeedPreDamping) < 0.5f)
		{
		if (!waterProxy.IsHeadUnderWater ())
		{
		m_onSurface = true;
		}
		}
		else
		{
		// we only leave the surface if the Character moves, otherwise we try and keep the
		// Character on the surface, even if they currently arent
		m_onSurface = false;
		}

		// Calculate and apply surface movement to the actorControllerComponent.
		float speedDelta = 0.0f;
		if (m_onSurface)
		{
		const float newWaterLevel = waterProxy.GetWaterLevel ();
		const float waterLevelDelta = clamp_tpl (newWaterLevel - m_lastWaterLevel, -1.0f, 1.0f);
		const float newCheckedTime = waterProxy.GetWaterLevelTimeUpdated ();
		const float timeDelta = newCheckedTime - m_lastWaterLevelTime;

		if (timeDelta > FLT_EPSILON)
		{
		speedDelta = waterLevelDelta / timeDelta;

		velocity.z += speedDelta;
		}

		m_lastWaterLevel = newWaterLevel;
		m_lastWaterLevelTime = newCheckedTime;
		}

		// Set request type and velocity
		actorControllerComponent.GetMoveRequest ().type = eCMT_Fly;
		actorControllerComponent.GetMoveRequest ().velocity = velocity;

		#ifdef STATE_DEBUG
		// DEBUG VELOCITY
		if (debug)
		{
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f - Vec3 (0, 0, 0.0f), 1.5f, "Velo[%1.3f, %1.3f, %1.3f]", velocity.x, velocity.y, velocity.z);
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f - Vec3 (0, 0, 0.4f), 1.5f, "Damp[%1.3f, %1.3f, %1.3f]", damping.x, damping.y, damping.z);
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f - Vec3 (0, 0, 0.6f), 1.5f, "FrameTime %1.4f", frameTime);
		gEnv->pRenderer->DrawLabel (entityPos - vRight * 1.5f + Vec3 (0, 0, 0.3f), 1.5f, "DeltaSpeed[%1.3f]", speedDelta);
		}
		#endif

		return true;*/

	return true;
}


void CActorStateSwim::OnEnter(CActorControllerComponent& actorControllerComponent)
{
	/*actorControllerComponent.m_stateSwimWaterTestProxy.OnEnterWater (Character);

	IPhysicalEntity* pPhysEnt = actorControllerComponent.GetEntity ()->GetPhysics ();
	if (pPhysEnt != nullptr)
	{
	// get current gravity before setting to zero.
	pe_player_dynamics simPar;
	if (pPhysEnt->GetParams (&simPar) != 0)
	{
	m_gravity = simPar.gravity;
	}
	CActorStateUtility::PhySetFly (Character);
	}

	m_lastWaterLevel = actorControllerComponent.m_stateSwimWaterTestProxy.GetWaterLevel ();
	m_lastWaterLevelTime = actorControllerComponent.m_stateSwimWaterTestProxy.GetWaterLevelTimeUpdated ();

	actorControllerComponent.durationInAir = 0.0f;

	if (actorControllerComponent.IsClient ())
	{
	ICameraMode::AnimationSettings animationSettings;
	animationSettings.positionFactor = 1.0f;
	animationSettings.rotationFactor = GetSwimParams ().m_stateSwim_animCameraFactor;

	actorControllerComponent.GetCharacterCamera ()->SetCameraModeWithAnimationBlendFactors (eCameraMode_PartialAnimationControlled, animationSettings, "Entering swim state");
	actorControllerComponent.SendMusicLogicEvent (eMUSICLOGICEVENT_CHARACTER_SWIM_ENTER);

	if (!actorControllerComponent.IsCinematicFlagActive (SActorStats::eCinematicFlag_HolsterWeapon))
	actorControllerComponent.HolsterItem (true);

	if (gEnv->bMultiCharacter)	// any left hand holding in SP?
	{
	actorControllerComponent.HideLeftHandObject (true);
	}
	}

	// Record 'Swim' telemetry stats.
	CStatsRecordingMgr::TryTrackEvent (&Character, eGSE_Swim, true);*/
}


void CActorStateSwim::OnExit(CActorControllerComponent& actorControllerComponent)
{
	/*actorControllerComponent.m_stateSwimWaterTestProxy.OnExitWater (Character);
	actorControllerComponent.m_actorPhysics.groundNormal = Vec3 (0, 0, 1);

	if (actorControllerComponent.IsClient ())
	{
	actorControllerComponent.GetCharacterCamera ()->SetCameraMode (eCameraMode_Default, "Leaving swim state");

	// Unselect underwater weapon here.
	actorControllerComponent.SendMusicLogicEvent (eMUSICLOGICEVENT_CHARACTER_SWIM_LEAVE);
	if (!actorControllerComponent.IsCinematicFlagActive (SActorStats::eCinematicFlag_HolsterWeapon))
	{
	actorControllerComponent.HolsterItem (false);
	}

	// Any left hand holding in SP?
	if (gEnv->bMultiCharacter)
	{
	actorControllerComponent.HideLeftHandObject (false);
	}

	actorControllerComponent.PlaySound (CActorControllerComponent::ESound_Underwater, false);

	UpdateSoundListener (Character);

	StopEnduranceSound (actorControllerComponent.GetEntityId ());
	}

	IPhysicalEntity* pPhysEnt = actorControllerComponent.GetEntity ()->GetPhysics ();
	if (pPhysEnt != nullptr)
	{
	CActorStateUtility::PhySetNoFly (Character, m_gravity);
	}

	// Record 'Swim' telemetry stats.

	CStatsRecordingMgr::TryTrackEvent (&Character, eGSE_Swim, false);*/
}


void CActorStateSwim::OnUpdate(CActorControllerComponent& actorControllerComponent, float frameTime)
{
	// Swim animation not playing after cut-scene due to "NoWeapon" not being equipped as it hasn't been loaded into the inventory yet.
	// This function is called until "NoWeapon" is equipped.
	/*if (actorControllerComponent.IsClient ())
	{
	const float breathingInterval = 5.0f;
	actorControllerComponent.m_pCharacterTypeComponent->UpdateSwimming (frameTime, breathingInterval);
	}*/
}


bool CActorStateSwim::DetectJump(CActorControllerComponent& actorControllerComponent, float frameTime, float* pVerticalSpeedModifier) const
{
	/*const float minInWaterTime = 0.35f;
	const CActorStateSwimWaterTestProxy& waterProxy = actorControllerComponent.m_stateSwimWaterTestProxy;
	const bool allowJump = waterProxy.GetSwimmingTimer () > minInWaterTime;

	if (allowJump)
	{
	// we broke the surface at a velocity enough to dolphin jump.
	if (!m_onSurface && (waterProxy.GetRelativeWaterLevel () > -GetSwimParams ().m_swimDolphinJumpDepth))
	{
	const float velZ = actorControllerComponent.GetActorPhysics()->velocity.z;

	if ((velZ > GetSwimParams ().m_swimDolphinJumpThresholdSpeed))
	{
	if (pVerticalSpeedModifier)
	{
	*pVerticalSpeedModifier = GetSwimParams ().m_swimDolphinJumpSpeedModification;
	}

	return true;
	}
	}
	}

	return false;*/
	return false;
}


void CActorStateSwim::UpdateSoundListener(CActorControllerComponent& actorControllerComponent)
{
	REINST("needs verification! check SDK version of the code later on...ILH");
}


void CActorStateSwim::PlayEnduranceSound(const EntityId ownerId)
{
	if (m_enduranceSwimSoundPlaying)
		return;

	m_enduranceSwimSoundPlaying = true;
}


void CActorStateSwim::StopEnduranceSound(const EntityId ownerId)
{
	if (!m_enduranceSwimSoundPlaying)
		return;

	m_enduranceSwimSoundPlaying = false;
}
}