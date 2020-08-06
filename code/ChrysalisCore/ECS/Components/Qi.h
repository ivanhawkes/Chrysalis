#pragma once

#include <ECS/Components/Components.h>
#include <ECS/Components/Spells/Spell.h>
#include <ECS/Components/Components.h>


namespace Chrysalis::ECS
{
struct Qi
{
	Qi() = default;
	virtual ~Qi() = default;

	Qi(AttributeType<float> qi) :
		qi(qi)
	{
	}


	inline bool operator==(const Qi& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	static void ReflectType(Schematyc::CTypeDesc<Qi>& desc)
	{
		desc.SetGUID("{6C1E5EE5-B467-48BF-843E-5A8D54B51F52}"_cry_guid);
		desc.SetLabel("Qi");
		desc.SetDescription("Qi of an actor.");
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(qi, "qi", "qi");
		ar(timeSinceLastSpellcast, "timeSinceLastSpellcast", "Time since they last cast a spell.");
		ar(qiRegenerationPerSecond, "qiRegenerationPerSecond", "Qi regeneration per second.");
	}

	/** Qi attribute. */
	AttributeType<float> qi;

	/** Time delta since the last spell cast. */
	float timeSinceLastSpellcast {0.0f};

	/** Qi regeneration per second. */
	float qiRegenerationPerSecond {0.02f};
};


struct UtiliseQi
{
	UtiliseQi() = default;
	virtual ~UtiliseQi() = default;

	UtiliseQi(float quantity) :
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


struct UtiliseQiOverTime
{
	UtiliseQiOverTime() = default;
	virtual ~UtiliseQiOverTime() = default;

	UtiliseQiOverTime(float quantity,
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

	/** The damage ticks need to occur at this interval. */
	float interval {1.0f};

	/** Gametime passed since the last tick. */
	float deltaSinceTick {0.0f};

	/** Ticks remaining. */
	float ticksRemaining {duration / interval};
};


struct ReplenishQi
{
	ReplenishQi() = default;
	virtual ~ReplenishQi() = default;

	ReplenishQi(float quantity) :
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


struct ReplenishQiOverTime
{
	ReplenishQiOverTime() = default;
	virtual ~ReplenishQiOverTime() = default;

	ReplenishQiOverTime(float quantity,
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