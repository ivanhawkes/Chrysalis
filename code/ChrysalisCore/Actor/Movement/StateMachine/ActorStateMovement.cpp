/**
Implements the character state movement class.

NOTE: If you use "SetStance" you can't also implement the ACTOR_EVENT_STANCE_CHANGED within the same hierarchy!
Use "OnSetStance" if you need this functionality!
*/
#include <StdAfx.h>

#include <StateMachine/StateMachine.h>
#include <IAnimatedCharacter.h>
#include <Actor/Actor.h>
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
class CActorStateMovement : private CStateHierarchy <IActorComponent>
{
	DECLARE_STATE_CLASS_BEGIN(IActorComponent, CActorStateMovement)
	DECLARE_STATE_CLASS_ADD(IActorComponent, MovementRoot);
	DECLARE_STATE_CLASS_ADD(IActorComponent, GroundMovement);
	DECLARE_STATE_CLASS_ADD(IActorComponent, Dead);
	DECLARE_STATE_CLASS_ADD(IActorComponent, Fly);
	DECLARE_STATE_CLASS_ADD(IActorComponent, Ground);
	DECLARE_STATE_CLASS_ADD(IActorComponent, GroundFall);
	DECLARE_STATE_CLASS_ADD(IActorComponent, FallTest);
	DECLARE_STATE_CLASS_ADD(IActorComponent, Jump);
	DECLARE_STATE_CLASS_ADD(IActorComponent, Fall);
	DECLARE_STATE_CLASS_ADD(IActorComponent, Slide);
	DECLARE_STATE_CLASS_ADD(IActorComponent, SlideFall);
	DECLARE_STATE_CLASS_ADD(IActorComponent, Swim);
	DECLARE_STATE_CLASS_ADD(IActorComponent, Intro);
	DECLARE_STATE_CLASS_ADD(IActorComponent, SwimTest);
	DECLARE_STATE_CLASS_ADD(IActorComponent, Ladder);
	DECLARE_STATE_CLASS_ADD_DUMMY(IActorComponent, NoMovement);
	DECLARE_STATE_CLASS_ADD_DUMMY(IActorComponent, GroundFallTest);
	DECLARE_STATE_CLASS_ADD_DUMMY(IActorComponent, SlideFallTest);
	DECLARE_STATE_CLASS_END(IActorComponent);

private:
	const TStateIndex StateGroundInput(IActorComponent& actorComponent, const SInputEventData& inputEvent);
	void StateSprintInput(IActorComponent& actorComponent, const SInputEventData& inputEvent);
	void ProcessSprint(IActorComponent& actorComponent, const SActorPrePhysicsData& prePhysicsEvent);
	void OnSpecialMove(IActorComponent& actorComponent, IActorEventListener::ESpecialMove specialMove);
	bool IsActionControllerValid(IActorComponent& actorComponent) const;

	void CreateWaterEffects();
	void ReleaseWaterEffects();
	void TriggerOutOfWaterEffectIfNeeded(const IActorComponent& actorComponent);

	void UpdateCharacterStanceTag(IActorComponent& actorComponent);

	CActorStateDead m_stateDead;
	CActorStateFly m_stateFly;
	CActorStateGround m_stateGround;
	CActorStateJump m_stateJump;
	CActorStateSwim m_stateSwim;
	CActorStateLadder m_stateLadder;
	//CWaterGameEffects* m_pWaterEffects;
};


DEFINE_STATE_CLASS_BEGIN(IActorComponent, CActorStateMovement, ACTOR_STATE_MOVEMENT, Ground)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, MovementRoot, Root)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, GroundMovement, MovementRoot)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, SwimTest, GroundMovement)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, Ground, SwimTest)
DEFINE_STATE_CLASS_ADD_DUMMY(IActorComponent, CActorStateMovement, GroundFallTest, FallTest, Ground)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, GroundFall, GroundFallTest)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, Slide, SwimTest)
DEFINE_STATE_CLASS_ADD_DUMMY(IActorComponent, CActorStateMovement, SlideFallTest, FallTest, Slide)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, SlideFall, SlideFallTest)
DEFINE_STATE_CLASS_ADD_DUMMY(IActorComponent, CActorStateMovement, NoMovement, SwimTest, MovementRoot)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, Jump, NoMovement)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, Fall, Jump)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, Swim, MovementRoot)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, Ladder, MovementRoot)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, Dead, Root)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, Fly, Root)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateMovement, Intro, Root)
DEFINE_STATE_CLASS_END(IActorComponent, CActorStateMovement);


