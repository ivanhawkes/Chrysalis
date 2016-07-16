#include <StdAfx.h>

#include "CharacterStateEvents.h"
#include <Actor/Character/Character.h>


class CCharacterStateEntry : private CStateHierarchy < CCharacter >
{
	DECLARE_STATE_CLASS_BEGIN(CCharacter, CCharacterStateEntry)
	DECLARE_STATE_CLASS_END(CCharacter);
};


DEFINE_STATE_CLASS_BEGIN(CCharacter, CCharacterStateEntry, CHARACTER_STATE_ENTRY, Root)
DEFINE_STATE_CLASS_END(CCharacter, CCharacterStateEntry);


const CCharacterStateEntry::TStateIndex CCharacterStateEntry::Root(CCharacter& Character, const SStateEvent& event)
{
	const ECharacterStateEvent eventID = static_cast<ECharacterStateEvent> (event.GetEventId());
	switch (eventID)
	{
		case CHARACTER_EVENT_ENTRY_CHARACTER:
			RequestTransitionState(Character, CHARACTER_STATE_MOVEMENT);
			break;

		case CHARACTER_EVENT_ENTRY_AI:
			RequestTransitionState(Character, CHARACTER_STATE_MOVEMENT_AI);
			break;
	}

	return State_Continue;
}
