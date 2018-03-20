#include <StdAfx.h>

#include "ActorStateJump.h"
#include <IVehicleSystem.h>
#include <Actor/ActorControllerComponent.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
/*#include "CharacterInput.h"
#include "ScreenEffects.h"
#include "StatsRecordingMgr.h"
#include "GameRules.h"
#include "GameCodeCoverage/GameCodeCoverageTracker.h"
#include "Weapon.h"
#include "UI/HUD/HUDEventWrapper.h"
#include "NetInputChainDebug.h"
#include "MovementAction.h"
#include "PersistantStats.h"*/


namespace Chrysalis
{
CActorStateJump::CActorStateJump()
	:
	m_jumpState(JState_None),
	m_jumpLock(0.0f),
	m_startFallingHeight(-FLT_MAX),
	m_firstPrePhysicsUpdate(false),
	m_jumpAction(NULL),
	m_expectedJumpEndHeight(0.0f)
{}


CActorStateJump::~CActorStateJump()
{}


void CActorStateJump::OnEnter(CActorControllerComponent& actorControllerComponent)
{
	CryLogAlways("Entered state jump");

	m_startFallingHeight = -FLT_MAX;
	m_jumpState = JState_None;
	m_jumpLock = 0.0f;
	m_expectedJumpEndHeight = 0.0f;
	m_firstPrePhysicsUpdate = true;
}


void CActorStateJump::OnJump(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, const float fVerticalSpeedModifier)
{
	/*actorControllerComponent.GetMoveRequest ().type = eCMT_JumpInstant;
	actorControllerComponent.GetMoveRequest ().jumping = true;

	StartJump (actorControllerComponent, isHeavyWeapon, fVerticalSpeedModifier);

	NETINPUT_TRACE (actorControllerComponent.GetEntityId (), (m_jumpState == JState_Jump));*/
}



void CActorStateJump::OnFall(CActorControllerComponent& actorControllerComponent)
{
	SetJumpState(actorControllerComponent, JState_Falling);
}


void CActorStateJump::StartJump(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, const float fVerticalSpeedModifier)
{
	/*	const SActorPhysics& actorPhysics = actorControllerComponent.GetActorPhysics ();
		const SActorStats& stats = *actorControllerComponent.GetActorState ();
		const float onGroundTime = 0.2f;

		float g = actorPhysics.gravity.len ();

		const float jumpHeightScale = 1.0f;
		const float jumpHeight = actorControllerComponent.GetActorParams ().jumpHeight * jumpHeightScale;

		float CharacterZ = actorControllerComponent.GetEntity ()->GetWorldPos ().z;
		float expectedJumpEndHeight = CharacterZ + jumpHeight;

		pe_CHARACTER_dimensions dimensions;
		IPhysicalEntity *pPhysics = actorControllerComponent.GetEntity ()->GetPhysics ();
		if (pPhysics && pPhysics->GetParams (&dimensions))
		{
		float physicsBottom = dimensions.heightCollider - dimensions.sizeCollider.z;
		if (dimensions.bUseCapsule)
		{
		physicsBottom -= dimensions.sizeCollider.x;
		}
		expectedJumpEndHeight += physicsBottom;
		}

		float jumpSpeed = 0.0f;

		if (g > 0.0f)
		{
		jumpSpeed = sqrt_tpl (2.0f*jumpHeight*(1.0f / g)) * g;

		if (isHeavyWeapon)
		{
		jumpSpeed *= g_pGameCVars->pl_movement.nonCombat_heavy_weapon_speed_scale;
		}
		}

		// This is used to easily find steep ground.
		float slopeDelta = (Vec3Constants<float>::fVec3_OneZ - actorPhysics.groundNormal).len ();

		SetJumpState (actorControllerComponent, JState_Jump);

		Vec3 jumpVec (ZERO);

		bool bNormalJump = true;

		actorControllerComponent.PlaySound (CActorControllerComponent::ESound_Jump);

		OnSpecialMove (actorControllerComponent, IActorEventListener::eSM_Jump);

		CCCPOINT_IF (actorControllerComponent.IsClient (), CharacterMovement_LocalCharacterNormalJump);
		CCCPOINT_IF (!actorControllerComponent.IsClient (), CharacterMovement_NonLocalCharacterNormalJump);

		{
		// This was causing the vertical jumping speed to be much slower.
		float verticalMult = max (1.0f - m_jumpLock, 0.3f);

		const Quat baseQuat = actorControllerComponent.GetBaseQuat ();
		jumpVec += baseQuat.GetColumn2 () * jumpSpeed * verticalMult;
		jumpVec.z += fVerticalSpeedModifier;

		#ifdef STATE_DEBUG
		if (g_pGameCVars->pl_debugInterpolation > 1)
		{
		CryWatch ("Jumping: vec from actorControllerComponent BaseQuat only = (%f, %f, %f)", jumpVec.x, jumpVec.y, jumpVec.z);
		}
		#endif

		if (g_pGameCVars->pl_adjustJumpAngleWithFloorNormal && actorPhysics.groundNormal.len2 () > 0.0f)
		{
		float vertical = clamp_tpl ((actorPhysics.groundNormal.z - 0.25f) / 0.5f, 0.0f, 1.0f);
		Vec3 modifiedJumpDirection = LERP (actorPhysics.groundNormal, Vec3 (0, 0, 1), vertical);
		jumpVec = modifiedJumpDirection * jumpVec.len ();
		}

		#ifdef STATE_DEBUG
		if (g_pGameCVars->pl_debugInterpolation > 1)
		{
		CryWatch ("Jumping (%f, %f, %f)", jumpVec.x, jumpVec.y, jumpVec.z);
		}
		#endif
		}

		NETINPUT_TRACE (actorControllerComponent.GetEntityId (), jumpVec);

		FinalizeVelocity (actorControllerComponent, jumpVec);

		if (!actorControllerComponent.IsRemote ())
		{
		actorControllerComponent.HasJumped (actorControllerComponent.GetMoveRequest ().velocity);
		}

		IPhysicalEntity* pPhysEnt = actorControllerComponent.GetEntity ()->GetPhysics ();
		if (pPhysEnt != NULL)
		{
		SAnimatedCharacterParams params = actorControllerComponent.m_pAnimatedCharacter->GetParams ();
		pe_player_dynamics pd;
		pd.kAirControl = actorControllerComponent.GetAirControl ()* g_pGameCVars->pl_jump_control.air_control_scale;
		pd.kAirResistance = actorControllerComponent.GetAirResistance () * g_pGameCVars->pl_jump_control.air_resistance_scale;

		params.inertia = actorControllerComponent.GetInertia () * g_pGameCVars->pl_jump_control.air_inertia_scale;

		if (actorControllerComponent.IsRemote () && (g_pGameCVars->pl_velocityInterpAirControlScale > 0))
		{
		pd.kAirControl = g_pGameCVars->pl_velocityInterpAirControlScale;
		}

		pPhysEnt->SetParams (&pd);

		// Let Animated actorControllerComponent handle the inertia
		actorControllerComponent.SetAnimatedCharacterParams (params);
		}

		m_expectedJumpEndHeight = expectedJumpEndHeight;
		m_bSprintJump = actorControllerComponent.IsSprinting ();*/
}


bool CActorStateJump::OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime)
{
	if (!m_firstPrePhysicsUpdate)
	{
		switch (m_jumpState)
		{
			case JState_Jump:
				UpdateJumping(actorControllerComponent, isHeavyWeapon, frameTime);
				break;

			case JState_Falling:
				UpdateFalling(actorControllerComponent, isHeavyWeapon, frameTime);
				break;
		}
	}

	m_firstPrePhysicsUpdate = false;

	//NETINPUT_TRACE (actorControllerComponent.GetEntityId (), (m_jumpState == JState_Jump));

	return(m_jumpState == JState_None);
}


