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


struct Health
{
	Health() = default;
	virtual ~Health() = default;

	Health(AttributeType<float> health) :
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
	AttributeType<float> health;

	/** Is the actor dead? */
	bool isDead {false};

	/** Is the actor immortal? */
	bool isImmortal {false};
};


struct Damage
{
	Damage() = default;
	virtual ~Damage() = default;

	Damage(float quantity, DamageType damageType) :
		quantity(quantity), damageType(damageType)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(targetTargetType, "targetTargetType", "targetTargetType");
		ar(quantity, "quantity", "quantity");
		ar(damageType, "damageType", "Damage Type");
	}

	/** Use the spell's target or source for this component's target. */
	TargetTargetType targetTargetType {TargetTargetType::target};

	/** Modify an attribute by this amount. */
	float quantity {0.0f};

	/** The type of damage. */
	DamageType damageType {DamageType::acid};
};


struct DamageOverTime
{
	DamageOverTime() = default;
	virtual ~DamageOverTime() = default;

	DamageOverTime(float quantity, DamageType damageType,
		float duration, float interval) :
		quantity(quantity), damageType(damageType),
		duration(duration), interval(interval)
	{
		ticksRemaining = duration / interval;
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(targetTargetType, "targetTargetType", "targetTargetType");
		ar(quantity, "quantity", "quantity");
		ar(damageType, "damageType", "damageType");
		ar(duration, "duration", "duration");
		ar(interval, "interval", "interval");
	}

	/** Use the spell's target or source for this component's target. */
	TargetTargetType targetTargetType {TargetTargetType::target};

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


struct Heal
{
	Heal() = default;
	virtual ~Heal() = default;

	Heal(float quantity) :
		quantity(quantity)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(targetTargetType, "targetTargetType", "targetTargetType");
		ar(quantity, "quantity", "quantity");
	}

	/** Use the spell's target or source for this component's target. */
	TargetTargetType targetTargetType {TargetTargetType::target};

	/** Modify an attribute by this amount. */
	float quantity {0.0f};
};


struct HealOverTime
{
	HealOverTime() = default;
	virtual ~HealOverTime() = default;

	HealOverTime(float quantity,
		float duration, float interval) :
		quantity(quantity), duration(duration), interval(interval)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(targetTargetType, "targetTargetType", "targetTargetType");
		ar(quantity, "quantity", "quantity");
		ar(duration, "duration", "duration");
		ar(interval, "interval", "interval");
	}

	/** Use the spell's target or source for this component's target. */
	TargetTargetType targetTargetType {TargetTargetType::target};

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