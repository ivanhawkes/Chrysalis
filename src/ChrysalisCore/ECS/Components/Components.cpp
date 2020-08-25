#include <StdAfx.h>

#include <ECS/Components/Components.h>
#include "CrySerialization/Enum.h"
#include "ECS/Components/Health.h"
#include "ECS/Components/Inventory.h"
#include "ECS/Components/Items.h"
#include "ECS/Components/Qi.h"
#include "ECS/Components/RenderLight.h"
#include "ECS/Components/Spells/Spell.h"
#include "ECS/Components/Spells/SpellActions.h"
#include "ECS/ECS.h"


namespace Chrysalis::ECS
{
template <typename TYPE>
void RegisterTypeWithMeta(entt::hashed_string nameHash)
{
	// Register the component with the meta. This allows searches for the component by name.
	entt::meta<TYPE>()
		.type(nameHash)
		.prop("name-hs"_hs, nameHash);

	// Let the simulation know about it as well.
	Simulation.RegisterTypeWithSimulation<TYPE>(nameHash);
}


//bool Serialize(Serialization::IArchive& ar, entt::entity& value, const char* szName, const char* szLabel)
//{
//	ar(to_underlying(value), "entityId", "An EnTT enity ID.");
//
//	return true;
//}


/** Every component needs to register itself with the meta before we can utilise it properly. */
void RegisterComponentsWithMeta()
{
	RegisterTypeWithMeta<Name>("name"_hs);
	RegisterTypeWithMeta<Prototype>("prototype"_hs);

	// Source and target entitiy Ids.
	RegisterTypeWithMeta<SourceEntity>("source-entity"_hs);
	RegisterTypeWithMeta<TargetEntity>("target-entity"_hs);

	// Health.
	RegisterTypeWithMeta<Health>("health"_hs);
	RegisterTypeWithMeta<Damage>("damage"_hs);
	RegisterTypeWithMeta<DamageOverTime>("damage-over-time"_hs);
	RegisterTypeWithMeta<SelfHarm>("self-harm"_hs);
	RegisterTypeWithMeta<Heal>("heal"_hs);
	RegisterTypeWithMeta<HealOverTime>("heal-over-time"_hs);

	// Qi.
	RegisterTypeWithMeta<Qi>("qi"_hs);
	RegisterTypeWithMeta<UtiliseQi>("utilise-qi"_hs);
	RegisterTypeWithMeta<UtiliseQiOverTime>("utilise-qi-over-time"_hs);
	RegisterTypeWithMeta<ReplenishQi>("replenish-qi"_hs);
	RegisterTypeWithMeta<ReplenishQiOverTime>("replenish-qi-over-time"_hs);

	// Spellcasts.
	RegisterTypeWithMeta<SpellFragment>("spell-fragment"_hs);
	RegisterTypeWithMeta<Spell>("spell"_hs);
	RegisterTypeWithMeta<SpellcastExecution>("spellcast-execution"_hs);
	RegisterTypeWithMeta<SpellCastDuration>("spell-cast-duration"_hs);
	
	// Spell actions.
	RegisterTypeWithMeta<SpellActionSchematyc>("spell-action-schematyc"_hs);
	RegisterTypeWithMeta<SpellActionDRS>("spell-action-drs"_hs);
	RegisterTypeWithMeta<SpellActionInspect>("spell-action-inspect"_hs);
	RegisterTypeWithMeta<SpellActionExamine>("spell-action-examine"_hs);
	RegisterTypeWithMeta<SpellActionTake>("spell-action-take"_hs);
	RegisterTypeWithMeta<SpellActionDrop>("spell-action-drop"_hs);
	RegisterTypeWithMeta<SpellActionThrow>("spell-action-throw"_hs);
	RegisterTypeWithMeta<SpellActionSwitch>("spell-action-switch"_hs);
	RegisterTypeWithMeta<SpellActionOpen>("spell-action-open"_hs);
	RegisterTypeWithMeta<SpellActionClose>("spell-action-close"_hs);
	RegisterTypeWithMeta<SpellActionUnlock>("spell-action-unlock"_hs);
	RegisterTypeWithMeta<SpellActionLock>("spell-action-lock"_hs);

	// Environment.
	RegisterTypeWithMeta<RenderLight>("render-light"_hs);

	// Items.
	RegisterTypeWithMeta<ItemClass>("item-class"_hs);

	// Range checking.
	RegisterTypeWithMeta<Range>("range"_hs);

	// Animation components.
	RegisterTypeWithMeta<AnimationFragmentSpellCast>("animation-fragment-spellcast"_hs);
	RegisterTypeWithMeta<AnimationFragmentEmote>("animation-fragment-emote"_hs);
	RegisterTypeWithMeta<AnimationTag>("animation-tag"_hs);

	// Simple components.
	RegisterTypeWithMeta<Timer>("timer"_hs);
	RegisterTypeWithMeta<Duration>("duration"_hs);
	RegisterTypeWithMeta<Aura>("aura"_hs);
	RegisterTypeWithMeta<Buff>("buff"_hs);
	RegisterTypeWithMeta<Debuff>("debuff"_hs);
	RegisterTypeWithMeta<Channelled>("channelled"_hs);
	RegisterTypeWithMeta<MovementFactor>("movement-factor"_hs);
	RegisterTypeWithMeta<CancelOnMovement>("cancel-on-movement"_hs);
	RegisterTypeWithMeta<AreaOfEffect>("area-of-effect"_hs);

	// Crowd control.
	RegisterTypeWithMeta<CrowdControlBlind>("crowd-control-blind"_hs);
	RegisterTypeWithMeta<CrowdControlDisarm>("crowd-control-disarm"_hs);
	RegisterTypeWithMeta<CrowdControlMovementRestricted>("crowd-control-movement-restricted"_hs);
	RegisterTypeWithMeta<CrowdControlRotationRestricted>("crowd-control-rotation-restricted"_hs);
	RegisterTypeWithMeta<CrowdControlFlee>("crowd-control-flee"_hs);
	RegisterTypeWithMeta<CrowdControlMindControl>("crowd-control-mind-control"_hs);
	RegisterTypeWithMeta<CrowdControlPull>("crowd-control-pull"_hs);
	RegisterTypeWithMeta<CrowdControlTaunt>("crowd-control-taunt"_hs);
	RegisterTypeWithMeta<CrowdControlThrow>("crowd-control-throw"_hs);
	RegisterTypeWithMeta<CrowdControlKockback>("crowd-control-knockback"_hs);
	RegisterTypeWithMeta<CrowdControlKnockdown>("crowd-control-knockdown"_hs);
	RegisterTypeWithMeta<CrowdControlPolymorph>("crowd-control-polymorph"_hs);
	RegisterTypeWithMeta<CrowdControlSilence>("crowd-control-silence"_hs);

	// Cooldowns.
	RegisterTypeWithMeta<Cooldown>("cooldown"_hs);
}
}