void CActorStateJump::OnExit(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon)
{
	/*	if (m_jumpAction)
		{
		m_jumpAction->TriggerExit ();
		m_jumpAction->Release ();
		m_jumpAction = NULL;
		}

		IPhysicalEntity* pPhysEnt = actorControllerComponent.GetEntity ()->GetPhysics ();
		if (pPhysEnt != NULL)
		{
		SAnimatedCharacterParams params = actorControllerComponent.m_pAnimatedCharacter->GetParams ();
		pe_player_dynamics pd;
		pd.kAirControl = actorControllerComponent.GetAirControl ();
		pd.kAirResistance = actorControllerComponent.GetAirResistance ();
		params.inertia = actorControllerComponent.GetInertia ();

		if (actorControllerComponent.IsRemote () && (g_pGameCVars->pl_velocityInterpAirControlScale > 0))
		{
		pd.kAirControl = g_pGameCVars->pl_velocityInterpAirControlScale;
		}

		pPhysEnt->SetParams (&pd);

		// Let Animated actorControllerComponent handle the inertia
		actorControllerComponent.SetAnimatedCharacterParams (params);
		}*/

	SetJumpState(actorControllerComponent, JState_None);
}


void CActorStateJump::OnSpecialMove(CActorControllerComponent& actorControllerComponent, IActorEventListener::ESpecialMove specialMove)
{
	/*	if (!actorControllerComponent.m_CharacterEventListeners.empty())
		{
			CActorControllerComponent::TCharacterEventListeners::const_iterator iter = actorControllerComponent.m_CharacterEventListeners.begin();
			CActorControllerComponent::TCharacterEventListeners::const_iterator cur;
			while (iter != actorControllerComponent.m_CharacterEventListeners.end())
			{
				cur = iter;
				++iter;
				(*cur)->OnSpecialMove(&actorControllerComponent, specialMove);
			}
		}*/
}


