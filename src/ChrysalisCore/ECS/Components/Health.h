#pragma once

#include <ECS/Components/Spells/Spell.h>


namespace Chrysalis::ECS
{
enum class DamageType
{
	acid,
	bleed,
	cold,
	collision,
	cleave,
	crush,
	decay,
	disease,
	electricity,
	energy,
	entropy,
	explosion,
	fire,
	holy,
	ice,
	nature,
	pierce,
	plasma,
	poison,
	qi,
	radiation,
	slash,
	tear,
	unholy
};


enum class BuffType
{
	none,

	// Resistances.
	acidResistance,
	bleedResistance,
	chiResistance,
	coldResistance,
	crushResistance,
	decayResistance,
	diseaseResistance,
	electricityResistance,
	energyResistance,
	entropyResistance,
	explosionResistance,
	fireResistance,
	holyResistance,
	iceResistance,
	natureResistance,
	pierceResistance,
	plasmaResistance,
	poisonResistance,
	radiationResistance,
	slashResistance,
	unholyResistance,

	// General buffs.
	//bleed, // Debuff - or could be more generic...mmm...not sure.
	//haste, // buff only or negative values for debuffs
	//disarmed, // stateful debuff
};


struct Health final
{
	Health() = default;
	~Health() = default;

	Health(AttributeType health) :
		health(health)
	{
	}


	inline bool operator==(const Health& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	static void ReflectType(Schematyc::CTypeDesc<Health>& desc)
	{
		desc.SetGUID("{2B292018-F820-42FA-AA5D-28681E814A4A}"_cry_guid);
		desc.SetLabel("Health");
		desc.SetDescription("Health of an actor.");
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(health, "health", "The actor's current life force.");
		ar(isDead, "isDead", "Are they dead?");
		ar(isImmortal, "isImmortal", "Are they immortal or immune to damage?");
	}


	/** Health attribute. */
	AttributeType health;

	/** Is the actor dead? */
	bool isDead {false};

	/** Is the actor immortal? */
	bool isImmortal {false};
};


struct Damage final
{
	Damage() = default;
	~Damage() = default;

	Damage(float quantity, DamageType damageType) :
		quantity(quantity), damageType(damageType)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(quantity, "quantity", "quantity");
		ar(damageType, "damageType", "Damage Type");
	}


	/** Modify an attribute by this amount. */
	float quantity {0.0f};

	/** The type of damage. */
	DamageType damageType {DamageType::acid};
};


struct DamageOverTime final
{
	DamageOverTime() = default;
	~DamageOverTime() = default;

	DamageOverTime(float quantity, DamageType damageType,
		float duration, float interval) :
		quantity(quantity), damageType(damageType),
		duration(duration), interval(interval)
	{
		ticksRemaining = duration / interval;
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(quantity, "quantity", "quantity");
		ar(damageType, "damageType", "damageType");
		ar(duration, "duration", "duration");
		ar(interval, "interval", "interval");
	}


	/** Modify an attribute by this amount. */
	float quantity {0.0f};

	/** The type of damage. */
	DamageType damageType {DamageType::acid};

	/** Limit the duration for this modifier. Given as remaining time in seconds. */
	float duration {10.0f};

	/** The ticks need to occur at this interval. */
	float interval {1.0f};

	/** Gametime passed since the last tick. */
	float deltaSinceTick {0.0f};

	/** Ticks remaining. */
	float ticksRemaining {duration / interval};
};


struct SelfHarm final
{
	SelfHarm() = default;
	~SelfHarm() = default;

	SelfHarm(float quantity, DamageType damageType) :
		quantity(quantity), damageType(damageType)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(quantity, "quantity", "quantity");
		ar(damageType, "damageType", "Damage Type");
	}


	/** Modify an attribute by this amount. */
	float quantity {0.0f};

	/** The type of damage. */
	DamageType damageType {DamageType::acid};
};


struct Heal final
{
	Heal() = default;
	~Heal() = default;

	Heal(float quantity) :
		quantity(quantity)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(quantity, "quantity", "quantity");
	}


	/** Modify an attribute by this amount. */
	float quantity {0.0f};
};


struct HealOverTime final
{
	HealOverTime() = default;
	~HealOverTime() = default;

	HealOverTime(float quantity,
		float duration, float interval) :
		quantity(quantity), duration(duration), interval(interval)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(quantity, "quantity", "quantity");
		ar(duration, "duration", "duration");
		ar(interval, "interval", "interval");
	}


	/** Modify an attribute by this amount. */
	float quantity {0.0f};

	/** Limit the duration for this modifier. Given as remaining time in seconds. */
	float duration {10.0f};

	/** The ticks need to occur at this interval. */
	float interval {1.0f};

	/** Gametime passed since the last tick. */
	float deltaSinceTick {0.0f};

	/** Ticks remaining. */
	float ticksRemaining {duration / interval};
};
}