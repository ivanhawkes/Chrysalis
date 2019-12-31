#pragma once

#include "Components.h"


namespace Chrysalis::ECS
{
struct Qi : public IComponent
{
	Qi() = default;
	virtual ~Qi() = default;

	Qi(AttributeType<float> qi) :
		qi(qi)
	{
	}


	inline bool operator==(const Qi& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{A5F65B14-983A-4BAA-BD59-381AFCEE6D76}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"qi"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<Qi>& desc)
	{
		desc.SetGUID(Qi().GetGuid());
		desc.SetLabel("Qi");
		desc.SetDescription("Qi");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(qi, "qi", "qi");

		return true;
	}

	/** Qi attribute. */
	AttributeType<float> qi;
};


struct UtiliseQi : public IComponent
{
	UtiliseQi() = default;
	virtual ~UtiliseQi() = default;

	UtiliseQi(float quantity) :
		quantity(quantity)
	{
	}

	inline bool operator==(const UtiliseQi& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{80E8F5BA-4765-4B81-BBF1-3ACAA6D6F596}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"utilise-qi"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<UtiliseQi>& desc)
	{
		desc.SetGUID(UtiliseQi().GetGuid());
		desc.SetLabel("UtiliseQi");
		desc.SetDescription("UtiliseQi");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(quantity, "quantity", "quantity");

		return true;
	}

	/** Modify an attribute by this amount. */
	float quantity;
};


struct UtiliseQiOverTime : public IComponent
{
	UtiliseQiOverTime() = default;
	virtual ~UtiliseQiOverTime() = default;

	UtiliseQiOverTime(float quantity,
		float duration, float interval) :
		quantity(quantity), duration(duration), interval(interval)
	{
	}

	inline bool operator==(const UtiliseQiOverTime& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{0764C388-CE73-406B-9B49-6616D661129F}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"utilise-qi-over-time"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<UtiliseQiOverTime>& desc)
	{
		desc.SetGUID(UtiliseQiOverTime().GetGuid());
		desc.SetLabel("UtiliseQiOverTime");
		desc.SetDescription("UtiliseQiOverTime");
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

	/** The damage ticks need to occur at this interval. */
	float interval {1.0f};

	/** Gametime passed since the last tick. */
	float deltaSinceTick {0.0f};

	/** Ticks remaining. */
	float ticksRemaining {duration / interval};
};


struct ReplenishQi : public IComponent
{
	ReplenishQi() = default;
	virtual ~ReplenishQi() = default;

	ReplenishQi(float quantity) :
		quantity(quantity)
	{
	}

	inline bool operator==(const ReplenishQi& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{BA94E0D1-890C-421E-995F-0B548818EDDA}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"replenish-qi"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<ReplenishQi>& desc)
	{
		desc.SetGUID(ReplenishQi().GetGuid());
		desc.SetLabel("ReplenishQi");
		desc.SetDescription("ReplenishQi");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(quantity, "quantity", "quantity");

		return true;
	}

	/** Modify an attribute by this amount. */
	float quantity;
};


struct ReplenishQiOverTime : public IComponent
{
	ReplenishQiOverTime() = default;
	virtual ~ReplenishQiOverTime() = default;

	ReplenishQiOverTime(float quantity,
		float duration, float interval) :
		quantity(quantity), duration(duration), interval(interval)
	{
	}

	inline bool operator==(const ReplenishQiOverTime& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{4F1E267E-EEB3-4243-B36D-DE5CF1E25B9E}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"replenish-qi-over-time"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<ReplenishQiOverTime>& desc)
	{
		desc.SetGUID(ReplenishQiOverTime().GetGuid());
		desc.SetLabel("ReplenishQiOverTime");
		desc.SetDescription("ReplenishQiOverTime");
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