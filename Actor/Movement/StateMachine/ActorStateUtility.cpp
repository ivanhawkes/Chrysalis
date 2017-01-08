#include <StdAfx.h>

#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include <IAnimatedCharacter.h>
#include <IItem.h>
#include <Actor/Character/Movement/StateMachine/CharacterStateJump.h>
#include <Actor/Actor.h>
#include <Utility/CryWatch.h>
/*#include "CharacterInput.h"
#include "NetCharacterInput.h"
#include "NetInputChainDebug.h"
#include "GameRules.h"
#include "GameRulesModules/IGameRulesObjectivesModule.h"
#include "GameCodeCoverage/GameCodeCoverageTracker.h"
#include "Weapon.h"*/


void CCharacterStateUtil::InitializeMoveRequest(SCharacterMoveRequest& acRequest)
{
	acRequest = SCharacterMoveRequest();
	acRequest.type = eCMT_Normal;
}


void CCharacterStateUtil::ProcessRotation(CActor& actor, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest)
{
	auto pEntity = actor.GetEntity();
	Quat rotationQuat = Quat(movementRequest.deltaAngles);

	// Physically rotate the actor in the direction indicated by the request.
	pEntity->SetRotation(pEntity->GetRotation() * Quat(movementRequest.deltaAngles));

	// Rotate the animation in the inverse direction, this will have the effect of nullifying the physical rotation.
	// The animated actor code will then take care of applying the required turning animations over the next few
	// frames - giving a smooth effect. 
	acRequest.rotation = rotationQuat.GetInverted();
}


void CCharacterStateUtil::FinalizeMovementRequest(CActor& actor, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest)
{
	// Remote Characters stuff...
	UpdateRemoteInterpolation(actor, movementRequest, acRequest);

	// Send the request to animated actor.
	if (actor.GetAnimatedCharacter())
	{
		acRequest.type = eCMT_Normal;
		acRequest.velocity = movementRequest.desiredVelocity;
		acRequest.rotation = Quat { movementRequest.deltaAngles };
		acRequest.prediction = movementRequest.prediction;
		acRequest.allowStrafe = movementRequest.allowStrafe;
		acRequest.proceduralLeaning = movementRequest.desiredLean;

		// Jumping.
		// TODO: Does jumping need a different request type of eCMT_JumpInstant or eCMT_JumpAccumulate?
		acRequest.jumping = movementRequest.shouldJump;

		// Send the request into the animated actor system.
		actor.GetAnimatedCharacter()->AddMovement(acRequest);
	}

	// TODO: find out what this line was used for. Seems to be used for footstep speed and debug, plus doubles up to test
	// if we're moving. Might be a better way to handle all that.
	//actor.m_lastRequestedVelocity = acRequest.velocity;

	// Reset the request for the next frame!
	InitializeMoveRequest(acRequest);
}


void CCharacterStateUtil::CalculateGroundOrJumpMovement(const CActor& actor, const SActorMovementRequest& movementRequest, const bool bigWeaponRestrict, Vec3 &move)
{
	/*	const bool isPlayer = actor.IsPlayer();
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
				backwardMul = (float) __fsel(desiredVelocityClamped.y, 1.0f, LERP(backwardMul, actor.m_params.backwardMultiplier, -desiredVelocityClamped.y));
			}

			const Quat oldBaseQuat = actor.GetEntity()->GetWorldRotation(); // we cannot use m_baseQuat: that one is already updated to a new orientation while desiredVelocity was relative to the old entity frame
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
		float scale = actor.GetStanceMaxSpeed(actor.GetStance());
		move *= scale;

		if (isPlayer)
		{
			const bool isCrouching = ((actor.m_actions & ACTION_CROUCH) != 0);
			AdjustMovementForEnvironment(actor, move, bigWeaponRestrict, isCrouching);
		}*/
}


bool CCharacterStateUtil::IsOnGround(CActor& actor)
{
	return(!actor.GetActorPhysics()->flags.AreAnyFlagsActive(SActorPhysics::EActorPhysicsFlags::Flying));
}


