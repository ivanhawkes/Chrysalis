#include <StdAfx.h>

#include "CharacterStateUtil.h"
#include <IAnimatedCharacter.h>
#include <IItem.h>
#include <Game/Game.h>
#include <ConsoleVariables/ConsoleVariables.h>
#include "CharacterStateJump.h"
#include <Actor/Character/Character.h>
#include "Utility/CryWatch.h"
#include "IGameObject.h"
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


void CCharacterStateUtil::ProcessRotation(CCharacter& character, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest)
{
	auto pEntity = character.GetEntity();
	Quat rotationQuat = Quat(movementRequest.deltaAngles);

	// Physically rotate the character in the direction indicated by the request.
	pEntity->SetRotation(pEntity->GetRotation() * Quat(movementRequest.deltaAngles));

	// Rotate the animation in the inverse direction, this will have the effect of nullifying the physical rotation.
	// The animated character code will then take care of applying the required turning animations over the next few
	// frames - giving a smooth effect. 
	acRequest.rotation = rotationQuat.GetInverted();
}


void CCharacterStateUtil::FinalizeMovementRequest(CCharacter& character, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest)
{
	// Remote Characters stuff...
	UpdateRemoteCharactersInterpolation(character, movementRequest, acRequest);

	// Send the request to animated character.
	if (character.GetAnimatedCharacter())
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

		// Send the request into the animated character system.
		character.GetAnimatedCharacter()->AddMovement(acRequest);
	}

	// TODO: find out what this line was used for. Seems to be used for footstep speed and debug, plus doubles up to test
	// if we're moving. Might be a better way to handle all that.
	//character.m_lastRequestedVelocity = acRequest.velocity;

	// Reset the request for the next frame!
	InitializeMoveRequest(acRequest);
}


void CCharacterStateUtil::CalculateGroundOrJumpMovement(const CCharacter& character, const SActorMovementRequest& movementRequest, const bool bigWeaponRestrict, Vec3 &move)
{
	/*	const bool isPlayer = character.IsPlayer();
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
				backwardMul = (float) __fsel(desiredVelocityClamped.y, 1.0f, LERP(backwardMul, character.m_params.backwardMultiplier, -desiredVelocityClamped.y));
			}

			const Quat oldBaseQuat = character.GetEntity()->GetWorldRotation(); // we cannot use m_baseQuat: that one is already updated to a new orientation while desiredVelocity was relative to the old entity frame
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
		float scale = character.GetStanceMaxSpeed(character.GetStance());
		move *= scale;

		if (isPlayer)
		{
			const bool isCrouching = ((character.m_actions & ACTION_CROUCH) != 0);
			AdjustMovementForEnvironment(character, move, bigWeaponRestrict, isCrouching);
		}*/
}


bool CCharacterStateUtil::IsOnGround(CCharacter& character)
{
	return(!character.GetActorPhysics()->flags.AreAnyFlagsActive(SActorPhysics::EActorPhysicsFlags::Flying));
}


bool CCharacterStateUtil::GetPhysicsLivingStat(const CCharacter& character, pe_status_living* physicsStatus)
{
	IPhysicalEntity *pPhysEnt = character.GetEntity()->GetPhysics();
	if (pPhysEnt)
		return(pPhysEnt->GetStatus(physicsStatus) > 0);

	return false;
}


void CCharacterStateUtil::AdjustMovementForEnvironment(const CCharacter& character, Vec3& move, const bool bigWeaponRestrict, const bool crouching)
{
	/*	float mult = (bigWeaponRestrict)
			? g_pGameCVars->pl_movement.nonCombat_heavy_weapon_speed_scale * character.GetModifiableValues().GetValue(kPMV_HeavyWeaponMovementMultiplier)
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
						if (pObjectives->CheckIsCharacterEntityUsingObjective(character.GetEntityId())) // Carrying a tick
						{
							mult *= g_pGameCVars->mp_extractionParams.carryingTick_SpeedScale;
						}
						break;
					}

					case eGM_CaptureTheFlag:
					{
						if (pObjectives->CheckIsCharacterEntityUsingObjective(character.GetEntityId())) // Carrying a flag
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

		if (character.IsSprinting())
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


void CCharacterStateUtil::PhySetFly(CCharacter& character)
{
	IPhysicalEntity* pPhysEnt = character.GetEntity()->GetPhysics();
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


void CCharacterStateUtil::PhySetNoFly(CCharacter& character, const Vec3& gravity)
{
	/*IPhysicalEntity* pPhysEnt = character.GetEntity()->GetPhysics();
	if (pPhysEnt != NULL)
	{
		pe_player_dynamics pd;
		pd.kAirControl = character.GetAirControl();
		pd.kAirResistance = character.GetAirResistance();
		pd.bSwimming = false;
		pd.gravity = gravity;
		character.m_actorPhysics.gravity = gravity;
		pPhysEnt->SetParams(&pd);
	}*/
}


