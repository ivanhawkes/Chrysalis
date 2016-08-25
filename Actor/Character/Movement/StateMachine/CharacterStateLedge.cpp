#include <StdAfx.h>

#include <Actor/Character/Character.h>
#include <IItemSystem.h>
#include <Game/Game.h>
#include <ConsoleVariables/ConsoleVariables.h>
#include "CharacterStateLedge.h"
#include "IAnimatedCharacter.h"
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include "Environment/Ledge/LedgeManager.h"
#include "Utility/CryWatch.h"
#include "Actor/Animation/Player/PlayerAnimation.h"
/*#include "Weapon.h"
#include "StatsRecordingMgr.h"
#include "PersistantStats.h"
#include "CharacterInput.h"
#include "GameActions.h"
#include "Melee.h"*/


// TODO: *** Lots of cases of the word 'player' being bulk replaced with 'character'. These all need going through to split
// them out into what is now correct and what needs to return to being 'player'.


static const float s_CharacterMax2DPhysicsVelocity = 9.f;


class CActionLedgeGrab : public TPlayerAction
{
public:

	DEFINE_ACTION("LedgeGrab");

	CActionLedgeGrab(CCharacter &Character, const QuatT &ledgeLoc, SLedgeTransitionData::EOnLedgeTransition transition, bool endCrouched,
		bool comingFromOnGround, bool comingFromSprint)
		: TPlayerAction(EPlayerActionPriority::PP_PlayerActionUrgent, PlayerMannequin.fragmentIDs.ledgeGrab),
		m_Character(Character),
		m_targetViewDirTime(0.2f),
		m_transitionType(transition),
		m_endCrouched(endCrouched),
		m_comingFromOnGround(comingFromOnGround),
		m_comingFromSprint(comingFromSprint),
		m_haveUnHolsteredWeapon(false)
	{
		SetParam("TargetPos", ledgeLoc);

		Ang3 viewAng;
		viewAng.SetAnglesXYZ(ledgeLoc.q);
		viewAng.y = 0.0f;
		m_targetViewDir = Quat::CreateRotationXYZ(viewAng);
	}


	virtual void OnInitialise()
	{
		const CTagDefinition *fragTagDef = m_context->controllerDef.GetFragmentTagDef(m_fragmentID);
		if (fragTagDef)
		{
			const SMannequinPlayerParams::Fragments::SledgeGrab& ledgeGrabFragment = PlayerMannequin.fragments.ledgeGrab;

			// TODO - remove transition type and change over to just using the ledge's m_ledgeFlagBitfield this 
			// would simplify everything and remove a lot of the dependency on CHARACTER_Params.xml
			switch (m_transitionType)
			{
				case SLedgeTransitionData::eOLT_VaultOnto:
				case SLedgeTransitionData::eOLT_HighVaultOnto:
				{
					const TagID vaultID = ledgeGrabFragment.fragmentTagIDs.up;
					fragTagDef->Set(m_fragTags, vaultID, true);
					break;
				}

				case SLedgeTransitionData::eOLT_VaultOver:
				case SLedgeTransitionData::eOLT_VaultOverIntoFall: // this flag is now deprecated
				case SLedgeTransitionData::eOLT_HighVaultOver:
				case SLedgeTransitionData::eOLT_HighVaultOverIntoFall: // this flag probably shouldn't be needed either in that case
				{
					const TagID vaultID = ledgeGrabFragment.fragmentTagIDs.over;
					fragTagDef->Set(m_fragTags, vaultID, true);
					break;
				}

				case SLedgeTransitionData::eOLT_QuickLedgeGrab:
				{
					const TagID vaultID = ledgeGrabFragment.fragmentTagIDs.quick;
					fragTagDef->Set(m_fragTags, vaultID, true);
					break;
				}
				// perhaps set ledge off of midair and falling here. But ledge should be the default in data
			}

			switch (m_transitionType)
			{
				case SLedgeTransitionData::eOLT_HighVaultOverIntoFall:
				case SLedgeTransitionData::eOLT_HighVaultOnto:
				case SLedgeTransitionData::eOLT_HighVaultOver:
				{
					const TagID highID = ledgeGrabFragment.fragmentTagIDs.high;
					fragTagDef->Set(m_fragTags, highID, true);
					// INTENTIONALLY NO BREAK TO FALL THROUGH
				}

				case SLedgeTransitionData::eOLT_VaultOverIntoFall:
				case SLedgeTransitionData::eOLT_VaultOnto:
				case SLedgeTransitionData::eOLT_VaultOver:
				{
					/*const TagID vaultID = ledgeGrabFragment.fragmentTagIDs.vault;
					fragTagDef->Set (m_fragTags, vaultID, true);
					SActorStats *pCharacterStats = m_Character.GetActorState ();
					pCharacterStats->forceSTAP = SActorStats::eFS_On;	// force STAP on for vaults
					pCharacterStats->bDisableTranslationPinning = true; // disables translation pinning for vaults (stops 3P legs from cutting through camera)
					*/
					break;
				}
			}

			if (m_endCrouched)
			{
				const TagID vaultID = ledgeGrabFragment.fragmentTagIDs.endCrouched;
				fragTagDef->Set(m_fragTags, vaultID, true);
			}

			//--- Disabling this for now whilst position adjusted anims with trimmed start-times don't work
			//--- Once this is resolved we'll re-enable.
			//if (m_comingFromOnGround)
			//{
			const TagID vaultID = ledgeGrabFragment.fragmentTagIDs.floor;
			fragTagDef->Set(m_fragTags, vaultID, true);
			//}
			//else
			//{
			//	const TagID vaultID = ledgeGrabFragment.fragmentTagIDs.fall;
			//	fragTagDef->Set(m_fragTags, vaultID, true);
			//}

			//was sprinting
			if (m_comingFromSprint)
			{
				const TagID floorSprintID = ledgeGrabFragment.fragmentTagIDs.floorSprint;
				fragTagDef->Set(m_fragTags, floorSprintID, true);
			}
		}

	}


