/**
Implements the character state movement class.

NOTE: If you use "SetStance" you can't also implement the ACTOR_EVENT_STANCE_CHANGED within the same hierarchy!
Use "OnSetStance" if you need this functionality!
*/
#include <StdAfx.h>

#include <StateMachine/StateMachine.h>
#include <IAnimatedCharacter.h>
#include <Actor/ActorComponent.h>
#include <Actor/Movement/StateMachine/ActorStateEvents.h>
#include "ActorStateEvents.h"
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include "ActorStateDead.h"
#include "ActorStateFly.h"
#include "ActorStateGround.h"
#include "ActorStateJump.h"
#include "ActorStateSwim.h"
#include "ActorStateLadder.h"
//#include "CharacterInput.h"
//#include "Weapon.h"
//#include "Melee.h"
//#include "SlideController.h"
//#include "Effects/GameEffects/WaterEffects.h"


namespace Chrysalis
{
class CActorStateMovement : private CStateHierarchy <CActorControllerComponent>
{
	DECLARE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateMovement)
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, MovementRoot);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, GroundMovement);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Dead);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Fly);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Ground);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, GroundFall);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, FallTest);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Jump);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Fall);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Slide);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, SlideFall);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Swim);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Intro);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, SwimTest);
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Ladder);
	DECLARE_STATE_CLASS_ADD_DUMMY(CActorControllerComponent, NoMovement);
	DECLARE_STATE_CLASS_ADD_DUMMY(CActorControllerComponent, GroundFallTest);
	DECLARE_STATE_CLASS_ADD_DUMMY(CActorControllerComponent, SlideFallTest);
	DECLARE_STATE_CLASS_END(CActorControllerComponent);

private:
	const TStateIndex StateGroundInput(CActorControllerComponent& actorControllerComponent, const SInputEventData& inputEvent);
	void StateSprintInput(CActorControllerComponent& actorControllerComponent, const SInputEventData& inputEvent);
	void ProcessSprint(CActorControllerComponent& actorControllerComponent, const SActorPrePhysicsData& prePhysicsEvent);
	void OnSpecialMove(CActorControllerComponent& actorControllerComponent, IActorEventListener::ESpecialMove specialMove);

	void CreateWaterEffects();
	void ReleaseWaterEffects();
	void TriggerOutOfWaterEffectIfNeeded(const CActorControllerComponent& actorControllerComponent);

	CActorStateDead m_stateDead;
	CActorStateFly m_stateFly;
	CActorStateGround m_stateGround;
	CActorStateJump m_stateJump;
	CActorStateSwim m_stateSwim;
	CActorStateLadder m_stateLadder;
	//CWaterGameEffects* m_pWaterEffects;
};


DEFINE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateMovement, ACTOR_STATE_MOVEMENT, Ground)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, MovementRoot, Root)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, GroundMovement, MovementRoot)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, SwimTest, GroundMovement)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, Ground, SwimTest)
DEFINE_STATE_CLASS_ADD_DUMMY(CActorControllerComponent, CActorStateMovement, GroundFallTest, FallTest, Ground)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, GroundFall, GroundFallTest)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, Slide, SwimTest)
DEFINE_STATE_CLASS_ADD_DUMMY(CActorControllerComponent, CActorStateMovement, SlideFallTest, FallTest, Slide)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, SlideFall, SlideFallTest)
DEFINE_STATE_CLASS_ADD_DUMMY(CActorControllerComponent, CActorStateMovement, NoMovement, SwimTest, MovementRoot)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, Jump, NoMovement)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, Fall, Jump)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, Swim, MovementRoot)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, Ladder, MovementRoot)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, Dead, Root)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, Fly, Root)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateMovement, Intro, Root)
DEFINE_STATE_CLASS_END(CActorControllerComponent, CActorStateMovement);


