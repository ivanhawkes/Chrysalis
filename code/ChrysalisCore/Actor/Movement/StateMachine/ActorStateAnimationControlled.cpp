#include <StdAfx.h>

#include "ActorStateEvents.h"
#include <Actor/Actor.h>
//#include "InteractiveActionController.h"


namespace Chrysalis
{
class CActorStateAnimationControlled : public CStateHierarchy < IActorComponent >
{
	DECLARE_STATE_CLASS_BEGIN(IActorComponent, CActorStateAnimationControlled)
	DECLARE_STATE_CLASS_ADD(IActorComponent, EntryChoice)
	DECLARE_STATE_CLASS_ADD(IActorComponent, InteractiveAction)
	DECLARE_STATE_CLASS_ADD(IActorComponent, StealthKill)
	DECLARE_STATE_CLASS_ADD(IActorComponent, CutScene)
	DECLARE_STATE_CLASS_END(IActorComponent)

private:
	//	CInteractiveActionController m_interactiveActionController;
};


DEFINE_STATE_CLASS_BEGIN(IActorComponent, CActorStateAnimationControlled, ACTOR_STATE_ANIMATION, EntryChoice)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateAnimationControlled, EntryChoice, Root)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateAnimationControlled, InteractiveAction, Root)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateAnimationControlled, StealthKill, Root)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateAnimationControlled, CutScene, Root)
DEFINE_STATE_CLASS_END(IActorComponent, CActorStateAnimationControlled)


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::EntryChoice(IActorComponent& actorComponent, const SStateEvent& event)
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


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::Root(IActorComponent& actorComponent, const SStateEvent& event)
{
	return State_Continue;
}


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::StealthKill(IActorComponent& actorComponent, const SStateEvent& event)
{
	//switch (event.GetEventId())
	//{
	//	case STATE_EVENT_ENTER:
	//		// #TODO: this made sense in Crysis 2, but might not for our system.
	//		m_flags.AddFlags(eActorStateFlags_Ground);
	//		if (actorComponent.IsPlayer())
	//		{
	//			gEnv->pGameFramework->AllowSave(false);
	//		}
	//		break;

	//	case STATE_EVENT_EXIT:
	//		// #TODO: this made sense in Crysis 2, but might not for our system.
	//		m_flags.ClearFlags(eActorStateFlags_Ground);
	//		if (actorComponent.IsPlayer())
	//		{
	//			gEnv->pGameFramework->AllowSave(true);
	//		}
	//		break;
	//}

	return State_Continue;
}


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::InteractiveAction(IActorComponent& actorComponent, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case STATE_EVENT_EXIT:
			/*m_flags.ClearFlags (eActorStateFlags_InteractiveAction);
			actorComponent.GetActorState ()->animationControlledID = 0;
			actorComponent.AnimationControlled (false);
			m_interactiveActionController.OnLeave (actorComponent);*/
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			/*const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData ();
			m_interactiveActionController.Update (actorComponent, prePhysicsEvent.m_frameTime, prePhysicsEvent.m_movement);

			if (!m_interactiveActionController.IsInInteractiveAction ())
			{
			RequestTransitionState (actorComponent, ACTOR_STATE_MOVEMENT);
			}*/
		}
		break;

		case ACTOR_EVENT_INTERACTIVE_ACTION:
		{
			/*m_flags.AddFlags (eActorStateFlags_InteractiveAction);

			const SStateEventInteractiveAction& action = static_cast<const SStateEventInteractiveAction&> (event);
			if (action.GetObjectEntityID () != 0)
			{
			m_interactiveActionController.OnEnter (actorComponent, action.GetObjectEntityID (), action.GetInteractionIndex ());
			}
			else if (action.GetObjectInteractionName () != NULL)
			{
			m_interactiveActionController.OnEnterByName (actorComponent, action.GetObjectInteractionName (), action.GetActionSpeed ());
			}
			else
			{
			// no valid data! Abort!
			RequestTransitionState (actorComponent, ACTOR_STATE_MOVEMENT);
			}

			actorComponent.GetActorState ()->animationControlledID = m_interactiveActionController.GetInteractiveObjectId ();
			actorComponent.AnimationControlled (true, action.GetShouldUpdateVisibility ());*/
		}
		break;

		case ACTOR_EVENT_DEAD:
			/*if (m_interactiveActionController.IsInInteractiveAction ())
			{
			m_interactiveActionController.Abort (actorComponent);
			}*/

			RequestTransitionState(actorComponent, ACTOR_STATE_MOVEMENT, ACTOR_EVENT_DEAD);
			break;
	}

	return State_Continue;
}


const CActorStateAnimationControlled::TStateIndex CActorStateAnimationControlled::CutScene(IActorComponent& actorComponent, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case STATE_EVENT_ENTER:
			actorComponent.OnBeginCutScene();
			break;

		case STATE_EVENT_EXIT:
			actorComponent.OnEndCutScene();
			break;

		case ACTOR_EVENT_CUTSCENE:
			if (!static_cast<const SStateEventCutScene&> (event).IsEnabling())
			{
				RequestTransitionState(actorComponent, ACTOR_STATE_MOVEMENT);
			}
			break;
	}

	return State_Continue;
}
}