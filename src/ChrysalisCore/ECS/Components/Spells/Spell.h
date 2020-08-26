#pragma once

#include "ECS/Components/Components.h"
#include <vector>


namespace Chrysalis::ECS
{
/**
	Requirements:

	description - in simple text, no markdown?
	resistanceType - is this appropriate at a spell level?
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
	singleTarget,
	groupMembers,
	raidMembers,
	cone,
	column,
	chain,
	sourceBasedAOE,
	targetBasedAOE,
	groundTargettedAOE,
};


/** Limit targetting to entities who match this aggression level. */
enum class TargetAggressionType
{
	faction,			// Faction members only.
	allied,				// Allied factions.
	neutral,			// Neither an ally nor an enemy.
	aggressive,			// Actively hostile.
};


// Spell cast duration.
using SpellCastDuration = SimpleComponent<float, "spell-cast-duration"_hs>;

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

// A timer.
using Cooldown = SimpleComponent<float, "cooldown"_hs>;


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


// If you move, something gets cancelled.
using CancelOnMovement = SimpleComponent<bool, "cancel-on-movement"_hs>;

// Spellcast is channelled. Effects occur during the channelling. Cancel on movement is generally expected to be paired with this component.
using Channelled = SimpleComponent<bool, "Channelled"_hs>;


struct SpellFragment final
{
	void Serialize(Serialization::IArchive& ar)
	{
		ar(targetType, "targetType", "Source of the spell - typically none or self.");
		ar(spellcastPayload, "spellcastPayload", "At what time should the spell payload be delivered?");
	}


	/** At what time should the spell payload be delivered? */
	SpellcastPayload spellcastPayload {SpellcastPayload::onCompletion};

	// Source for the spell will generally only be none or self.
	TargetType targetType {TargetType::self};
	
	// Which sort of targets can this spell be cast upon?
	TargetAggressionType targetAggressionType {TargetAggressionType::allied};
};


struct Spell final
{
	void Serialize(Serialization::IArchive& ar)
	{
		ar(fragments, "fragments", "A list of the fragments that make up this spell.");
	}


	// A spell is comprised of fragments.
	std::vector<Prototype> fragments;
};


struct SpellcastExecution final
{
	void Serialize(Serialization::IArchive& ar)
	{
		ar(executionTime, "execution-time", "Duration the spell has been executing.");
		ar(castExecutionStatus, "cast-execution-status", "Status of the casting mechanic.");
	}


	/** Duration the spell has been executing. */
	float executionTime {0.0f};

	/** Status of the casting mechanic. */
	SpellCastExecutionStatus castExecutionStatus {SpellCastExecutionStatus::queued};
};
}