bool CCharacterStateUtil::GetPhysicsLivingStatus(const CActor& actor, pe_status_living* physicsStatus)
{
	IPhysicalEntity *pPhysEnt = actor.GetEntity()->GetPhysics();
	if (pPhysEnt)
		return(pPhysEnt->GetStatus(physicsStatus) > 0);

	return false;
}


void CCharacterStateUtil::AdjustMovementForEnvironment(const CActor& actor, Vec3& move, const bool bigWeaponRestrict, const bool crouching)
{
	/*	float mult = (bigWeaponRestrict)
			? g_pGameCVars->pl_movement.nonCombat_heavy_weapon_speed_scale * actor.GetModifiableValues().GetValue(kPMV_HeavyWeaponMovementMultiplier)
			: 1.0f;

		if (gEnv->bMultiCharacter)
		{
			CGameRules *pGameRules = g_pGame->GetGameRules();
			IGameRulesObjectivesModule *pObjectives = pGameRules ? pGameRules->GetObjectivesModule() : NULL;

			if (pObjectives)
			{
				switch (pGameRules->GetGameMode())
				{
					case eGM_Extraction:
					{
						if (pObjectives->CheckIsCharacterEntityUsingObjective(actor.GetEntityId())) // Carrying a tick
						{
							mult *= g_pGameCVars->mp_extractionParams.carryingTick_SpeedScale;
						}
						break;
					}

					case eGM_CaptureTheFlag:
					{
						if (pObjectives->CheckIsCharacterEntityUsingObjective(actor.GetEntityId())) // Carrying a flag
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

		if (actor.IsSprinting())
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


void CCharacterStateUtil::PhySetFly(CActor& actor)
{
	IPhysicalEntity* pPhysEnt = actor.GetEntity()->GetPhysics();
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


void CCharacterStateUtil::PhySetNoFly(CActor& actor, const Vec3& gravity)
{
	/*IPhysicalEntity* pPhysEnt = actor.GetEntity()->GetPhysics();
	if (pPhysEnt != NULL)
	{
		pe_player_dynamics pd;
		pd.kAirControl = actor.GetAirControl();
		pd.kAirResistance = actor.GetAirResistance();
		pd.bSwimming = false;
		pd.gravity = gravity;
		actor.m_actorPhysics.gravity = gravity;
		pPhysEnt->SetParams(&pd);
	}*/
}


// TODO: Investigate jumping.

bool CCharacterStateUtil::IsJumpAllowed(CActor& actor, const SActorMovementRequest& movementRequest)
{
	/*const bool allowJump = movementRequest.shouldJump && !actor.IsJumping();

	if (allowJump)
	{
		// TODO: We need a TryJump (stephenn).

		// This is needed when jumping while standing directly under something that causes an immediate land, before and without
		// even being airborne for one frame. CharacterMovement set m_actorState.onGround=0.0f when the jump is triggered, which
		// prevents the on ground timer from before the jump to be inherited and incorrectly and prematurely used to
		// identify landing in MP.
		const SActorStats& stats = *actor.GetActorState();
		const SActorPhysics& actorPhysics = actor.GetActorPhysics();
		const float fUnconstrainedZ = actorPhysics.velocityUnconstrained.z;
		bool jumpFailed = (stats.onGround > 0.0f) && (fUnconstrainedZ <= 0.0f);

		const float onGroundTime = 0.2f;
		if (((stats.onGround > onGroundTime) || actor.IsRemote())) // && !jumpFailed )
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


void CCharacterStateUtil::RestorePhysics(CActor& actor)
{
	/*IAnimatedCharacter* pAC = actor.GetAnimatedCharacter();
	if (pAC)
	{
		SAnimatedCharacterParams params;

		params = pAC->GetParams();

		params.inertia = actor.GetInertia();
		params.inertiaAccel = actor.GetInertiaAccel();
		params.timeImpulseRecover = actor.GetTimeImpulseRecover();

		actor.SetAnimatedCharacterParams(params);
	}*/
}


