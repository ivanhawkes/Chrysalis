#include <StdAfx.h>

#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include <Actor/Movement/StateMachine/ActorStateJump.h>
#include <Components/Actor/ActorControllerComponent.h>
/*
#include <IItem.h>
#include <IAnimatedCharacter.h>
#include "CharacterInput.h"
#include "NetCharacterInput.h"
#include "NetInputChainDebug.h"
#include "GameRules.h"
#include "GameRulesModules/IGameRulesObjectivesModule.h"
#include "GameCodeCoverage/GameCodeCoverageTracker.h"
#include "Weapon.h"*/


namespace Chrysalis
{
void CActorStateUtility::CalculateGroundOrJumpMovement(const CActorControllerComponent& actorControllerComponent, const bool bigWeaponRestrict, Vec3 &move)
{
	/*	const bool isPlayer = actorControllerComponent.IsPlayer();
		const float totalMovement = fabsf(actorControllerComponent.GetVelocity().x) + fabsf(actorControllerComponent.GetVelocity().y);
		const bool moving = (totalMovement > FLT_EPSILON);

		if (moving)
		{
			Vec3 desiredVelocityClamped = actorControllerComponent.GetVelocity();
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
				backwardMul = (float) __fsel(desiredVelocityClamped.y, 1.0f, LERP(backwardMul, actorControllerComponent.m_params.backwardMultiplier, -desiredVelocityClamped.y));
			}

			const Quat oldBaseQuat = actorControllerComponent.GetEntity()->GetWorldRotation(); // we cannot use m_baseQuat: that one is already updated to a new orientation while desiredVelocity was relative to the old entity frame
			move += oldBaseQuat.GetColumn0() * desiredVelocityClamped.x * strafeMul * backwardMul;
			move += oldBaseQuat.GetColumn1() * desiredVelocityClamped.y * backwardMul;
		}

		// AI can set a custom sprint value, so don't cap the movement vector.
		if (actorControllerComponent.GetSprint() <= 0.0f)
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
		float scale = actorControllerComponent.GetStanceMaxSpeed(actorControllerComponent.GetStance());
		move *= scale;

		if (isPlayer)
		{
			const bool isCrouching = ((actorControllerComponent.m_actions & ACTION_CROUCH) != 0);
			AdjustMovementForEnvironment(actorControllerComponent, move, bigWeaponRestrict, isCrouching);
		}*/
}


bool CActorStateUtility::IsOnGround(CActorControllerComponent& actorControllerComponent)
{
	//return(!actorControllerComponent.GetActorPhysics()->flags.AreAnyFlagsActive(SActorPhysics::EActorPhysicsFlags::Flying));
	// TODO: CRITICAL: HACK: BROKEN: !!
	return true;
}


bool CActorStateUtility::GetPhysicsLivingStatus(const CActorControllerComponent& actorControllerComponent, pe_status_living* physicsStatus)
{
	IPhysicalEntity *pPhysEnt = actorControllerComponent.GetEntity()->GetPhysics();
	if (pPhysEnt)
		return(pPhysEnt->GetStatus(physicsStatus) > 0);

	return false;
}


void CActorStateUtility::AdjustMovementForEnvironment(const CActorControllerComponent& actorControllerComponent, Vec3& move, const bool bigWeaponRestrict, const bool crouching)
{
	/*	float mult = (bigWeaponRestrict)
			? g_pGameCVars->pl_movement.nonCombat_heavy_weapon_speed_scale * actorControllerComponent.GetModifiableValues().GetValue(kPMV_HeavyWeaponMovementMultiplier)
			: 1.0f;

		if (gEnv->bMultiCharacter)
		{
			CGameRules *pGameRules = gEnv->pGameFramework->GetGameRules();
			IGameRulesObjectivesModule *pObjectives = pGameRules ? pGameRules->GetObjectivesModule() : nullptr;

			if (pObjectives)
			{
				switch (pGameRules->GetGameMode())
				{
					case eGM_Extraction:
					{
						if (pObjectives->CheckIsCharacterEntityUsingObjective(actorControllerComponent.GetEntityId())) // Carrying a tick
						{
							mult *= g_pGameCVars->mp_extractionParams.carryingTick_SpeedScale;
						}
						break;
					}

					case eGM_CaptureTheFlag:
					{
						if (pObjectives->CheckIsCharacterEntityUsingObjective(actorControllerComponent.GetEntityId())) // Carrying a flag
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

		if (actorControllerComponent.IsSprinting())
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


void CActorStateUtility::PhySetFly(CActorControllerComponent& actorControllerComponent)
{
	IPhysicalEntity* pPhysEnt = actorControllerComponent.GetEntity()->GetPhysics();
	if (pPhysEnt != nullptr)
	{
		pe_player_dynamics pd;
		pd.bSwimming = true;
		pd.kAirControl = 1.0f;
		pd.kAirResistance = 0.0f;
		pd.gravity.zero();
		pPhysEnt->SetParams(&pd);
	}
}


void CActorStateUtility::PhySetNoFly(CActorControllerComponent& actorControllerComponent, const Vec3& gravity)
{
	/*IPhysicalEntity* pPhysEnt = actorControllerComponent.GetEntity()->GetPhysics();
	if (pPhysEnt != nullptr)
	{
		pe_player_dynamics pd;
		pd.kAirControl = actorControllerComponent.GetAirControl();
		pd.kAirResistance = actorControllerComponent.GetAirResistance();
		pd.bSwimming = false;
		pd.gravity = gravity;
		actorControllerComponent.m_actorPhysics.gravity = gravity;
		pPhysEnt->SetParams(&pd);
	}*/
}


// #TODO: Investigate jumping.

bool CActorStateUtility::IsJumpAllowed(CActorControllerComponent& actorControllerComponent)
{
	/*const bool allowJump = actorControllerComponent.GetShouldJump() && !actorControllerComponent.IsJumping();

	if (allowJump)
	{
		// #TODO: We need a TryJump (stephenn).

		// This is needed when jumping while standing directly under something that causes an immediate land, before and without
		// even being airborne for one frame. CharacterMovement set m_actorState.durationOnGround=0.0f when the jump is triggered, which
		// prevents the on ground timer from before the jump to be inherited and incorrectly and prematurely used to
		// identify landing in MP.
		const SActorStats& stats = *actorControllerComponent.GetActorState();
		const SActorPhysics& actorPhysics = actorControllerComponent.GetActorPhysics();
		const float fUnconstrainedZ = actorPhysics.velocityUnconstrained.z;
		bool jumpFailed = (stats.durationOnGround > 0.0f) && (fUnconstrainedZ <= 0.0f);

		const float onGroundTime = 0.2f;
		if (((stats.durationOnGround > onGroundTime) || actorControllerComponent.IsRemote())) // && !jumpFailed )
		{
			return true;
		}
		else
		{
			CCCPOINT_IF(true, CharacterMovement_PressJumpWhileNotAllowedToJump);
		}
	}*/

	return actorControllerComponent.GetShouldJump();
}


void CActorStateUtility::RestorePhysics(CActorControllerComponent& actorControllerComponent)
{
	/*IAnimatedCharacter* pAC = actorControllerComponent.GetAnimatedCharacter();
	if (pAC)
	{
		SAnimatedCharacterParams params;

		params = pAC->GetParams();

		params.inertia = actorControllerComponent.GetInertia();
		params.inertiaAccel = actorControllerComponent.GetInertiaAccel();
		params.timeImpulseRecover = actorControllerComponent.GetTimeImpulseRecover();

		actorControllerComponent.SetAnimatedCharacterParams(params);
	}*/
}


void CActorStateUtility::UpdatePhysicsState(CActorControllerComponent& actorControllerComponent, SActorPhysics& actorPhysics, float frameTime)
{
	/*const int currentFrameID = gEnv->pRenderer->GetFrameID();

	if (actorPhysics.lastFrameUpdate < currentFrameID)
	{
		pe_status_living livStat;
		if (!CActorStateUtility::GetPhysicsLivingStatus(actorControllerComponent, &livStat))
		{
			return;
		}

		SActorStats& stats = *actorControllerComponent.GetActorState();

		const Vec3 newVelocity = livStat.vel - livStat.velGround;
		actorPhysics.velocityDelta = newVelocity - actorPhysics.velocity;
		actorPhysics.velocity = newVelocity;
		actorPhysics.velocityUnconstrainedLast = actorPhysics.velocityUnconstrained;
		actorPhysics.velocityUnconstrained = livStat.velUnconstrained;
		actorPhysics.flags.SetFlags(SActorPhysics:WasFlying, actorPhysics.flags.AreAnyFlagsActive(SActorPhysics::EActorPhysicsFlags::Flying));
		actorPhysics.flags.SetFlags(SActorPhysics::EActorPhysicsFlags::Flying, livStat.bFlying > 0);
		actorPhysics.flags.SetFlags(SActorPhysics::EActorPhysicsFlags::Stuck, livStat.bStuck > 0);

		Vec3 flatVel(actorControllerComponent.m_pCharacterRotation->GetBaseQuat().GetInverted()*newVelocity);
		flatVel.z = 0;
		stats.speedFlat = flatVel.len();

		if (actorControllerComponent.IsInAir())
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
					event.ptr = &actorControllerComponent;
					event.paramAsBool = false;
					pGameObject->SendEvent(event);
				}
			}

			if (newGroundColliderId)
			{
				if (IGameObject* pGameObject = gEnv->pGameFramework->GetGameObject(newGroundColliderId))
				{
					SGameObjectEvent event(eGFE_StoodOnChange, eGOEF_ToExtensions);
					event.ptr = &actorControllerComponent;
					event.paramAsBool = true;
					pGameObject->SendEvent(event);
				}
			}

			actorPhysics.groundColliderId = newGroundColliderId;
		}

		IPhysicalEntity *pPhysEnt = actorControllerComponent.GetEntity()->GetPhysics();
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
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelo", livStat.vel.GetLength());
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloX", livStat.vel.x);
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloY", livStat.vel.y);
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloZ", livStat.vel.z);

		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloUn", livStat.velUnconstrained.GetLength());
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnX", livStat.velUnconstrained.x);
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnY", livStat.velUnconstrained.y);
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVeloUnZ", livStat.velUnconstrained.z);

		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelReq", livStat.velRequested.GetLength());
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelReqX", livStat.velRequested.x);
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelReqY", livStat.velRequested.y);
		actorControllerComponent.GetMovementDebug().DebugGraph_AddValue("PhysVelReqZ", livStat.velRequested.z);

		actorControllerComponent.GetMovementDebug().LogVelocityStats(actorControllerComponent.GetEntity(), livStat, stats.downwardsImpactVelocity, stats.fallSpeed);
#endif
	}*/
}


bool CActorStateUtility::IsMovingForward(const CActorControllerComponent& actorControllerComponent)
{
	/*const float fSpeedFlatSelector = actorControllerComponent.GetActorState ()->speedFlat - 0.1f;
	bool movingForward = (fSpeedFlatSelector > 0.1f);

	if (!gEnv->bMultiCharacter || actorControllerComponent.IsClient ())
	{
	// IsMovingForward() doesn't return particularly reliable results for client characters in MP, I think because of
	// interpolation inaccuracies on the Y velocity. This was causing client characters to incorrectly report that they're
	// no longer moving forwards, whereas all that had happened was they had stopped sprinting - and this in turn was
	// messing up the MP sprint stamina regeneration delays. Client characters have also been seen to stop sprinting due to
	// turning lightly, whereas on their local machine they were still sprinting fine... so I've tried changing it so
	// IsMovingForward() is only taken into account on local clients (and in SP!).
	const float thresholdSinAngle = sin_tpl ((3.141592f * 0.5f) - DEG2RAD (g_pGameCVars->pl_power_sprint.foward_angle));
	const float currentSinAngle = actorControllerComponent.GetVelocity().y;

	movingForward = movingForward && (currentSinAngle > thresholdSinAngle);
	}

	return movingForward;*/

	return false;
}


bool CActorStateUtility::IsSprintingAllowed(const CActorControllerComponent& actorControllerComponent, IItem* pCurrentActorItem)
{
	/*bool shouldSprint = false;
	const SActorStats& stats = actorControllerComponent.m_actorState;
	const bool movingForward = IsMovingForward(actorControllerComponent);

	bool restrictSprint = actorControllerComponent.IsJumping() || stats.bIgnoreSprinting || actorControllerComponent.IsSliding() || actorControllerComponent.IsCinematicFlagActive(SActorStats::eCinematicFlag_RestrictMovement);

	CWeapon* pWeapon = pCurrentActorItem ? static_cast<CWeapon*>(pCurrentActorItem->GetIWeapon()) : nullptr;
	bool isZooming = pWeapon ? pWeapon->IsZoomed() && !pWeapon->IsZoomingInOrOut() : false;
	if (pWeapon && !pWeapon->CanSprint())
		restrictSprint = true;

	restrictSprint = restrictSprint || (actorControllerComponent.GetSprintStaminaLevel() <= 0.f);

	if (!actorControllerComponent.IsSprinting())
	{
		shouldSprint = movingForward && !restrictSprint && !isZooming;
		CCCPOINT_IF(shouldSprint, CharacterMovement_SprintOn);
	}
	else
	{
		shouldSprint = movingForward && !restrictSprint && !isZooming;

		shouldSprint = shouldSprint && (!(actorControllerComponent.m_actions & ACTION_CROUCH));

		if (!shouldSprint && pWeapon)
		{
			pWeapon->ForcePendingActions();
		}
	}

	CCCPOINT_IF(!actorControllerComponent.IsSprinting() && !shouldSprint && (actorControllerComponent.m_actions & ACTION_SPRINT), CharacterMovement_SprintRequestIgnored);
	CCCPOINT_IF(actorControllerComponent.IsSprinting() && !shouldSprint, CharacterMovement_SprintOff);

	return shouldSprint;*/

	return false;
}


void CActorStateUtility::ApplyFallDamage(CActorControllerComponent& actorControllerComponent, const float startFallingHeight, const float fHeightofEntity)
{
	/*CRY_ASSERT(actorControllerComponent.IsClient());

	// Zero downwards impact velocity used for fall damage calculations if actorControllerComponent was in water within the last 0.5 seconds.
	// Strength jumping straight up and down should theoretically land with a safe velocity,
	// but together with the water surface stickyness the velocity can sometimes go above the safe impact velocity threshold.

	// DEPRECATED: comment left for posterity in case dedicated server problems re-appear (author cannot test it).
	// On dedicated server the actorControllerComponent can still be flying this frame as well,
	// since synced pos from client is interpolated/smoothed and will not land immediately,
	// even though the velocity is set to zero.
	// Thus we need to use the velocity change instead of landing to identify impact.

	// DT: 12475: Falling a huge distance to a ledge grab results in no damage.
	// Now using the last velocity because when ledge grabbing the velocity is unchanged for this frame, thus zero damage is applied.
	// Assuming this a physics lag issue, using the last good velocity should be more-or-less ok.
	const float downwardsImpactSpeed = -(float) __fsel(-(actorControllerComponent.m_stateSwimWaterTestProxy.GetSwimmingTimer() + 0.5f), actorControllerComponent.GetActorPhysics()->velocityUnconstrainedLast.z, 0.0f);

	const SActorStats& stats = *actorControllerComponent.GetActorState();

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
			if (actorControllerComponent.GetStance() == STANCE_CROUCH)
			{
				static_cast<CCharacterInput*>(actorControllerComponent.GetCharacterInput())->ClearCrouchAction();
			}

			velFraction = powf(velFraction, gEnv->bMultiCharacter ? healthCVars.fallDamage_CurveAttackMP : healthCVars.fallDamage_CurveAttack);

			const float maxHealth = actorControllerComponent.GetMaxHealth();
			const float currentHealth = actorControllerComponent.GetHealth();

			HitInfo hit;
			hit.dir.zero();
			hit.type = CGameRules::EHitType::Fall;
			hit.shooterId = hit.targetId = hit.weaponId = actorControllerComponent.GetEntityId();

			const float maxDamage = (float) __fsel(velFraction - 1.0f, maxHealth, max(0.0f, (gEnv->bMultiCharacter ? maxHealth : currentHealth) - healthCVars.fallDamage_health_threshold));

			hit.damage = velFraction * maxDamage;

			gEnv->pGameFramework->GetGameRules()->ClientHit(hit);

#ifdef CHARACTER_MOVEMENT_DEBUG_ENABLED
			actorControllerComponent.GetMovementDebug().LogFallDamage(actorControllerComponent.GetEntity(), velFraction, downwardsImpactSpeed, hit.damage);
		}
		else
		{
			actorControllerComponent.GetMovementDebug().LogFallDamageNone(actorControllerComponent.GetEntity(), downwardsImpactSpeed);
		}
#else
		}
#endif
	}*/
}


bool CActorStateUtility::DoesArmorAbsorptFallDamage(CActorControllerComponent& actorControllerComponent, const float downwardsImpactSpeed, float& absorptedDamageFraction)
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


void CActorStateUtility::CancelCrouchAndProneInputs(CActorControllerComponent& actorControllerComponent)
{
	/*ICharacterInput* CharacterInputInterface = actorControllerComponent.GetCharacterInput();

	if (CharacterInputInterface && CharacterInputInterface->GetType() == ICharacterInput::CHARACTER_INPUT)
	{
		CCharacterInput * CharacterInput = static_cast<CCharacterInput*>(CharacterInputInterface);
		CharacterInput->ClearCrouchAction();
		CharacterInput->ClearProneAction();
	}*/
}


void CActorStateUtility::ChangeStance(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	const SStateEventStanceChanged& stanceEvent = static_cast<const SStateEventStanceChanged&> (event).GetStance();
	const EActorStance stance = static_cast<EActorStance> (stanceEvent.GetStance());
	actorControllerComponent.SetStance(stance);
}
}