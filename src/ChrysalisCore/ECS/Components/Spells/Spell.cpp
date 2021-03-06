#include <StdAfx.h>

#include "Spell.h"
#include "CrySerialization/Enum.h"


namespace Chrysalis::ECS
{
// Entity targetting for both the source and target.
SERIALIZATION_ENUM_BEGIN(TargetType, "Target Type")
SERIALIZATION_ENUM(TargetType::none, "none", "none")
SERIALIZATION_ENUM(TargetType::self, "self", "self")
SERIALIZATION_ENUM(TargetType::groupMembers, "groupMembers", "groupMembers")
SERIALIZATION_ENUM(TargetType::raidMembers, "raidMembers", "raidMembers")
SERIALIZATION_ENUM(TargetType::singleTarget, "singleTarget", "singleTarget")
SERIALIZATION_ENUM(TargetType::cone, "cone", "cone")
SERIALIZATION_ENUM(TargetType::column, "column", "column")
SERIALIZATION_ENUM(TargetType::chain, "chain", "chain")
SERIALIZATION_ENUM(TargetType::sourceBasedAOE, "sourceBasedAOE", "sourceBasedAOE")
SERIALIZATION_ENUM(TargetType::targetBasedAOE, "targetBasedAOE", "targetBasedAOE")
SERIALIZATION_ENUM(TargetType::groundTargettedAOE, "groundTargettedAOE", "groundTargettedAOE")
SERIALIZATION_ENUM_END()

// Target target type.
SERIALIZATION_ENUM_BEGIN(TargetTargetType, "Target Target Type")
SERIALIZATION_ENUM(TargetTargetType::source, "source", "source")
SERIALIZATION_ENUM(TargetTargetType::target, "target", "target")
SERIALIZATION_ENUM_END()

// Target aggression.
SERIALIZATION_ENUM_BEGIN(TargetAggressionType, "Target Aggression Type")
SERIALIZATION_ENUM(TargetAggressionType::faction, "faction", "faction")
SERIALIZATION_ENUM(TargetAggressionType::allied, "allied", "allied")
SERIALIZATION_ENUM(TargetAggressionType::neutral, "neutral", "neutral")
SERIALIZATION_ENUM(TargetAggressionType::aggressive, "aggressive", "aggressive")
SERIALIZATION_ENUM_END()

// Spellcast Execution Status.
SERIALIZATION_ENUM_BEGIN(SpellcastPayload, "Spellcast Payload")
SERIALIZATION_ENUM(SpellcastPayload::immediate, "immediate", "immediate")
SERIALIZATION_ENUM(SpellcastPayload::channelledActive, "channelledActive", "channelledActive")
SERIALIZATION_ENUM(SpellcastPayload::channelledDuration, "channelledDuration", "channelledDuration")
SERIALIZATION_ENUM(SpellcastPayload::onCompletion, "onCompletion", "onCompletion")
SERIALIZATION_ENUM_END()

// Spellcast Execution Status.
SERIALIZATION_ENUM_BEGIN(SpellCastExecutionStatus, "Spellcast Execution Status")
SERIALIZATION_ENUM(SpellCastExecutionStatus::queued, "queued", "queued")
SERIALIZATION_ENUM(SpellCastExecutionStatus::casting, "casting", "casting")
SERIALIZATION_ENUM(SpellCastExecutionStatus::failed, "failed", "failed")
SERIALIZATION_ENUM(SpellCastExecutionStatus::cancelled, "cancelled", "cancelled")
SERIALIZATION_ENUM(SpellCastExecutionStatus::success, "success", "success")
SERIALIZATION_ENUM_END()


#ifdef IMGUI
void Spell::ImGuiRender()
{
}


void SpellcastExecution::ImGuiRender()
{
	ImGui::InputFloat("Execution Time:", &executionTime);
}
#endif
}
