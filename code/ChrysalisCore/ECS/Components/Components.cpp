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
	Simulation.RegisterTypeWithSimulation<TYPE>();
}


/** Every component needs to register itself with the meta before we can utilise it properly. */
void RegisterComponentsWithMeta()
{
	RegisterTypeWithMeta<Name>("name"_hs);
	RegisterTypeWithMeta<Prototype>("prototype"_hs);
	RegisterTypeWithMeta<SourceAndTarget>("source-and-target"_hs);

	// Health.
	RegisterTypeWithMeta<Health>("health"_hs);
	RegisterTypeWithMeta<Damage>("damage"_hs);
	RegisterTypeWithMeta<DamageOverTime>("damage-over-time"_hs);
	RegisterTypeWithMeta<Heal>("heal"_hs);
	RegisterTypeWithMeta<HealOverTime>("heal-over-time"_hs);

	// Qi.
	RegisterTypeWithMeta<Qi>("qi"_hs);
	RegisterTypeWithMeta<UtiliseQi>("utilise-qi"_hs);
	RegisterTypeWithMeta<UtiliseQiOverTime>("utilise-qi-over-time"_hs);
	RegisterTypeWithMeta<ReplenishQi>("replenish-qi"_hs);
	RegisterTypeWithMeta<ReplenishQiOverTime>("replenish-qi-over-time"_hs);

	// Spell.
	RegisterTypeWithMeta<Spell>("spell"_hs);
	RegisterTypeWithMeta<SpellcastExecution>("spellcast-execution"_hs);

	// Spell actions.
	RegisterTypeWithMeta<SpellActionSchematyc>("schematyc"_hs);
	RegisterTypeWithMeta<SpellActionDRS>("drs"_hs);
	RegisterTypeWithMeta<SpellActionInspect>("inspect"_hs);
	RegisterTypeWithMeta<SpellActionExamine>("examine"_hs);
	RegisterTypeWithMeta<SpellActionTake>("take"_hs);
	RegisterTypeWithMeta<SpellActionDrop>("drop"_hs);
	RegisterTypeWithMeta<SpellActionThrow>("throw"_hs);
	RegisterTypeWithMeta<SpellActionSwitch>("switch"_hs);
	RegisterTypeWithMeta<SpellActionOpen>("open"_hs);
	RegisterTypeWithMeta<SpellActionClose>("close"_hs);
	RegisterTypeWithMeta<SpellActionUnlock>("unlock"_hs);
	RegisterTypeWithMeta<SpellActionLock>("lock"_hs);

	// Environment.
	RegisterTypeWithMeta<RenderLight>("render-light"_hs);

	// Items.
	RegisterTypeWithMeta<ItemClass>("item-class"_hs);

	// TEST.
	RegisterTypeWithMeta<SaltComponent>("salt"_hs);
	RegisterTypeWithMeta<PepperComponent>("pepper"_hs);
	
	RegisterTypeWithMeta<CrowdControlNone>("crowd-control-none"_hs);
}
}
