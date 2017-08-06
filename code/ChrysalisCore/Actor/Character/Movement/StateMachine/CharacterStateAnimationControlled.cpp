#include <StdAfx.h>

#include "CharacterStateEvents.h"
#include "Actor/Character/Character.h"
//#include "InteractiveActionController.h"


namespace Chrysalis
{
class CCharacterStateAnimationControlled : public CStateHierarchy < CCharacterComponent >
{
	DECLARE_STATE_CLASS_BEGIN(CCharacterComponent, CCharacterStateAnimationControlled)
	DECLARE_STATE_CLASS_ADD(CCharacterComponent, EntryChoice)
	DECLARE_STATE_CLASS_ADD(CCharacterComponent, InteractiveAction)
	DECLARE_STATE_CLASS_ADD(CCharacterComponent, StealthKill)
	DECLARE_STATE_CLASS_ADD(CCharacterComponent, CutScene)
	DECLARE_STATE_CLASS_END(CCharacterComponent)

private:
	//	CInteractiveActionController m_interactiveActionController;
};


DEFINE_STATE_CLASS_BEGIN(CCharacterComponent, CCharacterStateAnimationControlled, CHARACTER_STATE_ANIMATION, EntryChoice)
DEFINE_STATE_CLASS_ADD(CCharacterComponent, CCharacterStateAnimationControlled, EntryChoice, Root)
DEFINE_STATE_CLASS_ADD(CCharacterComponent, CCharacterStateAnimationControlled, InteractiveAction, Root)
DEFINE_STATE_CLASS_ADD(CCharacterComponent, CCharacterStateAnimationControlled, StealthKill, Root)
DEFINE_STATE_CLASS_ADD(CCharacterComponent, CCharacterStateAnimationControlled, CutScene, Root)
DEFINE_STATE_CLASS_END(CCharacterComponent, CCharacterStateAnimationControlled)


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::EntryChoice(CCharacterComponent& Character, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case ACTOR_EVENT_INTERACTIVE_ACTION:
			return State_InteractiveAction;

		case CHARACTER_EVENT_CUTSCENE:
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


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::Root(CCharacterComponent& Character, const SStateEvent& event)
{
	return State_Continue;
}


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::StealthKill(CCharacterComponent& Character, const SStateEvent& event)
{
	//switch (event.GetEventId())
	//{
	//	case STATE_EVENT_ENTER:
	//		// #TODO: this made sense in Crysis 2, but might not for our system.
	//		m_flags.AddFlags(eActorStateFlags_Ground);
	//		if (Character.IsPlayer())
	//		{
	//			gEnv->pGameFramework->AllowSave(false);
	//		}
	//		break;

	//	case STATE_EVENT_EXIT:
	//		// #TODO: this made sense in Crysis 2, but might not for our system.
	//		m_flags.ClearFlags(eActorStateFlags_Ground);
	//		if (Character.IsPlayer())
	//		{
	//			gEnv->pGameFramework->AllowSave(true);
	//		}
	//		break;
	//}

	return State_Continue;
}


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::InteractiveAction(CCharacterComponent& Character, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case STATE_EVENT_EXIT:
			/*m_flags.ClearFlags (eActorStateFlags_InteractiveAction);
			Character.GetActorState ()->animationControlledID = 0;
			Character.AnimationControlled (false);
			m_interactiveActionController.OnLeave (Character);*/
			break;

		case ACTOR_EVENT_PREPHYSICSUPDATE:
		{
			/*const SActorPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventActorMovementPrePhysics&> (event).GetPrePhysicsData ();
			m_interactiveActionController.Update (Character, prePhysicsEvent.m_frameTime, prePhysicsEvent.m_movement);

			if (!m_interactiveActionController.IsInInteractiveAction ())
			{
			RequestTransitionState (Character, CHARACTER_STATE_MOVEMENT);
			}*/
		}
		break;

		case ACTOR_EVENT_INTERACTIVE_ACTION:
		{
			/*m_flags.AddFlags (eActorStateFlags_InteractiveAction);

			const SStateEventInteractiveAction& action = static_cast<const SStateEventInteractiveAction&> (event);
			if (action.GetObjectEntityID () != 0)
			{
			m_interactiveActionController.OnEnter (Character, action.GetObjectEntityID (), action.GetInteractionIndex ());
			}
			else if (action.GetObjectInteractionName () != NULL)
			{
			m_interactiveActionController.OnEnterByName (Character, action.GetObjectInteractionName (), action.GetActionSpeed ());
			}
			else
			{
			// no valid data! Abort!
			RequestTransitionState (Character, CHARACTER_STATE_MOVEMENT);
			}

			Character.GetActorState ()->animationControlledID = m_interactiveActionController.GetInteractiveObjectId ();
			Character.AnimationControlled (true, action.GetShouldUpdateVisibility ());*/
		}
		break;

		case ACTOR_EVENT_DEAD:
			/*if (m_interactiveActionController.IsInInteractiveAction ())
			{
			m_interactiveActionController.Abort (Character);
			}*/

			RequestTransitionState(Character, CHARACTER_STATE_MOVEMENT, ACTOR_EVENT_DEAD);
			break;
	}

	return State_Continue;
}


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::CutScene(CCharacterComponent& Character, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case STATE_EVENT_ENTER:
			Character.OnBeginCutScene();
			break;

		case STATE_EVENT_EXIT:
			Character.OnEndCutScene();
			break;

		case CHARACTER_EVENT_CUTSCENE:
			if (!static_cast<const SStateEventCutScene&> (event).IsEnabling())
			{
				RequestTransitionState(Character, CHARACTER_STATE_MOVEMENT);
			}
			break;
	}

	return State_Continue;
}
}