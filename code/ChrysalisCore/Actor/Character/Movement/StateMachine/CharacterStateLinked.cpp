#include <StdAfx.h>

#include <StateMachine/StateMachine.h>
#include "CharacterStateEvents.h"
#include <Actor/Character/Character.h>
/*#include "Weapon.h"*/


namespace Chrysalis
{
class CCharacterStateLinked : public CStateHierarchy < CCharacterComponent >
{
	DECLARE_STATE_CLASS_BEGIN(CCharacterComponent, CCharacterStateLinked)
	DECLARE_STATE_CLASS_ADD(CCharacterComponent, Enter)
	DECLARE_STATE_CLASS_ADD(CCharacterComponent, Vehicle)
	DECLARE_STATE_CLASS_ADD(CCharacterComponent, Entity)
	DECLARE_STATE_CLASS_END(CCharacterComponent)
};


DEFINE_STATE_CLASS_BEGIN(CCharacterComponent, CCharacterStateLinked, CHARACTER_STATE_LINKED, Enter)
DEFINE_STATE_CLASS_ADD(CCharacterComponent, CCharacterStateLinked, Enter, Root)
DEFINE_STATE_CLASS_ADD(CCharacterComponent, CCharacterStateLinked, Vehicle, Root)
DEFINE_STATE_CLASS_ADD(CCharacterComponent, CCharacterStateLinked, Entity, Root)
DEFINE_STATE_CLASS_END(CCharacterComponent, CCharacterStateLinked)


const CCharacterStateLinked::TStateIndex CCharacterStateLinked::Root(CCharacterComponent& Character, const SStateEvent& event)
{
	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case CHARACTER_EVENT_DETACH:
			// Dammit! With the new AIMOVEMENT hstate, you have to transition back to the correct movement hstate!
			// I guess this might actually be OK, so long as the AI and Characters need the same vehicle behaviour.
			// Ultimately, Sven expects there to be a separate AILINKED hstate, so hopefully this is short lived.
			//RequestTransitionState (Character, Character.IsAIControlled () ? CHARACTER_STATE_MOVEMENT_AI : CHARACTER_STATE_MOVEMENT);
			break;

		case STATE_EVENT_RELEASE:
			//Character.m_CharacterStateSwimWaterTestProxy.Reset (true);
			break;

		case ACTOR_EVENT_WEAPONCHANGED:
		{
			/*const CWeapon* pWeapon = static_cast<const CWeapon*>(event.GetData (0).GetPtr ());

			if (pWeapon && pWeapon->IsHeavyWeapon ())
			{
			m_flags.AddFlags (eActorStateFlags_CurrentItemIsHeavy);
			}
			else
			{
			m_flags.ClearFlags (eActorStateFlags_CurrentItemIsHeavy);
			}*/
			break;
		}

		case ACTOR_EVENT_DEAD:
			//RequestTransitionState (Character, Character.IsAIControlled () ? CHARACTER_STATE_MOVEMENT_AI : CHARACTER_STATE_MOVEMENT, ACTOR_EVENT_DEAD);
			break;
	}

	return State_Continue;
}


const CCharacterStateLinked::TStateIndex CCharacterStateLinked::Enter(CCharacterComponent& Character, const SStateEvent& event)
{
	//const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	//switch (eventID)
	//{
	//	case CHARACTER_EVENT_ATTACH:
	//		if (Character.GetLinkedVehicle())
	//		{
	//			return State_Vehicle;
	//		}
	//		return State_Entity;
	//}

	return State_Continue;
}


const CCharacterStateLinked::TStateIndex CCharacterStateLinked::Vehicle(CCharacterComponent& Character, const SStateEvent& event)
{
	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
		case STATE_EVENT_EXIT:
		{
			//Character.RefreshVisibilityState ();
			break;
		}
	}

	return State_Continue;
}


const CCharacterStateLinked::TStateIndex CCharacterStateLinked::Entity(CCharacterComponent& Character, const SStateEvent& event)
{
	return State_Continue;
}
}