// TODO: Investigate jumping.

bool CCharacterStateUtil::IsJumpAllowed(CCharacter& character, const SActorMovementRequest& movementRequest)
{
	/*const bool allowJump = movementRequest.shouldJump && !character.IsJumping();

	if (allowJump)
	{
		// TODO: We need a TryJump (stephenn).

		// This is needed when jumping while standing directly under something that causes an immediate land, before and without
		// even being airborne for one frame. CharacterMovement set m_actorState.onGround=0.0f when the jump is triggered, which
		// prevents the on ground timer from before the jump to be inherited and incorrectly and prematurely used to
		// identify landing in MP.
		const SActorStats& stats = *character.GetActorState();
		const SActorPhysics& actorPhysics = character.GetActorPhysics();
		const float fUnconstrainedZ = actorPhysics.velocityUnconstrained.z;
		bool jumpFailed = (stats.onGround > 0.0f) && (fUnconstrainedZ <= 0.0f);

		const float onGroundTime = 0.2f;
		if (((stats.onGround > onGroundTime) || character.IsRemote())) // && !jumpFailed )
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


void CCharacterStateUtil::RestoreCharacterPhysics(CCharacter& character)
{
	/*IAnimatedCharacter* pAC = character.GetAnimatedCharacter();
	if (pAC)
	{
		SAnimatedCharacterParams params;

		params = pAC->GetParams();

		params.inertia = character.GetInertia();
		params.inertiaAccel = character.GetInertiaAccel();
		params.timeImpulseRecover = character.GetTimeImpulseRecover();

		character.SetAnimatedCharacterParams(params);
	}*/
}


void CCharacterStateUtil::UpdateCharacterPhysicsStats(CCharacter& character, SActorPhysics& actorPhysics, float frameTime)
{
	/*const int currentFrameID = gEnv->pRenderer->GetFrameID();

	if (actorPhysics.lastFrameUpdate < currentFrameID)
	{
		pe_status_living livStat;
		if (!CCharacterStateUtil::GetPhysicsLivingStat(character, &livStat))
		{
			return;
		}

		SActorStats& stats = *character.GetActorState();

		const Vec3 newVelocity = livStat.vel - livStat.velGround;
		actorPhysics.velocityDelta = newVelocity - actorPhysics.velocity;
		actorPhysics.velocity = newVelocity;
		actorPhysics.velocityUnconstrainedLast = actorPhysics.velocityUnconstrained;
		actorPhysics.velocityUnconstrained = livStat.velUnconstrained;
		actorPhysics.flags.SetFlags(SActorPhysics:WasFlying, actorPhysics.flags.AreAnyFlagsActive(SActorPhysics::EActorPhysicsFlags::Flying));
		actorPhysics.flags.SetFlags(SActorPhysics::EActorPhysicsFlags::Flying, livStat.bFlying > 0);
		actorPhysics.flags.SetFlags(SActorPhysics::EActorPhysicsFlags::Stuck, livStat.bStuck > 0);

		Vec3 flatVel(character.m_pCharacterRotation->GetBaseQuat().GetInverted()*newVelocity);
		flatVel.z = 0;
		stats.speedFlat = flatVel.len();

		if (character.IsInAir())
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
				if (IGameObject* pGameObject = gEnv->pGame->GetIGameFramework()->GetGameObject(actorPhysics.groundColliderId))
				{
					SGameObjectEvent event(eGFE_StoodOnChange, eGOEF_ToExtensions);
					event.ptr = &character;
					event.paramAsBool = false;
					pGameObject->SendEvent(event);
				}
			}

			if (newGroundColliderId)
			{
				if (IGameObject* pGameObject = gEnv->pGame->GetIGameFramework()->GetGameObject(newGroundColliderId))
				{
					SGameObjectEvent event(eGFE_StoodOnChange, eGOEF_ToExtensions);
					event.ptr = &character;
					event.paramAsBool = true;
					pGameObject->SendEvent(event);
				}
			}

			actorPhysics.groundColliderId = newGroundColliderId;
		}

		IPhysicalEntity *pPhysEnt = character.GetEntity()->GetPhysics();
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
		character.GetMovementDebug().DebugGraph_AddValue("PhysVelo", livStat.vel.GetLength());
		character.GetMovementDebug().DebugGraph_AddValue("PhysVeloX", livStat.vel.x);
		character.GetMovementDebug().DebugGraph_AddValue("PhysVeloY", livStat.vel.y);
		character.GetMovementDebug().DebugGraph_AddValue("PhysVeloZ", livStat.vel.z);

		character.GetMovementDebug().DebugGraph_AddValue("PhysVeloUn", livStat.velUnconstrained.GetLength());
		character.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnX", livStat.velUnconstrained.x);
		character.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnY", livStat.velUnconstrained.y);
		character.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnZ", livStat.velUnconstrained.z);

		character.GetMovementDebug().DebugGraph_AddValue("PhysVelReq", livStat.velRequested.GetLength());
		character.GetMovementDebug().DebugGraph_AddValue("PhysVelReqX", livStat.velRequested.x);
		character.GetMovementDebug().DebugGraph_AddValue("PhysVelReqY", livStat.velRequested.y);
		character.GetMovementDebug().DebugGraph_AddValue("PhysVelReqZ", livStat.velRequested.z);

		character.GetMovementDebug().LogVelocityStats(character.GetEntity(), livStat, stats.downwardsImpactVelocity, stats.fallSpeed);
#endif
	}*/
}