void CCharacterStateUtil::UpdatePhysicsState(CActor& actor, SActorPhysics& actorPhysics, float frameTime)
{
	/*const int currentFrameID = gEnv->pRenderer->GetFrameID();

	if (actorPhysics.lastFrameUpdate < currentFrameID)
	{
		pe_status_living livStat;
		if (!CCharacterStateUtil::GetPhysicsLivingStatus(actor, &livStat))
		{
			return;
		}

		SActorStats& stats = *actor.GetActorState();

		const Vec3 newVelocity = livStat.vel - livStat.velGround;
		actorPhysics.velocityDelta = newVelocity - actorPhysics.velocity;
		actorPhysics.velocity = newVelocity;
		actorPhysics.velocityUnconstrainedLast = actorPhysics.velocityUnconstrained;
		actorPhysics.velocityUnconstrained = livStat.velUnconstrained;
		actorPhysics.flags.SetFlags(SActorPhysics:WasFlying, actorPhysics.flags.AreAnyFlagsActive(SActorPhysics::EActorPhysicsFlags::Flying));
		actorPhysics.flags.SetFlags(SActorPhysics::EActorPhysicsFlags::Flying, livStat.bFlying > 0);
		actorPhysics.flags.SetFlags(SActorPhysics::EActorPhysicsFlags::Stuck, livStat.bStuck > 0);

		Vec3 flatVel(actor.m_pCharacterRotation->GetBaseQuat().GetInverted()*newVelocity);
		flatVel.z = 0;
		stats.speedFlat = flatVel.len();

		if (actor.IsInAir())
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
					event.ptr = &actor;
					event.paramAsBool = false;
					pGameObject->SendEvent(event);
				}
			}

			if (newGroundColliderId)
			{
				if (IGameObject* pGameObject = gEnv->pGameFramework->GetGameObject(newGroundColliderId))
				{
					SGameObjectEvent event(eGFE_StoodOnChange, eGOEF_ToExtensions);
					event.ptr = &actor;
					event.paramAsBool = true;
					pGameObject->SendEvent(event);
				}
			}

			actorPhysics.groundColliderId = newGroundColliderId;
		}

		IPhysicalEntity *pPhysEnt = actor.GetEntity()->GetPhysics();
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
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVelo", livStat.vel.GetLength());
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVeloX", livStat.vel.x);
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVeloY", livStat.vel.y);
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVeloZ", livStat.vel.z);

		actor.GetMovementDebug().DebugGraph_AddValue("PhysVeloUn", livStat.velUnconstrained.GetLength());
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnX", livStat.velUnconstrained.x);
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnY", livStat.velUnconstrained.y);
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnZ", livStat.velUnconstrained.z);

		actor.GetMovementDebug().DebugGraph_AddValue("PhysVelReq", livStat.velRequested.GetLength());
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVelReqX", livStat.velRequested.x);
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVelReqY", livStat.velRequested.y);
		actor.GetMovementDebug().DebugGraph_AddValue("PhysVelReqZ", livStat.velRequested.z);

		actor.GetMovementDebug().LogVelocityStats(actor.GetEntity(), livStat, stats.downwardsImpactVelocity, stats.fallSpeed);
#endif
	}*/
}