	virtual void Enter()
	{
		TPlayerAction::Enter();

		IAnimatedCharacter* pAnimChar = m_Character.GetAnimatedCharacter();

		if (pAnimChar)
		{
			pAnimChar->SetMovementControlMethods(eMCM_Animation, eMCM_Animation);
			pAnimChar->RequestPhysicalColliderMode(eColliderMode_Disabled, eColliderModeLayer_Game, "CActionLedgeGrab::Enter()");
		}
		switch (m_transitionType)
		{
			case SLedgeTransitionData::eOLT_VaultOverIntoFall:
			case SLedgeTransitionData::eOLT_VaultOnto:
			case SLedgeTransitionData::eOLT_VaultOver:
			case SLedgeTransitionData::eOLT_HighVaultOverIntoFall:
			case SLedgeTransitionData::eOLT_HighVaultOnto:
			case SLedgeTransitionData::eOLT_HighVaultOver:
				break;
			default:
			{
				/*PlayerCameraAnimationSettings cameraAnimationSettings;
				cameraAnimationSettings.positionFactor = 1.0f;
				cameraAnimationSettings.rotationFactor = g_pGameCVars->pl_ledgeClamber.cameraBlendWeight;
				m_Character.PartialAnimationControlled (true, cameraAnimationSettings);*/
			}
			break;
		}
	}


	virtual void Exit()
	{
		TPlayerAction::Exit();

		/*IAnimatedCharacter* pAnimChar = m_Character.GetAnimatedCharacter ();

		if (pAnimChar)
		{
		pAnimChar->SetMovementControlMethods (eMCM_Entity, eMCM_Entity);
		pAnimChar->ForceRefreshPhysicalColliderMode ();
		pAnimChar->RequestPhysicalColliderMode (eColliderMode_Undefined, eColliderModeLayer_Game, "CActionLedgeGrab::Enter()");
		}

		if (!m_haveUnHolsteredWeapon)
		{
		m_haveUnHolsteredWeapon = true;
		m_Character.HolsterItem_NoNetwork (false, true, 0.5f);
		}

		m_Character.PartialAnimationControlled (false, CharacterCameraAnimationSettings ());

		SActorStats *pCharacterStats = m_Character.GetActorState ();
		pCharacterStats->forceSTAP = SActorStats::eFS_None;
		pCharacterStats->bDisableTranslationPinning = false;*/

		m_Character.StateMachineHandleEventMovement(ACTOR_EVENT_LEDGE_ANIM_FINISHED);
	}


	virtual EStatus Update(float timePassed)
	{
		TPlayerAction::Update(timePassed);

		// Update view direction.
		float t = m_activeTime / m_targetViewDirTime;
		t = min(t, 1.0f);
		Quat rotNew;
		rotNew.SetSlerp(m_Character.GetViewRotation(), m_targetViewDir, t);
		m_Character.SetViewRotation(rotNew);

		if (!m_Character.IsThirdPerson())
		{
			if (m_activeTime > 0.5f && !m_haveUnHolsteredWeapon)
			{
				m_haveUnHolsteredWeapon = true;
				//m_Character.HolsterItem_NoNetwork (false, true, 0.5f);
			}
		}

		return m_eStatus;
	}

private:
	CCharacter& m_Character;
	Quat m_targetViewDir;
	float m_targetViewDirTime;

	SLedgeTransitionData::EOnLedgeTransition m_transitionType;

	bool m_endCrouched;
	bool m_comingFromOnGround;
	bool m_comingFromSprint;
	bool m_haveUnHolsteredWeapon;
};


CCharacterStateLedge::SLedgeGrabbingParams CCharacterStateLedge::s_ledgeGrabbingParams;


CCharacterStateLedge::CCharacterStateLedge()
	: m_onLedge(false)
	, m_ledgeSpeedMultiplier(1.0f)
	, m_lastTimeOnLedge(0.0f)
	, m_exitVelocity(ZERO)
	, m_ledgePreviousPosition(ZERO)
	, m_ledgePreviousPositionDiff(ZERO)
	, m_ledgePreviousRotation(ZERO)
	, m_enterFromGround(false)
	, m_enterFromSprint(false)
{}


CCharacterStateLedge::~CCharacterStateLedge()
{}


bool CCharacterStateLedge::CanGrabOntoLedge(const CCharacter& Character)
{
	if (Character.IsClient())
	{
		/*		const float kAirTime [2] = {0.5f, 0.0f};
				const float inAirTime = Character.CCharacter::GetActorState ()->inAir;
				const bool canGrabFromAir = (inAirTime >= kAirTime [Character.IsJumping ()]);
				const SActorStats& CharacterStats = *Character.GetActorState ();

				IItem *pCurrentItem = Character.GetCurrentItem ();
				if (pCurrentItem)
				{
				IWeapon *pIWeapon = pCurrentItem->GetIWeapon ();
				if (pIWeapon)
				{
				CWeapon *pWeapon = static_cast<CWeapon*>(pIWeapon);
				CMelee *pMelee = pWeapon->GetMelee ();
				if (pMelee)
				{
				if (pMelee->IsAttacking ())
				{
				return false; // can't ledge grab when we're currently melee-ing
				}
				}
				}
				}

				return canGrabFromAir && (CharacterStats.pickAndThrowEntity == 0) && (Character.IsMovementRestricted () == false);*/
	}

	return false;
}


