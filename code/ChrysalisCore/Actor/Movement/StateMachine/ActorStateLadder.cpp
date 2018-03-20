#include <StdAfx.h>

#include "ActorStateLadder.h"
#include <Actor/Animation/ActorAnimation.h>
#include <Actor/ActorControllerComponent.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include <Entities/EntityScriptCalls.h>
#include "ActorStateEvents.h"
#include <Console/CVars.h>


namespace Chrysalis
{
#ifndef _RELEASE
#define LadderLog(...) do { if (g_cvars.m_ladder_logVerbosity > 0) { CryLog ("[LADDER] " __VA_ARGS__); } } while(0)
#define LadderLogIndent() INDENT_LOG_DURING_SCOPE(g_cvars.m_ladder_logVerbosity > 0)
static AUTOENUM_BUILDNAMEARRAY(s_onOffAnimTypeNames, ladderAnimTypeList);
#else
#define LadderLog(...) (void)(0)
#define LadderLogIndent(...) (void)(0)
#endif

#define USE_CLIMB_ANIMATIONS 1

static uint32 s_ladderFractionCRC = 0;


class CLadderAction : public CAnimationAction
{
public:
	CLadderAction(CActorStateLadder * ladderState, CActorControllerComponent& actorControllerComponent, FragmentID fragmentID,
		CActorStateLadder::ELadderAnimType animType, const char* cameraAnimFactorAtStart,
		const char* cameraAnimFactorAtEnd) :
		CAnimationAction(EActorActionPriority::eAAP_ActionUrgent, fragmentID),
		m_ladderState(ladderState),
		m_actorComponent(actorControllerComponent),
		m_animType(animType),
		m_cameraAnimFactorAtStart(0.f),
		m_cameraAnimFactorAtEnd(0.f),
		m_duration(0.f),
		m_interruptable(false)
	{
		IEntity* pLadder = gEnv->pEntitySystem->GetEntity(ladderState->GetLadderId());

		/*		if (pLadder)
				{
				EntityScripts::GetEntityProperty (pLadder, "Camera", cameraAnimFactorAtStart, m_cameraAnimFactorAtStart);
				EntityScripts::GetEntityProperty (pLadder, "Camera", cameraAnimFactorAtEnd, m_cameraAnimFactorAtEnd);
				}
				LadderLog ("Constructing %s instance for %s who's %s a ladder", GetName (), actorControllerComponent.GetEntity ()->GetEntityTextDescription (),actorControllerComponent.IsOnLadder () ? "on" : "not on");

				#ifndef _RELEASE
				ladderState->UpdateNumActions (1);
				#endif*/
	}

#ifndef _RELEASE
	~CLadderAction()
	{
		/*CRY_ASSERT (m_ladderState);
		m_ladderState->UpdateNumActions (-1);*/
	}
#endif


	void InitialiseWithParams(const char * directionText, const char * footText)
	{
		/*const CTagDefinition* pFragTagDef = m_context->controllerDef.GetFragmentTagDef (m_fragmentID);

		LadderLog ("Initializing %s instance for %s performing action defined in '%s' direction=%s, foot=%s", GetName (), m_actorComponent.GetEntity ()->GetEntityTextDescription (), pFragTagDef->GetFilename (), directionText, footText);
		LadderLogIndent ();

		if (directionText)
		{
		pFragTagDef->Set (m_fragTags, pFragTagDef->Find (directionText), true);
		}

		if (footText)
		{
		pFragTagDef->Set (m_fragTags, pFragTagDef->Find (footText), true);
		}*/
	}


	void OnInitialise()
	{
#ifndef _RELEASE
		/*CRY_ASSERT_TRACE (0, ("Unexpected anim type %d (%s) for %s action", m_animType, s_onOffAnimTypeNames [m_animType], GetName ()));*/
#endif
	}


	virtual void Enter()
	{
		/*LadderLog ("Entering %s instance for %s", GetName (), m_actorComponent.GetEntity ()->GetEntityTextDescription ());
		LadderLogIndent ();

		CAnimationAction::Enter ();

		IAnimatedCharacter* pAnimChar = m_actorComponent.GetAnimatedCharacter ();

		if (pAnimChar)
		{
		EMovementControlMethod method = m_interruptable ? eMCM_Entity : eMCM_Animation;
		pAnimChar->SetMovementControlMethods (method, method);
		}

		m_actorComponent.BlendPartialCameraAnim (m_cameraAnimFactorAtStart, 0.1f);
		m_actorComponent.SetCanTurnBody (false);

		m_ladderState->InformLadderAnimEnter (m_actorComponent, this);*/
	}


	virtual void Exit()
	{
		/*LadderLog ("Exiting %s instance for %s", GetName (), m_actorComponent.GetEntity ()->GetEntityTextDescription ());
		LadderLogIndent ();

		CAnimationAction::Exit ();

		m_ladderState->InformLadderAnimIsDone (m_actorComponent, this);*/
	}


	bool GetIsInterruptable()
	{
		return m_interruptable;
	}