const CActorStateMovement::TStateIndex CActorStateMovement::Root(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_INIT:
			//m_pWaterEffects = NULL;
//			if (actorControllerComponent.IsClient())
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
			//		actorControllerComponent.OnSetStance (stance);
			//	}
			//}
			//else
			//{
			//	EStance stance = actorControllerComponent.GetStance ();
			//	serializer.EnumValue ("StateStance", stance, STANCE_NULL, STANCE_LAST);
			//}
		}
		break;

		case ACTOR_EVENT_CUTSCENE:
			if (static_cast<const SStateEventCutScene&> (event).IsEnabling())
			{
				RequestTransitionState(actorControllerComponent, ACTOR_STATE_ANIMATION, event);
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

		case ACTOR_EVENT_DEAD:
			return State_Dead;

		case ACTOR_EVENT_INTERACTIVE_ACTION:
			RequestTransitionState(actorControllerComponent, ACTOR_STATE_ANIMATION, event);
			break;

			//case ACTOR_EVENT_BUTTONMASHING_SEQUENCE:
			//	RequestTransitionState(actorControllerComponent, ACTOR_STATE_ANIMATION, event);
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


const CActorStateMovement::TStateIndex CActorStateMovement::MovementRoot(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			// #TODO: What is this?
			//actorControllerComponent.SetCanTurnBody (true);
			break;

		case ACTOR_EVENT_LADDER:
			return State_Ladder;

		case ACTOR_EVENT_INPUT:
			return StateGroundInput(actorControllerComponent, static_cast<const SStateEventActorInput&> (event).GetInputEventData());

		case ACTOR_EVENT_FLY:
			return State_Fly;

		case ACTOR_EVENT_ATTACH:
			RequestTransitionState(actorControllerComponent, ACTOR_STATE_LINKED, event);
			break;

		case ACTOR_EVENT_INTRO_START:
			return State_Intro;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::GroundMovement(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			//CActorStateUtility::InitializeMoveRequest(actorControllerComponent.GetMoveRequest());
			actorControllerComponent.durationInAir = 0.f;
			break;

		case STATE_EVENT_EXIT:
			actorControllerComponent.durationOnGround = 0.f;
			m_flags.ClearFlags(eActorStateFlags_Sprinting);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
			actorControllerComponent.durationOnGround += prePhysicsEvent.m_frameTime;

			// Only send the event if we've been flying for 2 consecutive frames - this prevents some state thrashing.
			// TODO: CRITICAL: HACK: BROKEN: !!
			//if (actorControllerComponent.GetActorPhysics()->flags.AreAllFlagsActive(SActorPhysics::EActorPhysicsFlags::WasFlying | SActorPhysics::EActorPhysicsFlags::Flying))
			//{
			//	actorControllerComponent.StateMachineHandleEventMovement(SStateEventGroundColliderChanged(false));
			//}

			if (CActorStateUtility::IsJumpAllowed(actorControllerComponent))
			{
				actorControllerComponent.StateMachineHandleEventMovement(SStateEventJump(prePhysicsEvent));

				return State_Done;
			}
		}
		break;

		//case ACTOR_EVENT_SLIDE:
		//	return State_Slide;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Dead(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateDead.OnEnter(actorControllerComponent);
			break;

		case STATE_EVENT_EXIT:
			m_stateDead.OnLeave(actorControllerComponent);
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

			m_stateDead.OnPrePhysicsUpdate(actorControllerComponent, prePhysicsEvent.m_frameTime);
		}
		break;

		case ACTOR_EVENT_UPDATE:
		{
			CActorStateDead::UpdateCtx updateCtx;
			updateCtx.frameTime = static_cast<const SStateEventUpdate&>(event).GetFrameTime();
			m_stateDead.OnUpdate(actorControllerComponent, updateCtx);
		}
		break;

		case ACTOR_EVENT_DEAD:
			return State_Done;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Fly(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateFly.OnEnter(actorControllerComponent);
			actorControllerComponent.durationInAir = 0.0f;
			break;

		case STATE_EVENT_EXIT:
			actorControllerComponent.durationInAir = 0.0f;
			m_stateFly.OnExit(actorControllerComponent);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
			actorControllerComponent.durationInAir += prePhysicsEvent.m_frameTime;

			if (!m_stateFly.OnPrePhysicsUpdate(actorControllerComponent, prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}
		}
		return State_Done;

		//case ACTOR_EVENT_FLY:
		//{
		//	const int flyMode = static_cast<const SStateEventFly&> (event).GetFlyMode();
		//	actorControllerComponent.SetFlyMode(flyMode);
		//}
		//return State_Done;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Ladder(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());

	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_flags.AddFlags(eActorStateFlags_OnLadder);
			break;

		case STATE_EVENT_EXIT:
			m_flags.ClearFlags(eActorStateFlags_OnLadder);
			m_stateLadder.OnExit(actorControllerComponent);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

			if (!m_stateLadder.OnPrePhysicsUpdate(actorControllerComponent, prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}
		}
		break;

		case ACTOR_EVENT_LADDER:
		{
			//actorControllerComponent.GetActorState()->EnableStatusFlags(kActorStatus_onLadder);
			const SStateEventLadder& ladderEvent = static_cast<const SStateEventLadder&>(event);
			m_stateLadder.OnUseLadder(actorControllerComponent, ladderEvent.GetLadder());
		}
		break;

		case ACTOR_EVENT_LADDER_LEAVE:
		{
			//actorControllerComponent.GetActorState()->DisableStatusFlags(kActorStatus_onLadder);
			const SStateEventLeaveLadder& leaveLadderEvent = static_cast<const SStateEventLeaveLadder&>(event);
			ELadderLeaveLocation leaveLoc = leaveLadderEvent.GetLeaveLocation();
			m_stateLadder.LeaveLadder(actorControllerComponent, leaveLoc);
			if (leaveLoc == eLLL_Drop)
			{
				return State_Fall;
			}
		}
		break;

		case ACTOR_EVENT_LADDER_POSITION:
		{
			const SStateEventLadderPosition& ladderEvent = static_cast<const SStateEventLadderPosition&>(event);
			float heightFrac = ladderEvent.GetHeightFrac();
			m_stateLadder.SetHeightFrac(actorControllerComponent, heightFrac);
		}
		break;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Intro(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_GROUND:
		case ACTOR_EVENT_INTRO_FINISHED:
			return State_Ground;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Ground(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateGround.OnEnter(actorControllerComponent);
			m_flags.AddFlags(eActorStateFlags_Ground);
			break;

		case STATE_EVENT_EXIT:
			m_flags.ClearFlags(eActorStateFlags_Ground);
			m_stateGround.OnExit(actorControllerComponent);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

			ProcessSprint(actorControllerComponent, prePhysicsEvent);

			m_stateGround.OnPrePhysicsUpdate(actorControllerComponent, prePhysicsEvent.m_frameTime,
				m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy), actorControllerComponent.IsPlayer());
		}
		break;

		case ACTOR_EVENT_JUMP:
			return State_Jump;

		case ACTOR_EVENT_STANCE_CHANGED:
			CActorStateUtility::ChangeStance(actorControllerComponent, event);
			break;

			//case ACTOR_EVENT_STEALTHKILL:
			//	RequestTransitionState(actorControllerComponent, ACTOR_STATE_ANIMATION, event);
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


const CActorStateMovement::TStateIndex CActorStateMovement::GroundFall(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_GROUND_COLLIDER_CHANGED:
			if (static_cast<const SStateEventGroundColliderChanged&> (event).OnGround())
			{
				actorControllerComponent.durationInAir = 0.0f;

				return State_Ground;
			}
			break;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::SlideFall(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_GROUND_COLLIDER_CHANGED:
			if (static_cast<const SStateEventGroundColliderChanged&> (event).OnGround())
			{
				actorControllerComponent.durationInAir = 0.0f;

				return State_Slide;
			}
			break;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::FallTest(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	/*	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

		const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
		switch (eventID)
		{
			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				// We're considered durationInAir at this point.
				//  - NOTE: this does not mean CActorControllerComponent::IsInAir() returns true.
				float durationInAir = actorControllerComponent.durationInAir;
				durationInAir += prePhysicsEvent.m_frameTime;
				actorControllerComponent.durationInAir = durationInAir;

				if (!CActorStateUtility::IsOnGround(actorControllerComponent))
				{
					const float groundHeight = actorControllerComponent.m_stateSwimWaterTestProxy.GetLastRaycastResult();
					const float heightZ = actorControllerComponent.GetEntity()->GetPos().z;

					// Only consider falling if our fall distance is sufficient - this is to prevent transitioning to the Fall state
					// due to physics erroneously telling us we're flying (due to small bumps on the terrain/ground).
					if ((heightZ - groundHeight) > g_pGameCVars->pl_fallHeight)
					{
						// Only fall if we've been falling for a sufficient amount of time - this is for gameplay feel reasons.
						if (durationInAir > g_pGameCVars->pl_movement.ground_timeInAirToFall)
						{
							actorControllerComponent.StateMachineHandleEventMovement(ACTOR_EVENT_FALL);
							return State_Done;
						}
					}

					// Use the swim proxy to get the ground height as we're periodically casting that ray anyway.
					actorControllerComponent.m_stateSwimWaterTestProxy.ForceUpdateBottomLevel(actorControllerComponent);
				}
				else
				{
					// We're back on the ground, so send the collider changed event.
					actorControllerComponent.StateMachineHandleEventMovement(SStateEventGroundColliderChanged(true));
				}
				break;
			}
		}
	*/
	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Jump(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateJump.OnEnter(actorControllerComponent);
			m_flags.AddFlags(eActorStateFlags_InAir | eActorStateFlags_Jump);
			m_flags.ClearFlags(eActorStateFlags_Sprinting);
			break;

		case STATE_EVENT_EXIT:
		{
			m_flags.ClearFlags(
				eActorStateFlags_InAir |
				eActorStateFlags_Jump |
				eActorStateFlags_Sprinting);
			m_stateJump.OnExit(actorControllerComponent, m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy));
		}
		break;

		case ACTOR_EVENT_JUMP:
		{
			const float fVerticalSpeedModifier = static_cast<const SStateEventJump&> (event).GetVerticalSpeedModifier();
			m_stateJump.OnJump(actorControllerComponent, m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy), fVerticalSpeedModifier);

			const SActorPrePhysicsData& data = static_cast<const SStateEventJump&> (event).GetPrePhysicsEventData();
			actorControllerComponent.StateMachineHandleEventMovement(SStateEventActorMovementPrePhysics(&data));
		}
		break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
			actorControllerComponent.durationInAir += prePhysicsEvent.m_frameTime;

			if (m_stateJump.OnPrePhysicsUpdate(actorControllerComponent, m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy), prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}

			ProcessSprint(actorControllerComponent, prePhysicsEvent);

			//actorControllerComponent.m_stateSwimWaterTestProxy.ForceUpdateBottomLevel(actorControllerComponent);
		}
	}

	return State_Continue;
}

const CActorStateMovement::TStateIndex CActorStateMovement::Fall(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateJump.OnEnter(actorControllerComponent);
			m_stateJump.OnFall(actorControllerComponent);
			m_flags.AddFlags(eActorStateFlags_InAir);
			return State_Done;
	}

	return State_Continue;
}