void CCharacterStateLedge::OnEnter(CCharacter& Character, const SStateEventLedge& ledgeEvent)
{
	const SLedgeTransitionData::EOnLedgeTransition ledgeTransition = ledgeEvent.GetLedgeTransition();
	const LedgeId nearestGrabbableLedgeId(ledgeEvent.GetLedgeId());
	const bool isClient = Character.IsClient();

	// Record 'LedgeGrab' telemetry and game stats.
	/*if (ledgeTransition != SLedgeTransitionData::eOLT_None)
	{
	CStatsRecordingMgr::TryTrackEvent (&Character, eGSE_LedgeGrab);
	if (isClient)
	{
	g_pGame->GetPersistantStats ()->IncrementClientStats (EIPS_LedgeGrabs);
	}
	}

	m_onLedge = true;
	m_postSerializeLedgeTransition = ledgeTransition;

	if (IAnimatedCharacter* pAnimChar = Character.GetAnimatedCharacter ())
	{
	// Current weapon and input setup
	IItem* pCurrentItem = Character.GetCurrentItem ();
	CWeapon* pCurrentWeapon = (pCurrentItem != NULL) ? static_cast<CWeapon*>(pCurrentItem->GetIWeapon ()) : NULL;

	if (pCurrentWeapon != NULL)
	{
	pCurrentWeapon->FumbleGrenade ();
	pCurrentWeapon->CancelCharge ();
	}

	m_ledgeId = nearestGrabbableLedgeId;
	const SLedgeInfo ledgeInfo = g_pGame->GetLedgeManager ()->GetLedgeById (nearestGrabbableLedgeId);
	CRY_ASSERT (ledgeInfo.IsValid ());

	const bool useVault = ledgeInfo.AreAnyFlagsSet (kLedgeFlag_useVault | kLedgeFlag_useHighVault);
	if (!useVault)
	{
	Character.HolsterItem_NoNetwork (true);

	ICharacterInput* pCharacterInput = Character.GetCharacterInput ();
	if ((pCharacterInput != NULL) && (pCharacterInput->GetType () == ICharacterInput::CHARACTER_INPUT))
	{
	static_cast<CCharacterInput*>(pCharacterInput)->ForceStopSprinting ();
	}

	if (isClient)
	{
	g_pGameActions->FilterLedgeGrab ()->Enable (true);
	}

	if (ledgeInfo.AreFlagsSet (kLedgeFlag_endCrouched))
	{
	Character.OnSetStance (STANCE_CROUCH);
	}
	}
	else
	{
	if (pCurrentWeapon != NULL)
	{
	pCurrentWeapon->ClearDelayedFireTimeout ();
	}

	if (isClient)
	{
	g_pGameActions->FilterVault ()->Enable (true);
	}
	}

	// Figure out speed based on factors like ledge type
	m_ledgeSpeedMultiplier = GetLedgeGrabSpeed (Character);

	SLedgeTransitionData::EOnLedgeTransition transition = ledgeTransition;
	if ((transition > SLedgeTransitionData::eOLT_None) && (transition < SLedgeTransitionData::eOLT_MaxTransitions))
	{
	const SLedgeBlendingParams& ledgeGrabParams = GetLedgeGrabbingParams ().m_grabTransitionsParams [transition];
	{
	m_exitVelocity = ledgeGrabParams.m_vExitVelocity;
	}

	StartLedgeBlending (Character, ledgeGrabParams);
	}

	m_ledgePreviousPosition = ledgeInfo.GetPosition ();
	m_ledgePreviousRotation.SetRotationVDir (ledgeInfo.GetFacingDirection ());

	// Notify script side of ledge entity
	IEntity *pLedgeEntity = gEnv->pEntitySystem->GetEntity (ledgeInfo.GetEntityId ());
	if (pLedgeEntity != NULL)
	{
	EntityScripts::CallScriptFunction (pLedgeEntity, pLedgeEntity->GetScriptTable (), "StartUse", ScriptHandle (Character.GetEntity ()));
	}

	// Queue Mannequin action
	const bool endCrouched = ledgeInfo.AreFlagsSet (kLedgeFlag_endCrouched);
	const bool comingFromOnGround = ledgeEvent.GetComingFromOnGround ();
	const bool comingFromSprint = ledgeEvent.GetComingFromSprint ();

	m_enterFromGround = comingFromOnGround;
	m_enterFromSprint = comingFromSprint;

	CActionLedgeGrab *pLedgeGrabAction = new CActionLedgeGrab (Character, m_ledgeBlending.m_qtTargetLocation, transition, endCrouched, comingFromOnGround, comingFromSprint);
	pLedgeGrabAction->SetSpeedBias (m_ledgeSpeedMultiplier);
	Character.GetAnimatedCharacter ()->GetActionController ()->Queue (pLedgeGrabAction);

	if (!Character.IsRemote ())
	{
	Character.HasClimbedLedge (nearestGrabbableLedgeId, comingFromOnGround, comingFromSprint);
	}
	}*/
}


