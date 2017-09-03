#include <StdAfx.h>

#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include <IAnimatedCharacter.h>
#include <IItem.h>
#include <Actor/Movement/StateMachine/ActorStateJump.h>
#include <Actor/Actor.h>
/*#include "CharacterInput.h"
#include "NetCharacterInput.h"
#include "NetInputChainDebug.h"
#include "GameRules.h"
#include "GameRulesModules/IGameRulesObjectivesModule.h"
#include "GameCodeCoverage/GameCodeCoverageTracker.h"
#include "Weapon.h"*/


namespace Chrysalis
{
void CActorStateUtility::InitializeMoveRequest(SCharacterMoveRequest& acRequest)
{
	acRequest = SCharacterMoveRequest();
	acRequest.type = eCMT_Normal;
}


void CActorStateUtility::ProcessRotation(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest)
{
	auto pEntity = actorComponent.GetEntity();
	Quat rotationQuat = Quat(movementRequest.deltaAngles);

	// Physically rotate the actorComponent in the direction indicated by the request.
	pEntity->SetRotation(pEntity->GetRotation() * Quat(movementRequest.deltaAngles));

	// Rotate the animation in the inverse direction, this will have the effect of nullifying the physical rotation.
	// The animated actorComponent code will then take care of applying the required turning animations over the next few
	// frames - giving a smooth effect. 
	acRequest.rotation = rotationQuat.GetInverted();
}


void CActorStateUtility::FinalizeMovementRequest(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest)
{
	// Remote Characters stuff...
	UpdateRemoteInterpolation(actorComponent, movementRequest, acRequest);

	//// Send the request to animated actorComponent.
	//if (actorComponent.GetAnimatedCharacter())
	//{
	//	acRequest.type = eCMT_Normal;
	//	acRequest.velocity = movementRequest.desiredVelocity;
	//	acRequest.rotation = Quat { movementRequest.deltaAngles };
	//	acRequest.prediction = movementRequest.prediction;
	//	acRequest.allowStrafe = movementRequest.allowStrafe;
	//	acRequest.proceduralLeaning = movementRequest.desiredLean;

	//	// Jumping.
	//	// #TODO: Does jumping need a different request type of eCMT_JumpInstant or eCMT_JumpAccumulate?
	//	acRequest.jumping = movementRequest.shouldJump;

	//	// Send the request into the animated actorComponent system.
	//	actorComponent.GetAnimatedCharacter()->AddMovement(acRequest);
	//}

	// #TODO: find out what this line was used for. Seems to be used for footstep speed and debug, plus doubles up to test
	// if we're moving. Might be a better way to handle all that.
	//actorComponent.m_lastRequestedVelocity = acRequest.velocity;

	// Reset the request for the next frame!
	InitializeMoveRequest(acRequest);
}


void CActorStateUtility::CalculateGroundOrJumpMovement(const IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, const bool bigWeaponRestrict, Vec3 &move)
{
	/*	const bool isPlayer = actorComponent.IsPlayer();
		const float totalMovement = fabsf(movementRequest.desiredVelocity.x) + fabsf(movementRequest.desiredVelocity.y);
		const bool moving = (totalMovement > FLT_EPSILON);

		if (moving)
		{
			Vec3 desiredVelocityClamped = movementRequest.desiredVelocity;
			const float desiredVelocityMag = desiredVelocityClamped.GetLength();

			const float invDesiredVelocityMag = 1.0f / desiredVelocityMag;

			float strafeMul;

			if (bigWeaponRestrict)
			{
				strafeMul = g_pGameCVars->pl_movement.nonCombat_heavy_weapon_strafe_speed_scale;
			}
			else
			{
				strafeMul = g_pGameCVars->pl_movement.strafe_SpeedScale;
			}

			float backwardMul = 1.0f;

			desiredVelocityClamped = desiredVelocityClamped * (float) __fsel(-(desiredVelocityMag - 1.0f), 1.0f, invDesiredVelocityMag);

			// Going back?
			if (isPlayer) // Do not limit backwards movement when controlling AI.
			{
				backwardMul = (float) __fsel(desiredVelocityClamped.y, 1.0f, LERP(backwardMul, actorComponent.m_params.backwardMultiplier, -desiredVelocityClamped.y));
			}

			const Quat oldBaseQuat = actorComponent.GetEntity()->GetWorldRotation(); // we cannot use m_baseQuat: that one is already updated to a new orientation while desiredVelocity was relative to the old entity frame
			move += oldBaseQuat.GetColumn0() * desiredVelocityClamped.x * strafeMul * backwardMul;
			move += oldBaseQuat.GetColumn1() * desiredVelocityClamped.y * backwardMul;
		}

		// AI can set a custom sprint value, so don't cap the movement vector.
		if (movementRequest.sprint <= 0.0f)
		{
			//cap the movement vector to max 1
			float moveModule(move.len());

			//[Mikko] Do not limit backwards movement when controlling AI, otherwise it will disable sprinting.
			if (isPlayer)
			{
				move /= (float) __fsel(-(moveModule - 1.0f), 1.0f, moveModule);
			}
		}

		// Character movement don't need the m_frameTime, its handled already in the physics
		float scale = actorComponent.GetStanceMaxSpeed(actorComponent.GetStance());
		move *= scale;

		if (isPlayer)
		{
			const bool isCrouching = ((actorComponent.m_actions & ACTION_CROUCH) != 0);
			AdjustMovementForEnvironment(actorComponent, move, bigWeaponRestrict, isCrouching);
		}*/
}


bool CActorStateUtility::IsOnGround(IActorComponent& actorComponent)
{
	//return(!actorComponent.GetActorPhysics()->flags.AreAnyFlagsActive(SActorPhysics::EActorPhysicsFlags::Flying));
	// TODO: CRITICAL: HACK: BROKEN: !!
	return true;
}


bool CActorStateUtility::GetPhysicsLivingStatus(const IActorComponent& actorComponent, pe_status_living* physicsStatus)
{
	IPhysicalEntity *pPhysEnt = actorComponent.GetEntity()->GetPhysics();
	if (pPhysEnt)
		return(pPhysEnt->GetStatus(physicsStatus) > 0);

	return false;
}


void CActorStateUtility::AdjustMovementForEnvironment(const IActorComponent& actorComponent, Vec3& move, const bool bigWeaponRestrict, const bool crouching)
{
	/*	float mult = (bigWeaponRestrict)
			? g_pGameCVars->pl_movement.nonCombat_heavy_weapon_speed_scale * actorComponent.GetModifiableValues().GetValue(kPMV_HeavyWeaponMovementMultiplier)
			: 1.0f;

		if (gEnv->bMultiCharacter)
		{
			CGameRules *pGameRules = gEnv->pGameFramework->GetGameRules();
			IGameRulesObjectivesModule *pObjectives = pGameRules ? pGameRules->GetObjectivesModule() : NULL;

			if (pObjectives)
			{
				switch (pGameRules->GetGameMode())
				{
					case eGM_Extraction:
					{
						if (pObjectives->CheckIsCharacterEntityUsingObjective(actorComponent.GetEntityId())) // Carrying a tick
						{
							mult *= g_pGameCVars->mp_extractionParams.carryingTick_SpeedScale;
						}
						break;
					}

					case eGM_CaptureTheFlag:
					{
						if (pObjectives->CheckIsCharacterEntityUsingObjective(actorComponent.GetEntityId())) // Carrying a flag
						{
							mult *= g_pGameCVars->mp_ctfParams.carryingFlag_SpeedScale;
						}
						break;
					}

					default:
						break;
				}
			}
		}

		mult *= g_pGameCVars->pl_movement.speedScale;

		if (actorComponent.IsSprinting())
		{
			if (bigWeaponRestrict)
			{
				mult *= g_pGameCVars->pl_movement.nonCombat_heavy_weapon_sprint_scale;
			}
			else
			{
				mult *= g_pGameCVars->pl_movement.sprint_SpeedScale;
			}
		}
		else if (crouching)
		{
			if (bigWeaponRestrict)
			{
				mult *= g_pGameCVars->pl_movement.nonCombat_heavy_weapon_crouch_speed_scale;
			}
			else
			{
				mult *= g_pGameCVars->pl_movement.crouch_SpeedScale;
			}
		}

		move *= mult;*/
}


void CActorStateUtility::PhySetFly(IActorComponent& actorComponent)
{
	IPhysicalEntity* pPhysEnt = actorComponent.GetEntity()->GetPhysics();
	if (pPhysEnt != NULL)
	{
		pe_player_dynamics pd;
		pd.bSwimming = true;
		pd.kAirControl = 1.0f;
		pd.kAirResistance = 0.0f;
		pd.gravity.zero();
		pPhysEnt->SetParams(&pd);
	}
}


void CActorStateUtility::PhySetNoFly(IActorComponent& actorComponent, const Vec3& gravity)
{
	/*IPhysicalEntity* pPhysEnt = actorComponent.GetEntity()->GetPhysics();
	if (pPhysEnt != NULL)
	{
		pe_player_dynamics pd;
		pd.kAirControl = actorComponent.GetAirControl();
		pd.kAirResistance = actorComponent.GetAirResistance();
		pd.bSwimming = false;
		pd.gravity = gravity;
		actorComponent.m_actorPhysics.gravity = gravity;
		pPhysEnt->SetParams(&pd);
	}*/
}


// #TODO: Investigate jumping.

bool CActorStateUtility::IsJumpAllowed(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest)
{
	/*const bool allowJump = movementRequest.shouldJump && !actorComponent.IsJumping();

	if (allowJump)
	{
		// #TODO: We need a TryJump (stephenn).

		// This is needed when jumping while standing directly under something that causes an immediate land, before and without
		// even being airborne for one frame. CharacterMovement set m_actorState.durationOnGround=0.0f when the jump is triggered, which
		// prevents the on ground timer from before the jump to be inherited and incorrectly and prematurely used to
		// identify landing in MP.
		const SActorStats& stats = *actorComponent.GetActorState();
		const SActorPhysics& actorPhysics = actorComponent.GetActorPhysics();
		const float fUnconstrainedZ = actorPhysics.velocityUnconstrained.z;
		bool jumpFailed = (stats.durationOnGround > 0.0f) && (fUnconstrainedZ <= 0.0f);

		const float onGroundTime = 0.2f;
		if (((stats.durationOnGround > onGroundTime) || actorComponent.IsRemote())) // && !jumpFailed )
		{
			return true;
		}
		else
		{
			CCCPOINT_IF(true, CharacterMovement_PressJumpWhileNotAllowedToJump);
		}
	}*/

	return movementRequest.shouldJump;
}


void CActorStateUtility::RestorePhysics(IActorComponent& actorComponent)
{
	/*IAnimatedCharacter* pAC = actorComponent.GetAnimatedCharacter();
	if (pAC)
	{
		SAnimatedCharacterParams params;

		params = pAC->GetParams();

		params.inertia = actorComponent.GetInertia();
		params.inertiaAccel = actorComponent.GetInertiaAccel();
		params.timeImpulseRecover = actorComponent.GetTimeImpulseRecover();

		actorComponent.SetAnimatedCharacterParams(params);
	}*/
}


void CActorStateUtility::UpdatePhysicsState(IActorComponent& actorComponent, SActorPhysics& actorPhysics, float frameTime)
{
	/*const int currentFrameID = gEnv->pRenderer->GetFrameID();

	if (actorPhysics.lastFrameUpdate < currentFrameID)
	{
		pe_status_living livStat;
		if (!CActorStateUtility::GetPhysicsLivingStatus(actorComponent, &livStat))
		{
			return;
		}

		SActorStats& stats = *actorComponent.GetActorState();

		const Vec3 newVelocity = livStat.vel - livStat.velGround;
		actorPhysics.velocityDelta = newVelocity - actorPhysics.velocity;
		actorPhysics.velocity = newVelocity;
		actorPhysics.velocityUnconstrainedLast = actorPhysics.velocityUnconstrained;
		actorPhysics.velocityUnconstrained = livStat.velUnconstrained;
		actorPhysics.flags.SetFlags(SActorPhysics:WasFlying, actorPhysics.flags.AreAnyFlagsActive(SActorPhysics::EActorPhysicsFlags::Flying));
		actorPhysics.flags.SetFlags(SActorPhysics::EActorPhysicsFlags::Flying, livStat.bFlying > 0);
		actorPhysics.flags.SetFlags(SActorPhysics::EActorPhysicsFlags::Stuck, livStat.bStuck > 0);

		Vec3 flatVel(actorComponent.m_pCharacterRotation->GetBaseQuat().GetInverted()*newVelocity);
		flatVel.z = 0;
		stats.speedFlat = flatVel.len();

		if (actorComponent.IsInAir())
		{
			stats.maxAirSpeed = max(stats.maxAirSpeed, newVelocity.GetLengthFast());
		}
		else
		{
			stats.maxAirSpeed = 0.f;
		}
		float fSpeedFlatSelector = stats.speedFlat - 0.1f;


		const float groundNormalBlend = clamp_tpl(frameTime * 6.666f, 0.0f, 1.0f);
		actorPhysics.groundNormal = LERP(actorPhysics.groundNormal, livStat.groundSlope, groundNormalBlend);

		if (livStat.groundSurfaceIdxAux > 0)
			actorPhysics.groundMaterialIdx = livStat.groundSurfaceIdxAux;
		else
			actorPhysics.groundMaterialIdx = livStat.groundSurfaceIdx;

		actorPhysics.groundHeight = livStat.groundHeight;

		EntityId newGroundColliderId = 0;
		if (livStat.pGroundCollider)
		{
			IEntity* pEntity = gEnv->pEntitySystem->GetEntityFromPhysics(livStat.pGroundCollider);
			newGroundColliderId = pEntity ? pEntity->GetId() : 0;
		}

		if (newGroundColliderId != actorPhysics.groundColliderId)
		{
			if (actorPhysics.groundColliderId)
			{
				if (IGameObject* pGameObject = gEnv->pGameFramework->GetGameObject(actorPhysics.groundColliderId))
				{
					SGameObjectEvent event(eGFE_StoodOnChange, eGOEF_ToExtensions);
					event.ptr = &actorComponent;
					event.paramAsBool = false;
					pGameObject->SendEvent(event);
				}
			}

			if (newGroundColliderId)
			{
				if (IGameObject* pGameObject = gEnv->pGameFramework->GetGameObject(newGroundColliderId))
				{
					SGameObjectEvent event(eGFE_StoodOnChange, eGOEF_ToExtensions);
					event.ptr = &actorComponent;
					event.paramAsBool = true;
					pGameObject->SendEvent(event);
				}
			}

			actorPhysics.groundColliderId = newGroundColliderId;
		}

		IPhysicalEntity *pPhysEnt = actorComponent.GetEntity()->GetPhysics();
		if (pPhysEnt)
		{
			pe_status_dynamics dynStat;
			pPhysEnt->GetStatus(&dynStat);

			actorPhysics.angVelocity = dynStat.w;
			actorPhysics.mass = dynStat.mass;

			pe_player_dynamics simPar;
			if (pPhysEnt->GetParams(&simPar) != 0)
			{
				actorPhysics.gravity = simPar.gravity;
			}
		}

		actorPhysics.lastFrameUpdate = currentFrameID;

#ifdef CHARACTER_MOVEMENT_DEBUG_ENABLED
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelo", livStat.vel.GetLength());
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloX", livStat.vel.x);
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloY", livStat.vel.y);
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloZ", livStat.vel.z);

		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloUn", livStat.velUnconstrained.GetLength());
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnX", livStat.velUnconstrained.x);
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnY", livStat.velUnconstrained.y);
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnZ", livStat.velUnconstrained.z);

		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelReq", livStat.velRequested.GetLength());
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelReqX", livStat.velRequested.x);
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelReqY", livStat.velRequested.y);
		actorComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelReqZ", livStat.velRequested.z);

		actorComponent.GetMovementDebug().LogVelocityStats(actorComponent.GetEntity(), livStat, stats.downwardsImpactVelocity, stats.fallSpeed);
#endif
	}*/
}


void CActorStateUtility::UpdateRemoteInterpolation(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& frameRequest)
{
	/*if (!gEnv->bMultiCharacter)
		return;


	// ***
	// *** Interpolation for remote Characters
	// ***

	const bool isRemoteClient = actorComponent.IsRemote();
	const bool doVelInterpolation = isRemoteClient && actorComponent.GetCharacterInput();

	if (doVelInterpolation)
	{
		if (g_pGameCVars->pl_clientInertia >= 0.0f)
		{
			actorComponent.m_inertia = g_pGameCVars->pl_clientInertia;
		}
		actorComponent.m_inertiaAccel = actorComponent.m_inertia;

		const bool isNetJumping = g_pGameCVars->pl_velocityInterpSynchJump && movementRequest.jump && isRemoteClient;

		if (isNetJumping)
		{
#ifdef STATE_DEBUG
			if (g_pGameCVars->pl_debugInterpolation)
			{
				CryWatch("SetVel (%f, %f, %f)", actorComponent.m_jumpVel.x, actorComponent.m_jumpVel.y, actorComponent.m_jumpVel.z);
			}
#endif //_RELEASE
			frameRequest.velocity = actorComponent.m_jumpVel;
			frameRequest.type = eCMT_JumpAccumulate;
		}
		else
		{
			CNetCharacterInput *CharacterInput = static_cast<CNetCharacterInput*>(actorComponent.GetCharacterInput());
			Vec3 interVel, desiredVel;

			bool durationInAir;
			desiredVel = CharacterInput->GetDesiredVelocity(durationInAir);
			interVel = desiredVel;

			IPhysicalEntity* pPhysEnt = actorComponent.GetEntity()->GetPhysics();
			CRY_ASSERT_MESSAGE(pPhysEnt, "Entity not physicalised! TomB would like to look at this.");

			const bool isCharacterInAir = actorComponent.IsInAir();
			if (durationInAir && isCharacterInAir)
			{
				if (pPhysEnt && (g_pGameCVars->pl_velocityInterpAirDeltaFactor < 1.0f))
				{
					pe_status_dynamics dynStat;
					pPhysEnt->GetStatus(&dynStat);

					Vec3  velDiff = interVel - dynStat.v;
					interVel = dynStat.v + (velDiff * g_pGameCVars->pl_velocityInterpAirDeltaFactor);
					frameRequest.velocity.z = interVel.z;
					frameRequest.velocity.z -= actorComponent.GetActorPhysics()->gravity.z*gEnv->pTimer->GetFrameTime();
					frameRequest.type = eCMT_Fly;
				}
			}
			frameRequest.velocity.x = interVel.x;
			frameRequest.velocity.y = interVel.y;


#ifdef STATE_DEBUG
			if (pPhysEnt && g_pGameCVars->pl_debugInterpolation)
			{
				pe_status_dynamics dynStat;
				pPhysEnt->GetStatus(&dynStat);

				CryWatch("Cur: (%f %f %f) Des: (%f %f %f)", dynStat.v.x, dynStat.v.y, dynStat.v.z, desiredVel.x, desiredVel.y, desiredVel.z);
				CryWatch("Req: (%f %f %f) Type (%d)", frameRequest.velocity.x, frameRequest.velocity.y, frameRequest.velocity.z, frameRequest.type);
			}
#endif //!_RELEASE
		}
	}*/
}


bool CActorStateUtility::IsMovingForward(const IActorComponent& actorComponent, const SActorMovementRequest& movementRequest)
{
	/*const float fSpeedFlatSelector = actorComponent.GetActorState ()->speedFlat - 0.1f;
	bool movingForward = (fSpeedFlatSelector > 0.1f);

	if (!gEnv->bMultiCharacter || actorComponent.IsClient ())
	{
	// IsMovingForward() doesn't return particularly reliable results for client characters in MP, I think because of
	// interpolation inaccuracies on the Y velocity. This was causing client characters to incorrectly report that they're
	// no longer moving forwards, whereas all that had happened was they had stopped sprinting - and this in turn was
	// messing up the MP sprint stamina regeneration delays. Client characters have also been seen to stop sprinting due to
	// turning lightly, whereas on their local machine they were still sprinting fine... so I've tried changing it so
	// IsMovingForward() is only taken into account on local clients (and in SP!).
	const float thresholdSinAngle = sin_tpl ((3.141592f * 0.5f) - DEG2RAD (g_pGameCVars->pl_power_sprint.foward_angle));
	const float currentSinAngle = movementRequest.desiredVelocity.y;

	movingForward = movingForward && (currentSinAngle > thresholdSinAngle);
	}

	return movingForward;*/

	return false;
}


bool CActorStateUtility::IsSprintingAllowed(const IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, IItem* pCurrentActorItem)
{
	/*bool shouldSprint = false;
	const SActorStats& stats = actorComponent.m_actorState;
	const bool movingForward = IsMovingForward(actorComponent, movementRequest);

	bool restrictSprint = actorComponent.IsJumping() || stats.bIgnoreSprinting || actorComponent.IsSliding() || actorComponent.IsCinematicFlagActive(SActorStats::eCinematicFlag_RestrictMovement);

	CWeapon* pWeapon = pCurrentActorItem ? static_cast<CWeapon*>(pCurrentActorItem->GetIWeapon()) : NULL;
	bool isZooming = pWeapon ? pWeapon->IsZoomed() && !pWeapon->IsZoomingInOrOut() : false;
	if (pWeapon && !pWeapon->CanSprint())
		restrictSprint = true;

	restrictSprint = restrictSprint || (actorComponent.GetSprintStaminaLevel() <= 0.f);

	if (!actorComponent.IsSprinting())
	{
		shouldSprint = movingForward && !restrictSprint && !isZooming;
		CCCPOINT_IF(shouldSprint, CharacterMovement_SprintOn);
	}
	else
	{
		shouldSprint = movingForward && !restrictSprint && !isZooming;

		shouldSprint = shouldSprint && (!(actorComponent.m_actions & ACTION_CROUCH));

		if (!shouldSprint && pWeapon)
		{
			pWeapon->ForcePendingActions();
		}
	}

	CCCPOINT_IF(!actorComponent.IsSprinting() && !shouldSprint && (actorComponent.m_actions & ACTION_SPRINT), CharacterMovement_SprintRequestIgnored);
	CCCPOINT_IF(actorComponent.IsSprinting() && !shouldSprint, CharacterMovement_SprintOff);

	return shouldSprint;*/

	return false;
}


void CActorStateUtility::ApplyFallDamage(IActorComponent& actorComponent, const float startFallingHeight, const float fHeightofEntity)
{
	/*CRY_ASSERT(actorComponent.IsClient());

	// Zero downwards impact velocity used for fall damage calculations if actorComponent was in water within the last 0.5 seconds.
	// Strength jumping straight up and down should theoretically land with a safe velocity,
	// but together with the water surface stickyness the velocity can sometimes go above the safe impact velocity threshold.

	// DEPRECATED: comment left for posterity in case dedicated server problems re-appear (author cannot test it).
	// On dedicated server the actorComponent can still be flying this frame as well,
	// since synced pos from client is interpolated/smoothed and will not land immediately,
	// even though the velocity is set to zero.
	// Thus we need to use the velocity change instead of landing to identify impact.

	// DT: 12475: Falling a huge distance to a ledge grab results in no damage.
	// Now using the last velocity because when ledge grabbing the velocity is unchanged for this frame, thus zero damage is applied.
	// Assuming this a physics lag issue, using the last good velocity should be more-or-less ok.
	const float downwardsImpactSpeed = -(float) __fsel(-(actorComponent.m_stateSwimWaterTestProxy.GetSwimmingTimer() + 0.5f), actorComponent.GetActorPhysics()->velocityUnconstrainedLast.z, 0.0f);

	const SActorStats& stats = *actorComponent.GetActorState();

	CRY_ASSERT(NumberValid(downwardsImpactSpeed));
	const float MIN_FALL_DAMAGE_DISTANCE = 3.0f;
	const float fallDist = startFallingHeight - fHeightofEntity;
	if ((downwardsImpactSpeed > 0.0f) && (fallDist > MIN_FALL_DAMAGE_DISTANCE))
	{
		const SCharacterHealth& healthCVars = g_pGameCVars->pl_health;

		float velSafe = healthCVars.fallDamage_SpeedSafe;
		float velFatal = healthCVars.fallDamage_SpeedFatal;
		float velFraction = (float) __fsel(-(velFatal - velSafe), 1.0f, (downwardsImpactSpeed - velSafe) * (float) __fres(velFatal - velSafe));

		CRY_ASSERT(NumberValid(velFraction));

		if (velFraction > 0.0f)
		{
			//Stop crouching after taking falling damage
			if (actorComponent.GetStance() == STANCE_CROUCH)
			{
				static_cast<CCharacterInput*>(actorComponent.GetCharacterInput())->ClearCrouchAction();
			}

			velFraction = powf(velFraction, gEnv->bMultiCharacter ? healthCVars.fallDamage_CurveAttackMP : healthCVars.fallDamage_CurveAttack);

			const float maxHealth = actorComponent.GetMaxHealth();
			const float currentHealth = actorComponent.GetHealth();

			HitInfo hit;
			hit.dir.zero();
			hit.type = CGameRules::EHitType::Fall;
			hit.shooterId = hit.targetId = hit.weaponId = actorComponent.GetEntityId();

			const float maxDamage = (float) __fsel(velFraction - 1.0f, maxHealth, max(0.0f, (gEnv->bMultiCharacter ? maxHealth : currentHealth) - healthCVars.fallDamage_health_threshold));

			hit.damage = velFraction * maxDamage;

			gEnv->pGameFramework->GetGameRules()->ClientHit(hit);

#ifdef CHARACTER_MOVEMENT_DEBUG_ENABLED
			actorComponent.GetMovementDebug().LogFallDamage(actorComponent.GetEntity(), velFraction, downwardsImpactSpeed, hit.damage);
		}
		else
		{
			actorComponent.GetMovementDebug().LogFallDamageNone(actorComponent.GetEntity(), downwardsImpactSpeed);
		}
#else
		}
#endif
	}*/
}


bool CActorStateUtility::DoesArmorAbsorptFallDamage(IActorComponent& actorComponent, const float downwardsImpactSpeed, float& absorptedDamageFraction)
{
	/*CRY_ASSERT(!gEnv->bMultiCharacter);

	absorptedDamageFraction = 0.0f;

	const SCharacterHealth& healthCVars = g_pGameCVars->pl_health;

	const float speedEnergySafe = healthCVars.fallDamage_SpeedSafe;
	const float speedEnergyDeplete = healthCVars.fallDamage_SpeedFatal;
	float heavyArmorFactor = 1.0f;

	// Armour took all damage at no energy cost.
	if (downwardsImpactSpeed <= speedEnergySafe)
		return true;

	return false;*/

	return false;
}


void CActorStateUtility::CancelCrouchAndProneInputs(IActorComponent& actorComponent)
{
	/*ICharacterInput* CharacterInputInterface = actorComponent.GetCharacterInput();

	if (CharacterInputInterface && CharacterInputInterface->GetType() == ICharacterInput::CHARACTER_INPUT)
	{
		CCharacterInput * CharacterInput = static_cast<CCharacterInput*>(CharacterInputInterface);
		CharacterInput->ClearCrouchAction();
		CharacterInput->ClearProneAction();
	}*/
}


void CActorStateUtility::ChangeStance(IActorComponent& actorComponent, const SStateEvent& event)
{
	/*const SStateEventStanceChanged& stanceEvent = static_cast<const SStateEventStanceChanged&> (event).GetStance();
	const EStance stance = static_cast<EStance> (stanceEvent.GetStance());
	actorComponent.OnSetStance(stance);*/
}
}