void CActorStateJump::OnFullSerialize(TSerialize ser, CActorControllerComponent& actorControllerComponent)
{
	ser.BeginGroup("StateJump");
	ser.Value("JumpLock", m_jumpLock);
	if (ser.IsReading())
	{
		EJumpState state = JState_None;
		ser.EnumValue("jumpState", state, JState_None, EJumpState::_LAST);
		SetJumpState(actorControllerComponent, state);
	}
	else
	{
		ser.EnumValue("jumpState", m_jumpState, JState_None, EJumpState::_LAST);
	}
	ser.EndGroup();
}


void CActorStateJump::SetJumpState(CActorControllerComponent& actorControllerComponent, EJumpState jumpState)
{
	//if (jumpState != m_jumpState)
	//{
	//	CRY_ASSERT(m_jumpState >= JState_None && m_jumpState < EJumpState::_LAST);
	//	const EJumpState previousJumpState = m_jumpState;

	//	m_jumpState = jumpState;

	//	if (!m_jumpAction && !actorControllerComponent.IsAIControlled())
	//	{
	//		FragmentID fragID = FRAGMENT_ID_INVALID;
	//		switch (jumpState)
	//		{
	//			case JState_Jump:
	//				fragID = CharacterMannequin.fragmentIDs.MotionJump;
	//				break;

	//			case JState_Falling:
	//				fragID = CharacterMannequin.fragmentIDs.MotionInAir;
	//				break;
	//		}

	//		if (fragID != FRAGMENT_ID_INVALID)
	//		{
	//			m_jumpAction = new CCharacterJump(fragID, EActorActionPriority::PP_CharacterAction);
	//			m_jumpAction->AddRef();
	//			actorControllerComponent.GetActor()->QueueAction(*m_jumpAction);
	//		}
	//	}
	//}
}


CActorStateJump::EJumpState CActorStateJump::GetJumpState() const
{
	return m_jumpState;
}