void CCharacterStateLedge::OnPrePhysicsUpdate(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime)
{
	const bool gameIsPaused = (frameTime == 0.0f);

	IF_UNLIKELY(gameIsPaused)
	{
		return;
	}

	/*Character.GetMoveRequest ().type = eCMT_Fly;

	Vec3 posDiff (ZERO);
	const SLedgeInfo ledgeInfo = g_pGame->GetLedgeManager ()->GetLedgeById (m_ledgeId);
	if (ledgeInfo.IsValid ())
	{
	const Vec3 ledgePos = ledgeInfo.GetPosition ();
	const Quat ledgeRot = Quat::CreateRotationVDir (ledgeInfo.GetFacingDirection ());

	if ((ledgePos != m_ledgePreviousPosition) || (ledgeRot != m_ledgePreviousRotation))
	{
	posDiff = ledgePos - m_ledgePreviousPosition;

	const Quat rotToUse = ledgeRot * Quat::CreateRotationZ (DEG2RAD (180.f));

	Character.GetAnimatedCharacter ()->ForceMovement (QuatT (posDiff, Quat (IDENTITY)));
	Character.GetAnimatedCharacter ()->ForceOverrideRotation (rotToUse);

	m_ledgePreviousPosition = ledgePos;
	m_ledgePreviousRotation = ledgeRot;
	}

	m_ledgePreviousPositionDiff = posDiff;

	#ifdef STATE_DEBUG
	if (g_pGameCVars->pl_ledgeClamber.debugDraw)
	{
	const float XPOS = 200.0f;
	const float YPOS = 80.0f;
	const float FONT_SIZE = 2.0f;
	const float FONT_COLOUR [4] = {1, 1, 1, 1};
	Vec3 pos = Character.GetEntity ()->GetPos ();
	gEnv->pRenderer->Draw2dLabel (XPOS, YPOS + 20.0f, FONT_SIZE, FONT_COLOUR, false, "Ledge Grab: Cur: (%f %f %f) Tgt: (%f %f %f) T:%f", pos.x, pos.y, pos.z, m_ledgeBlending.m_qtTargetLocation.t.x, m_ledgeBlending.m_qtTargetLocation.t.y, m_ledgeBlending.m_qtTargetLocation.t.z, frameTime);
	gEnv->pRenderer->GetIRenderAuxGeom ()->DrawSphere (m_ledgeBlending.m_qtTargetLocation.t, 0.15f, ColorB (0, 0, 255, 255));
	gEnv->pRenderer->GetIRenderAuxGeom ()->DrawSphere (pos, 0.1f, ColorB (255, 0, 0, 255));

	}
	#endif //_RELEASE

	#if 1
	float desiredSpeed = 0.f;
	IMovementController *pMovementController = Character.GetMovementController ();
	if (pMovementController)
	{
	SMovementState curMovementState;
	pMovementController->GetMovementState (curMovementState);

	desiredSpeed = curMovementState.desiredSpeed;
	}

	// If we are falling or we are pushing to move on our inputs
	// pass through our moveRequest to keep the motion and mann tags consistent
	if ((ledgeInfo.AreAnyFlagsSet (kLedgeFlag_useVault | kLedgeFlag_useHighVault) && ledgeInfo.AreFlagsSet (kLedgeFlag_endFalling))
	|| desiredSpeed > 0.1f)
	{
	Vec3& moveRequestVelocity = Character.GetMoveRequest ().velocity;
	moveRequestVelocity = m_ledgeBlending.m_qtTargetLocation.q * m_exitVelocity;

	if (!posDiff.IsZeroFast () && (g_pGameCVars->g_ledgeGrabMovingledgeExitVelocityMult + frameTime > 0.f))
	{
	moveRequestVelocity += posDiff*__fres (frameTime)*g_pGameCVars->g_ledgeGrabMovingledgeExitVelocityMult;
	}
	}
	#endif
	}*/
}


void CCharacterStateLedge::OnAnimFinished(CCharacter &Character)
{
	/*	const SLedgeInfo ledgeInfo = g_pGame->GetLedgeManager ()->GetLedgeById (m_ledgeId);
		if (ledgeInfo.IsValid ())
		{
		if (ledgeInfo.AreAnyFlagsSet (kLedgeFlag_useVault | kLedgeFlag_useHighVault) && ledgeInfo.AreFlagsSet (kLedgeFlag_endFalling))
		{
		Character.StateMachineHandleEventMovement (ACTOR_EVENT_FALL);
		}
		else
		{
		Character.StateMachineHandleEventMovement (ACTOR_EVENT_GROUND);
		}
		}

		Vec3& moveRequestVelocity = Character.GetMoveRequest ().velocity;
		Character.GetMoveRequest ().jumping = true;
		moveRequestVelocity = m_ledgeBlending.m_qtTargetLocation.q * m_exitVelocity;

		float frameTime = gEnv->pTimer->GetFrameTime ();
		if (!m_ledgePreviousPositionDiff.IsZeroFast () && (g_pGameCVars->g_ledgeGrabMovingledgeExitVelocityMult * frameTime > 0.f))
		{
		moveRequestVelocity += m_ledgePreviousPositionDiff *__fres (frameTime)*g_pGameCVars->g_ledgeGrabMovingledgeExitVelocityMult;
		}*/
}