void CCharacterStateUtil::UpdateRemoteCharactersInterpolation(CCharacter& character, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& frameRequest)
{
	/*if (!gEnv->bMultiCharacter)
		return;


	// ***
	// *** Interpolation for remote Characters
	// ***

	const bool isRemoteClient = character.IsRemote();
	const bool doVelInterpolation = isRemoteClient && character.GetCharacterInput();

	if (doVelInterpolation)
	{
		if (g_pGameCVars->pl_clientInertia >= 0.0f)
		{
			character.m_inertia = g_pGameCVars->pl_clientInertia;
		}
		character.m_inertiaAccel = character.m_inertia;

		const bool isNetJumping = g_pGameCVars->pl_velocityInterpSynchJump && movementRequest.jump && isRemoteClient;

		if (isNetJumping)
		{
#ifdef STATE_DEBUG
			if (g_pGameCVars->pl_debugInterpolation)
			{
				CryWatch("SetVel (%f, %f, %f)", character.m_jumpVel.x, character.m_jumpVel.y, character.m_jumpVel.z);
			}
#endif //_RELEASE
			frameRequest.velocity = character.m_jumpVel;
			frameRequest.type = eCMT_JumpAccumulate;
		}
		else
		{
			CNetCharacterInput *CharacterInput = static_cast<CNetCharacterInput*>(character.GetCharacterInput());
			Vec3 interVel, desiredVel;

			bool inAir;
			desiredVel = CharacterInput->GetDesiredVelocity(inAir);
			interVel = desiredVel;

			IPhysicalEntity* pPhysEnt = character.GetEntity()->GetPhysics();
			CRY_ASSERT_MESSAGE(pPhysEnt, "Entity not physicalised! TomB would like to look at this.");

			const bool isCharacterInAir = character.IsInAir();
			if (inAir && isCharacterInAir)
			{
				if (pPhysEnt && (g_pGameCVars->pl_velocityInterpAirDeltaFactor < 1.0f))
				{
					pe_status_dynamics dynStat;
					pPhysEnt->GetStatus(&dynStat);

					Vec3  velDiff = interVel - dynStat.v;
					interVel = dynStat.v + (velDiff * g_pGameCVars->pl_velocityInterpAirDeltaFactor);
					frameRequest.velocity.z = interVel.z;
					frameRequest.velocity.z -= character.GetActorPhysics()->gravity.z*gEnv->pTimer->GetFrameTime();
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


bool CCharacterStateUtil::IsMovingForward(const CCharacter& character, const SActorMovementRequest& movementRequest)
{
	/*const float fSpeedFlatSelector = character.GetActorState ()->speedFlat - 0.1f;
	bool movingForward = (fSpeedFlatSelector > 0.1f);

	if (!gEnv->bMultiCharacter || character.IsClient ())
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


bool CCharacterStateUtil::IsSprintingAllowed(const CCharacter& character, const SActorMovementRequest& movementRequest, IItem* pCurrentCharacterItem)
{
	/*bool shouldSprint = false;
	const SActorStats& stats = character.m_actorState;
	const bool movingForward = IsMovingForward(character, movementRequest);

	bool restrictSprint = character.IsJumping() || stats.bIgnoreSprinting || character.IsSliding() || character.IsCinematicFlagActive(SActorStats::eCinematicFlag_RestrictMovement);

	CWeapon* pWeapon = pCurrentCharacterItem ? static_cast<CWeapon*>(pCurrentCharacterItem->GetIWeapon()) : NULL;
	bool isZooming = pWeapon ? pWeapon->IsZoomed() && !pWeapon->IsZoomingInOrOut() : false;
	if (pWeapon && !pWeapon->CanSprint())
		restrictSprint = true;

	restrictSprint = restrictSprint || (character.GetSprintStaminaLevel() <= 0.f);

	if (character.IsSprinting() == false)
	{
		shouldSprint = movingForward && !restrictSprint && !isZooming;
		CCCPOINT_IF(shouldSprint, CharacterMovement_SprintOn);
	}
	else
	{
		shouldSprint = movingForward && !restrictSprint && !isZooming;

		shouldSprint = shouldSprint && (!(character.m_actions & ACTION_CROUCH));

		if (!shouldSprint && pWeapon)
		{
			pWeapon->ForcePendingActions();
		}
	}

	CCCPOINT_IF(!character.IsSprinting() && !shouldSprint && (character.m_actions & ACTION_SPRINT), CharacterMovement_SprintRequestIgnored);
	CCCPOINT_IF(character.IsSprinting() && !shouldSprint, CharacterMovement_SprintOff);

	return shouldSprint;*/

	return false;
}