void CCharacterStateUtil::UpdateRemoteInterpolation(CActor& actor, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& frameRequest)
{
	/*if (!gEnv->bMultiCharacter)
		return;


	// ***
	// *** Interpolation for remote Characters
	// ***

	const bool isRemoteClient = actor.IsRemote();
	const bool doVelInterpolation = isRemoteClient && actor.GetCharacterInput();

	if (doVelInterpolation)
	{
		if (g_pGameCVars->pl_clientInertia >= 0.0f)
		{
			actor.m_inertia = g_pGameCVars->pl_clientInertia;
		}
		actor.m_inertiaAccel = actor.m_inertia;

		const bool isNetJumping = g_pGameCVars->pl_velocityInterpSynchJump && movementRequest.jump && isRemoteClient;

		if (isNetJumping)
		{
#ifdef STATE_DEBUG
			if (g_pGameCVars->pl_debugInterpolation)
			{
				CryWatch("SetVel (%f, %f, %f)", actor.m_jumpVel.x, actor.m_jumpVel.y, actor.m_jumpVel.z);
			}
#endif //_RELEASE
			frameRequest.velocity = actor.m_jumpVel;
			frameRequest.type = eCMT_JumpAccumulate;
		}
		else
		{
			CNetCharacterInput *CharacterInput = static_cast<CNetCharacterInput*>(actor.GetCharacterInput());
			Vec3 interVel, desiredVel;

			bool inAir;
			desiredVel = CharacterInput->GetDesiredVelocity(inAir);
			interVel = desiredVel;

			IPhysicalEntity* pPhysEnt = actor.GetEntity()->GetPhysics();
			CRY_ASSERT_MESSAGE(pPhysEnt, "Entity not physicalised! TomB would like to look at this.");

			const bool isCharacterInAir = actor.IsInAir();
			if (inAir && isCharacterInAir)
			{
				if (pPhysEnt && (g_pGameCVars->pl_velocityInterpAirDeltaFactor < 1.0f))
				{
					pe_status_dynamics dynStat;
					pPhysEnt->GetStatus(&dynStat);

					Vec3  velDiff = interVel - dynStat.v;
					interVel = dynStat.v + (velDiff * g_pGameCVars->pl_velocityInterpAirDeltaFactor);
					frameRequest.velocity.z = interVel.z;
					frameRequest.velocity.z -= actor.GetActorPhysics()->gravity.z*gEnv->pTimer->GetFrameTime();
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


bool CCharacterStateUtil::IsMovingForward(const CActor& actor, const SActorMovementRequest& movementRequest)
{
	/*const float fSpeedFlatSelector = actor.GetActorState ()->speedFlat - 0.1f;
	bool movingForward = (fSpeedFlatSelector > 0.1f);

	if (!gEnv->bMultiCharacter || actor.IsClient ())
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


bool CCharacterStateUtil::IsSprintingAllowed(const CActor& actor, const SActorMovementRequest& movementRequest, IItem* pCurrentActorItem)
{
	/*bool shouldSprint = false;
	const SActorStats& stats = actor.m_actorState;
	const bool movingForward = IsMovingForward(actor, movementRequest);

	bool restrictSprint = actor.IsJumping() || stats.bIgnoreSprinting || actor.IsSliding() || actor.IsCinematicFlagActive(SActorStats::eCinematicFlag_RestrictMovement);

	CWeapon* pWeapon = pCurrentActorItem ? static_cast<CWeapon*>(pCurrentActorItem->GetIWeapon()) : NULL;
	bool isZooming = pWeapon ? pWeapon->IsZoomed() && !pWeapon->IsZoomingInOrOut() : false;
	if (pWeapon && !pWeapon->CanSprint())
		restrictSprint = true;

	restrictSprint = restrictSprint || (actor.GetSprintStaminaLevel() <= 0.f);

	if (actor.IsSprinting() == false)
	{
		shouldSprint = movingForward && !restrictSprint && !isZooming;
		CCCPOINT_IF(shouldSprint, CharacterMovement_SprintOn);
	}
	else
	{
		shouldSprint = movingForward && !restrictSprint && !isZooming;

		shouldSprint = shouldSprint && (!(actor.m_actions & ACTION_CROUCH));

		if (!shouldSprint && pWeapon)
		{
			pWeapon->ForcePendingActions();
		}
	}

	CCCPOINT_IF(!actor.IsSprinting() && !shouldSprint && (actor.m_actions & ACTION_SPRINT), CharacterMovement_SprintRequestIgnored);
	CCCPOINT_IF(actor.IsSprinting() && !shouldSprint, CharacterMovement_SprintOff);

	return shouldSprint;*/

	return false;
}