void CCharacterStateLedge::OnExit(CCharacter& Character)
{
	IAnimatedCharacter* pAnimatedCharacter = Character.GetAnimatedCharacter();

	/*if ((pAnimatedCharacter != NULL) && (m_ledgeId.IsValid ()))
	{
	const SLedgeInfo ledgeInfo = g_pGame->GetLedgeManager ()->GetLedgeById (m_ledgeId);
	CRY_ASSERT (ledgeInfo.IsValid ());

	const bool useVault = ledgeInfo.AreAnyFlagsSet (kLedgeFlag_useVault | kLedgeFlag_useHighVault);
	const bool isClient = Character.IsClient ();
	if (isClient)
	{
	if (useVault)
	{
	g_pGameActions->FilterVault ()->Enable (false);
	}
	else
	{
	g_pGameActions->FilterLedgeGrab ()->Enable (false);
	}
	}

	Character.m_params.viewLimits.ClearViewLimit (SViewLimitParams::eVLS_Ledge);

	// Added to remove the curious lean the Character has after performing a ledge grab.
	Quat viewRotation = Character.GetViewRotation ();
	Ang3 viewAngles;
	viewAngles.SetAnglesXYZ (viewRotation);
	viewAngles.y = 0.0f;
	viewRotation = Quat::CreateRotationXYZ (viewAngles);
	Character.SetViewRotation (viewRotation);

	pAnimatedCharacter->ForceOverrideRotation (Character.GetEntity ()->GetWorldRotation ());

	if (ledgeInfo.AreFlagsSet (kLedgeFlag_endCrouched))
	{
	if (isClient)
	{
	ICharacterInput* pInput = Character.GetCharacterInput ();
	if ((pInput != NULL) && (pInput->GetType () == ICharacterInput::CHARACTER_INPUT))
	{
	static_cast<CCharacterInput*>(pInput)->AddCrouchAction ();
	}

	if (Character.IsThirdPerson () == false)
	{
	Character.m_torsoAimIK.Enable (true); //Force snap for smooth transition
	}
	}
	}
	}*/

	m_ledgeId = LedgeId();
}


void CCharacterStateLedge::UpdateNearestGrabbableLedge(const CCharacter& Character, SLedgeTransitionData* pLedgeState, const bool ignoreCharacterMovement)
{
	/*	pLedgeState->m_ledgeTransition = SLedgeTransitionData::eOLT_None;
		pLedgeState->m_nearestGrabbableLedgeId = LedgeId::invalid_id;

		// Get the position and movement direction for this entity.
		// Prepare as parameters for the ledge manager.
		const Matrix34& mtxWorldTm = Character.GetEntity ()->GetWorldTM ();
		Vec3 vPosition = mtxWorldTm.GetTranslation ();

		vPosition += Vec3 (0.0f, 0.0f, Character.GetStanceInfo (Character.GetStance ())->viewOffset.z);

		Vec3 vScanDirection = mtxWorldTm.TransformVector (GetLedgeGrabbingParams ().m_ledgeNearbyParams.m_vSearchDir);
		vScanDirection.NormalizeFast ();

		// Retrieve the ledge manager and run a query on whether a ledge is within the parameters.
		const CCharacterStateLedge::SLedgeNearbyParams& nearbyParams = GetLedgeGrabbingParams ().m_ledgeNearbyParams;
		const LedgeId ledgeId = g_pGame->GetLedgeManager ()->FindNearestLedge (vPosition, vScanDirection, GetLedgeGrabbingParams ().m_ledgeNearbyParams.m_fMaxDistance, DEG2RAD (nearbyParams.m_fMaxAngleDeviationFromSearchDirInDegrees), DEG2RAD (nearbyParams.m_fMaxExtendedAngleDeviationFromSearchDirInDegrees));
		if (ledgeId.IsValid ())
		{
		CWeapon* pCurrentWeapon = Character.GetWeapon (Character.GetCurrentItemId ());
		const bool canLedgeGrabWithWeapon = (pCurrentWeapon == NULL) || (pCurrentWeapon->CanLedgeGrab ());
		if (canLedgeGrabWithWeapon)
		{
		const bool characterMovesTowardLedge = IsCharacterMovingTowardsLedge (Character, ledgeId);
		const bool ledgePointsTowardCharacter = IsLedgePointingTowardCharacter (Character, ledgeId);
		const bool isPressingJump = Character.m_jumpButtonIsPressed;
		if ((characterMovesTowardLedge || ignoreCharacterMovement) && ledgePointsTowardCharacter)
		{
		pLedgeState->m_nearestGrabbableLedgeId = ledgeId;
		pLedgeState->m_ledgeTransition = GetBestTransitionToLedge (Character, Character.GetEntity ()->GetWorldPos (), ledgeId, pLedgeState);
		}
		}
		}*/
}


