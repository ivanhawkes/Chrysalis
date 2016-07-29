#include <StdAfx.h>

//#include "CharacterStateEvents.h"
//#include "CharacterStateUtil.h"
//#include "CharacterInput.h"
//#include <Actor/Character/Character.h>
//#include "CharacterStateDead.h"
//#include "CharacterStateGround.h"
//#include "CharacterStateJump.h"
//#include "CharacterStateSwim.h"
//#include "Weapon.h"
//
//
//class CCharacterStateAIMovement : private CStateHierarchy < CCharacter >
//{
//	DECLARE_STATE_CLASS_BEGIN (CCharacter, CCharacterStateAIMovement)
//	DECLARE_STATE_CLASS_ADD (CCharacter, MovementRoot);
//	DECLARE_STATE_CLASS_ADD (CCharacter, Movement);
//	DECLARE_STATE_CLASS_ADD (CCharacter, Dead);
//	DECLARE_STATE_CLASS_ADD (CCharacter, Ground);
//	DECLARE_STATE_CLASS_ADD (CCharacter, Fall);
//	DECLARE_STATE_CLASS_ADD (CCharacter, Swim);
//	DECLARE_STATE_CLASS_ADD (CCharacter, SwimTest);
//	DECLARE_STATE_CLASS_ADD_DUMMY (CCharacter, NoMovement);
//	DECLARE_STATE_CLASS_END (CCharacter);
//
//private:
//	const TStateIndex StateGroundInput (CCharacter& Character, const SInputEventData& inputEvent);
//	void StateSprintInput (CCharacter& Character, const SInputEventData& inputEvent);
//	void ProcessSprint (const CCharacter& Character, const SCharacterPrePhysicsData& prePhysicsEvent);
//
//private:
//
//	CCharacterStateDead m_stateDead;
//	CCharacterStateGround m_stateGround;
//	CCharacterStateJump m_stateJump;
//	CCharacterStateSwim m_stateSwim;
//};
//
//
//DEFINE_STATE_CLASS_BEGIN (CCharacter, CCharacterStateAIMovement, CHARACTER_STATE_AIMOVEMENT, Ground)
//DEFINE_STATE_CLASS_ADD (CCharacter, CCharacterStateAIMovement, MovementRoot, Root)
//DEFINE_STATE_CLASS_ADD (CCharacter, CCharacterStateAIMovement, Movement, MovementRoot)
//DEFINE_STATE_CLASS_ADD (CCharacter, CCharacterStateAIMovement, Ground, Movement)
//DEFINE_STATE_CLASS_ADD_DUMMY (CCharacter, CCharacterStateAIMovement, NoMovement, SwimTest, MovementRoot)
//DEFINE_STATE_CLASS_ADD (CCharacter, CCharacterStateAIMovement, Fall, NoMovement)
//DEFINE_STATE_CLASS_ADD (CCharacter, CCharacterStateAIMovement, Swim, MovementRoot)
//DEFINE_STATE_CLASS_ADD (CCharacter, CCharacterStateAIMovement, Dead, Root)
//DEFINE_STATE_CLASS_END (CCharacter, CCharacterStateAIMovement);
//
//
//const CCharacterStateAIMovement::TStateIndex CCharacterStateAIMovement::Root (CCharacter& Character, const SStateEvent& event)
//{
//	CRY_ASSERT (Character.IsAIControlled ());
//
//	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId ());
//	switch (eventID)
//	{
//		case CHARACTER_EVENT_WEAPONCHANGED:
//		{
//			m_flags.ClearFlags (ECharacterStateFlags_CurrentItemIsHeavy);
//
//			const CWeapon* pWeapon = static_cast<const CWeapon*> (event.GetData (0).GetPtr ());
//			if (pWeapon && pWeapon->IsHeavyWeapon ())
//			{
//				m_flags.AddFlags (ECharacterStateFlags_CurrentItemIsHeavy);
//			}
//		}
//			break;
//
//		case CHARACTER_EVENT_DEAD:
//			return State_Dead;
//
//		case STATE_EVENT_DEBUG:
//		{
//			AUTOENUM_BUILDNAMEARRAY (stateFlags, eCharacterStateFlags);
//			STATE_DEBUG_EVENT_LOG (this, event, false, state_white, "Active: StateMovement: CurrentFlags: %s", AutoEnum_GetStringFromBitfield (m_flags.GetRawFlags (), stateFlags, sizeof (stateFlags) / sizeof (char*)).c_str ());
//		}
//			break;
//	}
//
//	return State_Continue;
//}
//
//
//const CCharacterStateAIMovement::TStateIndex CCharacterStateAIMovement::MovementRoot (CCharacter& Character, const SStateEvent& event)
//{
//	CRY_ASSERT (Character.IsAIControlled ());
//
//	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId ());
//	switch (eventID)
//	{
//		case STATE_EVENT_ENTER:
//			Character.SetCanTurnBody (true);
//			break;
//
//		case CHARACTER_EVENT_INPUT:
//			return StateGroundInput (Character, static_cast<const SStateEventCharacterInput&> (event).GetInputEventData ());
//	}
//
//	return State_Continue;
//}
//
//
//const CCharacterStateAIMovement::TStateIndex CCharacterStateAIMovement::Movement (CCharacter& Character, const SStateEvent& event)
//{
//	CRY_ASSERT (Character.IsAIControlled ());
//
//	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId ());
//	switch (eventID)
//	{
//		case STATE_EVENT_ENTER:
//			CCharacterStateUtil::InitializeMoveRequest (Character.GetMoveRequest ());
//			break;
//
//		case STATE_EVENT_EXIT:
//			m_flags.ClearFlags (ECharacterStateFlags_Sprinting);
//			break;
//
//		case CHARACTER_EVENT_PREPHYSICSUPDATE:
//		{
//			const SCharacterPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventCharacterMovementPrePhysics&> (event).GetPrePhysicsData ();
//			Character.m_actorState.onGround += prePhysicsEvent.m_frameTime;
//
//			if (!CCharacterStateUtil::IsOnGround (Character))
//			{
//				// We're considered inAir at this point.
//				// NOTE: this does not mean CCharacter::IsInAir() returns true.
//				float inAir = Character.m_actorState.inAir;
//				inAir += prePhysicsEvent.m_frameTime;
//				Character.m_actorState.inAir = inAir;
//
//				// We can be purely time based for AI as we can't jump or navigate to a falling position, though, we might be pushed, thrown, etc.
//				if ((inAir > g_pGameCVars->pl_movement.ground_timeInAirToFall))
//				{
//					Character.StateMachineHandleEventMovement (CHARACTER_EVENT_FALL);
//				}
//			}
//			else
//			{
//				Character.m_actorState.inAir = 0.0f;
//			}
//
//			CCharacterStateUtil::ProcessRotation (Character, prePhysicsEvent.m_movement, Character.GetMoveRequest ());
//			CCharacterStateUtil::FinalizeMovementRequest (Character, prePhysicsEvent.m_movement, Character.GetMoveRequest ());
//		}
//			break;
//	}
//
//	return State_Continue;
//}
//
//
//const CCharacterStateAIMovement::TStateIndex CCharacterStateAIMovement::Dead (CCharacter& Character, const SStateEvent& event)
//{
//	CRY_ASSERT (Character.IsAIControlled ());
//
//	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId ());
//	switch (eventID)
//	{
//		case STATE_EVENT_ENTER:
//			m_stateDead.OnEnter (Character);
//			break;
//
//		case STATE_EVENT_EXIT:
//			m_stateDead.OnLeave (Character);
//			break;
//
//		case STATE_EVENT_SERIALIZE:
//		{
//			TSerialize& serializer = static_cast<const SStateEventSerialize&> (event).GetSerializer ();
//			m_stateDead.Serialize (serializer);
//		}
//			break;
//
//		case CHARACTER_EVENT_PREPHYSICSUPDATE:
//		{
//			const SCharacterPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventCharacterMovementPrePhysics&> (event).GetPrePhysicsData ();
//
//			m_stateDead.OnPrePhysicsUpdate (Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime);
//		}
//			break;
//
//		case CHARACTER_EVENT_UPDATE:
//		{
//			CCharacterStateDead::UpdateCtx updateCtx;
//			updateCtx.frameTime = static_cast<const SStateEventUpdate&>(event).GetFrameTime ();
//			m_stateDead.OnUpdate (Character, updateCtx);
//		}
//			break;
//
//		case CHARACTER_EVENT_DEAD:
//			return State_Done;
//	}
//
//	return State_Continue;
//}
//
//
//const CCharacterStateAIMovement::TStateIndex CCharacterStateAIMovement::Ground (CCharacter& Character, const SStateEvent& event)
//{
//	CRY_ASSERT (Character.IsAIControlled ());
//
//	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId ());
//	switch (eventID)
//	{
//		case STATE_EVENT_ENTER:
//			m_stateGround.OnEnter (Character);
//			m_flags.AddFlags (ECharacterStateFlags_Ground);
//			Character.m_actorState.inAir = 0.0f;
//			break;
//
//		case STATE_EVENT_EXIT:
//			m_flags.ClearFlags (ECharacterStateFlags_Ground);
//			m_stateGround.OnExit (Character);
//			break;
//
//		case CHARACTER_EVENT_PREPHYSICSUPDATE:
//		{
//			const SCharacterPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventCharacterMovementPrePhysics&> (event).GetPrePhysicsData ();
//
//			ProcessSprint (Character, prePhysicsEvent);
//
//			m_stateGround.OnPrePhysicsUpdate (Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime, m_flags.AreAnyFlagsActive (ECharacterStateFlags_CurrentItemIsHeavy), false);
//		}
//			break;
//
//		case CHARACTER_EVENT_STANCE_CHANGED:
//		{
//			CCharacterStateUtil::ChangeStance (Character, event);
//
//			const SStateEventStanceChanged& stanceEvent = static_cast<const SStateEventStanceChanged&>(event);
//			const EStance stance = static_cast<EStance>(stanceEvent.GetStance ());
//
//			CAIAnimationComponent* pAiAnimationComponent = Character.GetAIAnimationComponent ();
//			CRY_ASSERT (pAiAnimationComponent);
//			pAiAnimationComponent->GetAnimationState ().SetRequestedStance (stance);
//		}
//			break;
//
//		case CHARACTER_EVENT_FALL:
//			return State_Fall;
//	}
//
//	return State_Continue;
//}
//
//
//const CCharacterStateAIMovement::TStateIndex CCharacterStateAIMovement::Fall (CCharacter& Character, const SStateEvent& event)
//{
//	CRY_ASSERT (Character.IsAIControlled ());
//
//	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId ());
//	switch (eventID)
//	{
//		case STATE_EVENT_ENTER:
//			m_stateJump.OnEnter (Character);
//			m_stateJump.OnFall (Character);
//			m_flags.AddFlags (ECharacterStateFlags_InAir);
//			break;
//
//		case STATE_EVENT_EXIT:
//			m_flags.ClearFlags (
//				ECharacterStateFlags_InAir |
//				ECharacterStateFlags_Jump |
//				ECharacterStateFlags_Sprinting);
//			m_stateJump.OnExit (Character, m_flags.AreAnyFlagsActive (ECharacterStateFlags_CurrentItemIsHeavy));
//			break;
//
//		case CHARACTER_EVENT_PREPHYSICSUPDATE:
//		{
//			const SCharacterPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventCharacterMovementPrePhysics&> (event).GetPrePhysicsData ();
//			Character.m_actorState.inAir += prePhysicsEvent.m_frameTime;
//
//			if (m_stateJump.OnPrePhysicsUpdate (Character, m_flags.AreAnyFlagsActive (ECharacterStateFlags_CurrentItemIsHeavy), prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime))
//			{
//				return State_Ground;
//			}
//
//			CCharacterStateUtil::ProcessRotation (Character, prePhysicsEvent.m_movement, Character.GetMoveRequest ());
//			CCharacterStateUtil::FinalizeMovementRequest (Character, prePhysicsEvent.m_movement, Character.GetMoveRequest ());
//		}
//			break;
//	}
//
//	return State_Continue;
//}
//
//
//const CCharacterStateAIMovement::TStateIndex CCharacterStateAIMovement::Swim (CCharacter& Character, const SStateEvent& event)
//{
//	CRY_ASSERT (Character.IsAIControlled ());
//
//	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId ());
//	switch (eventID)
//	{
//		case STATE_EVENT_ENTER:
//			m_stateSwim.OnEnter (Character);
//			m_flags.AddFlags (ECharacterStateFlags_Swimming);
//			break;
//
//		case STATE_EVENT_EXIT:
//			m_flags.ClearFlags (ECharacterStateFlags_Swimming);
//			m_stateSwim.OnExit (Character);
//			break;
//
//		case CHARACTER_EVENT_PREPHYSICSUPDATE:
//		{
//			const SCharacterPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventCharacterMovementPrePhysics&> (event).GetPrePhysicsData ();
//
//			ProcessSprint (Character, prePhysicsEvent);
//
//			Character.m_CharacterStateSwim_WaterTestProxy.PreUpdateSwimming (Character, prePhysicsEvent.m_frameTime);
//			Character.m_CharacterStateSwim_WaterTestProxy.Update (Character, prePhysicsEvent.m_frameTime);
//
//			if (Character.m_CharacterStateSwim_WaterTestProxy.ShouldSwim ())
//			{
//				m_stateSwim.OnPrePhysicsUpdate (Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime);
//			}
//			else
//			{
//				return State_Ground;
//			}
//
//			CCharacterStateUtil::ProcessRotation (Character, prePhysicsEvent.m_movement, Character.GetMoveRequest ());
//			CCharacterStateUtil::FinalizeMovementRequest (Character, prePhysicsEvent.m_movement, Character.GetMoveRequest ());
//		}
//			break;
//
//		case CHARACTER_EVENT_UPDATE:
//			m_stateSwim.OnUpdate (Character, static_cast<const SStateEventUpdate&>(event).GetFrameTime ());
//			break;
//	}
//
//	return State_Continue;
//}
//
//const CCharacterStateAIMovement::TStateIndex CCharacterStateAIMovement::SwimTest (CCharacter& Character, const SStateEvent& event)
//{
//	CRY_ASSERT (Character.IsAIControlled ());
//
//	switch (event.GetEventId ())
//	{
//		case CHARACTER_EVENT_PREPHYSICSUPDATE:
//		{
//			const SCharacterPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventCharacterMovementPrePhysics&> (event).GetPrePhysicsData ();
//
//			Character.m_CharacterStateSwim_WaterTestProxy.PreUpdateNotSwimming (Character, prePhysicsEvent.m_frameTime);
//
//			Character.m_CharacterStateSwim_WaterTestProxy.Update (Character, prePhysicsEvent.m_frameTime);
//
//			if (Character.m_CharacterStateSwim_WaterTestProxy.ShouldSwim ())
//			{
//				return State_Swim;
//			}
//		}
//			break;
//	}
//
//	return State_Continue;
//}
//
//
//const CCharacterStateAIMovement::TStateIndex CCharacterStateAIMovement::StateGroundInput (CCharacter& Character, const SInputEventData& inputEvent)
//{
//	switch (inputEvent.m_inputEvent)
//	{
//		case SInputEventData::EInputEvent_Sprint:
//			StateSprintInput (Character, inputEvent);
//			break;
//	}
//
//	return State_Continue;
//}
//
//
//void CCharacterStateAIMovement::StateSprintInput (CCharacter& Character, const SInputEventData& inputEvent)
//{
//	CRY_ASSERT (inputEvent.m_inputEvent == SInputEventData::EInputEvent_Sprint);
//
//	m_flags.SetFlags (ECharacterStateFlags_SprintPressed, inputEvent.m_activationMode == eAAM_OnPress);
//}
//
//
//void CCharacterStateAIMovement::ProcessSprint (const CCharacter& Character, const SCharacterPrePhysicsData& prePhysicsEvent)
//{
//	if (m_flags.AreAnyFlagsActive (ECharacterStateFlags_SprintPressed | ECharacterStateFlags_Sprinting) && CCharacterStateUtil::IsSprintingAllowed (Character, prePhysicsEvent.m_movement, Character.GetCurrentItem ()))
//	{
//		m_flags.AddFlags (ECharacterStateFlags_Sprinting);
//		if (Character.GetCharacterInput ()->GetType () == ICharacterInput::CHARACTER_INPUT)
//		{
//			static_cast<CCharacterInput*> (Character.GetCharacterInput ())->ClearCrouchAction ();
//		}
//	}
//	else
//	{
//		m_flags.ClearFlags (ECharacterStateFlags_Sprinting);
//	}
//}