void CActorStateJump::Landed(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float fallSpeed)
{
	//#ifdef STATE_DEBUG
	//	bool remoteControlled = false;
	//	IVehicle* pVehicle = actorControllerComponent.GetLinkedVehicle();
	//	if (pVehicle)
	//	{
	//		IVehicleSeat* pVehicleSeat = pVehicle->GetSeatForPassenger(actorControllerComponent.GetEntityId());
	//		if (pVehicleSeat && pVehicleSeat->IsRemoteControlled())
	//		{
	//			remoteControlled = true;
	//		}
	//	}
	//	CRY_ASSERT_MESSAGE(actorControllerComponent.GetLinkedEntity() == NULL || remoteControlled, "Cannot 'land' when you're linked to another entity!");
	//#endif

		/*const SActorStats& stats = actorControllerComponent.m_actorState;

		Vec3 CharacterPosition = actorControllerComponent.GetEntity ()->GetWorldPos ();
		IPhysicalEntity *phys = actorControllerComponent.GetEntity ()->GetPhysics ();
		IMaterialEffects *mfx = gEnv->pGameFramework->GetIMaterialEffects ();

		const SActorPhysics& actorPhysics = actorControllerComponent.GetActorPhysics ();
		int matID = actorPhysics.groundMaterialIdx != -1 ? actorPhysics.groundMaterialIdx : mfx->GetDefaultSurfaceIndex ();

		const float fHeightofEntity = CharacterPosition.z;
		const float worldWaterLevel = actorControllerComponent.m_stateSwimWaterTestProxy.GetWaterLevel ();

		TMFXEffectId effectId = mfx->GetEffectId ("bodyfall", matID);
		if (effectId != InvalidEffectId)
		{
		SMFXRunTimeEffectParams params;
		Vec3 direction = Vec3 (0, 0, 0);
		if (IMovementController *pMV = actorControllerComponent.GetMovementController ())
		{
		SMovementState state;
		pMV->GetMovementState (state);
		direction = state.aimDirection;
		}
		params.pos = CharacterPosition + direction;

		float landFallParamVal = (float) __fsel (-(fallSpeed - 7.5f), 0.25f, 0.75f);
		params.AddSoundParam ("landfall", landFallParamVal);

		const float speedParamVal = min (fabsf ((actorPhysics.velocity.z * 0.1f)), 1.0f);
		params.AddSoundParam ("speed", speedParamVal);

		params.soundScale = 1.0f;

		mfx->ExecuteEffect (effectId, params);
		}

		bool heavyLanded = false;

		IItem* pCurrentItem = actorControllerComponent.GetCurrentItem ();
		CWeapon* pCurrentWeapon = pCurrentItem ? static_cast<CWeapon*>(pCurrentItem->GetIWeapon ()) : NULL;

		if (fallSpeed > 0.0f && actorControllerComponent.IsCharacter ())
		{
		if (!gEnv->bMultiCharacter)
		{
		const float verticalSpeed = fabs (fallSpeed);
		const float speedForHeavyLand = g_pGameCVars->pl_health.fallSpeed_HeavyLand;
		if ((verticalSpeed >= speedForHeavyLand) && (actorControllerComponent.GetPickAndThrowEntity () == 0) && !actorControllerComponent.IsDead ())
		{
		if (!isHeavyWeapon)
		{
		if (pCurrentWeapon)
		{
		pCurrentWeapon->FumbleGrenade ();
		pCurrentWeapon->CancelCharge ();
		}

		actorControllerComponent.StartInteractiveActionByName ("HeavyLand", false);
		}
		heavyLanded = true;
		}
		}
		}

		if (actorControllerComponent.m_isClient)
		{
		if (fallSpeed > 0.0f)
		{
		const float fallIntensityMultiplier = stats.wasHit ? g_pGameCVars->pl_fall_intensity_hit_multiplier : g_pGameCVars->pl_fall_intensity_multiplier;
		const float fallIntensityMax = g_pGameCVars->pl_fall_intensity_max;
		const float fallTimeMultiplier = g_pGameCVars->pl_fall_time_multiplier;
		const float fallTimeMax = g_pGameCVars->pl_fall_time_max;
		const float zoomMultiCharacter = (pCurrentWeapon && pCurrentWeapon->IsZoomed ()) ? 0.2f : 1.0f;
		const float direction = ((cry_rand () % 2) == 0) ? -1.0f : 1.0f;
		const float intensity = clamp_tpl (fallIntensityMultiplier*fallSpeed*zoomMultiCharacter, 0.0f, fallIntensityMax);
		const float shakeTime = clamp_tpl (fallTimeMultiplier*fallSpeed*zoomMultiCharacter, 0.0f, fallTimeMax);
		const Vec3 rotation = Vec3 (-0.5f, 0.15f*direction, 0.05f*direction);

		if (CScreenEffects* pGameScreenEffects = gEnv->pGameFramework->GetScreenEffects ())
		{
		pGameScreenEffects->CamShake (rotation*intensity, Vec3 (0, 0, 0), shakeTime, shakeTime, 0.05f, CScreenEffects::eCS_GID_Character);
		}

		IForceFeedbackSystem* pForceFeedback = gEnv->pGameFramework->GetIForceFeedbackSystem ();
		CRY_ASSERT (pForceFeedback);

		ForceFeedbackFxId fxId = pForceFeedback->GetEffectIdByName ("landFF");
		pForceFeedback->PlayForceFeedbackEffect (fxId, SForceFeedbackRuntimeParams (intensity, 0.0f));

		if (fallSpeed > 7.0f)
		{
		actorControllerComponent.PlaySound (CActorControllerComponent::ESound_Fall_Drop);
		}

		CActorControllerComponent::ECharacterSounds CharacterSound = heavyLanded ? CActorControllerComponent::ESound_Gear_HeavyLand : CActorControllerComponent::ESound_Gear_Land;
		actorControllerComponent.PlaySound (CharacterSound, true);
		}
		CCCPOINT (CharacterMovement_LocalCharacterLanded);
		}

		if (gEnv->pAISystem)
		{
		// Notify AI
		//If silent feet active, ignore here
		const float noiseSupression = 0.0f;
		const float fAISoundRadius = (g_pGameCVars->ai_perception.landed_baseRadius + (g_pGameCVars->ai_perception.landed_speedMultiplier * fallSpeed)) * (1.0f - noiseSupression);
		SAIStimulus stim (AISTIM_SOUND, AISOUND_MOVEMENT_LOUD, actorControllerComponent.GetEntityId (), 0,
		actorControllerComponent.GetEntity ()->GetWorldPos () + actorControllerComponent.GetEyeOffset (), ZERO, fAISoundRadius);
		gEnv->pAISystem->RegisterStimulus (stim);
		}

		// Record 'Land' telemetry stats.
		CStatsRecordingMgr::TryTrackEvent (&actorControllerComponent, eGSE_Land, fallSpeed);

		if (fallSpeed > 0.0f)
		{
		//actorControllerComponent.CreateScriptEvent (heavyLanded ? "heavylanded" : "landed", stats.fallSpeed);
		}*/
}