void CCharacterStateUtil::ApplyFallDamage(CCharacter& character, const float startFallingHeight, const float fHeightofEntity)
{
	/*CRY_ASSERT(character.IsClient());

	// Zero downwards impact velocity used for fall damage calculations if character was in water within the last 0.5 seconds.
	// Strength jumping straight up and down should theoretically land with a safe velocity,
	// but together with the water surface stickyness the velocity can sometimes go above the safe impact velocity threshold.

	// DEPRECATED: comment left for posterity in case dedicated server problems re-appear (author cannot test it).
	// On dedicated server the character can still be flying this frame as well,
	// since synced pos from client is interpolated/smoothed and will not land immediately,
	// even though the velocity is set to zero.
	// Thus we need to use the velocity change instead of landing to identify impact.

	// DT: 12475: Falling a huge distance to a ledge grab results in no damage.
	// Now using the last velocity because when ledge grabbing the velocity is unchanged for this frame, thus zero damage is applied.
	// Assuming this a physics lag issue, using the last good velocity should be more-or-less ok.
	const float downwardsImpactSpeed = -(float) __fsel(-(character.m_CharacterStateSwimWaterTestProxy.GetSwimmingTimer() + 0.5f), character.GetActorPhysics()->velocityUnconstrainedLast.z, 0.0f);

	const SActorStats& stats = *character.GetActorState();

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
			if (character.GetStance() == STANCE_CROUCH)
			{
				static_cast<CCharacterInput*>(character.GetCharacterInput())->ClearCrouchAction();
			}

			velFraction = powf(velFraction, gEnv->bMultiCharacter ? healthCVars.fallDamage_CurveAttackMP : healthCVars.fallDamage_CurveAttack);

			const float maxHealth = character.GetMaxHealth();
			const float currentHealth = character.GetHealth();

			HitInfo hit;
			hit.dir.zero();
			hit.type = CGameRules::EHitType::Fall;
			hit.shooterId = hit.targetId = hit.weaponId = character.GetEntityId();

			const float maxDamage = (float) __fsel(velFraction - 1.0f, maxHealth, max(0.0f, (gEnv->bMultiCharacter ? maxHealth : currentHealth) - healthCVars.fallDamage_health_threshold));

			hit.damage = velFraction * maxDamage;

			g_pGame->GetGameRules()->ClientHit(hit);

#ifdef CHARACTER_MOVEMENT_DEBUG_ENABLED
			character.GetMovementDebug().LogFallDamage(character.GetEntity(), velFraction, downwardsImpactSpeed, hit.damage);
		}
		else
		{
			character.GetMovementDebug().LogFallDamageNone(character.GetEntity(), downwardsImpactSpeed);
		}
#else
		}
#endif
	}*/
}


bool CCharacterStateUtil::DoesArmorAbsorptFallDamage(CCharacter& character, const float downwardsImpactSpeed, float& absorptedDamageFraction)
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


void CCharacterStateUtil::CancelCrouchAndProneInputs(CCharacter& character)
{
	/*ICharacterInput* CharacterInputInterface = character.GetCharacterInput();

	if (CharacterInputInterface && CharacterInputInterface->GetType() == ICharacterInput::CHARACTER_INPUT)
	{
		CCharacterInput * CharacterInput = static_cast<CCharacterInput*>(CharacterInputInterface);
		CharacterInput->ClearCrouchAction();
		CharacterInput->ClearProneAction();
	}*/
}


void CCharacterStateUtil::ChangeStance(CCharacter& character, const SStateEvent& event)
{
	/*const SStateEventStanceChanged& stanceEvent = static_cast<const SStateEventStanceChanged&> (event).GetStance();
	const EStance stance = static_cast<EStance> (stanceEvent.GetStance());
	character.OnSetStance(stance);*/
}
