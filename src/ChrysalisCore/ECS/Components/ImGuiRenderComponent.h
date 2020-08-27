#pragma once

#include <entt/entt.hpp>


/** All the functions needed for rendering out components to ImGui should be kept here. */

namespace Chrysalis::ECS
{
#ifdef IMGUI

// Attributes.
void ImGuiRenderComponent(AttributeType ar);

// Name and prototypes for references.
void ImGuiRenderComponent(Name ar);
void ImGuiRenderComponent(Prototype ar);

// Source and target entitiy Ids.
void ImGuiRenderComponent(SourceEntity ar);
void ImGuiRenderComponent(TargetEntity ar);

// Health.
void ImGuiRenderComponent(Health ar);
void ImGuiRenderComponent(Damage ar);
void ImGuiRenderComponent(DamageOverTime ar);
void ImGuiRenderComponent(SelfHarm ar);
void ImGuiRenderComponent(Heal ar);
void ImGuiRenderComponent(HealOverTime ar);

// Qi.
void ImGuiRenderComponent(Qi ar);
void ImGuiRenderComponent(UtiliseQi ar);
void ImGuiRenderComponent(UtiliseQiOverTime ar);
void ImGuiRenderComponent(ReplenishQi ar);
void ImGuiRenderComponent(ReplenishQiOverTime ar);

// Spellcasts.
void ImGuiRenderComponent(Spell ar);
void ImGuiRenderComponent(SpellFragment ar);
void ImGuiRenderComponent(SpellTargetType ar);
void ImGuiRenderComponent(SpellTargetAggressionType ar);
void ImGuiRenderComponent(SpellcastExecution ar);
void ImGuiRenderComponent(SpellCastDuration ar);

// Spell actions.
void ImGuiRenderComponent(SpellActionSchematyc ar);
void ImGuiRenderComponent(SpellActionDRS ar);
void ImGuiRenderComponent(SpellActionInspect ar);
void ImGuiRenderComponent(SpellActionExamine ar);
void ImGuiRenderComponent(SpellActionTake ar);
void ImGuiRenderComponent(SpellActionDrop ar);
void ImGuiRenderComponent(SpellActionThrow ar);
void ImGuiRenderComponent(SpellActionSwitch ar);
void ImGuiRenderComponent(SpellActionOpen ar);
void ImGuiRenderComponent(SpellActionClose ar);
void ImGuiRenderComponent(SpellActionUnlock ar);
void ImGuiRenderComponent(SpellActionLock ar);

// Environment.
void ImGuiRenderComponent(RenderLight ar);

// Items.
void ImGuiRenderComponent(ItemClass ar);

// Range checking.
void ImGuiRenderComponent(Range ar);

// Animation components.
void ImGuiRenderComponent(AnimationFragmentSpellCast ar);
void ImGuiRenderComponent(AnimationFragmentEmote ar);
void ImGuiRenderComponent(AnimationTag ar);

// Simple components.
void ImGuiRenderComponent(Timer ar);
void ImGuiRenderComponent(Duration ar);
void ImGuiRenderComponent(Aura ar);
void ImGuiRenderComponent(Buff ar);
void ImGuiRenderComponent(Debuff ar);
void ImGuiRenderComponent(Channelled ar);
void ImGuiRenderComponent(MovementFactor ar);
void ImGuiRenderComponent(CancelOnMovement ar);
void ImGuiRenderComponent(AreaOfEffect ar);

// Crowd control.
void ImGuiRenderComponent(CrowdControlBlind ar);
void ImGuiRenderComponent(CrowdControlDisarm ar);
void ImGuiRenderComponent(CrowdControlMovementRestricted ar);
void ImGuiRenderComponent(CrowdControlRotationRestricted ar);
void ImGuiRenderComponent(CrowdControlFlee ar);
void ImGuiRenderComponent(CrowdControlMindControl ar);
void ImGuiRenderComponent(CrowdControlPull ar);
void ImGuiRenderComponent(CrowdControlTaunt ar);
void ImGuiRenderComponent(CrowdControlThrow ar);
void ImGuiRenderComponent(CrowdControlKockback ar);
void ImGuiRenderComponent(CrowdControlKnockdown ar);
void ImGuiRenderComponent(CrowdControlPolymorph ar);
void ImGuiRenderComponent(CrowdControlSilence ar);

// Cooldowns.
void ImGuiRenderComponent(Cooldown ar);

#endif
}