#include <StdAfx.h>

#include "ActorStateEvents.h"
#include <Components/Actor/ActorControllerComponent.h>
//#include <IItem.h>
//#include "InteractiveActionController.h"


namespace Chrysalis
{
class CActorStateAnimationControlled : public CStateHierarchy<CActorControllerComponent>
{
	DECLARE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateAnimationControlled)
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, EntryChoice)
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, InteractiveAction)
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, StealthKill)
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, CutScene)
	DECLARE_STATE_CLASS_END(CActorControllerComponent)

private:
	//	CInteractiveActionController m_interactiveActionController;
};


DEFINE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateAnimationControlled, ACTOR_STATE_ANIMATION, EntryChoice)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateAnimationControlled, EntryChoice, Root)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateAnimationControlled, InteractiveAction, Root)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateAnimationControlled, StealthKill, Root)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateAnimationControlled, CutScene, Root)
DEFINE_STATE_CLASS_END(CActorControllerComponent, CActorStateAnimationControlled)


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::EntryChoice(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case ACTOR_EVENT_INTERACTIVE_ACTION:
			return State_InteractiveAction;

		case ACTOR_EVENT_CUTSCENE:
			if (static_cast<const SStateEventCutScene&> (event).IsEnabling())
			{
				return State_CutScene;
			}
			else
			{
				CRY_ASSERT(false);
			}
			break;
	}

	return State_Continue;
}


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::Root(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	return State_Continue;
}


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::StealthKill(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	//switch (event.GetEventId())
	//{
	//	case STATE_EVENT_ENTER:
	//		// #TODO: this made sense in Crysis 2, but might not for our system.
	//		m_flags.AddFlags(eActorStateFlags_Ground);
	//		if (actorControllerComponent.IsPlayer())
	//		{
	//			gEnv->pGameFramework->AllowSave(false);
	//		}
	//		break;

	//	case STATE_EVENT_EXIT:
	//		// #TODO: this made sense in Crysis 2, but might not for our system.
	//		m_flags.ClearFlags(eActorStateFlags_Ground);
	//		if (actorControllerComponent.IsPlayer())
	//		{
	//			gEnv->pGameFramework->AllowSave(true);
	//		}
	//		break;
	//}

	return State_Continue;
}


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::InteractiveAction(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case STATE_EVENT_EXIT:
			/*m_flags.ClearFlags (eActorStateFlags_InteractiveAction);
			actorControllerComponent.GetActorState ()->animationControlledID = 0;
			actorControllerComponent.AnimationControlled (false);
			m_interactiveActionController.OnLeave (actorControllerComponent);*/
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			/*const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData ();
			m_interactiveActionController.Update (actorControllerComponent, prePhysicsEvent.m_frameTime, prePhysicsEvent.m_movement);

			if (!m_interactiveActionController.IsInInteractiveAction ())
			{
			RequestTransitionState (actorControllerComponent, ACTOR_STATE_MOVEMENT);
			}*/
		}
		break;

		case ACTOR_EVENT_INTERACTIVE_ACTION:
		{
			/*m_flags.AddFlags (eActorStateFlags_InteractiveAction);

			const SStateEventInteractiveAction& action = static_cast<const SStateEventInteractiveAction&> (event);
			if (action.GetObjectEntityID () != 0)
			{
			m_interactiveActionController.OnEnter (actorControllerComponent, action.GetObjectEntityID (), action.GetInteractionIndex ());
			}
			else if (action.GetObjectInteractionName () != nullptr)
			{
			m_interactiveActionController.OnEnterByName (actorControllerComponent, action.GetObjectInteractionName (), action.GetActionSpeed ());
			}
			else
			{
			// no valid data! Abort!
			RequestTransitionState (actorControllerComponent, ACTOR_STATE_MOVEMENT);
			}

			actorControllerComponent.GetActorState ()->animationControlledID = m_interactiveActionController.GetInteractiveObjectId ();
			actorControllerComponent.AnimationControlled (true, action.GetShouldUpdateVisibility ());*/
		}
		break;

		case ACTOR_EVENT_DEAD:
			/*if (m_interactiveActionController.IsInInteractiveAction ())
			{
			m_interactiveActionController.Abort (actorControllerComponent);
			}*/

			RequestTransitionState(actorControllerComponent, ACTOR_STATE_MOVEMENT, ACTOR_EVENT_DEAD);
			break;
	}

	return State_Continue;
}


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::CutScene(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case STATE_EVENT_ENTER:
			actorControllerComponent.OnBeginCutScene();
			break;

		case STATE_EVENT_EXIT:
			actorControllerComponent.OnEndCutScene();
			break;

		case ACTOR_EVENT_CUTSCENE:
			if (!static_cast<const SStateEventCutScene&> (event).IsEnabling())
			{
				RequestTransitionState(actorControllerComponent, ACTOR_STATE_MOVEMENT);
			}
			break;
	}

	return State_Continue;
}
}