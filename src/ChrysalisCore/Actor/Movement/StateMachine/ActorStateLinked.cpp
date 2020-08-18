#include <StdAfx.h>

#include <StateMachine/StateMachine.h>
#include "ActorStateEvents.h"
#include <Components/Actor/ActorControllerComponent.h>
/*#include "Weapon.h"*/


namespace Chrysalis
{
class CActorStateLinked : public CStateHierarchy<CActorControllerComponent>
{
	DECLARE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateLinked)
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Enter)
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Vehicle)
	DECLARE_STATE_CLASS_ADD(CActorControllerComponent, Entity)
	DECLARE_STATE_CLASS_END(CActorControllerComponent)
};


DEFINE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateLinked, ACTOR_STATE_LINKED, Enter)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateLinked, Enter, Root)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateLinked, Vehicle, Root)
DEFINE_STATE_CLASS_ADD(CActorControllerComponent, CActorStateLinked, Entity, Root)
DEFINE_STATE_CLASS_END(CActorControllerComponent, CActorStateLinked)


const CActorStateLinked::TStateIndex CActorStateLinked::Root(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_DETACH:
			// Dammit! With the new AIMOVEMENT hstate, you have to transition back to the correct movement hstate!
			// I guess this might actually be OK, so long as the AI and Characters need the same vehicle behaviour.
			// Ultimately, Sven expects there to be a separate AILINKED hstate, so hopefully this is short lived.
			//RequestTransitionState (actorControllerComponent, actorControllerComponent.IsAIControlled () ? ACTOR_STATE_MOVEMENT_AI : ACTOR_STATE_MOVEMENT);
			break;

		case STATE_EVENT_RELEASE:
			//actorControllerComponent.m_stateSwimWaterTestProxy.Reset (true);
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
			//RequestTransitionState (actorControllerComponent, actorControllerComponent.IsAIControlled () ? ACTOR_STATE_MOVEMENT_AI : ACTOR_STATE_MOVEMENT, ACTOR_EVENT_DEAD);
			break;
	}

	return State_Continue;
}


const CActorStateLinked::TStateIndex CActorStateLinked::Enter(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	//const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	//switch (eventID)
	//{
	//	case ACTOR_EVENT_ATTACH:
	//		if (actorControllerComponent.GetLinkedVehicle())
	//		{
	//			return State_Vehicle;
	//		}
	//		return State_Entity;
	//}

	return State_Continue;
}


const CActorStateLinked::TStateIndex CActorStateLinked::Vehicle(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
		case STATE_EVENT_EXIT:
		{
			//actorControllerComponent.RefreshVisibilityState ();
			break;
		}
	}

	return State_Continue;
}


const CActorStateLinked::TStateIndex CActorStateLinked::Entity(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	return State_Continue;
}
}