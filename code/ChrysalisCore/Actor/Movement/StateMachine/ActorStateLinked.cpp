#include <StdAfx.h>

#include <StateMachine/StateMachine.h>
#include "ActorStateEvents.h"
#include <Actor/Actor.h>
/*#include "Weapon.h"*/


namespace Chrysalis
{
class CActorStateLinked : public CStateHierarchy <IActorComponent>
{
	DECLARE_STATE_CLASS_BEGIN(IActorComponent, CActorStateLinked)
	DECLARE_STATE_CLASS_ADD(IActorComponent, Enter)
	DECLARE_STATE_CLASS_ADD(IActorComponent, Vehicle)
	DECLARE_STATE_CLASS_ADD(IActorComponent, Entity)
	DECLARE_STATE_CLASS_END(IActorComponent)
};


DEFINE_STATE_CLASS_BEGIN(IActorComponent, CActorStateLinked, ACTOR_STATE_LINKED, Enter)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateLinked, Enter, Root)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateLinked, Vehicle, Root)
DEFINE_STATE_CLASS_ADD(IActorComponent, CActorStateLinked, Entity, Root)
DEFINE_STATE_CLASS_END(IActorComponent, CActorStateLinked)


const CActorStateLinked::TStateIndex CActorStateLinked::Root(IActorComponent& actorComponent, const SStateEvent& event)
{
	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_DETACH:
			// Dammit! With the new AIMOVEMENT hstate, you have to transition back to the correct movement hstate!
			// I guess this might actually be OK, so long as the AI and Characters need the same vehicle behaviour.
			// Ultimately, Sven expects there to be a separate AILINKED hstate, so hopefully this is short lived.
			//RequestTransitionState (actorComponent, actorComponent.IsAIControlled () ? ACTOR_STATE_MOVEMENT_AI : ACTOR_STATE_MOVEMENT);
			break;

		case STATE_EVENT_RELEASE:
			//actorComponent.m_stateSwimWaterTestProxy.Reset (true);
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
			//RequestTransitionState (actorComponent, actorComponent.IsAIControlled () ? ACTOR_STATE_MOVEMENT_AI : ACTOR_STATE_MOVEMENT, ACTOR_EVENT_DEAD);
			break;
	}

	return State_Continue;
}


const CActorStateLinked::TStateIndex CActorStateLinked::Enter(IActorComponent& actorComponent, const SStateEvent& event)
{
	//const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	//switch (eventID)
	//{
	//	case ACTOR_EVENT_ATTACH:
	//		if (actorComponent.GetLinkedVehicle())
	//		{
	//			return State_Vehicle;
	//		}
	//		return State_Entity;
	//}

	return State_Continue;
}


const CActorStateLinked::TStateIndex CActorStateLinked::Vehicle(IActorComponent& actorComponent, const SStateEvent& event)
{
	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case STATE_EVENT_ENTER:
		case STATE_EVENT_EXIT:
		{
			//actorComponent.RefreshVisibilityState ();
			break;
		}
	}

	return State_Continue;
}


const CActorStateLinked::TStateIndex CActorStateLinked::Entity(IActorComponent& actorComponent, const SStateEvent& event)
{
	return State_Continue;
}
}