SLedgeTransitionData::EOnLedgeTransition CCharacterStateLedge::GetBestTransitionToLedge(const CCharacter& Character, const Vec3& refPosition, const LedgeId& ledgeId, SLedgeTransitionData* pLedgeState)
{
	/*	CRY_ASSERT_MESSAGE (ledgeId.IsValid (), "Ledge index should be valid");
		const CLedgeManager* pLedgeManager = g_pGame->GetLedgeManager ();
		const SLedgeInfo detectedLedge = pLedgeManager->GetLedgeById (ledgeId);

		CRY_ASSERT (detectedLedge.IsValid ());
		const bool useVault = detectedLedge.AreFlagsSet (kLedgeFlag_useVault);
		const bool useHighVault = detectedLedge.AreFlagsSet (kLedgeFlag_useHighVault);
		const bool isThinLedge = detectedLedge.AreFlagsSet (kLedgeFlag_isThin);
		const bool endFalling = detectedLedge.AreFlagsSet (kLedgeFlag_endFalling);
		const bool usableByMarines = detectedLedge.AreFlagsSet (kledgeFlag_usableByMarines);
		const Vec3 ledgePos = detectedLedge.GetPosition ();

		if (CanReachPlatform (Character, ledgePos, refPosition))
		{
		return SLedgeTransitionData::eOLT_None;
		}

		QuatT targetLocation;

		const Quat pRot = Quat::CreateRotationVDir (detectedLedge.GetFacingDirection ());

		const Vec3 vToVec = pLedgeManager->FindVectorToClosestPointOnLedge (refPosition, detectedLedge);
		targetLocation.q = pRot * Quat::CreateRotationZ (DEG2RAD (180));

		float bestDistanceSqr = 100.0f;
		SLedgeTransitionData::EOnLedgeTransition bestTransition = SLedgeTransitionData::eOLT_None;

		// If required could add handling to reject ledges based on marines within CLedgeManager::FindNearestLedge()
		if (!usableByMarines)
		{
		return SLedgeTransitionData::eOLT_None;
		}

		// Get nearest entry point, from available transitions
		for (int i = 0; i < SLedgeTransitionData::eOLT_MaxTransitions; ++i)
		{
		const SLedgeBlendingParams& ledgeBlendParams = GetLedgeGrabbingParams ().m_grabTransitionsParams [i];
		targetLocation.t = refPosition + vToVec - (Vec3 (0, 0, ledgeBlendParams.m_vPositionOffset.z)) - ((targetLocation.q * FORWARD_DIRECTION) * ledgeBlendParams.m_vPositionOffset.y);

		if ((ledgeBlendParams.m_ledgeType == eLT_Thin) && !isThinLedge)
		{
		continue;
		}
		else if ((ledgeBlendParams.m_ledgeType == eLT_Wide) && isThinLedge)
		{
		continue;
		}
		else if (ledgeBlendParams.m_bIsVault != useVault)
		{
		continue;
		}
		else if (ledgeBlendParams.m_bIsHighVault != useHighVault)
		{
		continue;
		}
		else if (ledgeBlendParams.m_bEndFalling != endFalling)
		{
		continue;
		}

		float distanceSqr = (targetLocation.t - refPosition).len2 ();
		if (distanceSqr < bestDistanceSqr)
		{
		bestTransition = (SLedgeTransitionData::EOnLedgeTransition)(i);
		bestDistanceSqr = distanceSqr;
		}
		}

		return bestTransition;*/

	return SLedgeTransitionData::EOnLedgeTransition::eOLT_None;
}


bool CCharacterStateLedge::CanReachPlatform(const CCharacter& Character, const Vec3& ledgePosition, const Vec3& refPosition)
{
	/*const SActorStats& stats = *Character.GetActorState ();
	const SActorPhysics& actorPhysics = Character.GetActorPhysics ();

	const Vec3 distanceVector = ledgePosition - refPosition;

	// Some rough prediction for vertical movement...
	const Vec2 distanceVector2D = Vec2 (distanceVector.x, distanceVector.y);
	const float distance2DtoWall = distanceVector2D.GetLength ();
	const float estimatedTimeToReachWall = stats.speedFlat ? distance2DtoWall / stats.speedFlat : distance2DtoWall;

	const float predictedHeightWhenReachingWall = refPosition.z + (actorPhysics.velocity.z * estimatedTimeToReachWall) + (0.5f * actorPhysics.gravity.z * (estimatedTimeToReachWall*estimatedTimeToReachWall));
	const float predictedHeightWhenReachingWallDiff = (ledgePosition.z - predictedHeightWhenReachingWall);

	// If falling already ...
	if (actorPhysics.velocity.z < 0.0f)
	{
	return (predictedHeightWhenReachingWallDiff < 0.1f);
	}
	else if ((fabs (actorPhysics.gravity.z) > 0.0f) && !gEnv->bMultiCharacter) // Causes problems with network in multiCharacter
	{
	//Going up ...
	const float estimatedTimeToReachZeroVerticalSpeed = -actorPhysics.velocity.z / actorPhysics.gravity.z;
	const float predictedHeightWhenReachingZeroSpeed = refPosition.z + (actorPhysics.velocity.z * estimatedTimeToReachZeroVerticalSpeed) + (0.5f * actorPhysics.gravity.z * (estimatedTimeToReachZeroVerticalSpeed*estimatedTimeToReachZeroVerticalSpeed));
	const float predictedHeightWhenReachingZeroSpeedDiff = (ledgePosition.z - predictedHeightWhenReachingZeroSpeed);

	return ((predictedHeightWhenReachingZeroSpeedDiff < 0.2f) && !Character.m_jumpButtonIsPressed);
	}

	return false;*/

	return false;
}


QuatT CCharacterStateLedge::CalculateLedgeOffsetLocation(const Matrix34& worldMat, const Vec3& vPositionOffset, const bool keepOrientation) const
{
	/*QuatT targetLocation;
	const Vec3& vWorldPos = worldMat.GetTranslation ();
	const bool doOrientationBlending = !keepOrientation;

	const CLedgeManager* pLedgeManager = g_pGame->GetLedgeManager ();
	CRY_ASSERT (m_ledgeId.IsValid ());

	const SLedgeInfo& ledgeInfo = pLedgeManager->GetLedgeById (m_ledgeId);
	CRY_ASSERT (ledgeInfo.IsValid ());

	const Quat pRot = doOrientationBlending ? Quat::CreateRotationVDir (ledgeInfo.GetFacingDirection ()) : Quat (worldMat);

	const Vec3 vToVec = pLedgeManager->FindVectorToClosestPointOnLedge (vWorldPos, ledgeInfo);

	targetLocation.q = doOrientationBlending ? pRot * Quat::CreateRotationZ (DEG2RAD (180)) : pRot;
	targetLocation.t = vWorldPos + vToVec;

	return targetLocation;*/

	return QuatT(IDENTITY);
}