	void UpdateCameraAnimFactor()
	{
		/*if (GetStatus () == IAction::Installed)
		{
		const IScope & rootScope = GetRootScope ();
		const float timeLeft = rootScope.CalculateFragmentTimeRemaining ();
		const float duration = max (m_duration, timeLeft);
		float fractionComplete = 1.f;
		if (duration > 0.0f)
		{
		fractionComplete = 1.f - (timeLeft / duration);
		}

		m_duration = duration;
		m_actorComponent.BlendPartialCameraAnim (LERP (m_cameraAnimFactorAtStart, m_cameraAnimFactorAtEnd, fractionComplete), 0.1f);
		}*/
	}


protected:
	CActorControllerComponent& m_actorComponent;
	CActorStateLadder * m_ladderState;
	CActorStateLadder::ELadderAnimType m_animType;
	float m_cameraAnimFactorAtStart;
	float m_cameraAnimFactorAtEnd;
	float m_duration;
	bool m_interruptable;
};


class CActionLadderGetOn : public CLadderAction
{
public:
	DEFINE_ACTION("LadderGetOn");

	CActionLadderGetOn(CActorStateLadder * ladderState, CActorControllerComponent& actorControllerComponent, CActorStateLadder::ELadderAnimType animType) :
		CLadderAction(ladderState, actorControllerComponent, actorControllerComponent.GetActor()->GetMannequinParams()->fragmentIDs.LadderGetOn, animType, "cameraAnimFraction_getOn", "cameraAnimFraction_onLadder")
	{}


	virtual void OnInitialise()
	{
		/*switch (m_animType)
		{
		case CActorStateLadder::kLadderAnimType_atBottom:
		InitialiseWithParams ("up", NULL);
		return;

		case CActorStateLadder::kLadderAnimType_atTopLeftFoot:
		InitialiseWithParams ("down", "left");
		return;

		case CActorStateLadder::kLadderAnimType_atTopRightFoot:
		InitialiseWithParams ("down", "right");
		return;
		}

		CLadderAction::OnInitialise ();*/
	}


	virtual void Exit()
	{
		/*if (IAnimatedCharacter* pAnimChar = m_actorComponent.GetAnimatedCharacter ())
		{
		Quat animQuat = pAnimChar->GetAnimLocation ().q;

		m_actorComponent.SetViewRotation (animQuat);
		}

		CLadderAction::Exit ();*/
	}
};


class CActionLadderGetOff : public CLadderAction
{
public:
	DEFINE_ACTION("LadderGetOff");

	CActionLadderGetOff(CActorStateLadder * ladderState, CActorControllerComponent& actorControllerComponent, CActorStateLadder::ELadderAnimType animType) :
		CLadderAction(ladderState, actorControllerComponent, actorControllerComponent.GetActor()->GetMannequinParams()->fragmentIDs.LadderGetOff, animType, "cameraAnimFraction_onLadder", "cameraAnimFraction_getOff")
	{}


	virtual void OnInitialise()
	{
		/*switch (m_animType)
		{
		case CActorStateLadder::kLadderAnimType_atBottom:
		{
		InitialiseWithParams ("down", NULL);
		return;
		}

		case CActorStateLadder::kLadderAnimType_atTopLeftFoot:
		{
		InitialiseWithParams ("up", "left");
		return;
		}

		case CActorStateLadder::kLadderAnimType_atTopRightFoot:
		{
		InitialiseWithParams ("up", "right");
		return;
		}
		}

		CLadderAction::OnInitialise ();*/
	}


	virtual void Exit()
	{
		/*if (IAnimatedCharacter* pAnimChar = m_actorComponent.GetAnimatedCharacter ())
		{
		Quat animQuat = pAnimChar->GetAnimLocation ().q;

		m_actorComponent.SetViewRotation (animQuat);
		}

		CLadderAction::Exit ();*/
	}
};


class CActionLadderClimbUpDown : public CLadderAction
{
public:
	DEFINE_ACTION("LadderClimbUpDown");

	CActionLadderClimbUpDown(CActorStateLadder* ladderState, CActorControllerComponent& actorControllerComponent) :
		CLadderAction(ladderState, actorControllerComponent, actorControllerComponent.GetActor()->GetMannequinParams()->fragmentIDs.LadderClimb, CActorStateLadder::kLadderAnimType_upLoop, "cameraAnimFraction_onLadder", "cameraAnimFraction_onLadder")
	{
		m_interruptable = true;
	}


