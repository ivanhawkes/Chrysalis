/**
\file	D:\CRYENGINE3\Chrysalis\Source\Chrysalis\Actor\Character\Movement\StateMachine\CharacterStateMovement.cpp

Implements the character state movement class.

NOTE: If you use "SetStance" you can't also implement the ACTOR_EVENT_STANCE_CHANGED within the same hierarchy!
Use "OnSetStance" if you need this functionality!
*/
#include <StdAfx.h>

#include <StateMachine/StateMachine.h>
#include <IAnimatedCharacter.h>
#include <Actor/Character/Character.h>
#include <Actor/Movement/StateMachine/ActorStateEvents.h>
#include "CharacterStateEvents.h"
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include "CharacterStateDead.h"
#include "CharacterStateFly.h"
#include "CharacterStateGround.h"
#include "CharacterStateJump.h"
#include "CharacterStateSwim.h"
#include "CharacterStateLadder.h"
/*#include "CharacterInput.h"
#include "Weapon.h"
#include "Melee.h"
#include "SlideController.h"
#include "Effects/GameEffects/WaterEffects.h"*/


class CCharacterStateMovement : private CStateHierarchy < CCharacter >
{
	DECLARE_STATE_CLASS_BEGIN(CCharacter, CCharacterStateMovement)
		DECLARE_STATE_CLASS_ADD(CCharacter, MovementRoot);
		DECLARE_STATE_CLASS_ADD(CCharacter, GroundMovement);
		DECLARE_STATE_CLASS_ADD(CCharacter, Dead);
		DECLARE_STATE_CLASS_ADD(CCharacter, Fly);
		DECLARE_STATE_CLASS_ADD(CCharacter, Ground);
		DECLARE_STATE_CLASS_ADD(CCharacter, GroundFall);
		DECLARE_STATE_CLASS_ADD(CCharacter, FallTest);
		DECLARE_STATE_CLASS_ADD(CCharacter, Jump);
		DECLARE_STATE_CLASS_ADD(CCharacter, Fall);
		DECLARE_STATE_CLASS_ADD(CCharacter, Slide);
		DECLARE_STATE_CLASS_ADD(CCharacter, SlideFall);
		DECLARE_STATE_CLASS_ADD(CCharacter, Swim);
		//		DECLARE_STATE_CLASS_ADD (CCharacter, Spectate);
		DECLARE_STATE_CLASS_ADD(CCharacter, Intro);
		DECLARE_STATE_CLASS_ADD(CCharacter, SwimTest);
		DECLARE_STATE_CLASS_ADD(CCharacter, Ledge);
		DECLARE_STATE_CLASS_ADD(CCharacter, Ladder);
		DECLARE_STATE_CLASS_ADD_DUMMY(CCharacter, NoMovement);
		DECLARE_STATE_CLASS_ADD_DUMMY(CCharacter, GroundFallTest);
		DECLARE_STATE_CLASS_ADD_DUMMY(CCharacter, SlideFallTest);
	DECLARE_STATE_CLASS_END(CCharacter);

private:
	const TStateIndex StateGroundInput(CCharacter& Character, const SInputEventData& inputEvent);
	void StateSprintInput(CCharacter& Character, const SInputEventData& inputEvent);
	void ProcessSprint(CCharacter& Character, const SActorPrePhysicsData& prePhysicsEvent);
	void OnSpecialMove(CCharacter &Character, IActorEventListener::ESpecialMove specialMove);
	bool IsActionControllerValid(CCharacter& Character) const;

	void CreateWaterEffects();
	void ReleaseWaterEffects();
	void TriggerOutOfWaterEffectIfNeeded(const CCharacter& Character);

	void UpdateCharacterStanceTag(CCharacter &Character);

	CCharacterStateDead m_stateDead;
	CCharacterStateFly m_stateFly;
	CCharacterStateGround m_stateGround;
	CCharacterStateJump m_stateJump;
	CCharacterStateSwim m_stateSwim;
	CCharacterStateLedge m_stateCharacterLedge;
	CCharacterStateLadder m_stateLadder;

	/*CCharacterStateSpectate m_stateSpectate;*/
	/*CSlideController m_slideController;*/
	/*CWaterGameEffects* m_pWaterEffects;*/
};


