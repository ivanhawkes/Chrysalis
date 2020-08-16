#pragma once

#include <entt/entt.hpp>
#include <CryExtension/CryGUID.h>


/** Thinking about making this a single include header for convenience. */

namespace Chrysalis::ECS
{
void RegisterComponentsWithMeta();


struct AttributeType
{
	AttributeType() = default;
	virtual ~AttributeType() = default;

	AttributeType(float base, float baseModifiers, float modifiers) :
		base(base), baseModifiers(baseModifiers), modifiers(modifiers)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(base, "base", "base");
		ar(baseModifiers, "baseModifiers", "baseModifiers");
		ar(modifiers, "modifiers", "modifiers");
	}


#ifdef IMGUI
	void ImGuiRender();
#endif


	/** Returns the current value for base, after it's modifiers have been applied. */
	const float GetBaseAttribute() const
	{
		return base + baseModifiers;
	}


	/** Returns the current value for the attribute, after it's modifiers have been applied. */
	const float GetAttribute() const
	{
		return base + modifiers;
	}

	/** Represents the attribute without any modifiers applied to it. */
	float base {100.0f};

	/** This modifier makes changes to the base value, instead of the frame value. Typical use would be for a health / strength
	buff that increases the base value of the attribute. */
	float baseModifiers {0};

	/** Modifiers for this frame. Should be calculated each frame prior to calculating the current value. */
	float modifiers {0};
};


template<entt::id_type label>
struct FlagComponent
{
	void Serialize(Serialization::IArchive& ar) {  }

#ifdef IMGUI
	void ImGuiRender() {};
#endif

private:
	// Adding this member prevents EnTT from optimising this component out when saving and loading.
	// TODO: Find out how to make these take no space but avoid being skipped by EnTT.
	int wastedSpace {0};
};


/** Template for a simple component that has only one member, and the name of that member can be 'value'. */
template<typename TYPE, entt::id_type label>
struct SimpleComponent
{
	SimpleComponent() = default;
	virtual ~SimpleComponent() = default;

	SimpleComponent(TYPE value) :value(value) {};

	void Serialize(Serialization::IArchive& ar)
	{
		ar(value, "value", "A value.");
	}

#ifdef IMGUI
	void ImGuiRender() {};
#endif

private:
	TYPE value;
};


struct Name
{
	Name() = default;
	virtual ~Name() = default;

	Name(string name, string displayName) :
		name(name), displayName(displayName)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(name, "name", "The name of this entity. It should be unique.");
		ar(displayName, "displayName", "The display name for this entity.");
	}


#ifdef IMGUI
	void ImGuiRender();
#endif


	/** A unique name. */
	string name;

	/** A name which can be used in the UI. */
	string displayName;
};


/**	A way of marking an entity and saying that is it based on the components of a different entity. The prototype entity
	needs to have a matching name and must exist in a registry of the designer's choice.

*/

struct Prototype
{
	Prototype() = default;
	virtual ~Prototype() = default;

	Prototype(entt::entity prototypeEntityId) :
		prototypeEntityId(prototypeEntityId)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(prototypeEntityId, "prototypeEntityId", "Entity Id for the prototype this entity uses as it's base.");
	}


#ifdef IMGUI
	void ImGuiRender() {};
#endif


	/** Unique Id for the prototye of this entiity. */
	entt::entity prototypeEntityId {entt::null};
};


struct SourceEntity
{
	SourceEntity() = default;
	virtual ~SourceEntity() = default;

	SourceEntity(entt::entity sourceEntity, EntityId crySourceEntityId) :
		sourceEntityId(sourceEntity), crySourceEntityId(crySourceEntityId)
	{
	}

	void Serialize(Serialization::IArchive& ar)
	{
		ar(sourceEntityId, "sourceEntityId", "Source Entity");
		ar(crySourceEntityId, "crySourceEntityId", "Cry Source Entity ID");
	}


#ifdef IMGUI
	void ImGuiRender() {};
#endif


	/** The source entity (EnTT). */
	entt::entity sourceEntityId {entt::null};

	/** The source entity (CRYENGINE). */
	EntityId crySourceEntityId {INVALID_ENTITYID};
};


struct TargetEntity
{
	TargetEntity() = default;
	virtual ~TargetEntity() = default;

	TargetEntity(entt::entity targetEntity, EntityId cryTargetEntityId) :
		targetEntityId(targetEntity), cryTargetEntityId(cryTargetEntityId)
	{
	}

	void Serialize(Serialization::IArchive& ar)
	{
		ar(targetEntityId, "targetEntityId", "Target Entity");
		ar(cryTargetEntityId, "cryTargetEntityId", "Cry Target Entity ID");
	}


#ifdef IMGUI
	void ImGuiRender() {};
#endif


	/** The target entity (EnTT).*/
	entt::entity targetEntityId {entt::null};

	/** The target entity (CRYENGINE). */
	EntityId cryTargetEntityId {INVALID_ENTITYID};
};


struct Range
{
	Range() = default;
	virtual ~Range() = default;


	void Serialize(Serialization::IArchive& ar)
	{
		ar(minRange, "minRange", "A minimum range in metres.");
		ar(maxRange, "maxRange", "A maximum range in metres.");
	}


#ifdef IMGUI
	void ImGuiRender();
#endif


	/** A minimum range (metres). */
	float minRange {0.0f};

	/** A maximum range (metres). */
	float maxRange {30.0f};
};


// Simple components.
using Timer = SimpleComponent<float, "Timer"_hs>;							// A timer for counting up or down.
using Aura = FlagComponent<"Aura"_hs>;										// Indicates this is an aura.
using Buff = FlagComponent<"Buff"_hs>;										// Indicates this is a buff.
using Debuff = FlagComponent<"Debuff"_hs>;									// Indicates this is a debuff.
using Channelled = SimpleComponent<bool, "Channelled"_hs>;					// Spellcast is channlled - this may just be a pair of crowd controls or might work with them.
using AnimationFragment = SimpleComponent<string, "AnimationFragment"_hs>;	// A fragment to pass to mannequin.
using AnimationTag = SimpleComponent<string, "AnimationTag"_hs>;			// A tag to pass to mannequin.
using MovementFactor = SimpleComponent<float, "MovementFactor"_hs>;			// Apply factor to movement. You can stop, slow, or accelerate an entity.
using CancelOnMovement = SimpleComponent<bool, "cancel-on-movement"_hs>;	// If you move, something gets cancelled.
using AreaOfEffect = SimpleComponent<float, "AreaOfEffect"_hs>;				// Flags something as an AOE and provides a radius for that AOE.


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
	Ownership() = default;
	virtual ~Ownership() = default;

	Ownership(entt::entity ownerId) :
		ownerId(ownerId)
	{
	}


	void Serialize(Serialization::IArchive& ar)
	{
		ar(ownerId, "ownerId", "Entity Id for the prototype this entity uses as it's base.");
	}


#ifdef IMGUI
	void ImGuiRender() {};
#endif


	/** Unique Id for the prototye of this entiity. */
	entt::entity ownerId {entt::null};;
};
}