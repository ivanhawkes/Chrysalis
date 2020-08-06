#include <StdAfx.h>

#include "ActorStateGround.h"
#include <Components/Actor/ActorControllerComponent.h>
#include "ActorStateJump.h"
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
/*#include "GameCodeCoverage/GameCodeCoverageTracker.h"
#include "MovementAction.h"*/


namespace Chrysalis
{
CActorStateGround::CActorStateGround()
	: m_inertiaIsZero(false)
{}


void CActorStateGround::OnEnter(CActorControllerComponent& actorControllerComponent)
{
	actorControllerComponent.durationInAir = 0.0f;

	// Ensure inertia is set!
	CActorStateUtility::RestorePhysics(actorControllerComponent);
}


void CActorStateGround::OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent,	float frameTime, const bool isHeavyWeapon, const bool isLocalPlayer)
{
	/*const Matrix34A baseMtx = Matrix34A(actorControllerComponent.GetBaseQuat());
	Matrix34A baseMtxZ(baseMtx * Matrix33::CreateScale(Vec3Constants<float>::fVec3_OneZ));
	baseMtxZ.SetTranslation(Vec3Constants<float>::fVec3_Zero);

	const CAutoAimManager& autoAimManager = gEnv->pGameFramework->GetAutoAimManager();
	const EntityId closeCombatTargetId = autoAimManager.GetCloseCombatSnapTarget();
	const IActor* pCloseCombatTarget = isLocalPlayer && closeCombatTargetId && actorControllerComponent.IsClient() ? CActorComponent::GetActor(closeCombatTargetId) : nullptr;

	// This is to restore inertia if the ProcessAlignToTarget set it previously.
	if (m_inertiaIsZero)
	{
		CActorStateUtility::RestorePhysics(actorControllerComponent);

		m_inertiaIsZero = false;
	}

	// Process movement.
	const bool isRemote = isLocalPlayer && !actorControllerComponent.IsClient();

	Vec3 move(ZERO);
	CActorStateUtility::CalculateGroundOrJumpMovement(actorControllerComponent, isHeavyWeapon, move);
	actorControllerComponent.GetMoveRequest().type = eCMT_Normal;

	// Apply movement.
	Vec3 desiredVel(ZERO);
	Vec3 entityPos = actorControllerComponent.GetEntity()->GetWorldPos();
	Vec3 entityRight(actorControllerComponent.GetBaseQuat().GetColumn0());

	hwvec3 xmDesiredVel = HWV3Zero();

	hwmtx33 xmBaseMtxZ;
	HWMtx33LoadAligned(xmBaseMtxZ, baseMtxZ);
	hwmtx33 xmBaseMtxZOpt = HWMtx33GetOptimized(xmBaseMtxZ);

	hwvec3 xmMove = HWVLoadVecUnaligned(&move);
	simdf fGroundNormalZ;

#ifdef STATE_DEBUG
	bool debugJumping = (g_pGameCVars->pl_debug_jumping != 0);
#endif

	const SActorStats& stats = *actorControllerComponent.GetActorState();

	{
		xmDesiredVel = xmMove;

		Vec3 groundNormal = actorControllerComponent.GetActorPhysics()->groundNormal;

		if (!gEnv->bMultiCharacter)
		{
			if (actorControllerComponent.IsAIControlled())
				fGroundNormalZ = SIMDFLoadFloat(square(groundNormal.z));
			else
				fGroundNormalZ = SIMDFLoadFloat(groundNormal.z);
		}
		else
		{
			// If the hill steepness is greater than our minimum threshold
			if (groundNormal.z > 1.f - cosf(g_pGameCVars->pl_movement.mp_slope_speed_multiplier_minHill))
			{
				// Check if we are trying to move up or downhill
				groundNormal.z = 0.f;
				groundNormal.Normalize();

				Vec3 moveDir = move;
				moveDir.z = 0.f;
				moveDir.Normalize();

				float normalDotMove = groundNormal.Dot(moveDir);
				//Apply speed multiplier based on moving up/down hill and hill steepness
				float multiplier = normalDotMove < 0.f ? g_pGameCVars->pl_movement.mp_slope_speed_multiplier_uphill : g_pGameCVars->pl_movement.mp_slope_speed_multiplier_downhill;
				fGroundNormalZ = SIMDFLoadFloat(1.f - (1.f - actorControllerComponent.GetActorPhysics()->groundNormal.z) * multiplier);
			}
			else
			{
				fGroundNormalZ = SIMDFLoadFloat(1.0f);
			}
		}

		const float depthHi = g_pGameCVars->cl_shallowWaterDepthHi;
		const float depthLo = g_pGameCVars->cl_shallowWaterDepthLo;
		const float relativeBottomDepth = actorControllerComponent.m_stateSwimWaterTestProxy.GetRelativeBottomDepth();

		if (relativeBottomDepth > depthLo)
		{ // Shallow water speed slowdown
			float shallowWaterMultiplier = 1.0f;

			shallowWaterMultiplier = isLocalPlayer
				? g_pGameCVars->cl_shallowWaterSpeedMulCharacter
				: g_pGameCVars->cl_shallowWaterSpeedMulAI;

			shallowWaterMultiplier = max(shallowWaterMultiplier, 0.1f);
			CRY_ASSERT(shallowWaterMultiplier <= 1.0f);

			float shallowWaterDepthSpan = (depthHi - depthLo);
			shallowWaterDepthSpan = max(0.1f, shallowWaterDepthSpan);
			float slowdownFraction = (relativeBottomDepth - depthLo) / shallowWaterDepthSpan;
			slowdownFraction = clamp_tpl(slowdownFraction, 0.0f, 1.0f);
			shallowWaterMultiplier = LERP(1.0f, shallowWaterMultiplier, slowdownFraction);
			shallowWaterMultiplier = (float) __fsel(-relativeBottomDepth, 1.0f, shallowWaterMultiplier);
			simdf vfShallowWaterMultiplier = SIMDFLoadFloat(shallowWaterMultiplier);
			xmDesiredVel = HWVMultiplySIMDF(xmDesiredVel, vfShallowWaterMultiplier);
		}
	}

	// Slow down on sloped terrain, simply proportional to the slope.
	xmDesiredVel = HWVMultiplySIMDF(xmDesiredVel, fGroundNormalZ);

	//be sure desired velocity is flat to the ground
	hwvec3 vDesiredVelVert = HWMtx33RotateVecOpt(xmBaseMtxZOpt, xmDesiredVel);

	xmDesiredVel = HWVSub(xmDesiredVel, vDesiredVelVert);

	HWVSaveVecUnaligned(&desiredVel, xmDesiredVel);

	if (isLocalPlayer)
	{
		Vec3 modifiedSlopeNormal = actorControllerComponent.GetActorPhysics()->groundNormal;
		float h = Vec2(modifiedSlopeNormal.x, modifiedSlopeNormal.y).GetLength(); // #TODO: OPT: sqrt(x*x+y*y)
		float v = modifiedSlopeNormal.z;
		float slopeAngleCur = RAD2DEG(atan2_tpl(h, v));

		const float divisorH = (float) __fsel(-h, 1.0f, h);
		const float divisorV = (float) __fsel(-v, 1.0f, v);

		const float invV = __fres(divisorV);
		const float invH = __fres(divisorH);

		const float slopeAngleHor = 10.0f;
		const float slopeAngleVer = 50.0f;
		float slopeAngleFraction = clamp_tpl((slopeAngleCur - slopeAngleHor) * __fres(slopeAngleVer - slopeAngleHor), 0.0f, 1.0f);

		slopeAngleFraction = slopeAngleFraction * slopeAngleFraction * slopeAngleFraction;

		float slopeAngleMod = LERP(0.0f, 90.0f, slopeAngleFraction);

		float s, c;

		sincos_tpl(DEG2RAD(slopeAngleMod), &s, &c);

		const float hMultiplier = (float) __fsel(-h, 1.0f, s * invH);
		const float vMultiplier = (float) __fsel(-v, 1.0f, c * invV);

		modifiedSlopeNormal.x *= hMultiplier;
		modifiedSlopeNormal.y *= hMultiplier;
		modifiedSlopeNormal.z *= vMultiplier;

		//Normalize the slope normal if possible
		const float fSlopeNormalLength = modifiedSlopeNormal.len();
		const float fSlopeNormalLengthSafe = (float) __fsel(fSlopeNormalLength - 0.000001f, fSlopeNormalLength, 1.0f);
		modifiedSlopeNormal = modifiedSlopeNormal * __fres(fSlopeNormalLengthSafe);

		float alignment = min(modifiedSlopeNormal * desiredVel, 0.0f);

		// Also affect air control (but not as much), to prevent jumping up against steep slopes.

		alignment *= (float) __fsel(-fabsf(stats.durationOnGround), LERP(0.7f, 1.0f, 1.0f - clamp_tpl(modifiedSlopeNormal.z * 100.0f, 0.0f, 1.0f)), 1.0f);

		modifiedSlopeNormal.z = modifiedSlopeNormal.z;

		desiredVel -= modifiedSlopeNormal * alignment;

#ifdef STATE_DEBUG
		if (debugJumping)
		{
			actorControllerComponent.DebugGraph_AddValue("GroundSlope", slopeAngleCur);
			actorControllerComponent.DebugGraph_AddValue("GroundSlopeMod", slopeAngleMod);
		}
#endif
	}

	Vec3 newVelocity = desiredVel;
	const float fNewSpeed = newVelocity.len();
	const float fVelocityMultiplier = (float) __fsel(fNewSpeed - 22.0f, __fres(fNewSpeed + FLT_EPSILON) * 22.0f, 1.0f);

	// #TODO: Maybe we should tell physics about this new velocity ? Or maybe SActorStats::velocity ? (stephenn).
	actorControllerComponent.GetMoveRequest().velocity = newVelocity * (stats.flashBangStunMult * fVelocityMultiplier);

#ifdef STATE_DEBUG
	if (g_pGameCVars->pl_debug_movement > 0)
	{
		const char* filter = g_pGameCVars->pl_debug_filter->GetString();
		const char* name = actorControllerComponent.GetEntity()->GetName();
		if ((strcmp(filter, "0") == 0) || (strcmp(filter, name) == 0))
		{
			float white [] = { 1.0f, 1.0f, 1.0f, 1.0f };
			IRenderAuxText::Draw2dLabel(20, 450, 2.0f, white, false, "Speed: %.3f m/s", actorControllerComponent.GetMoveRequest().velocity.len());

			if (g_pGameCVars->pl_debug_movement > 1)
			{
				IRenderAuxText::Draw2dLabel(35, 470, 1.8f, white, false, "Stance Speed: %.3f m/s - (%sSprinting)", actorControllerComponent.GetStanceMaxSpeed(actorControllerComponent.GetStance()), actorControllerComponent.IsSprinting() ? "" : "Not ");
			}
		}
	}
#endif

	if (isLocalPlayer)
	{
		CheckForVaultTrigger(actorControllerComponent, frameTime);
	}*/
}


