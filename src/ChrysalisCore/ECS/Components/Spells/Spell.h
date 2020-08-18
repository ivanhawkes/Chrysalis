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


// Loss of sight, movement and rotation restricted, ambling around.
using CrowdControlBlind = FlagComponent<"crowd-control-blind"_hs>;

// Primary weapon disabled.
using CrowdControlDisarm = FlagComponent<"crowd-control-disarm"_hs>;

// Movement restricted.
using CrowdControlMovementRestricted = SimpleComponent<float, "crowd-control-movement-restricted"_hs>;

// Rotation restricted.
using CrowdControlRotationRestricted = SimpleComponent<float, "crowd-control-rotation-restricted"_hs>;

// Ambling around quaking in fear. Can be used with movement and rotation restrictions.
using CrowdControlFlee = FlagComponent<"crowd-control-flee"_hs>;

// Under the control of another entity.
using CrowdControlMindControl = FlagComponent<"crowd-control-mind-control"_hs>;

// Pulled towards something with force.
using CrowdControlPull = FlagComponent<"crowd-control-pull"_hs>;

// Attacks are forced to be directed towards a specific entity.
using CrowdControlTaunt = FlagComponent<"crowd-control-taunt"_hs>;

// Thrown to the ground.
using CrowdControlThrow = FlagComponent<"crowd-control-throw"_hs>;

// Physically knocked back a step or two.
using CrowdControlKockback = FlagComponent<"crowd-control-knockback"_hs>;

// Physically knocked back a step or two and onto your arse.
using CrowdControlKnockdown = FlagComponent<"crowd-control-knockdown"_hs>;

// Turned into a harmless critter.
using CrowdControlPolymorph = FlagComponent<"crowd-control-polymorph"_hs>;

// Restricts use of spells, shouts and other vocal abilities.
using CrowdControlSilence = FlagComponent<"crowd-control-silence"_hs>;

// A timer for counting up or down.
using Cooldown = SimpleComponent<float, "cooldown"_hs>;


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
	queued,				// The spell has been added to the processing queue.
	casting,			// The spell is currently casting.
	failed,				// The spell cast failed, whiffed or fizzled.
	cancelled,			// The player requested a cancel of the spell cast.
	success,			// The spell completed successfully.
};


struct Spell final
{
	void Serialize(Serialization::IArchive& ar)
	{
		ar(castDuration, "castDuration", "The length of time it takes to cast this spell. Instant cast spells should be zero.");
		ar(sourceTargetType, "sourceTargetType", "Source of the spell - typically none or self.");
		ar(targetTargetType, "targetTargetType", "Target for the spell. May target self, others, or even AoEs.");
		ar(spellcastPayload, "spellcastPayload", "At what time should the spell payload be delivered?");
	}


#ifdef IMGUI
	void ImGuiRender();
#endif


	/** The length of time it takes to cast this spell. Instant cast spells should be zero. */
	float castDuration {2.0f};

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


struct SpellcastExecution final
{
	void Serialize(Serialization::IArchive& ar)
	{
		ar(executionTime, "execution-time", "Duration the spell has been executing.");
		ar(castExecutionStatus, "cast-execution-status", "Status of the casting mechanic.");
	}


#ifdef IMGUI
	void ImGuiRender();
#endif


	/** Duration the spell has been executing. */
	float executionTime {0.0f};

	/** Status of the casting mechanic. */
	SpellCastExecutionStatus castExecutionStatus {SpellCastExecutionStatus::queued};
};
}