void CCharacterStateUtil::ApplyFallDamage(CActor& actor, const float startFallingHeight, const float fHeightofEntity)
{
	/*CRY_ASSERT(actor.IsClient());

	// Zero downwards impact velocity used for fall damage calculations if actor was in water within the last 0.5 seconds.
	// Strength jumping straight up and down should theoretically land with a safe velocity,
	// but together with the water surface stickyness the velocity can sometimes go above the safe impact velocity threshold.

	// DEPRECATED: comment left for posterity in case dedicated server problems re-appear (author cannot test it).
	// On dedicated server the actor can still be flying this frame as well,
	// since synced pos from client is interpolated/smoothed and will not land immediately,
	// even though the velocity is set to zero.
	// Thus we need to use the velocity change instead of landing to identify impact.

	// DT: 12475: Falling a huge distance to a ledge grab results in no damage.
	// Now using the last velocity because when ledge grabbing the velocity is unchanged for this frame, thus zero damage is applied.
	// Assuming this a physics lag issue, using the last good velocity should be more-or-less ok.
	const float downwardsImpactSpeed = -(float) __fsel(-(actor.m_CharacterStateSwimWaterTestProxy.GetSwimmingTimer() + 0.5f), actor.GetActorPhysics()->velocityUnconstrainedLast.z, 0.0f);

	const SActorStats& stats = *actor.GetActorState();

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
			if (actor.GetStance() == STANCE_CROUCH)
			{
				static_cast<CCharacterInput*>(actor.GetCharacterInput())->ClearCrouchAction();
			}

			velFraction = powf(velFraction, gEnv->bMultiCharacter ? healthCVars.fallDamage_CurveAttackMP : healthCVars.fallDamage_CurveAttack);

			const float maxHealth = actor.GetMaxHealth();
			const float currentHealth = actor.GetHealth();

			HitInfo hit;
			hit.dir.zero();
			hit.type = CGameRules::EHitType::Fall;
			hit.shooterId = hit.targetId = hit.weaponId = actor.GetEntityId();

			const float maxDamage = (float) __fsel(velFraction - 1.0f, maxHealth, max(0.0f, (gEnv->bMultiCharacter ? maxHealth : currentHealth) - healthCVars.fallDamage_health_threshold));

			hit.damage = velFraction * maxDamage;

			g_pGame->GetGameRules()->ClientHit(hit);

#ifdef CHARACTER_MOVEMENT_DEBUG_ENABLED
			actor.GetMovementDebug().LogFallDamage(actor.GetEntity(), velFraction, downwardsImpactSpeed, hit.damage);
		}
		else
		{
			actor.GetMovementDebug().LogFallDamageNone(actor.GetEntity(), downwardsImpactSpeed);
		}
#else
		}
#endif
	}*/
}


bool CCharacterStateUtil::DoesArmorAbsorptFallDamage(CActor& actor, const float downwardsImpactSpeed, float& absorptedDamageFraction)
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


void CCharacterStateUtil::CancelCrouchAndProneInputs(CActor& actor)
{
	/*ICharacterInput* CharacterInputInterface = actor.GetCharacterInput();

	if (CharacterInputInterface && CharacterInputInterface->GetType() == ICharacterInput::CHARACTER_INPUT)
	{
		CCharacterInput * CharacterInput = static_cast<CCharacterInput*>(CharacterInputInterface);
		CharacterInput->ClearCrouchAction();
		CharacterInput->ClearProneAction();
	}*/
}


void CCharacterStateUtil::ChangeStance(CActor& actor, const SStateEvent& event)
{
	/*const SStateEventStanceChanged& stanceEvent = static_cast<const SStateEventStanceChanged&> (event).GetStance();
	const EStance stance = static_cast<EStance> (stanceEvent.GetStance());
	actor.OnSetStance(stance);*/
}
