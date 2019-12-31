#pragma once

#include <entt/entt.hpp>

/** Thinking about making this a single include header for convenience.
*/

namespace Chrysalis::ECS
{

enum class DamageType
{
	acid,
	bleed,
	qi,
	cold,
	collision,
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
	radiation,
	slash,
	tear,
	unholy
};


enum class TargetType
{
	none,

	self,
	singleTarget,
	cone,
	column,
	chain,
	sourceBasedAOE,
	targetBasedAOE,
	groundTargettedAOE,
};


enum class CrowdControlType
{
	none,

	blind,						// Loss of sight, movement and rotation restricted, ambling around.
	disarmed,					// Primary weapon disabled.
	forcedActionCharm,			// Movement and rotation restricted.
	forcedActionEntangled,		// Movement and rotation restricted.
	forcedActionFear,			// Movement and rotation restricted, ambling around quaking in fear.
	forcedActionFlee,			// Lose of movement control. Running around wildly.
	forcedActionMindControl,	// Under the control of another entity.
	forcedActionPulled,			// Pulled towards something with force.
	forcedActionTaunt,			// Attacks are forced to be directed towards a specific entity.
	forcedActionThrow,			// Thrown onto the ground. Fairly quick recovery.
	knockback,					// Physically knocked back a step or two.
	knockbackAOE,				// Physically knocked back a step or two.
	knockdown,					// Physically knocked back a step or two and onto your arse.
	knockdownAOE,				// Physically knocked back a step or two and onto your arse.
	polymorph,					// Turned into a harmless critter.
	silence,					// Restricts use of spells, shouts and other vocal abilities.
	slow,						// Movement slowed.
	snare,						// Movement and rotation restricted. Held in place with leg trapped in a snare.
	stun,						// Movement and rotation restricted. Birds twitter about your head.
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


struct IComponent
{
	IComponent() = default;
	virtual ~IComponent() = default;

	// This should be pure virtual but the ECS needs to be able to instantiate the struct, so...here's nothing.
	virtual bool Serialize(Serialization::IArchive& archive) { return true; };

	virtual const CryGUID& GetGuid() const
	{
		static CryGUID guid = "{DEADDEAD-DEAD-DEAD-DEAD-DEADDEADDEAD}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"icomponent"_hs};

		return nameHS;
	}
};


template<typename TYPE>
struct AttributeType
{
	AttributeType() = default;
	virtual ~AttributeType() = default;

	AttributeType(TYPE base, TYPE baseModifiers, TYPE modifiers) :
		base(base), baseModifiers(baseModifiers), modifiers(modifiers)
	{
	}


	bool Serialize(Serialization::IArchive& archive)
	{
		archive(base, "base", "base");
		archive(baseModifiers, "baseModifiers", "baseModifiers");
		archive(modifiers, "modifiers", "modifiers");

		return true;
	}

	/** Returns the current value for base, after it's modifiers have been applied. */
	const TYPE GetBaseAttribute() const
	{
		return base + baseModifiers;
	}


	/** Returns the current value for the attribute, after it's modifiers have been applied. */
	const TYPE GetAttribute() const
	{
		return base + modifiers;
	}

	/** Represents the attribute without any modifiers applied to it. */
	TYPE base;

	/** This modifier makes changes to the base value, instead of the frame value. Typical use would be for a health / strength 
	buff that increases the base value of the attribute. */
	TYPE baseModifiers;

	/** Modifiers for this frame. Should be calculated each frame prior to calculating the current value. */
	TYPE modifiers;
};


struct Name : public IComponent
{
	Name() = default;
	virtual ~Name() = default;

	Name(string name, string displayName) :
		name(name), displayName(displayName)
	{
	}
	

	inline bool operator==(const Name& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{8BEB64DA-F589-4671-96E9-D136A5E5DED7}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"name"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<Name>& desc)
	{
		desc.SetGUID(Name().GetGuid());
		desc.SetLabel("Name");
		desc.SetDescription("Name");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		archive(name, "name", "name");
		archive(displayName, "displayName", "displayName");

		return true;
	}

	/** A unique name for this particular item class. */
	string name;

	/** A name which can be used in the UI. */
	string displayName;
};


struct SourceAndTarget : public IComponent
{
	SourceAndTarget() = default;
	virtual ~SourceAndTarget() = default;

	SourceAndTarget(entt::entity sourceEntity, entt::entity targetEntity) :
		sourceEntity(sourceEntity), targetEntity(targetEntity)
	{
	}

	inline bool operator==(const SourceAndTarget& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


	const CryGUID& GetGuid() const override final
	{
		static CryGUID guid = "{CCE7B371-EF45-413F-B726-557A2EC427E5}"_cry_guid;

		return guid;
	}


	virtual const entt::hashed_string& GetHashedName() const
	{
		static constexpr entt::hashed_string nameHS {"source-and-target"_hs};

		return nameHS;
	}


	static void ReflectType(Schematyc::CTypeDesc<SourceAndTarget>& desc)
	{
		desc.SetGUID(SourceAndTarget().GetGuid());
		desc.SetLabel("SourceAndTarget");
		desc.SetDescription("SourceAndTarget");
	}


	bool Serialize(Serialization::IArchive& archive) override final
	{
		return true;
	}

	/** The source of the heal. */
	entt::entity sourceEntity;

	/** The target that will receive the heal - require a Health component. */
	entt::entity targetEntity;
};


struct Requirement
{
	/**  */
	// Need some way to have a list of requirements e.g. level, class, profession, skill, race
};


// A weapon is also an item, and thus must always have an ItemClass component in it's entity.
struct WeaponClass
{
	/**  */
};


struct Mesh
{
	/** File path to a mesh. */
	string modelFilePath;

	/** File path to a material which should be applied to the mesh. Can be empty. */
	string materialFilePath;
};


struct LootTableReference
{
	/** A reference to the loot table which should be used when calculating loot. */
	string lootTableReference;
};


// Should there be one of these for each sort of lock type? Do we remove them once the lock is opened / broken?
struct LockedWithKey
{
	/**  */
	string key;
};


struct Key
{
	/**  */
	string key;
};


struct Ownership
{
	/**  */
	int owner;
};


void RegisterComponentsWithMeta();

}