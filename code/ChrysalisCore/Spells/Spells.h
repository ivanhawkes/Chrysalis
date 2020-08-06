#pragma once

#include "Interfaces/ISpell.h"
#include <ECS/Components/Spells/Spell.h>


namespace Chrysalis
{
struct SpellCastOpen : public ISpellcasting
{

	SpellCastOpen(ECS::Name name, ECS::Spell spell, ECS::SourceAndTarget sourceAndTarget)
		:name(name), spell(spell), sourceAndTarget(sourceAndTarget) {}
	~SpellCastOpen() = default;

	// ISpellcasting
	virtual void OnSpellStart() { CryLogAlways("Spell cast open: start"); }
	virtual void OnSpellTick() { /*CryLogAlways("Spell cast open: tick");*/ }
	virtual void OnSpellInterupt() { CryLogAlways("Spell cast open: interupt"); }
	virtual void OnSpellFail() { CryLogAlways("Spell cast open: fail"); }
	virtual void OnSpellComplete() { CryLogAlways("Spell cast open: complete"); }
	// ~ISpellcasting

	ECS::Name name;
	ECS::Spell spell;
	ECS::SourceAndTarget sourceAndTarget;
};
}