#pragma once

#include "Components.h"


namespace Chrysalis::ECS
{
struct Health : public IComponent
{
	Health() = default;
	virtual ~Health() = default;

	Health(AttributeType<float> health) :
		health(health)
	{
	}

	inline bool operator==(const Health& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{8788DADD-08B8-4BE8-B519-E0BD5D61E932}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"health"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<Health>& desc)
	{
		desc.SetGUID(Health().GetGuid());
		desc.SetLabel("Health");
		desc.SetDescription("Health");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(health, "health", "health");

		return true;
	}

	/** Health attribute. */
	AttributeType<float> health;

	/** Is the actor dead? */
	bool isDead {false};
};


struct Damage : public IComponent
{
	Damage() = default;
	virtual ~Damage() = default;

	Damage(float quantity, DamageType damageType) :
		quantity(quantity), damageType(damageType)
	{
	}

	inline bool operator==(const Damage& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{FD6F9B82-06A7-436E-9268-008A567ADF3C}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"damage"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<Damage>& desc)
	{
		desc.SetGUID(Damage().GetGuid());
		desc.SetLabel("Damage");
		desc.SetDescription("Damage");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(quantity, "quantity", "quantity");
		archive(damageType, "damageType", "Damage Type");

		return true;
	}

	/** Modify an attribute by this amount. */
	float quantity;

	/** The type of damage. */
	DamageType damageType;
};


struct DamageOverTime : public IComponent
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

	inline bool operator==(const DamageOverTime& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{E0DC02B3-3BC8-4E72-99F3-B3D59CEC3433}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"damage-over-time"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<DamageOverTime>& desc)
	{
		desc.SetGUID(DamageOverTime().GetGuid());
		desc.SetLabel("DamageOverTime");
		desc.SetDescription("DamageOverTime");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(quantity, "quantity", "quantity");
		archive(damageType, "damageType", "Damage Type");
		archive(duration, "duration", "duration");
		archive(interval, "interval", "interval");

		return true;
	}

	/** Modify an attribute by this amount. */
	float quantity;

	/** The type of damage. */
	DamageType damageType;

	/** Limit the duration for this modifier. Given as remaining time in seconds. */
	float duration {10.0f};

	/** The ticks need to occur at this interval. */
	float interval {1.0f};

	/** Gametime passed since the last tick. */
	float deltaSinceTick {0.0f};

	/** Ticks remaining. */
	float ticksRemaining {duration / interval};
};


struct Heal : public IComponent
{
	Heal() = default;
	virtual ~Heal() = default;

	Heal(float quantity) :
		quantity(quantity)
	{
	}

	inline bool operator==(const Heal& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{1389ECC0-2BDF-492F-966A-2FC7C83CBA5F}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"heal"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<Heal>& desc)
	{
		desc.SetGUID(Heal().GetGuid());
		desc.SetLabel("Heal");
		desc.SetDescription("Heal");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(quantity, "quantity", "quantity");

		return true;
	}

	/** Modify an attribute by this amount. */
	float quantity;
};


struct HealOverTime : public IComponent
{
	HealOverTime() = default;
	virtual ~HealOverTime() = default;

	HealOverTime(float quantity,
		float duration, float interval) :
		quantity(quantity), duration(duration), interval(interval)
	{
	}

	inline bool operator==(const HealOverTime& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{B24CD70D-0D25-412D-93C9-072FE20736B4}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"heal-over-time"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<HealOverTime>& desc)
	{
		desc.SetGUID(HealOverTime().GetGuid());
		desc.SetLabel("HealOverTime");
		desc.SetDescription("HealOverTime");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(quantity, "quantity", "quantity");
		archive(duration, "duration", "duration");
		archive(interval, "interval", "interval");

		return true;
	}

	/** Modify an attribute by this amount. */
	float quantity;

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