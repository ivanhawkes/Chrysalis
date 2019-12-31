#pragma once

#include "Components.h"


namespace Chrysalis::ECS
{

/** When spells are cast they require some changes from the prototype in order to function correctly.
	In order to get this right we need to know what changes it expects, and apply those as required.
	This should provide a high degree of flexibility, since we will be able to handle common spell
	types without knowlegde of their specifics. */

enum class SpellRewire
{
	simple,				// Only need to apply source and target.
	damage,				// Apply source, target, update damage values and qi use.
	heal,				// Apply source, target, update heal values and qi use.
	regenerate,			// ** Special case - mana regen on another
	crowdControl,		// Apply source, target - and whatever else...not sure yet.
	custom				// Need custom logic and handling to make this work.
};


enum class SpellCastStyle
{
	instant,			// Spell is cast instantly. Can't be interupted. Can be cast while moving.
	movementAllowed,	// A cast time applies - may be cast while moving.
	turret,				// A cast time applies. No movement allowed.
	channelled			// Continuous concentration requirement. No movement allowed.
};


struct Spell : public IComponent
{
	Spell() = default;
	virtual ~Spell() = default;

	inline bool operator==(const Spell& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{58CA55D0-2B66-4910-9135-29D196E53EFB}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"spell"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<Spell>& desc)
	{
		desc.SetGUID(Spell().GetGuid());
		desc.SetLabel("Spell");
		desc.SetDescription("Spell");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(spellRewire, "spell-rewire", "Actions which need to be taken before spell can be fired.");
		archive(range, "range", "Maximum range at which this can be cast.");

		return true;
	}

	/** Code that needs to run after a spell is copied to fix up all the broken requirements e.g. source, target, spell bonuses */
	SpellRewire spellRewire {SpellRewire::damage};

	/** Maximum range at which this can be cast. */
	float range;
};
}