const CActorStateMovement::TStateIndex CActorStateMovement::Root(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_INIT:
			//m_pWaterEffects = NULL;
//			if (actorComponent.IsClient())
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
			//		actorComponent.OnSetStance (stance);
			//	}
			//}
			//else
			//{
			//	EStance stance = actorComponent.GetStance ();
			//	serializer.EnumValue ("StateStance", stance, STANCE_NULL, STANCE_LAST);
			//}
		}
		break;

		case ACTOR_EVENT_CUTSCENE:
			if (static_cast<const SStateEventCutScene&> (event).IsEnabling())
			{
				RequestTransitionState(actorComponent, ACTOR_STATE_ANIMATION, event);
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
			RequestTransitionState(actorComponent, ACTOR_STATE_ANIMATION, event);
			break;

			//case ACTOR_EVENT_BUTTONMASHING_SEQUENCE:
			//	RequestTransitionState(actorComponent, ACTOR_STATE_ANIMATION, event);
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


const CActorStateMovement::TStateIndex CActorStateMovement::MovementRoot(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			// #TODO: What is this?
			//actorComponent.SetCanTurnBody (true);
			break;

		case ACTOR_EVENT_LADDER:
			return State_Ladder;

		case ACTOR_EVENT_INPUT:
			return StateGroundInput(actorComponent, static_cast<const SStateEventActorInput&> (event).GetInputEventData());

		case ACTOR_EVENT_FLY:
			return State_Fly;

		case ACTOR_EVENT_ATTACH:
			RequestTransitionState(actorComponent, ACTOR_STATE_LINKED, event);
			break;

		case ACTOR_EVENT_INTRO_START:
			return State_Intro;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::GroundMovement(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			CActorStateUtility::InitializeMoveRequest(actorComponent.GetMoveRequest());
			actorComponent.GetActorState()->durationInAir = 0.f;
			break;

		case STATE_EVENT_EXIT:
			actorComponent.GetActorState()->durationOnGround = 0.f;
			m_flags.ClearFlags(eActorStateFlags_Sprinting);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
			actorComponent.GetActorState()->durationOnGround += prePhysicsEvent.m_frameTime;

			// Only send the event if we've been flying for 2 consecutive frames - this prevents some state thrashing.
			// TODO: CRITICAL: HACK: BROKEN: !!
			//if (actorComponent.GetActorPhysics()->flags.AreAllFlagsActive(SActorPhysics::EActorPhysicsFlags::WasFlying | SActorPhysics::EActorPhysicsFlags::Flying))
			//{
			//	actorComponent.StateMachineHandleEventMovement(SStateEventGroundColliderChanged(false));
			//}

			if (CActorStateUtility::IsJumpAllowed(actorComponent, prePhysicsEvent.m_movement))
			{
				actorComponent.StateMachineHandleEventMovement(SStateEventJump(prePhysicsEvent));

				return State_Done;
			}

			CActorStateUtility::ProcessRotation(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
			CActorStateUtility::FinalizeMovementRequest(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
		}
		break;

		//case ACTOR_EVENT_SLIDE:
		//	return State_Slide;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Dead(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateDead.OnEnter(actorComponent);
			break;

		case STATE_EVENT_EXIT:
			m_stateDead.OnLeave(actorComponent);
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

			m_stateDead.OnPrePhysicsUpdate(actorComponent, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime);
		}
		break;

		case ACTOR_EVENT_UPDATE:
		{
			CActorStateDead::UpdateCtx updateCtx;
			updateCtx.frameTime = static_cast<const SStateEventUpdate&>(event).GetFrameTime();
			m_stateDead.OnUpdate(actorComponent, updateCtx);
		}
		break;

		case ACTOR_EVENT_DEAD:
			return State_Done;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Fly(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateFly.OnEnter(actorComponent);
			actorComponent.GetActorState()->durationInAir = 0.0f;
			break;

		case STATE_EVENT_EXIT:
			actorComponent.GetActorState()->durationInAir = 0.0f;
			m_stateFly.OnExit(actorComponent);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
			actorComponent.GetActorState()->durationInAir += prePhysicsEvent.m_frameTime;

			if (!m_stateFly.OnPrePhysicsUpdate(actorComponent, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}

			// ILH: check this code over once some movement is possible. Make sure the physics works as expected.

			// Process movement.
			CActorStateUtility::ProcessRotation(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
			CActorStateUtility::FinalizeMovementRequest(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
		}
		return State_Done;

		//case ACTOR_EVENT_FLY:
		//{
		//	const int flyMode = static_cast<const SStateEventFly&> (event).GetFlyMode();
		//	actorComponent.SetFlyMode(flyMode);
		//}
		//return State_Done;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Ladder(IActorComponent& actorComponent, const SStateEvent& event)
{
	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());

	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_flags.AddFlags(eActorStateFlags_OnLadder);
			break;

		case STATE_EVENT_EXIT:
			m_flags.ClearFlags(eActorStateFlags_OnLadder);
			m_stateLadder.OnExit(actorComponent);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

			if (!m_stateLadder.OnPrePhysicsUpdate(actorComponent, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}

			CActorStateUtility::ProcessRotation(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
			CActorStateUtility::FinalizeMovementRequest(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
		}
		break;

		case ACTOR_EVENT_LADDER:
		{
			actorComponent.GetActorState()->EnableStatusFlags(kActorStatus_onLadder);
			const SStateEventLadder& ladderEvent = static_cast<const SStateEventLadder&>(event);
			m_stateLadder.OnUseLadder(actorComponent, ladderEvent.GetLadder());
		}
		break;

		case ACTOR_EVENT_LADDER_LEAVE:
		{
			actorComponent.GetActorState()->DisableStatusFlags(kActorStatus_onLadder);
			const SStateEventLeaveLadder& leaveLadderEvent = static_cast<const SStateEventLeaveLadder&>(event);
			ELadderLeaveLocation leaveLoc = leaveLadderEvent.GetLeaveLocation();
			m_stateLadder.LeaveLadder(actorComponent, leaveLoc);
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
			m_stateLadder.SetHeightFrac(actorComponent, heightFrac);
		}
		break;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Intro(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_GROUND:
		case ACTOR_EVENT_INTRO_FINISHED:
			return State_Ground;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Ground(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateGround.OnEnter(actorComponent);
			m_flags.AddFlags(eActorStateFlags_Ground);
			break;

		case STATE_EVENT_EXIT:
			m_flags.ClearFlags(eActorStateFlags_Ground);
			m_stateGround.OnExit(actorComponent);
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

			ProcessSprint(actorComponent, prePhysicsEvent);

			m_stateGround.OnPrePhysicsUpdate(actorComponent, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime,
				m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy), actorComponent.IsPlayer());
		}
		break;

		case ACTOR_EVENT_JUMP:
			return State_Jump;

		case ACTOR_EVENT_STANCE_CHANGED:
			CActorStateUtility::ChangeStance(actorComponent, event);
			break;

			//case ACTOR_EVENT_STEALTHKILL:
			//	RequestTransitionState(actorComponent, ACTOR_STATE_ANIMATION, event);
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


const CActorStateMovement::TStateIndex CActorStateMovement::GroundFall(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_GROUND_COLLIDER_CHANGED:
			if (static_cast<const SStateEventGroundColliderChanged&> (event).OnGround())
			{
				actorComponent.GetActorState()->durationInAir = 0.0f;

				return State_Ground;
			}
			break;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::SlideFall(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_GROUND_COLLIDER_CHANGED:
			if (static_cast<const SStateEventGroundColliderChanged&> (event).OnGround())
			{
				actorComponent.GetActorState()->durationInAir = 0.0f;

				return State_Slide;
			}
			break;
	}

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::FallTest(IActorComponent& actorComponent, const SStateEvent& event)
{
	/*	CRY_ASSERT(!actorComponent.IsAIControlled());

		const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
		switch (eventID)
		{
			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				// We're considered durationInAir at this point.
				//  - NOTE: this does not mean IActorComponent::IsInAir() returns true.
				float durationInAir = actorComponent.GetActorState()->durationInAir;
				durationInAir += prePhysicsEvent.m_frameTime;
				actorComponent.GetActorState()->durationInAir = durationInAir;

				if (!CActorStateUtility::IsOnGround(actorComponent))
				{
					const float groundHeight = actorComponent.m_stateSwimWaterTestProxy.GetLastRaycastResult();
					const float heightZ = actorComponent.GetEntity()->GetPos().z;

					// Only consider falling if our fall distance is sufficient - this is to prevent transitioning to the Fall state
					// due to physics erroneously telling us we're flying (due to small bumps on the terrain/ground).
					if ((heightZ - groundHeight) > g_pGameCVars->pl_fallHeight)
					{
						// Only fall if we've been falling for a sufficient amount of time - this is for gameplay feel reasons.
						if (durationInAir > g_pGameCVars->pl_movement.ground_timeInAirToFall)
						{
							actorComponent.StateMachineHandleEventMovement(ACTOR_EVENT_FALL);
							return State_Done;
						}
					}

					// Use the swim proxy to get the ground height as we're periodically casting that ray anyway.
					actorComponent.m_stateSwimWaterTestProxy.ForceUpdateBottomLevel(actorComponent);
				}
				else
				{
					// We're back on the ground, so send the collider changed event.
					actorComponent.StateMachineHandleEventMovement(SStateEventGroundColliderChanged(true));
				}
				break;
			}
		}
	*/
	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::Jump(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateJump.OnEnter(actorComponent);
			m_flags.AddFlags(eActorStateFlags_InAir | eActorStateFlags_Jump);
			m_flags.ClearFlags(eActorStateFlags_Sprinting);
			break;

		case STATE_EVENT_EXIT:
		{
			m_flags.ClearFlags(
				eActorStateFlags_InAir |
				eActorStateFlags_Jump |
				eActorStateFlags_Sprinting);
			m_stateJump.OnExit(actorComponent, m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy));
		}
		break;

		case ACTOR_EVENT_JUMP:
		{
			const float fVerticalSpeedModifier = static_cast<const SStateEventJump&> (event).GetVerticalSpeedModifier();
			m_stateJump.OnJump(actorComponent, m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy), fVerticalSpeedModifier);

			const SActorPrePhysicsData& data = static_cast<const SStateEventJump&> (event).GetPrePhysicsEventData();
			actorComponent.StateMachineHandleEventMovement(SStateEventActorMovementPrePhysics(&data));
		}
		break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();
			actorComponent.GetActorState()->durationInAir += prePhysicsEvent.m_frameTime;

			if (m_stateJump.OnPrePhysicsUpdate(actorComponent, m_flags.AreAnyFlagsActive(eActorStateFlags_CurrentItemIsHeavy), prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime))
			{
				return State_Ground;
			}

			ProcessSprint(actorComponent, prePhysicsEvent);

			//actorComponent.m_stateSwimWaterTestProxy.ForceUpdateBottomLevel(actorComponent);

			CActorStateUtility::ProcessRotation(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
			CActorStateUtility::FinalizeMovementRequest(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
		}
	}

	return State_Continue;
}

const CActorStateMovement::TStateIndex CActorStateMovement::Fall(IActorComponent& actorComponent, const SStateEvent& event)
{
	CRY_ASSERT(!actorComponent.IsAIControlled());

	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
			m_stateJump.OnEnter(actorComponent);
			m_stateJump.OnFall(actorComponent);
			m_flags.AddFlags(eActorStateFlags_InAir);
			return State_Done;
	}

	return State_Continue;
}

const CActorStateMovement::TStateIndex CActorStateMovement::Slide(IActorComponent& actorComponent, const SStateEvent& event)
{
	/*	CRY_ASSERT(!actorComponent.IsAIControlled());

		const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
		switch (eventID)
		{
			case STATE_EVENT_ENTER:
				actorComponent.SetCanTurnBody(false);
				m_slideController.GoToState(actorComponent, eSlideState_Sliding);
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
				m_slideController.GoToState(actorComponent, eSlideState_None);
				actorComponent.SetCanTurnBody(true);
				break;

			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				m_slideController.Update(actorComponent, prePhysicsEvent.m_frameTime, prePhysicsEvent.m_movement, m_flags.AreAnyFlagsActive(eActorStateFlags_NetSlide), m_flags.AreAnyFlagsActive(eActorStateFlags_NetExitingSlide), actorComponent.GetMoveRequest());

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

				const float timer = m_slideController.DoSlideKick(actorComponent);
				kickData.GetMelee()->SetDelayTimer(timer);
				break;
			}

			case ACTOR_EVENT_SLIDE_EXIT:
				m_slideController.GoToState(actorComponent, eSlideState_ExitingSlide);
				return State_Ground;

			case ACTOR_EVENT_SLIDE_EXIT_LAZY:
				m_slideController.LazyExitSlide(actorComponent);
				break;

			case ACTOR_EVENT_SLIDE_EXIT_FORCE:
				m_slideController.GoToState(actorComponent, eSlideState_None);
				return State_Ground;

			case ACTOR_EVENT_SLIDE:
				return State_Done;

			case ACTOR_EVENT_STANCE_CHANGED:
				CActorStateUtility::ChangeStance(actorComponent, event);
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


const CActorStateMovement::TStateIndex CActorStateMovement::Swim(IActorComponent& actorComponent, const SStateEvent& event)
{
	/*	CRY_ASSERT(!actorComponent.IsAIControlled());

		const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
		switch (eventID)
		{
			case STATE_EVENT_ENTER:
				m_stateSwim.OnEnter(actorComponent);
				m_flags.AddFlags(eActorStateFlags_Swimming);
				actorComponent.OnSetStance(STANCE_SWIM);
				actorComponent.GetActorState()->durationOnGround = 0.0f;
				actorComponent.GetActorState()->durationInAir = 0.0f;
				break;

			case STATE_EVENT_EXIT:
				m_flags.ClearFlags(eActorStateFlags_Swimming);
				m_stateSwim.OnExit(actorComponent);
				actorComponent.OnSetStance(STANCE_STAND);

				break;

			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				ProcessSprint(actorComponent, prePhysicsEvent);

				actorComponent.m_stateSwimWaterTestProxy.PreUpdateSwimming(actorComponent, prePhysicsEvent.m_frameTime);
				actorComponent.m_stateSwimWaterTestProxy.Update(actorComponent, prePhysicsEvent.m_frameTime);

				TriggerOutOfWaterEffectIfNeeded(actorComponent);

				if (actorComponent.m_stateSwimWaterTestProxy.ShouldSwim())
				{
					float verticalSpeedModifier = 0.0f;
					if (m_stateSwim.DetectJump(actorComponent, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime, &verticalSpeedModifier))
					{
						actorComponent.StateMachineHandleEventMovement(SStateEventJump(prePhysicsEvent, verticalSpeedModifier));

						return State_Done;
					}
					m_stateSwim.OnPrePhysicsUpdate(actorComponent, prePhysicsEvent.m_movement, prePhysicsEvent.m_frameTime);
				}
				else
				{
					return State_Ground;
				}

				CActorStateUtility::ProcessRotation(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
				CActorStateUtility::FinalizeMovementRequest(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetMoveRequest());
			}
				break;

			case ACTOR_EVENT_JUMP:
				return State_Jump;

			case ACTOR_EVENT_UPDATE:
				m_stateSwim.OnUpdate(actorComponent, static_cast<const SStateEventUpdate&>(event).GetFrameTime());
				break;

			case ACTOR_EVENT_FALL:
				return State_Fall;
		}
	*/
	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::SwimTest(IActorComponent& actorComponent, const SStateEvent& event)
{
	/*	CRY_ASSERT(!actorComponent.IsAIControlled());

		switch (event.GetEventId())
		{
			case ACTOR_EVENT_PREPHYSICSUPDATE:
			{
				const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData();

				actorComponent.m_stateSwimWaterTestProxy.PreUpdateNotSwimming(actorComponent, prePhysicsEvent.m_frameTime);

				actorComponent.m_stateSwimWaterTestProxy.Update(actorComponent, prePhysicsEvent.m_frameTime);

				TriggerOutOfWaterEffectIfNeeded(actorComponent);

				if (actorComponent.m_stateSwimWaterTestProxy.ShouldSwim())
				{
					return State_Swim;
				}
			}
				break;
		}*/

	return State_Continue;
}


const CActorStateMovement::TStateIndex CActorStateMovement::StateGroundInput(IActorComponent& actorComponent, const SInputEventData& inputEvent)
{
	switch (inputEvent.m_inputEvent)
	{
		case SInputEventData::EInputEvent_Sprint:
			StateSprintInput(actorComponent, inputEvent);
			break;
	}

	return State_Continue;
}


void CActorStateMovement::StateSprintInput(IActorComponent& actorComponent, const SInputEventData& inputEvent)
{
	CRY_ASSERT(inputEvent.m_inputEvent == SInputEventData::EInputEvent_Sprint);
	inputEvent.m_activationMode == eAAM_OnPress ? m_flags.AddFlags(eActorStateFlags_SprintPressed)
		: m_flags.ClearFlags(eActorStateFlags_SprintPressed);
}


void CActorStateMovement::ProcessSprint(IActorComponent& actorComponent, const SActorPrePhysicsData& prePhysicsEvent)
{
	/*	// If sprint toggle is active, then we want to sprint if it is pressed or if we are sprinting.
		// If sprint toggle is off, then we only want to sprint if it is pressed.
		uint32 flagsToCheck = g_pGameCVars->cl_sprintToggle ? (eActorStateFlags_SprintPressed | eActorStateFlags_Sprinting) : (eActorStateFlags_SprintPressed);

		if (m_flags.AreAnyFlagsActive(flagsToCheck) && CActorStateUtility::IsSprintingAllowed(actorComponent, prePhysicsEvent.m_movement, actorComponent.GetCurrentItem()))
		{
			if (!actorComponent.IsSprinting())
			{
				// notify IActorEventListener about sprinting (just once)
				OnSpecialMove(actorComponent, IActorEventListener::eSM_SpeedSprint);
			}

			m_flags.AddFlags(eActorStateFlags_Sprinting);
			if (actorComponent.GetCharacterInput()->GetType() == ICharacterInput::CHARACTER_INPUT)
			{
				static_cast<CCharacterInput*> (actorComponent.GetCharacterInput())->ClearCrouchAction();
				actorComponent.SetStance(STANCE_STAND);
			}
		}
		else
		{
			m_flags.ClearFlags(eActorStateFlags_Sprinting);
		}*/
}


void CActorStateMovement::OnSpecialMove(IActorComponent& actorComponent, IActorEventListener::ESpecialMove specialMove)
{
	/*	if (!actorComponent.m_CharacterEventListeners.empty() )
		{
			IActorComponent::TCharacterEventListeners::const_iterator iter = actorComponent.m_CharacterEventListeners.begin();
			IActorComponent::TCharacterEventListeners::const_iterator cur;
			while (iter != actorComponent.m_CharacterEventListeners.end())
			{
				cur = iter;
				++iter;
				(*cur)->OnSpecialMove(&actorComponent, specialMove);
			}
		}*/
}


bool CActorStateMovement::IsActionControllerValid(IActorComponent& actorComponent) const
{
	//IAnimatedCharacter* pAnimatedCharacter = actorComponent.GetAnimatedCharacter();

	//if (pAnimatedCharacter != NULL)
	//{
	//	return (pAnimatedCharacter->GetActionController() != NULL);
	//}

	return false;
}


void CActorStateMovement::TriggerOutOfWaterEffectIfNeeded(const IActorComponent& actorComponent)
{
	/*if (m_pWaterEffects != NULL)
	{
	CRY_ASSERT (actorComponent.IsClient ());

	if (actorComponent.m_stateSwimWaterTestProxy.IsHeadComingOutOfWater ())
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


void CActorStateMovement::UpdateCharacterStanceTag(IActorComponent& actorComponent)
{
	/*IAnimatedCharacter *pAnimatedCharacter = actorComponent.GetAnimatedCharacter ();
	if (pAnimatedCharacter)
	{
	IActionController *pActionController = pAnimatedCharacter->GetActionController ();
	if (pActionController)
	{
	actorComponent.SetStanceTag (actorComponent.GetStance (), pActionController->GetContext ().state);
	}
	}*/
}
}