void CCharacterStateLedge::StartLedgeBlending(CCharacter& Character, const SLedgeBlendingParams &blendingParams)
{
	CRY_ASSERT(m_ledgeSpeedMultiplier > 0);
	m_ledgeBlending.m_qtTargetLocation = CalculateLedgeOffsetLocation(Character.GetEntity()->GetWorldTM(), blendingParams.m_vPositionOffset, blendingParams.m_bKeepOrientation);
	Vec3 forceLookVector = m_ledgeBlending.m_qtTargetLocation.GetColumn1();
	forceLookVector.z = 0.0f;
	forceLookVector.Normalize();
	m_ledgeBlending.m_forwardDir = forceLookVector;
}


bool CCharacterStateLedge::IsCharacterMovingTowardsLedge(const CCharacter& Character, const LedgeId& ledgeId)
{
	CRY_ASSERT_MESSAGE(ledgeId.IsValid(), "Ledge index should be valid");

	// If stick is pressed forwards, always take this ledge 0.2f --> big enough to allow some room for stick deadzone.
	/*const Vec3 inputMovement = Character.GetBaseQuat ().GetInverted () * Character.GetLastRequestedVelocity ();
	if (inputMovement.y >= 0.2f)
	{
	const IEntity* pCharacterEntity = Character.GetEntity ();
	const Vec3 vCharacterPos = pCharacterEntity->GetWorldPos ();

	const CLedgeManager* pLedgeManager = g_pGame->GetLedgeManager ();
	const SLedgeInfo detectedLedge = pLedgeManager->GetLedgeById (ledgeId);
	if (detectedLedge.IsValid ())
	{
	Vec3 vCharactertoLedge = pLedgeManager->FindVectorToClosestPointOnLedge (vCharacterPos, detectedLedge);

	vCharactertoLedge.z = 0.0f;

	Vec3 vVelocity = Character.GetActorPhysics()->velocity;
	vVelocity.z = 0.0f;

	return (vCharactertoLedge * vVelocity > 0.0f);
	}
	}

	return false;*/

	return false;
}


bool CCharacterStateLedge::IsLedgePointingTowardCharacter(const CCharacter& Character, const LedgeId& ledgeId)
{
	CRY_ASSERT_MESSAGE(ledgeId.IsValid(), "Ledge index should be valid");

	/*	const CLedgeManager* pLedgeManager = g_pGame->GetLedgeManager ();
		const SLedgeInfo detectedLedge = pLedgeManager->GetLedgeById (ledgeId);

		if (detectedLedge.IsValid ())
		{
		const IEntity* pCharacterEntity = Character.GetEntity ();
		const Vec3 vLedgePos = detectedLedge.GetPosition ();
		const Vec3 vCharacterPos = pCharacterEntity->GetWorldPos ();
		const Vec3 vLedgeToCharacter = vCharacterPos - vLedgePos;

		return ((detectedLedge.GetFacingDirection () * vLedgeToCharacter) >= 0.0f);
		}

		return false;*/

	return false;
}


void CCharacterStateLedge::SLedgeGrabbingParams::SetParamsFromXml(const IItemParamsNode* pParams)
{
	const char* gameModeParams = "LedgeGrabbingParams";

	const IItemParamsNode* pLedgeGrabParams = pParams ? pParams->GetChild(gameModeParams) : NULL;

	if (pLedgeGrabParams)
	{
		pLedgeGrabParams->GetAttribute("normalSpeedUp", m_fNormalSpeedUp);
		pLedgeGrabParams->GetAttribute("mobilitySpeedUp", m_fMobilitySpeedUp);
		pLedgeGrabParams->GetAttribute("mobilitySpeedUpMaximum", m_fMobilitySpeedUpMaximum);

		m_ledgeNearbyParams.SetParamsFromXml(pLedgeGrabParams->GetChild("LedgeNearByParams"));

		m_grabTransitionsParams [SLedgeTransitionData::eOLT_MidAir].SetParamsFromXml(pLedgeGrabParams->GetChild("PullUpMidAir"));
		m_grabTransitionsParams [SLedgeTransitionData::eOLT_Falling].SetParamsFromXml(pLedgeGrabParams->GetChild("PullUpFalling"));
		m_grabTransitionsParams [SLedgeTransitionData::eOLT_VaultOver].SetParamsFromXml(pLedgeGrabParams->GetChild("VaultOver"));
		m_grabTransitionsParams [SLedgeTransitionData::eOLT_VaultOverIntoFall].SetParamsFromXml(pLedgeGrabParams->GetChild("VaultOverIntoFall"));
		m_grabTransitionsParams [SLedgeTransitionData::eOLT_VaultOnto].SetParamsFromXml(pLedgeGrabParams->GetChild("VaultOnto"));
		m_grabTransitionsParams [SLedgeTransitionData::eOLT_HighVaultOver].SetParamsFromXml(pLedgeGrabParams->GetChild("HighVaultOver"));
		m_grabTransitionsParams [SLedgeTransitionData::eOLT_HighVaultOverIntoFall].SetParamsFromXml(pLedgeGrabParams->GetChild("HighVaultOverIntoFall"));
		m_grabTransitionsParams [SLedgeTransitionData::eOLT_HighVaultOnto].SetParamsFromXml(pLedgeGrabParams->GetChild("HighVaultOnto"));
		m_grabTransitionsParams [SLedgeTransitionData::eOLT_QuickLedgeGrab].SetParamsFromXml(pLedgeGrabParams->GetChild("PullUpQuick"));
	}
}