	virtual void OnInitialise()
	{
		InitialiseWithParams("up", "loop");
	}
};


CActorStateLadder::CActorStateLadder()
{
#ifndef _RELEASE
	m_dbgNumActions = 0;
#endif

	/*m_mostRecentlyEnteredAction = NULL;

	if (s_ladderFractionCRC == 0)
	{
	s_ladderFractionCRC = gEnv->pSystem->GetCrc32Gen ()->GetCRC32Lowercase ("LadderFraction");
	}*/
}


void CActorStateLadder::SetClientCharacterOnLadder(IEntity * pLadder, bool onOff)
{
	/*bool renderLadderLast = false;
	EntityScripts::GetEntityProperty (pLadder, "Camera", "bRenderLadderLast", renderLadderLast);

	const uint32 applyRenderFlags [2] = {0, ENTITY_SLOT_RENDER_NEAREST};
	const uint32 oldFlags = pLadder->GetSlotFlags (0);
	const uint32 newFlags = (oldFlags & ~ENTITY_SLOT_RENDER_NEAREST) | applyRenderFlags [onOff && renderLadderLast];

	pLadder->SetSlotFlags (0, newFlags);*/
}


void CActorStateLadder::OnUseLadder(CActorControllerComponent& actorControllerComponent, IEntity* pLadder)
{
	/*CRY_ASSERT (pLadder);

	LadderLog ("%s has started using ladder %s", actorControllerComponent.GetEntity ()->GetEntityTextDescription (), pLadder->GetEntityTextDescription ());
	LadderLogIndent ();

	#ifndef _RELEASE
	CRY_ASSERT_TRACE (m_dbgNumActions == 0, ("%s shouldn't have any leftover ladder actions but has %d", actorControllerComponent.GetEntity ()->GetEntityTextDescription (), m_dbgNumActions));
	#endif

	CRY_ASSERT (actorControllerComponent.IsOnLadder ());
	actorControllerComponent.UpdateVisibility ();

	if (actorControllerComponent.IsClient ())
	{
	actorControllerComponent.GetCharacterInput ()->AddInputCancelHandler (this);
	SetClientCharacterOnLadder (pLadder, true);
	}

	const Vec3 worldPos (pLadder->GetWorldPos ());
	const Vec3 direction (pLadder->GetWorldTM ().GetColumn1 ());
	const Vec3 CharacterEntityPos (actorControllerComponent.GetEntity ()->GetWorldPos ());

	float height = 0.f;
	float horizontalViewLimit = 0.f;
	float stopClimbingDistanceFromBottom = 0.f;
	float stopClimbingDistanceFromTop = 0.f;
	float CharacterHorizontalOffset = 0.f;
	float distanceBetweenRungs = 0.f;

	float verticalUpViewLimit = 0.f;
	float verticalDownViewLimit = 0.f;
	float getOnDistanceAwayTop = 0.f;
	float getOnDistanceAwayBottom = 0.f;
	bool ladderUseThirdPerson = false;

	EntityScripts::GetEntityProperty (pLadder, "height", height);
	EntityScripts::GetEntityProperty (pLadder, "ViewLimits", "horizontalViewLimit", horizontalViewLimit);
	EntityScripts::GetEntityProperty (pLadder, "Offsets", "stopClimbDistanceFromBottom", stopClimbingDistanceFromBottom);
	EntityScripts::GetEntityProperty (pLadder, "Offsets", "stopClimbDistanceFromTop", stopClimbingDistanceFromTop);
	EntityScripts::GetEntityProperty (pLadder, "Offsets", "CharacterHorizontalOffset", CharacterHorizontalOffset);
	EntityScripts::GetEntityProperty (pLadder, "Camera", "distanceBetweenRungs", distanceBetweenRungs);
	EntityScripts::GetEntityProperty (pLadder, "ViewLimits", "verticalUpViewLimit", verticalUpViewLimit);
	EntityScripts::GetEntityProperty (pLadder, "ViewLimits", "verticalDownViewLimit", verticalDownViewLimit);
	EntityScripts::GetEntityProperty (pLadder, "Offsets", "getOnDistanceAwayTop", getOnDistanceAwayTop);
	EntityScripts::GetEntityProperty (pLadder, "Offsets", "getOnDistanceAwayBottom", getOnDistanceAwayBottom);
	EntityScripts::GetEntityProperty (pLadder, "Camera", "bUseThirdPersonCamera", ladderUseThirdPerson);

	const float heightOffsetBottom = stopClimbingDistanceFromBottom;
	const float heightOffsetTop = stopClimbingDistanceFromTop;

	m_ladderBottom = worldPos + (direction * CharacterHorizontalOffset);
	m_ladderBottom.z += heightOffsetBottom;
	float ladderClimbableHeight = height - heightOffsetTop - heightOffsetBottom;
	m_playGetOffAnim = kLadderAnimType_none;
	m_playGetOnAnim = kLadderAnimType_none;
	SetMostRecentlyEnteredAction (NULL);
	m_ladderEntityId = pLadder->GetId ();
	m_numRungsFromBottomPosition = 0;
	m_fractionBetweenRungs = 0.f;
	m_climbInertia = 0.f;
	m_scaleSettle = 0.f;

	SendLadderFlowgraphEvent (actorControllerComponent, pLadder, "CharacterOn");

	const float numberOfRungsFloat = ladderClimbableHeight / distanceBetweenRungs;
	m_topRungNumber = (uint32) max (0.f, numberOfRungsFloat + 0.5f);

	actorControllerComponent.SetCanTurnBody (false);
	actorControllerComponent.GetActorParams ().viewLimits.SetViewLimit (-direction, DEG2RAD (horizontalViewLimit),
	0.f, DEG2RAD (verticalUpViewLimit),
	DEG2RAD (verticalDownViewLimit), SViewLimitParams::eVLS_Ladder);

	CActorStateUtility::PhySetFly (actorControllerComponent);
	CActorStateUtility::CancelCrouchAndProneInputs (actorControllerComponent);

	IAnimatedCharacter* pAnimChar = actorControllerComponent.GetAnimatedCharacter ();

	if (pAnimChar)
	{
	pAnimChar->RequestPhysicalColliderMode (eColliderMode_Disabled, eColliderModeLayer_Game, "CActorStateLadder::OnUseLadder");
	pAnimChar->EnableRigidCollider (0.45f);
	}

	Vec3 snapCharacterToPosition = m_ladderBottom;
	Quat snapCharacterToRotation = pLadder->GetWorldRotation ();

	bool playLowerAnimation = true;

	if (CharacterEntityPos.z > m_ladderBottom.z + ladderClimbableHeight - 0.1f)
	{
	snapCharacterToPosition.z += height;
	snapCharacterToPosition -= direction * getOnDistanceAwayTop;
	m_playGetOnAnim = (m_topRungNumber & 1) ? kLadderAnimType_atTopRightFoot : kLadderAnimType_atTopLeftFoot;
	m_numRungsFromBottomPosition = m_topRungNumber;
	}
	else if (CharacterEntityPos.z < m_ladderBottom.z + 0.1f)
	{
	snapCharacterToPosition.z -= heightOffsetBottom;
	snapCharacterToPosition += direction * getOnDistanceAwayBottom;
	snapCharacterToRotation *= Quat::CreateRotationZ (gf_PI);
	m_playGetOnAnim = kLadderAnimType_atBottom;
	}
	else
	{
	snapCharacterToPosition.z = clamp_tpl (CharacterEntityPos.z, m_ladderBottom.z, m_ladderBottom.z + ladderClimbableHeight);
	snapCharacterToRotation *= Quat::CreateRotationZ (gf_PI);
	playLowerAnimation = false;
	m_playGetOnAnim = kLadderAnimType_midAirGrab;
	}

	if (actorControllerComponent.IsInPickAndThrowMode ())
	{
	actorControllerComponent.HolsterItem (true);
	}
	else
	{
	CWeapon * pCurrentWeapon = actorControllerComponent.GetWeapon (actorControllerComponent.GetCurrentItemId ());
	if (pCurrentWeapon)
	{
	pCurrentWeapon->SetPlaySelectAction (playLowerAnimation);
	}
	}

	actorControllerComponent.GetEntity ()->SetPosRotScale (snapCharacterToPosition, snapCharacterToRotation, Vec3 (1.f, 1.f, 1.f));

	if (ladderUseThirdPerson)
	{
	actorControllerComponent.SetThirdPerson (true);
	}

	float heightFrac = (m_numRungsFromBottomPosition + m_fractionBetweenRungs) / m_topRungNumber;
	actorControllerComponent.OnUseLadder (m_ladderEntityId, heightFrac);*/
}


// Called when the actorControllerComponent finishes his exit animation (or from OnExit if it triggers no exit animation)
static void LadderExitIsComplete(CActorControllerComponent& actorControllerComponent)
{
	LadderLog("Ladder exit is complete");
	LadderLogIndent();

	/*	CRY_ASSERT (!actorControllerComponent.IsOnLadder ());
		actorControllerComponent.BlendPartialCameraAnim (0.0f, 0.1f);
		actorControllerComponent.SetCanTurnBody (true);
		//actorControllerComponent.SelectLastValidItem();

		if (!actorControllerComponent.IsCinematicFlagActive (SActorStats::eCinematicFlag_HolsterWeapon))
		{
		actorControllerComponent.HolsterItem (false);
		}

		IAnimatedCharacter* pAnimChar = actorControllerComponent.GetAnimatedCharacter ();

		if (pAnimChar)
		{
		pAnimChar->SetMovementControlMethods (eMCM_Entity, eMCM_Entity);
		pAnimChar->ForceRefreshPhysicalColliderMode ();
		pAnimChar->RequestPhysicalColliderMode (eColliderMode_Undefined, eColliderModeLayer_Game, "CActorStateLadder::OnExit()");
		pAnimChar->DisableRigidCollider ();
		}*/
}


void CActorStateLadder::OnExit(CActorControllerComponent& actorControllerComponent)
{
	/*LadderLog ("%s has stopped using ladder (%s)", actorControllerComponent.GetEntity ()->GetEntityTextDescription (), s_onOffAnimTypeNames [m_playGetOffAnim]);
	LadderLogIndent ();

	CRY_ASSERT (!actorControllerComponent.IsOnLadder ());
	actorControllerComponent.UpdateVisibility ();

	ICharacterInput * pCharacterInput = actorControllerComponent.GetCharacterInput ();

	if (pCharacterInput)
	{
	pCharacterInput->RemoveInputCancelHandler (this);
	}

	IEntity * pLadder = gEnv->pEntitySystem->GetEntity (m_ladderEntityId);

	if (pLadder)
	{
	if (actorControllerComponent.IsClient ())
	{
	SetClientCharacterOnLadder (pLadder, false);
	}
	SendLadderFlowgraphEvent (actorControllerComponent, pLadder, "CharacterOff");
	}

	m_ladderBottom.zero ();
	m_ladderEntityId = 0;

	actorControllerComponent.GetActorParams ().viewLimits.ClearViewLimit (SViewLimitParams::eVLS_Ladder);

	pe_player_dynamics simPar;

	IPhysicalEntity* piPhysics = actorControllerComponent.GetEntity ()->GetPhysics ();
	if (!piPhysics || piPhysics->GetParams (&simPar) == 0)
	{
	return;
	}

	IAnimatedCharacter* pAnimChar = actorControllerComponent.GetAnimatedCharacter ();
	CActorStateUtility::PhySetNoFly (actorControllerComponent, simPar.gravity);
	CActorStateUtility::CancelCrouchAndProneInputs (actorControllerComponent);

	InterruptCurrentAnimation ();
	SetMostRecentlyEnteredAction (NULL);

	if (m_playGetOffAnim)
	{
	QueueLadderAction (actorControllerComponent, new CActionLadderGetOff (this, actorControllerComponent, m_playGetOffAnim));
	}
	else
	{
	// Finishing the above 'get off' animation will retrieve the actorControllerComponent's weapon...
	// if we're not playing one then we unholster it now.
	QueueLadderAction (actorControllerComponent, NULL);
	LadderExitIsComplete (actorControllerComponent);
	}

	bool ladderUseThirdPerson = false;
	EntityScripts::GetEntityProperty (pLadder, "Camera", "bUseThirdPersonCamera", ladderUseThirdPerson);

	if (ladderUseThirdPerson && actorControllerComponent.IsViewFirstPerson())
	{
	actorControllerComponent.SetThirdPerson (false);
	}

	ELadderLeaveLocation loc = eLLL_Drop;
	if (m_playGetOffAnim == kLadderAnimType_atBottom)
	{
	loc = eLLL_Bottom;
	}
	else if (m_playGetOffAnim == kLadderAnimType_atTopLeftFoot || m_playGetOffAnim == kLadderAnimType_atTopRightFoot)
	{
	loc = eLLL_Top;
	}

	actorControllerComponent.OnLeaveLadder (loc);*/
}


void CActorStateLadder::InterruptCurrentAnimation()
{
	if (m_mostRecentlyEnteredAction)
	{
		m_mostRecentlyEnteredAction->ForceFinish();
	}
}


void CActorStateLadder::QueueLadderAction(CActorControllerComponent& actorControllerComponent, CLadderAction* action)
{
	LadderLog("Queuing %s ladder anim '%s'", actorControllerComponent.GetEntity()->GetEntityTextDescription(), action ? action->GetName() : "NULL");
	LadderLogIndent();

	if (action)
	{
		//actorControllerComponent.GetActor()->QueueAction(*action);
	}
}


void CActorStateLadder::SendLadderFlowgraphEvent(CActorControllerComponent& actorControllerComponent, IEntity * pLadderEntity, const char * eventName)
{
	/*SEntityEvent event (ENTITY_EVENT_SCRIPT_EVENT);
	event.nParam [0] = (INT_PTR) eventName;
	event.nParam [1] = IEntityClass::EVT_ENTITY;

	EntityId entityId = actorControllerComponent.GetEntityId ();
	event.nParam [2] = (INT_PTR) &entityId;

	pLadderEntity->SendEvent (event);*/
}


bool CActorStateLadder::OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent, float frameTime)
{
	/*	Vec3 requiredMovement (ZERO);

		IEntity * pLadder = gEnv->pEntitySystem->GetEntity (m_ladderEntityId);

		CRY_ASSERT (actorControllerComponent.IsOnLadder ());

		#ifndef _RELEASE
		if (gEnv->pGameFramework->GetCVars()->m_ladder_logVerbosity)
		{
		CryWatch ("[LADDER] RUNG=$3%u/%u$o FRAC=$3%.2f$o: %s is %.2fm up a ladder, move=%.2f - %s, %s, %s, camAnim=%.2f, $7INERTIA=%.2f SETTLE=%.2f", m_numRungsFromBottomPosition, m_topRungNumber, m_fractionBetweenRungs, actorControllerComponent.GetEntity ()->GetEntityTextDescription (), actorControllerComponent.GetEntity ()->GetWorldPos ().z - m_ladderBottom.z, requiredMovement.z, actorControllerComponent.CanTurnBody () ? "$4can turn body$o" : "$3cannot turn body$o", (actorControllerComponent.GetEntity ()->GetSlotFlags (0) & ENTITY_SLOT_RENDER_NEAREST) ? "render nearest" : "render normal", actorControllerComponent.IsOnLadder () ? "$3on a ladder$o" : "$4not on a ladder$o", actorControllerComponent.GetActorState ()->partialCameraAnimFactor, m_climbInertia, m_scaleSettle);

		if (m_mostRecentlyEnteredAction && m_mostRecentlyEnteredAction->GetStatus () == IAction::Installed)
		{
		const IScope & animScope = m_mostRecentlyEnteredAction->GetRootScope ();
		const float timeRemaining = animScope.CalculateFragmentTimeRemaining ();
		CryWatch ("[LADDER] Animation: '%s' (timeActive=%.2f timeRemaining=%.2f speed=%.2f)", m_mostRecentlyEnteredAction->GetName (), m_mostRecentlyEnteredAction->GetActiveTime (), timeRemaining, m_mostRecentlyEnteredAction->GetSpeedBias ());
		}
		else
		{
		CryWatch ("[LADDER] Animation: %s", m_mostRecentlyEnteredAction ? "NOT PLAYING" : "NONE");
		}
		}
		#endif

		IScriptTable * pEntityScript = pLadder ? pLadder->GetScriptTable () : NULL;
		SmartScriptTable propertiesTable;

		if (pEntityScript)
		{
		pEntityScript->GetValue ("Properties", propertiesTable);
		}

		int bUsable = 0;
		if (pLadder == NULL || (propertiesTable->GetValue ("bUsable", bUsable) && bUsable == 0))
		{
		actorControllerComponent.StateMachineHandleEventMovement (SStateEventLeaveLadder (eLLL_Drop));
		}
		else if (m_playGetOnAnim != kLadderAnimType_none)
		{
		if (!actorControllerComponent.IsCinematicFlagActive (SActorStats::eCinematicFlag_HolsterWeapon))
		{
		actorControllerComponent.HolsterItem (true);
		}

		IItem * pItem = actorControllerComponent.GetCurrentItem ();
		bool canPlayGetOnAnim = (pItem == NULL);

		if (!canPlayGetOnAnim)
		{
		// Waiting for actorControllerComponent to switch to 'NoWeapon' item - check this is still happening! (If not, warn
		// and play the get on animation anyway - let's not get stuck here!)
		EntityId switchingToItemID = actorControllerComponent.GetActorState ()->exchangeItemStats.switchingToItemID;
		IEntity * pEntity = gEnv->pEntitySystem->GetEntity (switchingToItemID);

		if (pEntity == NULL)
		{
		GameWarning ("!%s should be switching to 'NoWeapon' but is using %s and switching to %s",
		actorControllerComponent.GetEntity ()->GetEntityTextDescription (), pItem->GetEntity ()->GetClass ()->GetName (),
		pEntity ? pEntity->GetClass ()->GetName () : "<NULL>");
		canPlayGetOnAnim = true;
		}
		}

		if (canPlayGetOnAnim)
		{
		// Currently we don't have a mid-air grab animation, so let's just go straight to the climbing anim if that's why we're here
		if (m_playGetOnAnim != kLadderAnimType_midAirGrab)
		{
		QueueLadderAction (actorControllerComponent, new CActionLadderGetOn (this, actorControllerComponent, m_playGetOnAnim));
		}
		QueueLadderAction (actorControllerComponent, new CActionLadderClimbUpDown (this, actorControllerComponent));
		m_playGetOnAnim = kLadderAnimType_none;
		}
		}
		else if (m_mostRecentlyEnteredAction && !m_mostRecentlyEnteredAction->GetIsInterruptable ())
		{
		m_mostRecentlyEnteredAction->UpdateCameraAnimFactor ();
		}
		else if (actorControllerComponent.GetActorState ())
		{
		float pushUpDown = actorControllerComponent.GetVelocity().y;
		const float deflection = fabsf (pushUpDown);

		float movementInertiaDecayRate = 0.f;
		float movementAcceleration = 0.f;
		float movementSettleSpeed = 0.f;
		float movementSpeedUpwards = 0.f;
		float movementSpeedDownwards = 0.f;

		EntityScripts::GetEntityProperty (pLadder, "Movement", "movementInertiaDecayRate", movementInertiaDecayRate);
		EntityScripts::GetEntityProperty (pLadder, "Movement", "movementAcceleration", movementAcceleration);
		EntityScripts::GetEntityProperty (pLadder, "Movement", "movementSettleSpeed", movementSettleSpeed);
		EntityScripts::GetEntityProperty (pLadder, "Movement", "movementSpeedUpwards", movementSpeedUpwards);
		EntityScripts::GetEntityProperty (pLadder, "Movement", "movementSpeedDownwards", movementSpeedDownwards);

		const float inertiaDecayAmount = frameTime * movementInertiaDecayRate * (1.f - deflection);

		if (m_climbInertia >= 0.f)
		{
		if (m_fractionBetweenRungs > 0.5f || m_climbInertia < 0.0001f)
		{
		m_climbInertia = max (0.f, m_climbInertia - inertiaDecayAmount);
		}
		else
		{
		m_climbInertia = min (1.f, m_climbInertia + inertiaDecayAmount * 0.1f);
		}
		}
		else
		{
		if (m_fractionBetweenRungs <= 0.5f || m_climbInertia > -0.0001f)
		{
		m_climbInertia = min (0.f, m_climbInertia + inertiaDecayAmount);
		}
		else
		{
		m_climbInertia = max (-1.f, m_climbInertia - inertiaDecayAmount * 0.1f);
		}
		}

		m_climbInertia = clamp_tpl (m_climbInertia + pushUpDown * movementAcceleration * frameTime, -1.f, 1.f);
		m_scaleSettle = min (m_scaleSettle + inertiaDecayAmount, 1.f - fabsf (m_climbInertia));

		const float maxAutoSettleMovement = frameTime * m_scaleSettle * movementSettleSpeed;
		float settleToHere = (m_fractionBetweenRungs > 0.5f) ? 1.f : 0.f;
		const float settleOffset = (settleToHere - m_fractionBetweenRungs);

		m_fractionBetweenRungs += frameTime * m_climbInertia * (float) __fsel (m_climbInertia, movementSpeedUpwards, movementSpeedDownwards) + clamp_tpl (settleOffset, -maxAutoSettleMovement, maxAutoSettleMovement);

		if (m_fractionBetweenRungs < 0.f)
		{
		if (m_numRungsFromBottomPosition > 0)
		{
		--m_numRungsFromBottomPosition;
		m_fractionBetweenRungs += 1.f;
		}
		else
		{
		m_fractionBetweenRungs = 0.f;
		if (pushUpDown < -0.5f)
		{
		m_playGetOffAnim = kLadderAnimType_atBottom;
		}
		}
		}
		else
		{
		if (m_fractionBetweenRungs >= 1.f)
		{
		++m_numRungsFromBottomPosition;
		m_fractionBetweenRungs -= 1.f;
		}

		if (m_topRungNumber == m_numRungsFromBottomPosition)
		{
		m_fractionBetweenRungs = 0.f;
		if (pushUpDown > 0.5f)
		{
		int topBlockedValue = 0;
		const bool bTopIsBlocked = propertiesTable->GetValue ("bTopBlocked", topBlockedValue) && topBlockedValue;

		if (!bTopIsBlocked)
		{
		m_playGetOffAnim = (m_topRungNumber & 1) ? kLadderAnimType_atTopRightFoot : kLadderAnimType_atTopLeftFoot;
		}
		#ifndef _RELEASE
		else if (gEnv->pGameFramework->GetCVars()->m_ladder_logVerbosity)
		{
		CryWatch ("[LADDER] %s can't climb up and off %s - top is blocked", actorControllerComponent.GetEntity ()->GetName (), pLadder->GetEntityTextDescription ());
		}
		#endif
		}
		}

		float heightFrac = (m_numRungsFromBottomPosition + m_fractionBetweenRungs) / m_topRungNumber;
		actorControllerComponent.OnLadderPositionUpdated (heightFrac);
		}

		float distanceBetweenRungs = 0.f;
		EntityScripts::GetEntityProperty (pLadder, "Camera", "distanceBetweenRungs", distanceBetweenRungs);

		const Vec3 stopAtPosBottom = m_ladderBottom;
		const float distanceUpLadder = (m_numRungsFromBottomPosition + m_fractionBetweenRungs) * distanceBetweenRungs;
		const Vec3 setThisPosition (stopAtPosBottom.x, stopAtPosBottom.y, stopAtPosBottom.z + distanceUpLadder);

		actorControllerComponent.GetEntity ()->SetPos (setThisPosition);

		if (m_mostRecentlyEnteredAction)
		{
		const float animFraction = m_fractionBetweenRungs * 0.5f + ((m_numRungsFromBottomPosition & 1) ? 0.5f : 0.0f);

		#ifndef _RELEASE
		if (gEnv->pGameFramework->GetCVars()->m_ladder_logVerbosity)
		{
		CryWatch ("[LADDER] $7Setting anim fraction to %.4f", animFraction);
		}
		#endif

		m_mostRecentlyEnteredAction->SetParam (s_ladderFractionCRC, animFraction);
		}
		}
		*/
	return m_playGetOffAnim == kLadderAnimType_none;
}


