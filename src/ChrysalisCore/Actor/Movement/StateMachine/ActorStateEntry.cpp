#include <StdAfx.h>

#include "ActorStateEvents.h"
#include <Components/Actor/ActorControllerComponent.h>


namespace Chrysalis
{
class CActorStateEntry : private CStateHierarchy<CActorControllerComponent>
{
	DECLARE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateEntry)
	DECLARE_STATE_CLASS_END(CActorControllerComponent);
};


DEFINE_STATE_CLASS_BEGIN(CActorControllerComponent, CActorStateEntry, ACTOR_STATE_ENTRY, Root)
DEFINE_STATE_CLASS_END(CActorControllerComponent, CActorStateEntry);


const CActorStateEntry::TStateIndex CActorStateEntry::Root(CActorControllerComponent& actorControllerComponent, const SStateEvent& event)
{
	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_ENTRY:
			RequestTransitionState(actorControllerComponent, ACTOR_STATE_MOVEMENT);
			break;

		case ACTOR_EVENT_ENTRY_AI:
			RequestTransitionState(actorControllerComponent, ACTOR_STATE_MOVEMENT_AI);
			break;
	}

	return State_Continue;
}
}