DEFINE_STATE_CLASS_BEGIN(CCharacter, CCharacterStateMovement, CHARACTER_STATE_MOVEMENT, Ground)
	DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, MovementRoot, Root)
		DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, GroundMovement, MovementRoot)
			DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, SwimTest, GroundMovement)
				DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Ground, SwimTest)
					DEFINE_STATE_CLASS_ADD_DUMMY(CCharacter, CCharacterStateMovement, GroundFallTest, FallTest, Ground)
						DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, GroundFall, GroundFallTest)
				DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Slide, SwimTest)
					DEFINE_STATE_CLASS_ADD_DUMMY(CCharacter, CCharacterStateMovement, SlideFallTest, FallTest, Slide)
						DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, SlideFall, SlideFallTest)
		DEFINE_STATE_CLASS_ADD_DUMMY(CCharacter, CCharacterStateMovement, NoMovement, SwimTest, MovementRoot)
			DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Jump, NoMovement)
				DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Fall, Jump)
		DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Swim, MovementRoot)
		DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Ladder, MovementRoot)
	DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Dead, Root)
	DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Ledge, Root)
	DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Fly, Root)
	//	DEFINE_STATE_CLASS_ADD (CCharacter, CCharacterStateMovement, Spectate, Root)
	DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateMovement, Intro, Root)