const Vec3 CActorStateJump::CalculateInAirJumpExtraVelocity(const CActorControllerComponent& actorControllerComponent, const Vec3& desiredVelocity) const
{
	/*const SActorStats& stats = actorControllerComponent.m_actorState;
	const float speedUpFactor = 0.175f;

	Vec3 jumpExtraVelocity (0.0f, 0.0f, 0.0f);

	const SActorPhysics& actorPhysics = actorControllerComponent.GetActorPhysics ();
	if (actorPhysics.velocity.z > 0.0f)
	{
	//Note: Desired velocity is flat (not 'z' component), so jumpHeight should not be altered
	jumpExtraVelocity = desiredVelocity * speedUpFactor;
	}
	else
	{
	// Note: this makes the jump feel less 'floaty', by accelerating the actorControllerComponent slightly down and compensates
	// the extra travelled distance when going up.
	const float g = actorPhysics.gravity.len ();
	if (g > 0.0f)
	{
	const float jumpHeightScale = 1.0f;
	const float jumpHeight = actorControllerComponent.m_params.jumpHeight * jumpHeightScale;

	const float estimatedLandTime = sqrt_tpl (2.0f*jumpHeight*(1.0f / g)) * (1.0f - speedUpFactor);
	CRY_ASSERT (estimatedLandTime > 0.0f);
	if (estimatedLandTime > 0.0f)
	{
	const float requiredGravity = (2.0f*jumpHeight) / (estimatedLandTime * estimatedLandTime);
	const float initialAccelerationScale = clamp_tpl ((-actorPhysics.velocity.z * 0.6f), 0.0f, 1.0f);
	jumpExtraVelocity = (requiredGravity - g) * actorPhysics.gravity.GetNormalized () * initialAccelerationScale;
	}
	}
	}

	return jumpExtraVelocity;*/

	return Vec3(ZERO);
}


void CActorStateJump::UpdateJumping(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime)
{
	Vec3 desiredVel(ZERO);
	if (UpdateCommon(actorControllerComponent, isHeavyWeapon, frameTime, &desiredVel))
	{
		Vec3 jumpExtraForce(0, 0, 0);
		switch (GetJumpState())
		{
			case JState_Jump:
				jumpExtraForce = CalculateInAirJumpExtraVelocity(actorControllerComponent, desiredVel);
				break;
		}
		desiredVel += jumpExtraForce;

		FinalizeVelocity(actorControllerComponent, desiredVel);

		if (CActorStateUtility::IsOnGround(actorControllerComponent))
		{
			Land(actorControllerComponent, isHeavyWeapon, frameTime);
		}
	}
}


