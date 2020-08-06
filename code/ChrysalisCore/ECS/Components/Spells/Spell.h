#pragma once

#include "ECS/Components/Components.h"


namespace Chrysalis::ECS
{
/**
	Requirements:

	targetType - enemy, ally, neutral?
	description - in simple text, no markdown?
	resistanceType - is this appropriate at a spell level?
	castTime - in seconds
	requireLoS
	add hate
	dot stacks
	max targets
	damage reduction for multiple targets

	*** use the meta to store a means of executing the spell components? maybe with their support for functions / properties / etc
	will we need to use a flag component to let us know which entities need this special processing? this would allow us to queue them
	up with the other spells e.g. damage and heals
	
	*** ISpellContainer - allows a component to contain a list of spells that can be cast when interacting with them. Players and NPCs
	would need spells for things like trading, inspecting, ... or maybe that should just be intrinsic - will need to flesh that
	out a little better.

*/


/** Who should the spell try and target? Some target types need an actual entity for the target, while others
	are ground / area targetted.
*/
enum class TargetType
{
	none,

	self,
	groupMembers,
	raidMembers,
	singleTarget,
	cone,
	column,
	chain,
	sourceBasedAOE,
	targetBasedAOE,
	groundTargettedAOE,
};


/** Used to allow each individual component to select which of the two available spell targets to use for it's actual target.
	Giving each component this ability to select the actual target opens up the means to have mana cost come from the target 
	instead of the caster (mana burn) and life steal abilities. */
enum class TargetTargetType
{
	source,
	target,
};


/** Limit targetting to entities who match this aggression level. */
enum class TargetAggressionType
{
	faction,			// Faction members only.
	allied,				// Allied factions.
	neutral,			// Neither an ally nor an enemy.
	aggressive,			// Actively hostile.
};


enum class SpellCastStyle
{
	immediate,			// Spell is cast instantly. Can't be interupted. Can be cast while moving.
	movementAllowed,	// A cast time applies - may be cast while moving.
	turret,				// A cast time applies. No movement allowed.
	channelled			// Continuous concentration requirement. No movement allowed.
};


/** When spells are cast they require some changes from the prototype in order to function correctly.
	In order to get this right we need to know what changes it expects, and apply those as required.
	This should provide a high degree of flexibility, since we will be able to handle common spell
	types without knowlegde of their specifics. */

enum class SpellRewire
{
	none,				// Nothing special needs to be done.
	damage,				// Apply source, target, update damage values and qi use.
	heal,				// Apply source, target, update heal values and qi use.
	regenerate,			// ** Special case - mana regen on another
	crowdControl,		// Apply source, target - and whatever else...not sure yet.
	custom				// Need custom logic and handling to make this work.
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

using CrowdControlNone = entt::tag<"crowd-control-none"_hs>;
using CrowdControlBlind = entt::tag<"crowd-control-blind"_hs>;
using CrowdControlDisarm = entt::tag<"crowd-control-disarm"_hs>;
//using CrowdControl = entt::tag<"crowd-control-"_hs>;


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


enum class SpellcastPayload
{
	immediate,			// The spell payload fires immediately.
	channelledActive,	// The spell payload is channelled over time as long the key is held down.
	channelledDuration,	// The spell payload is channelled for the duration of the spell.
	onCompletion,		// The spell payload fires on completion of the cast duration.
};


enum class SpellCastExecutionStatus
{
	initialised,		// The spell is ready to start casting.
	casting,			// The spell is currently casting.
	failed,				// The spell cast failed, whiffed or fizzled.
	cancelled,			// The player requested a cancel of the spell cast.
	success,			// The spell completed successfully.
};


struct Spell
{
	Spell() = default;
	virtual ~Spell() = default;


	void Serialize(Serialization::IArchive& ar)
	{
		ar(spellRewire, "spell-rewire", "Actions which need to be taken before spell can be fired.");
		ar(minRange, "minRange", "Minimum range at which this can be cast.");
		ar(maxRange, "maxRange", "Maximum range at which this can be cast.");
		ar(castDuration, "castDuration", "The length of time it takes to cast this spell. Instant cast spells should be zero.");
		ar(cooldown, "cooldown", "Cooldown. The number of seconds before this spell can be cast again.");
		ar(globalCooldown, "globalCooldown", "Global cooldown. The number of seconds before *any* spell can be cast again.");
		ar(sourceTargetType, "sourceTargetType", "Source of the spell - typically none or self.");
		ar(targetTargetType, "targetTargetType", "Target for the spell. May target self, others, or even AoEs.");
		ar(spellcastPayload, "spellcastPayload", "At what time should the spell payload be delivered?");
	}


	/** Code that needs to run after a spell is copied to fix up all the broken requirements e.g. source, target, spell bonuses */
	SpellRewire spellRewire {SpellRewire::damage};

	/** Number of actions this spell will take before being removed. */
	uint32 actions {0};

	/** Minimum range at which this can be cast. */
	float minRange {0.0f};

	/** Maximum range at which this can be cast. */
	float maxRange {30.0f};

	/** The length of time it takes to cast this spell. Instant cast spells should be zero. */
	float castDuration {2.0f};

	/** Cooldown. The number of seconds before this spell can be cast again. */
	float cooldown {2.0f};

	/** Global cooldown. The number of seconds before *any* spell can be cast again. */
	float globalCooldown {0.0f};

	/** At what time should the spell payload be delivered? */
	SpellcastPayload spellcastPayload {SpellcastPayload::onCompletion};

	// Source for the spell will generally only be none or self.
	TargetType sourceTargetType {TargetType::self};
	
	// The target for the spell can be any valid form of target, including ones that do not include a target entity but rather an AOE
	// or chain of targets. Note: chained targets may still need a valid target entity for the first in the chain.
	TargetType targetTargetType {TargetType::singleTarget};

	// Which sort of targets can this spell be cast upon?
	TargetAggressionType targetAggressionType {TargetAggressionType::allied};
};


struct SpellcastExecution
{
	SpellcastExecution() = default;
	virtual ~SpellcastExecution() = default;


	void Serialize(Serialization::IArchive& ar)
	{
		ar(executionTime, "execution-time", "Duration the spell has been executing.");
		ar(castExecutionStatus, "cast-execution-status", "Status of the casting mechanic.");
	}


	/** Duration the spell has been executing. */
	float executionTime {0.0f};

	/** Status of the casting mechanic. */
	SpellCastExecutionStatus castExecutionStatus {SpellCastExecutionStatus::initialised};
};
}