DEFINE_STATE_CLASS_END(CCharacter, CCharacterStateMovement);


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Root(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_INIT:
			//m_pWaterEffects = NULL;
			if (Character.IsClient())
			{
				CreateWaterEffects();
			}
			break;

		case STATE_EVENT_RELEASE:
			ReleaseWaterEffects();
			break;

		case STATE_EVENT_SERIALIZE:
		{
			//TSerialize& serializer = static_cast<const SStateEventSerialize&> (event).GetSerializer ();
			//if (serializer.IsReading ())
			//{
			//	EStance stance = STANCE_STAND;
			//	serializer.EnumValue ("StateStance", stance, STANCE_NULL, STANCE_LAST);
			//	if (stance != STANCE_NULL)
			//	{
			//		Character.OnSetStance (stance);
			//	}
			//}
			//else
			//{
			//	EStance stance = Character.GetStance ();
			//	serializer.EnumValue ("StateStance", stance, STANCE_NULL, STANCE_LAST);
			//}
		}
		break;

		case CHARACTER_EVENT_CUTSCENE:
			if (static_cast<const SStateEventCutScene&> (event).IsEnabling())
			{
				RequestTransitionState(Character, CHARACTER_STATE_ANIMATION, event);
			}
			break;

		case ACTOR_EVENT_WEAPONCHANGED:
		{
			//m_flags.ClearFlags (eActorStateFlags_CurrentItemIsHeavy);

			//const CWeapon* pWeapon = static_cast<const CWeapon*> (event.GetData (0).GetPtr ());
			//if (pWeapon && pWeapon->IsHeavyWeapon ())
			//{
			//	m_flags.AddFlags (eActorStateFlags_CurrentItemIsHeavy);
			//}
		}
		break;

		//case CHARACTER_EVENT_SPECTATE:
		//	return State_Spectate;

		case ACTOR_EVENT_DEAD:
			return State_Dead;

		case ACTOR_EVENT_INTERACTIVE_ACTION:
			RequestTransitionState(Character, CHARACTER_STATE_ANIMATION, event);
			break;

			//case CHARACTER_EVENT_BUTTONMASHING_SEQUENCE:
			//	RequestTransitionState(Character, CHARACTER_STATE_ANIMATION, event);
			//	break;

		case ACTOR_EVENT_BECOME_LOCAL_PLAYER:
			CreateWaterEffects();
			break;

		case STATE_EVENT_DEBUG:
		{
			AUTOENUM_BUILDNAMEARRAY(stateFlags, eActorStateFlags);
			STATE_DEBUG_EVENT_LOG(this, event, false, state_red, "Active: StateMovement: CurrentFlags: %s", AutoEnum_GetStringFromBitfield(m_flags.GetRawFlags(), stateFlags, sizeof(stateFlags) / sizeof(char*)).c_str());
		}
		break;
	}

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::MovementRoot(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			// TODO: What is this?
			//Character.SetCanTurnBody (true);
			break;

		case ACTOR_EVENT_LEDGE:
			return State_Ledge;

		case CHARACTER_EVENT_LADDER:
			return State_Ladder;

		case ACTOR_EVENT_INPUT:
			return StateGroundInput(Character, static_cast<const SStateEventActorInput&> (event).GetInputEventData());

		case ACTOR_EVENT_FLY:
			return State_Fly;

		case CHARACTER_EVENT_ATTACH:
			RequestTransitionState(Character, CHARACTER_STATE_LINKED, event);
			break;

		case CHARACTER_EVENT_INTRO_START:
			return State_Intro;
	}

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::GroundMovement(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			CCharacterStateUtil::InitializeMoveRequest(Character.GetMoveRequest());
			Character.GetActorState()->inAir = 0.f;
			break;

		case STATE_EVENT_EXIT:
			Character.GetActorState()->onGround = 0.f;
			m_flags.ClearFlags(eActorStateFlags_Sprinting);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
			Character.GetActorState()->onGround += prePhysicsEvent.m_frameTime;

			// Only send the event if we've been flying for 2 consecutive frames - this prevents some state thrashing.
			if (Character.GetActorPhysics()->flags.AreAllFlagsActive(SActorPhysics::EActorPhysicsFlags::WasFlying | SActorPhysics::EActorPhysicsFlags::Flying))
			{
				Character.StateMachineHandleEventMovement(SStateEventGroundColliderChanged(false));
			}

			if (CCharacterStateUtil::IsJumpAllowed(Character, prePhysicsEvent.m_movement))
			{
				Character.StateMachineHandleEventMovement(SStateEventJump(prePhysicsEvent));

				return State_Done;
			}

			CCharacterStateUtil::ProcessRotation(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
			CCharacterStateUtil::FinalizeMovementRequest(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
		}
		break;

		//case CHARACTER_EVENT_SLIDE:
		//	return State_Slide;
	}

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Dead(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateDead.OnEnter(Character);
			break;

		case STATE_EVENT_EXIT:
			m_stateDead.OnLeave(Character);
			break;

		case STATE_EVENT_SERIALIZE:
		{
			TSerialize& serializer = static_cast<const SStateEventSerialize&> (event).GetSerializer();
			m_stateDead.Serialize(serializer);
		}
		break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

			m_stateDead.OnPrePhysicsUpdate(Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime);
		}
		break;

		case ACTOR_EVENT_UPDATE:
		{
			CCharacterStateDead::UpdateCtx updateCtx;
			updateCtx.frameTime = static_cast<const SStateEventUpdate&>(event).GetFrameTime();
			m_stateDead.OnUpdate(Character, updateCtx);
		}
		break;

		case ACTOR_EVENT_DEAD:
			return State_Done;
	}

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Fly(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateFly.OnEnter(Character);
			Character.GetActorState()->inAir = 0.0f;
			break;

		case STATE_EVENT_EXIT:
			Character.GetActorState()->inAir = 0.0f;
			m_stateFly.OnExit(Character);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
			Character.GetActorState()->inAir += prePhysicsEvent.m_frameTime;

			if (!m_stateFly.OnPrePhysicsUpdate(Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}

			// ILH: check this code over once some movement is possible. Make sure the physics works as expected.

			// Process movement.
			CCharacterStateUtil::ProcessRotation(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
			CCharacterStateUtil::FinalizeMovementRequest(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
		}
		return State_Done;

		case ACTOR_EVENT_FLY:
		{
			const int flyMode = static_cast<const SStateEventFly&> (event).GetFlyMode();
			Character.SetFlyMode(flyMode);
		}
		return State_Done;

	}

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Ladder(CCharacter& Character, const SStateEvent& event)
{
	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());

	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_flags.AddFlags(eActorStateFlags_OnLadder);
			break;

		case STATE_EVENT_EXIT:
			m_flags.ClearFlags(eActorStateFlags_OnLadder);
			m_stateLadder.OnExit(Character);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

			if (!m_stateLadder.OnPrePhysicsUpdate(Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}

			CCharacterStateUtil::ProcessRotation(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
			CCharacterStateUtil::FinalizeMovementRequest(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
		}
		break;

		case CHARACTER_EVENT_LADDER:
		{
			Character.GetActorState()->EnableStatusFlags(kActorStatus_onLadder);
			const SStateEventLadder& ladderEvent = static_cast<const SStateEventLadder&>(event);
			m_stateLadder.OnUseLadder(Character, ladderEvent.GetLadder());
		}
		break;

		case CHARACTER_EVENT_LADDER_LEAVE:
		{
			Character.GetActorState()->DisableStatusFlags(kActorStatus_onLadder);
			const SStateEventLeaveLadder& leaveLadderEvent = static_cast<const SStateEventLeaveLadder&>(event);
			ELadderLeaveLocation leaveLoc = leaveLadderEvent.GetLeaveLocation();
			m_stateLadder.LeaveLadder(Character, leaveLoc);
			if (leaveLoc == eLLL_Drop)
			{
				return State_Fall;
			}
		}
		break;

		case CHARACTER_EVENT_LADDER_POSITION:
		{
			const SStateEventLadderPosition& ladderEvent = static_cast<const SStateEventLadderPosition&>(event);
			float heightFrac = ladderEvent.GetHeightFrac();
			m_stateLadder.SetHeightFrac(Character, heightFrac);
		}
		break;
	}

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Intro(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_GROUND:
		case CHARACTER_EVENT_INTRO_FINISHED:
			return State_Ground;
	}

	return State_Continue;
}


/*const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Spectate(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateSpectate.OnEnter(Character);
			Character.SetHealth(0.0f);
			m_flags.AddFlags(eActorStateFlags_Spectator);
			break;

		case STATE_EVENT_EXIT:
			m_flags.ClearFlags(eActorStateFlags_Spectator);
			m_stateSpectate.OnExit(Character);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

			if (!m_stateSpectate.OnPrePhysicsUpdate(Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}

			// Unsure if Spectate requires a move request.
			CCharacterStateUtil::ProcessRotation(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
			CCharacterStateUtil::FinalizeMovementRequest(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
		}
			break;

		case ACTOR_EVENT_UPDATE:
		{
			if (Character.IsClient())
			{
				m_stateSpectate.UpdateFade(static_cast<const SStateEventUpdate&>(event).GetFrameTime());
			}
		}
			break;

		//case CHARACTER_EVENT_RESET_SPECTATOR_SCREEN:
		//	m_stateSpectate.ResetFadeParameters();
		//	break;

		case CHARACTER_EVENT_SPECTATE:
			return State_Done;

		case ACTOR_EVENT_DEAD:
			// when we enter this state, we SetHealth(0.0f), this will trigger a transition to the Dead state without this interception.
			return State_Done;
	}

	return State_Continue;
}*/


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Ground(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateGround.OnEnter(Character);
			m_flags.AddFlags(eActorStateFlags_Ground);
			break;

		case STATE_EVENT_EXIT:
			m_flags.ClearFlags(eActorStateFlags_Ground);
			m_stateGround.OnExit(Character);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

			ProcessSprint(Character, prePhysicsEvent);

			m_stateGround.OnPrePhysicsUpdate(Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime,
				m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy), Character.IsPlayer());
		}
		break;

		case ACTOR_EVENT_JUMP:
			return State_Jump;

		case ACTOR_EVENT_STANCE_CHANGED:
			CCharacterStateUtil::ChangeStance(Character, event);
			break;

			//case CHARACTER_EVENT_STEALTHKILL:
			//	RequestTransitionState(Character, CHARACTER_STATE_ANIMATION, event);
			//	break;

		case ACTOR_EVENT_FALL:
			return State_Fall;

		case ACTOR_EVENT_GROUND_COLLIDER_CHANGED:
			if (!static_cast<const SStateEventGroundColliderChanged&> (event).OnGround())
			{
				return State_GroundFall;
			}
			break;
	}

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::GroundFall(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_GROUND_COLLIDER_CHANGED:
			if (static_cast<const SStateEventGroundColliderChanged&> (event).OnGround())
			{
				Character.GetActorState()->inAir = 0.0f;

				return State_Ground;
			}
			break;
	}

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::SlideFall(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_GROUND_COLLIDER_CHANGED:
			if (static_cast<const SStateEventGroundColliderChanged&> (event).OnGround())
			{
				Character.GetActorState()->inAir = 0.0f;

				return State_Slide;
			}
			break;
	}

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::FallTest(CCharacter& Character, const SStateEvent& event)
{
	/*	CRY_ASSERT(!Character.IsAIControlled());

		const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
		switch (eventID)
		{
			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				// We're considered inAir at this point.
				//  - NOTE: this does not mean CCharacter::IsInAir() returns true.
				float inAir = Character.GetActorState()->inAir;
				inAir += prePhysicsEvent.m_frameTime;
				Character.GetActorState()->inAir = inAir;

				if (!CCharacterStateUtil::IsOnGround(Character))
				{
					const float groundHeight = Character.m_CharacterStateSwimWaterTestProxy.GetLastRaycastResult();
					const float heightZ = Character.GetEntity()->GetPos().z;

					// Only consider falling if our fall distance is sufficient - this is to prevent transitioning to the Fall state
					// due to physics erroneously telling us we're flying (due to small bumps on the terrain/ground).
					if ((heightZ - groundHeight) > g_pGameCVars->pl_fallHeight)
					{
						// Only fall if we've been falling for a sufficient amount of time - this is for gameplay feel reasons.
						if (inAir > g_pGameCVars->pl_movement.ground_timeInAirToFall)
						{
							Character.StateMachineHandleEventMovement(ACTOR_EVENT_FALL);
							return State_Done;
						}
					}

					// Use the swim proxy to get the ground height as we're periodically casting that ray anyway.
					Character.m_CharacterStateSwimWaterTestProxy.ForceUpdateBottomLevel(Character);
				}
				else
				{
					// We're back on the ground, so send the collider changed event.
					Character.StateMachineHandleEventMovement(SStateEventGroundColliderChanged(true));
				}
				break;
			}
		}
	*/
	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Jump(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateJump.OnEnter(Character);
			m_flags.AddFlags(eActorStateFlags_InAir | eActorStateFlags_Jump);
			m_flags.ClearFlags(eActorStateFlags_Sprinting);
			break;

		case STATE_EVENT_EXIT:
		{
			m_flags.ClearFlags(
				eActorStateFlags_InAir |
				eActorStateFlags_Jump |
				eActorStateFlags_Sprinting);
			m_stateJump.OnExit(Character, m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy));
		}
		break;

		case ACTOR_EVENT_JUMP:
		{
			const float fVerticalSpeedModifier = static_cast<const SStateEventJump&> (event).GetVerticalSpeedModifier();
			m_stateJump.OnJump(Character, m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy), fVerticalSpeedModifier);

			const SActorPrePhysicsData& data = static_cast<const SStateEventJump&> (event).GetPrePhysicsEventData();
			Character.StateMachineHandleEventMovement(SStateEventActorMovementPrePhysics(&data));
		}
		break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
			Character.GetActorState()->inAir += prePhysicsEvent.m_frameTime;

			if (m_stateJump.OnPrePhysicsUpdate(Character, m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy), prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}

			ProcessSprint(Character, prePhysicsEvent);

			//SLedgeTransitionData ledge(LedgeId::invalid_id);
			//if (CCharacterStateLedge::TryLedgeGrab(Character, m_stateJump.GetExpectedJumpEndHeight(),
			//	m_stateJump.GetStartFallingHeight(), m_stateJump.GetSprintJump(), &ledge, Character.m_jumpButtonIsPressed))
			//{
			//	ledge.m_comingFromOnGround = false; // this definitely true here?
			//	ledge.m_comingFromSprint = m_flags.AreAnyFlagsActive(eActorStateFlags_Sprinting);

			//	SStateEventLedge ledgeEvent(ledge);
			//	Character.StateMachineHandleEventMovement(ledgeEvent);
			//}

			//Character.m_CharacterStateSwimWaterTestProxy.ForceUpdateBottomLevel(Character);

			CCharacterStateUtil::ProcessRotation(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
			CCharacterStateUtil::FinalizeMovementRequest(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
		}
	}

	return State_Continue;
}

