#include <StdAfx.h>

#include "CharacterStateEvents.h"
//#include "InteractiveActionController.h"


class CCharacterStateAnimationControlled : public CStateHierarchy < CCharacter >
{
	DECLARE_STATE_CLASS_BEGIN(CCharacter, CCharacterStateAnimationControlled)
		DECLARE_STATE_CLASS_ADD(CCharacter, EntryChoice)
		DECLARE_STATE_CLASS_ADD(CCharacter, InteractiveAction)
		DECLARE_STATE_CLASS_ADD(CCharacter, StealthKill)
		DECLARE_STATE_CLASS_ADD(CCharacter, CutScene)
	DECLARE_STATE_CLASS_END(CCharacter)

private:
	//	CInteractiveActionController m_interactiveActionController;
};


DEFINE_STATE_CLASS_BEGIN(CCharacter, CCharacterStateAnimationControlled, CHARACTER_STATE_ANIMATION, EntryChoice)
	DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateAnimationControlled, EntryChoice, Root)
	DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateAnimationControlled, InteractiveAction, Root)
	DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateAnimationControlled, StealthKill, Root)
	DEFINE_STATE_CLASS_ADD(CCharacter, CCharacterStateAnimationControlled, CutScene, Root)
DEFINE_STATE_CLASS_END(CCharacter, CCharacterStateAnimationControlled)


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::EntryChoice(CCharacter& Character, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case CHARACTER_EVENT_INTERACTIVE_ACTION:
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


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::Root(CCharacter& Character, const SStateEvent& event)
{
	return State_Continue;
}


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::StealthKill(CCharacter& Character, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case STATE_EVENT_ENTER:
			// TODO: this made sense in Crysis 2, but might not for our system.
			m_flags.AddFlags(ECharacterStateFlags_Ground);
			if (Character.IsPlayer())
			{
				gEnv->pGame->GetIGameFramework()->AllowSave(false);
			}
			break;

		case STATE_EVENT_EXIT:
			// TODO: this made sense in Crysis 2, but might not for our system.
			m_flags.ClearFlags(ECharacterStateFlags_Ground);
			if (Character.IsPlayer())
			{
				gEnv->pGame->GetIGameFramework()->AllowSave(true);
			}
			break;
	}

	return State_Continue;
}


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::InteractiveAction(CCharacter& Character, const SStateEvent& event)
{
	switch (event.GetEventId())
	{
		case STATE_EVENT_EXIT:
			/*m_flags.ClearFlags (ECharacterStateFlags_InteractiveAction);
			Character.GetActorState ()->animationControlledID = 0;
			Character.AnimationControlled (false);
			m_interactiveActionController.OnLeave (Character);*/
			break;

		case CHARACTER_EVENT_PREPHYSICSUPDATE:
		{
			/*const SCharacterPrePhysicsData& prePhysicsEvent = static_cast<const SStateEventCharacterMovementPrePhysics&> (event).GetPrePhysicsData ();
			m_interactiveActionController.Update (Character, prePhysicsEvent.m_frameTime, prePhysicsEvent.m_movement);

			if (!m_interactiveActionController.IsInInteractiveAction ())
			{
			RequestTransitionState (Character, CHARACTER_STATE_MOVEMENT);
			}*/
		}
			break;

		case CHARACTER_EVENT_INTERACTIVE_ACTION:
		{
			/*m_flags.AddFlags (ECharacterStateFlags_InteractiveAction);

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

		case CHARACTER_EVENT_DEAD:
			/*if (m_interactiveActionController.IsInInteractiveAction ())
			{
			m_interactiveActionController.Abort (Character);
			}*/

			RequestTransitionState(Character, CHARACTER_STATE_MOVEMENT, CHARACTER_EVENT_DEAD);
			break;
	}

	return State_Continue;
}


const CCharacterStateAnimationControlled::TStateIndex CCharacterStateAnimationControlled::CutScene(CCharacter& Character, const SStateEvent& event)
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