void CActorStateJump::UpdateFalling(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime)
{
	Vec3 desiredVel(ZERO);

	if (!CActorStateUtility::IsOnGround(actorControllerComponent))
	{
		if (UpdateCommon(actorControllerComponent, isHeavyWeapon, frameTime, &desiredVel))
		{
			FinalizeVelocity(actorControllerComponent, desiredVel);
		}
	}
	else
	{
		// We were falling, now we're on ground - we landed!
		Land(actorControllerComponent, isHeavyWeapon, frameTime);
	}
}


bool CActorStateJump::UpdateCommon(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime, Vec3* pDesiredVel)
{
	Vec3 move(ZERO);
	const bool  bigWeaponRestrict = isHeavyWeapon;
	CActorStateUtility::CalculateGroundOrJumpMovement(actorControllerComponent, bigWeaponRestrict, move);

	return UpdateCommon(actorControllerComponent, isHeavyWeapon, move, frameTime, pDesiredVel);
}


bool CActorStateJump::UpdateCommon(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, const Vec3 &move, float frameTime, Vec3* pDesiredVel)
{
	/*GetDesiredVelocity (move, actorControllerComponent, pDesiredVel);

	const SActorPhysics& actorPhysics = actorControllerComponent.GetActorPhysics ();

	// generate stats.
	if (actorPhysics.velocity * actorPhysics.gravity > 0.0f)
	{
	const float fHeightofEntity = actorControllerComponent.GetEntity ()->GetWorldTM ().GetTranslation ().z;
	m_startFallingHeight = (float) __fsel (-actorControllerComponent.GetActorState()->fallSpeed, fHeightofEntity, max (m_startFallingHeight, fHeightofEntity));
	actorControllerComponent.GetActorState()->fallSpeed = -actorPhysics.velocity.z;
	}

	if (!gEnv->bMultiCharacter && actorControllerComponent.IsInPickAndThrowMode () && (actorControllerComponent.GetActorState()->fallSpeed > 10.f))
	actorControllerComponent.ExitPickAndThrow ();

	// durationInAir is set to 0.0f if we're swimming later - before refactoring this test happened *after* that, hence this test is here.
	m_jumpLock = (float) __fsel (-fabsf (actorControllerComponent.durationInAir), max (0.0f, m_jumpLock - frameTime), m_jumpLock);

	return true;*/

	return true;
}


void CActorStateJump::FinalizeVelocity(CActorControllerComponent& actorControllerComponent, const Vec3& newVelocity)
{
	/*const float fNewSpeed = newVelocity.len ();
	const float fVelocityMultiplier = (float) __fsel (fNewSpeed - 22.0f, __fres (fNewSpeed + FLT_EPSILON) * 22.0f, 1.0f);

	// #TODO: Maybe we should tell physics about this new velocity ? Or maybe SActorStats::velocity ? (stephenn).
	actorControllerComponent.GetMoveRequest ().velocity = newVelocity * fVelocityMultiplier;*/
}


void CActorStateJump::Land(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime)
{
	/*if (gEnv->bMultiCharacter && IsJumping ())
	{
	m_jumpLock = min (g_pGameCVars->pl_jump_baseTimeAddedPerJump + (m_jumpLock * g_pGameCVars->pl_jump_currentTimeMultiplierOnJump), g_pGameCVars->pl_jump_maxTimerValue);
	}

	const float fHeightofEntity = actorControllerComponent.GetEntity ()->GetWorldPos ().z;

	if (actorControllerComponent.IsClient ())
	{
	CActorStateUtility::ApplyFallDamage (actorControllerComponent, m_startFallingHeight, fHeightofEntity);
	}

	// #TODO: Physics sync.
	const float fallSpeed = actorControllerComponent.GetActorState()->fallSpeed;
	Landed (actorControllerComponent, isHeavyWeapon, fabsf (actorControllerComponent.GetActorPhysics()->velocityDelta.z)); // fallspeed might be incorrect on a dedicated server (pos is synced from client, but also smoothed).

	actorControllerComponent.GetActorState()->wasHit = false;

	SetJumpState (actorControllerComponent, JState_None);

	if (actorControllerComponent.GetActorState()->fallSpeed)
	{
	actorControllerComponent.GetActorState()->fallSpeed = 0.0f;

	const float worldWaterLevel = actorControllerComponent.m_stateSwimWaterTestProxy.GetWaterLevel ();
	if (fHeightofEntity < worldWaterLevel)
	{
	//actorControllerComponent.CreateScriptEvent ("jump_splash", worldWaterLevel - fHeightofEntity);
	}
	}*/
}