const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Fall(CCharacter& Character, const SStateEvent& event)
{
	CRY_ASSERT(!Character.IsAIControlled());

	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateJump.OnEnter(Character);
			m_stateJump.OnFall(Character);
			m_flags.AddFlags(eActorStateFlags_InAir);
			return State_Done;
	}

	return State_Continue;
}

const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Slide(CCharacter& Character, const SStateEvent& event)
{
	/*	CRY_ASSERT(!Character.IsAIControlled());

		const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
		switch (eventID)
		{
			case STATE_EVENT_ENTER:
				Character.SetCanTurnBody(false);
				m_slideController.GoToState(Character, eSlideState_Sliding);
				m_flags.AddFlags(eActorStateFlags_Sliding);
				m_flags.ClearFlags(eActorStateFlags_Sprinting);
				break;

			case STATE_EVENT_EXIT:
				m_flags.ClearFlags(
					eActorStateFlags_Sprinting |
					eActorStateFlags_ExitingSlide |
					eActorStateFlags_NetExitingSlide |
					eActorStateFlags_NetSlide |
					eActorStateFlags_Sliding);
				m_slideController.GoToState(Character, eSlideState_None);
				Character.SetCanTurnBody(true);
				break;

			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				m_slideController.Update(Character, prePhysicsEvent.m_frameTime, prePhysicsEvent.m_movement, m_flags.AreAnyFlagsActive(eActorStateFlags_NetSlide), m_flags.AreAnyFlagsActive(eActorStateFlags_NetExitingSlide), Character.GetMoveRequest());

				m_slideController.GetCurrentState() == eSlideState_ExitingSlide ? m_flags.AddFlags(eActorStateFlags_ExitingSlide) : m_flags.ClearFlags(eActorStateFlags_ExitingSlide);

				if (m_slideController.IsActive())
				{
					return State_Continue;
				}
			}
				return State_Ground;

			case CHARACTER_EVENT_SLIDE_KICK:
			{
				const SStateEventSlideKick& kickData = static_cast<const SStateEventSlideKick&> (event);

				const float timer = m_slideController.DoSlideKick(Character);
				kickData.GetMelee()->SetDelayTimer(timer);
				break;
			}

			case CHARACTER_EVENT_SLIDE_EXIT:
				m_slideController.GoToState(Character, eSlideState_ExitingSlide);
				return State_Ground;

			case CHARACTER_EVENT_SLIDE_EXIT_LAZY:
				m_slideController.LazyExitSlide(Character);
				break;

			case CHARACTER_EVENT_SLIDE_EXIT_FORCE:
				m_slideController.GoToState(Character, eSlideState_None);
				return State_Ground;

			case CHARACTER_EVENT_SLIDE:
				return State_Done;

			case ACTOR_EVENT_STANCE_CHANGED:
				CCharacterStateUtil::ChangeStance(Character, event);
				break;

			case ACTOR_EVENT_FALL:
				return State_Fall;

			case ACTOR_EVENT_JUMP:
				return State_Done;

			case ACTOR_EVENT_GROUND_COLLIDER_CHANGED:
				if (!static_cast<const SStateEventGroundColliderChanged&> (event).OnGround())
				{
					return State_SlideFall;
				}
				break;
		}
	*/
	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Swim(CCharacter& Character, const SStateEvent& event)
{
	/*	CRY_ASSERT(!Character.IsAIControlled());

		const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
		switch (eventID)
		{
			case STATE_EVENT_ENTER:
				m_stateSwim.OnEnter(Character);
				m_flags.AddFlags(eActorStateFlags_Swimming);
				Character.OnSetStance(STANCE_SWIM);
				Character.GetActorState()->onGround = 0.0f;
				Character.GetActorState()->inAir = 0.0f;
				break;

			case STATE_EVENT_EXIT:
				m_flags.ClearFlags(eActorStateFlags_Swimming);
				m_stateSwim.OnExit(Character);
				Character.OnSetStance(STANCE_STAND);

				break;

			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				ProcessSprint(Character, prePhysicsEvent);

				Character.m_CharacterStateSwimWaterTestProxy.PreUpdateSwimming(Character, prePhysicsEvent.m_frameTime);
				Character.m_CharacterStateSwimWaterTestProxy.Update(Character, prePhysicsEvent.m_frameTime);

				TriggerOutOfWaterEffectIfNeeded(Character);

				if (Character.m_CharacterStateSwimWaterTestProxy.ShouldSwim())
				{
					float verticalSpeedModifier = 0.0f;
					if (m_stateSwim.DetectJump(Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime, &verticalSpeedModifier))
					{
						Character.StateMachineHandleEventMovement(SStateEventJump(prePhysicsEvent, verticalSpeedModifier));

						return State_Done;
					}
					m_stateSwim.OnPrePhysicsUpdate(Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime);
				}
				else
				{
					return State_Ground;
				}

				CCharacterStateUtil::ProcessRotation(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
				CCharacterStateUtil::FinalizeMovementRequest(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
			}
				break;

			case ACTOR_EVENT_JUMP:
				return State_Jump;

			case ACTOR_EVENT_UPDATE:
				m_stateSwim.OnUpdate(Character, static_cast<const SStateEventUpdate&>(event).GetFrameTime());
				break;

			case ACTOR_EVENT_FALL:
				return State_Fall;
		}
	*/
	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::SwimTest(CCharacter& Character, const SStateEvent& event)
{
	/*	CRY_ASSERT(!Character.IsAIControlled());

		switch (event.GetEventId())
		{
			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				Character.m_CharacterStateSwimWaterTestProxy.PreUpdateNotSwimming(Character, prePhysicsEvent.m_frameTime);

				Character.m_CharacterStateSwimWaterTestProxy.Update(Character, prePhysicsEvent.m_frameTime);

				TriggerOutOfWaterEffectIfNeeded(Character);

				if (Character.m_CharacterStateSwimWaterTestProxy.ShouldSwim())
				{
					return State_Swim;
				}
			}
				break;
		}*/

	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::Ledge(CCharacter& Character, const SStateEvent& event)
{
	/*	CRY_ASSERT(!Character.IsAIControlled());

		switch (event.GetEventId())
		{
			case STATE_EVENT_ENTER:
				m_flags.AddFlags(eActorStateFlags_Ledge);
				Character.SetLastTimeInLedge(gEnv->pTimer->GetAsyncCurTime());
				Character.SetStance(STANCE_STAND);
				if (gEnv->bMultiCharacter && Character.IsClient())
				{
					SHUDEvent ledgeEvent(eHUDEvent_OnUseLedge);
					ledgeEvent.AddData(true);
					CHUDEventDispatcher::CallEvent(ledgeEvent);
				}
				break;

			case STATE_EVENT_EXIT:
				m_flags.ClearFlags(eActorStateFlags_Ledge);
				m_stateCharacterLedge.OnExit(Character);
				if (gEnv->bMultiCharacter && Character.IsClient())
				{
					SHUDEvent ledgeEvent(eHUDEvent_OnUseLedge);
					ledgeEvent.AddData(false);
					CHUDEventDispatcher::CallEvent(ledgeEvent);
				}
				break;

			case STATE_EVENT_SERIALIZE:
			{
				TSerialize& serializer = static_cast<const SStateEventSerialize&> (event).GetSerializer();

				m_stateCharacterLedge.Serialize(serializer);
				break;
			}

			case STATE_EVENT_POST_SERIALIZE:
			{
				m_stateCharacterLedge.PostSerialize(Character);
				break;
			}

			case ACTOR_EVENT_LEDGE:
			{
				const SStateEventLedge &ledgeEvent = static_cast<const SStateEventLedge&> (event);

				const bool comingFromSprint = ledgeEvent.GetComingFromSprint();
				if (comingFromSprint)
				{
					m_flags.AddFlags(eActorStateFlags_Sprinting); // movement will have stripped this flag as it exited.. we want to carry on sprinting at the end of this sprint vault if possible
				}

				m_stateCharacterLedge.OnEnter(Character, ledgeEvent);
				return State_Done;
			}

			case ACTOR_EVENT_LEDGE_ANIM_FINISHED:
				m_stateCharacterLedge.OnAnimFinished(Character);
				break;

			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
				m_stateCharacterLedge.OnPrePhysicsUpdate(Character, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime);
				CCharacterStateUtil::FinalizeMovementRequest(Character, prePhysicsEvent.m_movement, Character.GetMoveRequest());
				break;
			}

			case ACTOR_EVENT_FALL:
				return State_Fall;

			case ACTOR_EVENT_GROUND:
				return State_Ground;

			case ACTOR_EVENT_INPUT:
				// Ensure any sprint input changes are updated and not lost mid-ledge grab / vault.
				return StateGroundInput(Character, static_cast<const SStateEventActorInput&> (event).GetInputEventData());
		}
	*/
	return State_Continue;
}


const CCharacterStateMovement::TStateIndex CCharacterStateMovement::StateGroundInput(CCharacter& Character, const SInputEventData& inputEvent)
{
	switch (inputEvent.m_inputEvent)
	{
		case SInputEventData::EInputEvent_Sprint:
			StateSprintInput(Character, inputEvent);
			break;
	}

	return State_Continue;
}


void CCharacterStateMovement::StateSprintInput(CCharacter& Character, const SInputEventData& inputEvent)
{
	CRY_ASSERT(inputEvent.m_inputEvent == SInputEventData::EInputEvent_Sprint);
	inputEvent.m_activationMode == eAAM_OnPress ? m_flags.AddFlags(eActorStateFlags_SprintPressed)
		: m_flags.ClearFlags(eActorStateFlags_SprintPressed);
}


void CCharacterStateMovement::ProcessSprint(CCharacter& Character, const SActorPrePhysicsData& prePhysicsEvent)
{
	/*	// If sprint toggle is active, then we want to sprint if it is pressed or if we are sprinting.
		// If sprint toggle is off, then we only want to sprint if it is pressed.
		uint32 flagsToCheck = g_pGameCVars->cl_sprintToggle ? (eActorStateFlags_SprintPressed | eActorStateFlags_Sprinting) : (eActorStateFlags_SprintPressed);

		if (m_flags.AreAnyFlagsActive(flagsToCheck) && CCharacterStateUtil::IsSprintingAllowed(Character, prePhysicsEvent.m_movement, Character.GetCurrentItem()))
		{
			if (!Character.IsSprinting())
			{
				// notify IActorEventListener about sprinting (just once)
				OnSpecialMove(Character, IActorEventListener::eSM_SpeedSprint);
			}

			m_flags.AddFlags(eActorStateFlags_Sprinting);
			if (Character.GetCharacterInput()->GetType() == ICharacterInput::CHARACTER_INPUT)
			{
				static_cast<CCharacterInput*> (Character.GetCharacterInput())->ClearCrouchAction();
				Character.SetStance(STANCE_STAND);
			}
		}
		else
		{
			m_flags.ClearFlags(eActorStateFlags_Sprinting);
		}*/
}


void CCharacterStateMovement::OnSpecialMove(CCharacter &Character, IActorEventListener::ESpecialMove specialMove)
{
	/*	if (Character.m_CharacterEventListeners.empty() == false)
		{
			CCharacter::TCharacterEventListeners::const_iterator iter = Character.m_CharacterEventListeners.begin();
			CCharacter::TCharacterEventListeners::const_iterator cur;
			while (iter != Character.m_CharacterEventListeners.end())
			{
				cur = iter;
				++iter;
				(*cur)->OnSpecialMove(&Character, specialMove);
			}
		}*/
}


bool CCharacterStateMovement::IsActionControllerValid(CCharacter& Character) const
{
	IAnimatedCharacter* pAnimatedCharacter = Character.GetAnimatedCharacter();

	if (pAnimatedCharacter != NULL)
	{
		return (pAnimatedCharacter->GetActionController() != NULL);
	}

	return false;
}


void CCharacterStateMovement::TriggerOutOfWaterEffectIfNeeded(const CCharacter& Character)
{
	/*if (m_pWaterEffects != NULL)
	{
	CRY_ASSERT (Character.IsClient ());

	if (Character.m_CharacterStateSwimWaterTestProxy.IsHeadComingOutOfWater ())
	{
	m_pWaterEffects->OnCameraComingOutOfWater ();
	}
	}*/
}


void CCharacterStateMovement::CreateWaterEffects()
{
	/*if (m_pWaterEffects == NULL)
	{
	m_pWaterEffects = new CWaterGameEffects ();
	m_pWaterEffects->Initialise ();
	}*/
}


void CCharacterStateMovement::ReleaseWaterEffects()
{
	/*if (m_pWaterEffects != NULL)
	{
	m_pWaterEffects->Release ();
	delete m_pWaterEffects;
	m_pWaterEffects = NULL;
	}*/
}


void CCharacterStateMovement::UpdateCharacterStanceTag(CCharacter &Character)
{
	/*IAnimatedCharacter *pAnimatedCharacter = Character.GetAnimatedCharacter ();
	if (pAnimatedCharacter)
	{
	IActionController *pActionController = pAnimatedCharacter->GetActionController ();
	if (pActionController)
	{
	Character.SetStanceTag (Character.GetStance (), pActionController->GetContext ().state);
	}
	}*/
}