void CCharacterStateLedge::SLedgeNearbyParams::SetParamsFromXml(const IItemParamsNode* pParams)
{
	if (pParams)
	{
		pParams->GetAttribute("searchDir", m_vSearchDir);
		pParams->GetAttribute("maxDistance", m_fMaxDistance);
		pParams->GetAttribute("maxAngleDeviationFromSearchDirInDegrees", m_fMaxAngleDeviationFromSearchDirInDegrees);
		pParams->GetAttribute("maxExtendedAngleDeviationFromSearchDirInDegrees", m_fMaxExtendedAngleDeviationFromSearchDirInDegrees);
	}
}


void CCharacterStateLedge::SLedgeBlendingParams::SetParamsFromXml(const IItemParamsNode* pParams)
{
	if (pParams)
	{
		pParams->GetAttribute("positionOffset", m_vPositionOffset);
		pParams->GetAttribute("moveDuration", m_fMoveDuration);
		pParams->GetAttribute("correctionDuration", m_fCorrectionDuration);

		const char *pLedgeType = pParams->GetAttribute("ledgeType");
		if (pLedgeType)
		{
			if (!stricmp(pLedgeType, "Wide"))
			{
				m_ledgeType = eLT_Wide;
			}
			else if (!stricmp(pLedgeType, "Thin"))
			{
				m_ledgeType = eLT_Thin;
			}
		}

		int isVault = 0;
		pParams->GetAttribute("isVault", isVault);
		m_bIsVault = (isVault != 0);

		int isHighVault = 0;
		pParams->GetAttribute("isHighVault", isHighVault);
		m_bIsHighVault = (isHighVault != 0);

		pParams->GetAttribute("exitVelocityY", m_vExitVelocity.y);
		pParams->GetAttribute("exitVelocityZ", m_vExitVelocity.z);
		int keepOrientation = 0;
		pParams->GetAttribute("keepOrientation", keepOrientation);
		m_bKeepOrientation = (keepOrientation != 0);
		int endFalling = 0;
		pParams->GetAttribute("endFalling", endFalling);
		m_bEndFalling = (endFalling != 0);
	}
}


CCharacterStateLedge::SLedgeGrabbingParams& CCharacterStateLedge::GetLedgeGrabbingParams()
{
	return s_ledgeGrabbingParams;
}


bool CCharacterStateLedge::TryLedgeGrab(CCharacter& Character, const float expectedJumpEndHeight, const float startJumpHeight, const bool bSprintJump, SLedgeTransitionData* pLedgeState, const bool ignoreCharacterMovementWhenFindingLedge)
{
	CRY_ASSERT(pLedgeState);

	if (!CanGrabOntoLedge(Character))
	{
		return false;
	}

	UpdateNearestGrabbableLedge(Character, pLedgeState, ignoreCharacterMovementWhenFindingLedge);

	/*if (pLedgeState->m_ledgeTransition != SLedgeTransitionData::eOLT_None)
	{
	bool bDoLedgeGrab = true;

	const bool isFalling = Character.GetActorPhysics()->velocity*Character.GetActorPhysics()->gravity > 0.0f;
	if (!isFalling)
	{
	const SLedgeInfo ledgeInfo = g_pGame->GetLedgeManager ()->GetLedgeById (LedgeId (pLedgeState->m_nearestGrabbableLedgeId));
	CRY_ASSERT (ledgeInfo.IsValid ());

	const bool isWindow = ledgeInfo.AreFlagsSet (kLedgeFlag_isWindow);
	const bool useVault = ledgeInfo.AreAnyFlagsSet (kLedgeFlag_useVault | kLedgeFlag_useHighVault);

	if (!isWindow) // Windows always use ledge grabs
	{
	float ledgeHeight = ledgeInfo.GetPosition ().z;
	if (expectedJumpEndHeight > (ledgeHeight + g_pGameCVars->g_ledgeGrabClearHeight))
	{
	if (useVault)
	{
	// TODO - this code doesn't seem right.. needs looking at
	float CharacterZ = Character.GetEntity ()->GetWorldPos ().z;
	if (CharacterZ + g_pGameCVars->g_vaultMinHeightDiff > ledgeHeight)
	{
	bDoLedgeGrab = false;
	}
	else
	{
	bDoLedgeGrab = bSprintJump;
	}
	}
	else
	{
	bDoLedgeGrab = false;
	}
	}
	}
	}

	if (bDoLedgeGrab)
	{
	if (Character.IsClient ())
	{
	const float fHeightofEntity = Character.GetEntity ()->GetWorldPos ().z;
	CCharacterStateUtil::ApplyFallDamage (Character, startJumpHeight, fHeightofEntity);
	}

	}
	return bDoLedgeGrab;
	}

	return false;*/

	return false;
}


float CCharacterStateLedge::GetLedgeGrabSpeed(const CCharacter& Character) const
{
	const float kSpeedMultiplier [3] = { GetLedgeGrabbingParams().m_fNormalSpeedUp,
		GetLedgeGrabbingParams().m_fMobilitySpeedUp,
		GetLedgeGrabbingParams().m_fMobilitySpeedUpMaximum };

	return kSpeedMultiplier [0];
}


void CCharacterStateLedge::Serialize(TSerialize serializer)
{
	uint16 ledgeId = m_ledgeId;

	serializer.Value("LedgeId", ledgeId);
	serializer.Value("LedgeTransition", m_postSerializeLedgeTransition);

	if (serializer.IsReading())
	{
		m_ledgeId = ledgeId;
	}
}


void CCharacterStateLedge::PostSerialize(CCharacter& Character)
{
	// Re-start the ledge grab
	SLedgeTransitionData ledgeData(m_ledgeId);
	ledgeData.m_ledgeTransition = (SLedgeTransitionData::EOnLedgeTransition)m_postSerializeLedgeTransition;

	OnEnter(Character, SStateEventLedge(ledgeData));
}