bool CActorStateGround::CheckForVaultTrigger(CActorControllerComponent & actorControllerComponent, float frameTime)
{
	/*const int enableVaultFromStandingCVar = g_pGameCVars->pl_ledgeClamber.enableVaultFromStanding;
	const bool doCheck = (enableVaultFromStandingCVar == 3) || ((enableVaultFromStandingCVar > 0) && actorControllerComponent.m_jumpButtonIsPressed);

	if (doCheck)
	{
		SLedgeTransitionData ledgeTransition(LedgeId::invalid_id);
		const float zPos = actorControllerComponent.GetEntity()->GetWorldPos().z;
		const bool ignoreMovement = (enableVaultFromStandingCVar == 2);

		if (CActorStateLedge::TryLedgeGrab(actorControllerComponent, zPos, zPos, true, &ledgeTransition, ignoreMovement) && ledgeTransition.m_ledgeTransition != SLedgeTransitionData::eOLT_None)
		{
			CRY_ASSERT(LedgeId(ledgeTransition.m_nearestGrabbableLedgeId).IsValid());
			const SLedgeInfo ledgeInfo = gEnv->pGameFramework->GetLedgeManager()->GetLedgeById(LedgeId(ledgeTransition.m_nearestGrabbableLedgeId));

			CRY_ASSERT(ledgeInfo.IsValid());

			if (ledgeInfo.AreAnyFlagsSet(kLedgeFlag_useVault | kLedgeFlag_useHighVault))
			{
#ifdef STATE_DEBUG

				if (g_pGameCVars->pl_ledgeClamber.debugDraw)
				{
					const char * transitionName = s_ledgeTransitionNames [ledgeTransition.m_ledgeTransition];

					IEntity* pEntity = gEnv->pEntitySystem->GetEntity(ledgeInfo.GetEntityId());
					CryWatch("[LEDGEGRAB] $5%s nearest ledge: %s%s%s%s, transition=%s", actorControllerComponent.GetEntity()->GetEntityTextDescription(), pEntity ? pEntity->GetEntityTextDescription() : "none", ledgeInfo.AreFlagsSet(kLedgeFlag_isThin) ? " THIN" : "", ledgeInfo.AreFlagsSet(kLedgeFlag_isWindow) ? " WINDOW" : "", ledgeInfo.AreFlagsSet(kLedgeFlag_endCrouched) ? " ENDCROUCHED" : "", transitionName);
				}

#endif

				if (actorControllerComponent.m_jumpButtonIsPressed || enableVaultFromStandingCVar == 3)
				{
					ledgeTransition.m_comingFromOnGround = true;
					ledgeTransition.m_comingFromSprint = actorControllerComponent.IsSprinting();

					SStateEventLedge ledgeEvent(ledgeTransition);
					actorControllerComponent.StateMachineHandleEventMovement(ledgeEvent);
					return true;
				}
				else
				{
#ifdef STATE_DEBUG
					if (g_pGameCVars->pl_ledgeClamber.debugDraw)
					{
						const char * message = nullptr;
						switch (ledgeTransition.m_ledgeTransition)
						{
							case SLedgeTransitionData::eOLT_VaultOnto:
								message = "CLIMB";
								break;

							case SLedgeTransitionData::eOLT_VaultOver:
								message = "VAULT";
								break;

							default:
								CRY_ASSERT_TRACE(0, ("Unexpected ledge transition #%d when trying to display HUD prompt for vault-from-standing!", ledgeTransition.m_ledgeTransition));
								break;
						}

						if (message)
						{
							const float textColor [4] = { 1.f, 1.f, 1.f, 1.0f };
							const float bracketColor [4] = { 0.7f, 0.7f, 0.7f, 1.0f };
							const float iconSize = 4.f;
							const float textSize = 2.f;
							const float iconColor [4] = { 0.3f, 1.f, 0.3f, 1.0f };
							const char * iconText = "A";

							IRenderAuxText::Draw2dLabel((gEnv->pRenderer->GetWidth() * 0.5f), (gEnv->pRenderer->GetHeight() * 0.65f), iconSize, bracketColor, true, "( )");
							IRenderAuxText::Draw2dLabel((gEnv->pRenderer->GetWidth() * 0.5f), (gEnv->pRenderer->GetHeight() * 0.65f), iconSize, iconColor, true, "%s", iconText);
							IRenderAuxText::Draw2dLabel((gEnv->pRenderer->GetWidth() * 0.5f), (gEnv->pRenderer->GetHeight() * 0.72f), textSize, textColor, true, "%s", message);
						}
					}
#endif
				}
			}
		}
	}*/

	return false;
}


void CActorStateGround::OnUpdate(CActorControllerComponent& actorControllerComponent, float frameTime)
{}


void CActorStateGround::OnExit(CActorControllerComponent& actorControllerComponent)
{}
}