const CActorStateMovement::TStateIndex CActorStateMovement::Slide(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	/*	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

		const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
		switch (eventID)
		{
			case STATE_EVENT_ENTER:
				actorControllerComponent.SetCanTurnBody(false);
				m_slideController.GoToState(actorControllerComponent, eSlideState_Sliding);
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
				m_slideController.GoToState(actorControllerComponent, eSlideState_None);
				actorControllerComponent.SetCanTurnBody(true);
				break;

			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				m_slideController.Update(actorControllerComponent, prePhysicsEvent.m_frameTime, prePhysicsEvent.m_movement, m_flags.AreAnyFlagsActive(eActorStateFlags_NetSlide), m_flags.AreAnyFlagsActive(eActorStateFlags_NetExitingSlide), actorControllerComponent.GetMoveRequest());

				m_slideController.GetCurrentState() == eSlideState_ExitingSlide ? m_flags.AddFlags(eActorStateFlags_ExitingSlide) : m_flags.ClearFlags(eActorStateFlags_ExitingSlide);

				if (m_slideController.IsActive())
				{
					return State_Continue;
				}
			}
				return State_Ground;

			case ACTOR_EVENT_SLIDE_KICK:
			{
				const SStateEventSlideKick& kickData = static_cast<const SStateEventSlideKick&> (event);

				const float timer = m_slideController.DoSlideKick(actorControllerComponent);
				kickData.GetMelee()->SetDelayTimer(timer);
				break;
			}

			case ACTOR_EVENT_SLIDE_EXIT:
				m_slideController.GoToState(actorControllerComponent, eSlideState_ExitingSlide);
				return State_Ground;

			case ACTOR_EVENT_SLIDE_EXIT_LAZY:
				m_slideController.LazyExitSlide(actorControllerComponent);
				break;

			case ACTOR_EVENT_SLIDE_EXIT_FORCE:
				m_slideController.GoToState(actorControllerComponent, eSlideState_None);
				return State_Ground;

			case ACTOR_EVENT_SLIDE:
				return State_Done;

			case ACTOR_EVENT_STANCE_CHANGED:
				CActorStateUtility::ChangeStance(actorControllerComponent, event);
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


const CActorStateMovement::TStateIndex CActorStateMovement::Swim(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	/*	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

		const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
		switch (eventID)
		{
			case STATE_EVENT_ENTER:
				m_stateSwim.OnEnter(actorControllerComponent);
				m_flags.AddFlags(eActorStateFlags_Swimming);
				actorControllerComponent.OnSetStance(STANCE_SWIM);
				actorControllerComponent.durationOnGround = 0.0f;
				actorControllerComponent.durationInAir = 0.0f;
				break;

			case STATE_EVENT_EXIT:
				m_flags.ClearFlags(eActorStateFlags_Swimming);
				m_stateSwim.OnExit(actorControllerComponent);
				actorControllerComponent.OnSetStance(STANCE_STAND);

				break;

			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				ProcessSprint(actorControllerComponent, prePhysicsEvent);

				actorControllerComponent.m_stateSwimWaterTestProxy.PreUpdateSwimming(actorControllerComponent, prePhysicsEvent.m_frameTime);
				actorControllerComponent.m_stateSwimWaterTestProxy.Update(actorControllerComponent, prePhysicsEvent.m_frameTime);

				TriggerOutOfWaterEffectIfNeeded(actorControllerComponent);

				if (actorControllerComponent.m_stateSwimWaterTestProxy.ShouldSwim())
				{
					float verticalSpeedModifier = 0.0f;
					if (m_stateSwim.DetectJump(actorControllerComponent, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime, &verticalSpeedModifier))
					{
						actorControllerComponent.StateMachineHandleEventMovement(SStateEventJump(prePhysicsEvent, verticalSpeedModifier));

						return State_Done;
					}
					m_stateSwim.OnPrePhysicsUpdate(actorControllerComponent, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime);
				}
				else
				{
					return State_Ground;
				}
			}
				break;

			case ACTOR_EVENT_JUMP:
				return State_Jump;

			case ACTOR_EVENT_UPDATE:
				m_stateSwim.OnUpdate(actorControllerComponent, static_cast<const SStateEventUpdate&>(event).GetFrameTime());
				break;

			case ACTOR_EVENT_FALL:
				return State_Fall;
		}
	*/
	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::SwimTest(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	/*	CRY_ASSERT(!actorControllerComponent.IsAIControlled());

		switch (event.GetEventId())
		{
			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				actorControllerComponent.m_stateSwimWaterTestProxy.PreUpdateNotSwimming(actorControllerComponent, prePhysicsEvent.m_frameTime);

				actorControllerComponent.m_stateSwimWaterTestProxy.Update(actorControllerComponent, prePhysicsEvent.m_frameTime);

				TriggerOutOfWaterEffectIfNeeded(actorControllerComponent);

				if (actorControllerComponent.m_stateSwimWaterTestProxy.ShouldSwim())
				{
					return State_Swim;
				}
			}
				break;
		}*/

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::StateGroundInput(CActorControllerComponent& actorControllerComponent, const SInputEventData& inputEvent)
{
	switch (inputEvent.m_inputEvent)
	{
		case SInputEventData::EInputEvent_Sprint:
			StateSprintInput(actorControllerComponent, inputEvent);
			break;
	}

	return State_Continue;
}


void CActorStateMovement::StateSprintInput(CActorControllerComponent& actorControllerComponent, const SInputEventData& inputEvent)
{
	CRY_ASSERT(inputEvent.m_inputEvent == SInputEventData::EInputEvent_Sprint);
	inputEvent.m_activationMode == eAAM_OnPress ? m_flags.AddFlags(eActorStateFlags_SprintPressed)
		: m_flags.ClearFlags(eActorStateFlags_SprintPressed);
}


void CActorStateMovement::ProcessSprint(CActorControllerComponent& actorControllerComponent, const SActorPrePhysicsData& prePhysicsEvent)
{
	/*	// If sprint toggle is active, then we want to sprint if it is pressed or if we are sprinting.
		// If sprint toggle is off, then we only want to sprint if it is pressed.
		uint32 flagsToCheck = g_pGameCVars->cl_sprintToggle ? (eActorStateFlags_SprintPressed | eActorStateFlags_Sprinting) : (eActorStateFlags_SprintPressed);

		if (m_flags.AreAnyFlagsActive(flagsToCheck) && CActorStateUtility::IsSprintingAllowed(actorControllerComponent, prePhysicsEvent.m_movement, actorControllerComponent.GetCurrentItem()))
		{
			if (!actorControllerComponent.IsSprinting())
			{
				// notify IActorEventListener about sprinting (just once)
				OnSpecialMove(actorControllerComponent, IActorEventListener::eSM_SpeedSprint);
			}

			m_flags.AddFlags(eActorStateFlags_Sprinting);
			if (actorControllerComponent.GetCharacterInput()->GetType() == ICharacterInput::CHARACTER_INPUT)
			{
				static_cast<CCharacterInput*> (actorControllerComponent.GetCharacterInput())->ClearCrouchAction();
				actorControllerComponent.SetStance(STANCE_STAND);
			}
		}
		else
		{
			m_flags.ClearFlags(eActorStateFlags_Sprinting);
		}*/
}


void CActorStateMovement::OnSpecialMove(CActorControllerComponent& actorControllerComponent, IActorEventListener::ESpecialMove specialMove)
{
	/*	if (!actorControllerComponent.m_CharacterEventListeners.empty() )
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


void CActorStateMovement::TriggerOutOfWaterEffectIfNeeded(const CActorControllerComponent& actorControllerComponent)
{
	/*if (m_pWaterEffects != NULL)
	{
	CRY_ASSERT (actorControllerComponent.IsClient ());

	if (actorControllerComponent.m_stateSwimWaterTestProxy.IsHeadComingOutOfWater ())
	{
	m_pWaterEffects->OnCameraComingOutOfWater ();
	}
	}*/
}


void CActorStateMovement::CreateWaterEffects()
{
	/*if (m_pWaterEffects == NULL)
	{
	m_pWaterEffects = new CWaterGameEffects ();
	m_pWaterEffects->Initialise ();
	}*/
}


void CActorStateMovement::ReleaseWaterEffects()
{
	/*if (m_pWaterEffects != NULL)
	{
	m_pWaterEffects->Release ();
	delete m_pWaterEffects;
	m_pWaterEffects = NULL;
	}*/
}
}