void CActorStateLadder::LeaveLadder(CActorControllerComponent& actorControllerComponent, ELadderLeaveLocation leaveLocation)
{
	switch (leaveLocation)
	{
		case eLLL_Drop:
			/*actorControllerComponent.GetMoveRequest ().velocity = Vec3 (0.f, 0.f, -1.f);
			actorControllerComponent.GetMoveRequest ().type = eCMT_Impulse;*/
			break;

		case eLLL_Top:
			m_playGetOffAnim = (m_topRungNumber & 1) ? kLadderAnimType_atTopRightFoot : kLadderAnimType_atTopLeftFoot;
			break;

		case eLLL_Bottom:
			m_playGetOffAnim = kLadderAnimType_atBottom;
			break;
	}
}


void CActorStateLadder::SetHeightFrac(CActorControllerComponent& actorControllerComponent, float heightFrac)
{
	heightFrac *= m_topRungNumber;
	m_numRungsFromBottomPosition = (int)heightFrac;
	m_fractionBetweenRungs = heightFrac - m_numRungsFromBottomPosition;
}


bool CActorStateLadder::IsUsableLadder(CActorControllerComponent& actorControllerComponent, IEntity* pLadder, const SmartScriptTable& ladderProperties)
{
	bool retVal = false;

	/*	if (pLadder && !actorControllerComponent.IsOnLadder () && actorControllerComponent.CanTurnBody ())
		{
		float height = 0.f;
		ladderProperties->GetValue ("height", height);

		if (height > 0.f)
		{
		const Matrix34& ladderTM = pLadder->GetWorldTM ();
		Vec3 ladderPos = ladderTM.GetTranslation ();
		Vec3 CharacterPos = actorControllerComponent.GetEntity ()->GetWorldPos ();

		const char* angleVariable = ((CharacterPos.z + 0.1f) > (ladderPos.z + height)) ? "approachAngleTop" : "approachAngle";

		float angleRange = 0.f;
		ladderProperties->GetValue (angleVariable, angleRange);

		retVal = true;

		if (angleRange != 0.f)
		{
		Vec2 ladderToCharacter ((CharacterPos - ladderPos));
		Vec2 ladderDirection (ladderTM.GetColumn1 ());

		ladderToCharacter.Normalize ();
		ladderDirection.Normalize ();

		if (angleRange < 0.f)
		{
		// Negative angle means you want the middle of the available range to be from the opposite direction.
		angleRange = -angleRange;
		ladderToCharacter = -ladderToCharacter;
		}

		if (ladderToCharacter.Dot (ladderDirection) < cos (DEG2RAD (angleRange)))
		{
		retVal = false;
		}
		}
		}
		}

		#ifndef _RELEASE
		if (gEnv->pGameFramework->GetCVars()->m_ladder_logVerbosity)
		{
		if (pLadder)
		{
		float distanceBetweenRungs = 0.f;
		float stopClimbingDistanceFromBottom = 0.f;

		EntityScripts::GetEntityProperty (pLadder, "Camera", "distanceBetweenRungs", distanceBetweenRungs);
		EntityScripts::GetEntityProperty (pLadder, "Offsets", "stopClimbDistanceFromBottom", stopClimbingDistanceFromBottom);

		ColorB ladderColour (150, 150, 255, 150);
		IRenderAuxGeom * pGeom = gEnv->pRenderer->GetIRenderAuxGeom ();
		const Vec3 ladderBasePos = pLadder->GetWorldPos ();
		const Matrix34& ladderTM = pLadder->GetWorldTM ();
		float height = 0.f;
		AABB entityBounds;
		pLadder->GetLocalBounds (entityBounds);
		const Vec3 rungEndSideways = ladderTM.GetColumn0 () * entityBounds.GetSize ().x * 0.5f;
		EntityScripts::GetEntityProperty (pLadder, "height", height);
		const Vec3 offsetToTop = height * ladderTM.GetColumn2 ();

		for (float rungHeight = stopClimbingDistanceFromBottom; rungHeight < height; rungHeight += distanceBetweenRungs)
		{
		const Vec3 rungCentre (ladderBasePos.x, ladderBasePos.y, ladderBasePos.z + rungHeight);
		pGeom->DrawLine (rungCentre - rungEndSideways, ladderColour, rungCentre + rungEndSideways, ladderColour, 20.f);
		}

		pGeom->DrawLine (ladderBasePos - rungEndSideways, ladderColour, ladderBasePos - rungEndSideways + offsetToTop, ladderColour, 20.f);
		pGeom->DrawLine (ladderBasePos + rungEndSideways, ladderColour, ladderBasePos + rungEndSideways + offsetToTop, ladderColour, 20.f);
		}

		CryWatch ("[LADDER] Is %s usable by %s? %s", pLadder ? pLadder->GetEntityTextDescription () : "<NULL ladder entity>", actorControllerComponent.GetEntity ()->GetEntityTextDescription (), retVal ? "$3YES$o" : "$4NO$o");
		}
		#endif
		*/
	return retVal;
}


