#pragma once

#include <ECS/Components/Components.h>
#include <ECS/Components/Spells/Spell.h>
#include <ECS/Components/Components.h>


namespace Chrysalis::ECS
{
struct Qi final
{
	Qi() = default;
	~Qi() = default;

	Qi(AttributeType qi) :
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
	AttributeType qi;

	/** Time delta since the last spell cast. */
	float timeSinceLastSpellcast {0.0f};

	/** Qi regeneration per second. */
	float qiRegenerationPerSecond {0.02f};
};


struct UtiliseQi final
{
	UtiliseQi() = default;
	~UtiliseQi() = default;

	UtiliseQi(float quantity) :
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


struct ReplenishQi final
{
	ReplenishQi() = default;
	~ReplenishQi() = default;

	ReplenishQi(float quantity) :
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
}