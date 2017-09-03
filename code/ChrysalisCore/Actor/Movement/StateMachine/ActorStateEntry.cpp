#include <StdAfx.h>

#include "ActorStateEvents.h"
#include <Actor/Actor.h>


namespace Chrysalis
{
class CActorStateEntry : private CStateHierarchy <IActorComponent>
{
	DECLARE_STATE_CLASS_BEGIN(IActorComponent, CActorStateEntry)
	DECLARE_STATE_CLASS_END(IActorComponent);
};


DEFINE_STATE_CLASS_BEGIN(IActorComponent, CActorStateEntry, ACTOR_STATE_ENTRY, Root)
DEFINE_STATE_CLASS_END(IActorComponent, CActorStateEntry);


const CActorStateEntry::TStateIndex CActorStateEntry::Root(IActorComponent& actorComponent, const SStateEvent& event)
{
	const EActorStateEvent eventID = static_cast<EActorStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_ENTRY:
			RequestTransitionState(actorComponent, ACTOR_STATE_MOVEMENT);
			break;

		case ACTOR_EVENT_ENTRY_AI:
			RequestTransitionState(actorComponent, ACTOR_STATE_MOVEMENT_AI);
			break;
	}

	return State_Continue;
}
}