void CActorStateLadder::SetMostRecentlyEnteredAction(CLadderAction * thisAction)
{
	if (thisAction)
	{
		thisAction->AddRef();
	}

	if (m_mostRecentlyEnteredAction)
	{
		m_mostRecentlyEnteredAction->Release();
	}

	m_mostRecentlyEnteredAction = thisAction;
}


void CActorStateLadder::InformLadderAnimEnter(CActorControllerComponent& actorControllerComponent, CLadderAction * thisAction)
{
	CRY_ASSERT(thisAction);

	LadderLog("Action '%s' has been entered", thisAction->GetName());
	LadderLogIndent();

	SetMostRecentlyEnteredAction(thisAction);
}


void CActorStateLadder::InformLadderAnimIsDone(CActorControllerComponent& actorControllerComponent, CLadderAction * thisAction)
{
	CRY_ASSERT(thisAction);

	LadderLog("Action '%s' is done", thisAction->GetName());
	LadderLogIndent();

	/*	if (actorControllerComponent.IsClient ())
		{
		if (actorControllerComponent.IsOnLadder ())
		{
		if (IEntity * pLadder = gEnv->pEntitySystem->GetEntity (m_ladderEntityId))
		{
		SetClientCharacterOnLadder (pLadder, true);
		}
		}
		}

		if (thisAction == m_mostRecentlyEnteredAction)
		{
		if (!actorControllerComponent.IsOnLadder () && !thisAction->GetIsInterruptable ())
		{
		LadderExitIsComplete (actorControllerComponent);
		}
		}*/
}


bool CActorStateLadder::HandleCancelInput(CPlayerComponent & player, TCancelButtonBitfield cancelButtonPressed)
{
	/*	CRY_ASSERT (actorControllerComponent.IsOnLadder ());

		if ((cancelButtonPressed & (kCancelPressFlag_switchItem)) == 0)
		{
		if (m_mostRecentlyEnteredAction && m_mostRecentlyEnteredAction->GetIsInterruptable ())
		{
		actorControllerComponent.StateMachineHandleEventMovement (SStateEventLeaveLadder (eLLL_Drop));
		}
		return true;
		}*/

	return false;
}


#ifndef _RELEASE
void CActorStateLadder::UpdateNumActions(int change)
{
	CRY_ASSERT(change == 1 || change == -1);

	CRY_ASSERT(m_dbgNumActions >= 0);
	m_dbgNumActions += change;

	CRY_ASSERT(m_dbgNumActions >= 0);
}
#endif
}