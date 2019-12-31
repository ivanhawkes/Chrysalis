#include <StdAfx.h>

#include "Components.h"
#include "CrySerialization/Enum.h"
#include "ECS/Components/Health.h"
#include "ECS/Components/Inventory.h"
#include "ECS/Components/Items.h"
#include "ECS/Components/Qi.h"
#include "ECS/Components/Spell.h"


namespace Chrysalis::ECS
{
SERIALIZATION_ENUM_BEGIN(DamageType, "Damage Type")
SERIALIZATION_ENUM(DamageType::acid, "acid", "Acid")
SERIALIZATION_ENUM(DamageType::bleed, "bleed", "Bleed")
SERIALIZATION_ENUM(DamageType::qi, "qi", "qi")
SERIALIZATION_ENUM(DamageType::cold, "cold", "cold")
SERIALIZATION_ENUM(DamageType::collision, "collision", "collision")
SERIALIZATION_ENUM(DamageType::crush, "crush", "crush")
SERIALIZATION_ENUM(DamageType::decay, "decay", "decay")
SERIALIZATION_ENUM(DamageType::disease, "disease", "disease")
SERIALIZATION_ENUM(DamageType::electricity, "electricity", "electricity")
SERIALIZATION_ENUM(DamageType::energy, "energy", "energy")
SERIALIZATION_ENUM(DamageType::entropy, "entropy", "entropy")
SERIALIZATION_ENUM(DamageType::explosion, "explosion", "explosion")
SERIALIZATION_ENUM(DamageType::fire, "fire", "fire")
SERIALIZATION_ENUM(DamageType::holy, "holy", "holy")
SERIALIZATION_ENUM(DamageType::ice, "ice", "ice")
SERIALIZATION_ENUM(DamageType::nature, "nature", "nature")
SERIALIZATION_ENUM(DamageType::pierce, "pierce", "pierce")
SERIALIZATION_ENUM(DamageType::plasma, "plasma", "plasma")
SERIALIZATION_ENUM(DamageType::poison, "poison", "poison")
SERIALIZATION_ENUM(DamageType::radiation, "radiation", "radiation")
SERIALIZATION_ENUM(DamageType::slash, "slash", "slash")
SERIALIZATION_ENUM(DamageType::tear, "tear", "tear")
SERIALIZATION_ENUM(DamageType::unholy, "unholy", "unholy")
SERIALIZATION_ENUM_END()

SERIALIZATION_ENUM_BEGIN(TargetType, "Target Type")
SERIALIZATION_ENUM(TargetType::self, "self", "self")
SERIALIZATION_ENUM(TargetType::singleTarget, "singleTarget", "singleTarget")
SERIALIZATION_ENUM(TargetType::cone, "cone", "cone")
SERIALIZATION_ENUM(TargetType::column, "column", "column")
SERIALIZATION_ENUM(TargetType::chain, "chain", "chain")
SERIALIZATION_ENUM(TargetType::sourceBasedAOE, "sourceBasedAOE", "sourceBasedAOE")
SERIALIZATION_ENUM(TargetType::targetBasedAOE, "targetBasedAOE", "targetBasedAOE")
SERIALIZATION_ENUM(TargetType::groundTargettedAOE, "groundTargettedAOE", "groundTargettedAOE")
SERIALIZATION_ENUM_END()


// Use a registry to create a new instance of a type, assign it to an entity, and then return a reference to that instance.
template<typename Type>
Type& assign(const entt::entity entity, entt::registry* registry)
{
	return registry->assign<Type>(entity);
}


/** Every component needs to register itself with the meta before we can utilise it properly. */
void RegisterComponentsWithMeta()
{
	// The base component is required in order to call the .base function.
	entt::meta<ECS::IComponent>()
		.type(ECS::IComponent().GetHashedName());

	// General.
	entt::meta<ECS::Name>()
		.base<ECS::IComponent>()
		.type(ECS::Name().GetHashedName())
		.ctor<&assign<ECS::Name>, entt::as_alias_t>();

	entt::meta<ECS::SourceAndTarget>()
		.base<ECS::IComponent>()
		.type(ECS::SourceAndTarget().GetHashedName())
		.ctor<&assign<ECS::SourceAndTarget>, entt::as_alias_t>();

	// Health.
	entt::meta<ECS::Health>()
		.base<ECS::IComponent>()
		.type(ECS::Health().GetHashedName())
		.ctor<&assign<ECS::Health>, entt::as_alias_t>();

	entt::meta<ECS::Damage>()
		.base<ECS::IComponent>()
		.type(ECS::Damage().GetHashedName())
		.ctor<&assign<ECS::Damage>, entt::as_alias_t>();

	entt::meta<ECS::DamageOverTime>()
		.base<ECS::IComponent>()
		.type(ECS::DamageOverTime().GetHashedName())
		.ctor<&assign<ECS::DamageOverTime>, entt::as_alias_t>();

	entt::meta<ECS::Heal>()
		.base<ECS::IComponent>()
		.type(ECS::Heal().GetHashedName())
		.ctor<&assign<ECS::Heal>, entt::as_alias_t>();

	entt::meta<ECS::HealOverTime>()
		.base<ECS::IComponent>()
		.type(ECS::HealOverTime().GetHashedName())
		.ctor<&assign<ECS::HealOverTime>, entt::as_alias_t>();

	// Qi.
	entt::meta<ECS::Qi>()
		.base<ECS::IComponent>()
		.type(ECS::Qi().GetHashedName())
		.ctor<&assign<ECS::Qi>, entt::as_alias_t>();

	entt::meta<ECS::UtiliseQi>()
		.base<ECS::IComponent>()
		.type(ECS::UtiliseQi().GetHashedName())
		.ctor<&assign<ECS::UtiliseQi>, entt::as_alias_t>();

	entt::meta<ECS::UtiliseQiOverTime>()
		.base<ECS::IComponent>()
		.type(ECS::UtiliseQiOverTime().GetHashedName())
		.ctor<&assign<ECS::UtiliseQiOverTime>, entt::as_alias_t>();

	entt::meta<ECS::ReplenishQi>()
		.base<ECS::IComponent>()
		.type(ECS::ReplenishQi().GetHashedName())
		.ctor<&assign<ECS::ReplenishQi>, entt::as_alias_t>();

	entt::meta<ECS::ReplenishQiOverTime>()
		.base<ECS::IComponent>()
		.type(ECS::ReplenishQiOverTime().GetHashedName())
		.ctor<&assign<ECS::ReplenishQiOverTime>, entt::as_alias_t>();

	// Spell.
	entt::meta<ECS::Spell>()
		.base<ECS::IComponent>()
		.type(ECS::Spell().GetHashedName())
		.ctor<&assign<ECS::Spell>, entt::as_alias_t>();

	// Items.
	entt::meta<ECS::ItemClass>()
		.base<ECS::IComponent>()
		.type(ECS::ItemClass().GetHashedName())
		.ctor<&assign<ECS::ItemClass>, entt::as_alias_t>();
}
}
