#include <StdAfx.h>

#include "CharacterStateEvents.h"
#include <Actor/Character/Character.h>


namespace Chrysalis
{
class CCharacterStateEntry : private CStateHierarchy < CCharacterComponent >
{
	DECLARE_STATE_CLASS_BEGIN(CCharacterComponent, CCharacterStateEntry)
	DECLARE_STATE_CLASS_END(CCharacterComponent);
};


DEFINE_STATE_CLASS_BEGIN(CCharacterComponent, CCharacterStateEntry, CHARACTER_STATE_ENTRY, Root)
DEFINE_STATE_CLASS_END(CCharacterComponent, CCharacterStateEntry);


const CCharacterStateEntry::TStateIndex CCharacterStateEntry::Root(CCharacterComponent& Character, const SStateEvent& event)
{
	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case ACTOR_EVENT_ENTRY:
			RequestTransitionState(Character, CHARACTER_STATE_MOVEMENT);
			break;

		case ACTOR_EVENT_ENTRY_AI:
			RequestTransitionState(Character, CHARACTER_STATE_MOVEMENT_AI);
			break;
	}

	return State_Continue;
}
}