void CActorStateJump::GetDesiredVelocity(const Vec3 & move, const CActorControllerComponent& actorControllerComponent, Vec3* pDesiredVel) const
{
	// Generate jump velocity.
	/*SIMDFConstant (xmfMaxMove, 1.0f);

	simdf fGroundNormalZ = xmfMaxMove;

	hwvec3 xmMove = HWVLoadVecUnaligned (&move);
	if (move.len2 () > 0.01f)
	{
	const Matrix34A baseMtx = Matrix34A (actorControllerComponent.GetBaseQuat ());
	Matrix34A baseMtxZ (baseMtx * Matrix33::CreateScale (Vec3 (0, 0, 1)));
	baseMtxZ.SetTranslation (Vec3 (0, 0, 0));

	hwmtx33 xmBaseMtxZ;
	HWMtx33LoadAligned (xmBaseMtxZ, baseMtxZ);
	hwmtx33 xmBaseMtxZOpt = HWMtx33GetOptimized (xmBaseMtxZ);

	hwvec3 xmDesiredVel = HWV3Zero ();
	if (actorControllerComponent.IsRemote ())
	{
	xmDesiredVel = xmMove;
	}
	else
	{
	hwvec3 xmVelocity = HWVLoadVecUnaligned (&actorControllerComponent.GetActorPhysics()->velocity);

	SIMDFConstant (xmfZero, 0.0f);
	SIMDFConstant (xmfDiffMultiplier, 0.3f);
	SIMDFConstant (xmfMaxDiff, 0.1f);
	SIMDFConstant (xmfMinMove, 0.5f);
	SIMDFConstant (xmfOnePointFive, 1.5f);

	hwvec3 xmMoveFlat = HWVSub (xmMove, HWMtx33RotateVecOpt (xmBaseMtxZOpt, xmMove));
	hwvec3 xmCurrVelFlat = HWVSub (xmVelocity, HWMtx33RotateVecOpt (xmBaseMtxZOpt, xmVelocity));

	simdf xmfCurrVelSizeSq = HWV3LengthSq (xmCurrVelFlat);

	hwvec3 xmMoveFlatNormalized = HWV3Normalize (xmMoveFlat);
	hwvec3 xmCurDirFlatTemp = HWV3Normalize (xmCurrVelFlat);

	hwvec3 xmCurVelFlatNormalized = HWVSelectSIMDF (xmCurDirFlatTemp, HWV3Zero (), SIMDFLessThanEqual (xmfCurrVelSizeSq, xmfZero));

	simdf fDot = HWV3Dot (xmMoveFlatNormalized, xmCurVelFlatNormalized);

	hwvec3 xmScaledMoveFlat = HWVMultiplySIMDF (xmMoveFlat, SIMDFClamp (fDot, xmfMinMove, xmfMaxMove));
	simdf fMoveMult = SIMDFMax (SIMDFMult (SIMDFAbs (fDot), xmfDiffMultiplier), xmfMaxDiff);

	hwvec3 xmReducedMove = HWVMultiplySIMDF (HWVSub (xmMoveFlat, xmCurrVelFlat), fMoveMult);

	xmDesiredVel = HWVSelectSIMDF (xmScaledMoveFlat, xmReducedMove, SIMDFLessThan (fDot, xmfZero));

	simdf xmfDesiredVelSizeSq = HWV3LengthSq (xmDesiredVel);

	hwvec3 xmDesiredVelNorm = HWV3Normalize (xmDesiredVel);

	hwvec3 xmClampedVel = HWVMultiplySIMDF (xmDesiredVelNorm, SIMDFMax (xmfOnePointFive, SIMDFSqrt (xmfCurrVelSizeSq)));

	xmDesiredVel = HWVSelectSIMDF (xmClampedVel, xmDesiredVel, SIMDFLessThan (xmfDesiredVelSizeSq, xmfCurrVelSizeSq));
	}

	HWVSaveVecUnaligned (pDesiredVel, xmDesiredVel);
	}*/
}
}