#include <StdAfx.h>

#include "Systems.h"
#include "ECS/Components/Health.h"
#include "ECS/Components/Inventory.h"
#include "ECS/Components/Items.h"
#include "ECS/Components/Qi.h"


namespace Chrysalis::ECS
{
// ***
// *** Health System
// ***


void SystemApplyDamage(entt::registry& registry)
{
	// Apply any damage to the damage modifiers.
	auto view = registry.view<ECS::Damage, ECS::SourceAndTarget>();
	for (auto entity : view)
	{
		// Get the components.
		auto& damage = view.get<ECS::Damage>(entity);
		auto& sourceAndTarget = view.get<ECS::SourceAndTarget>(entity);
		auto& targetHealth = registry.get<ECS::Health>(sourceAndTarget.targetEntity);

		// Get the health component for the target entity and apply the damage to it's health modifier.
		targetHealth.health.modifiers -= damage.quantity;

		// Remove just the component.
		registry.remove<ECS::Damage>(entity);
	}
}


void SystemApplyDamageOverTime(float dt, entt::registry& registry)
{
	// Apply any damage to the damage modifiers.
	auto view = registry.view<ECS::DamageOverTime, ECS::SourceAndTarget>();
	for (auto entity : view)
	{
		// Get the components..
		auto& damage = view.get<ECS::DamageOverTime>(entity);
		auto& sourceAndTarget = view.get<ECS::SourceAndTarget>(entity);

		damage.deltaSinceTick += dt;
		if ((damage.deltaSinceTick >= damage.interval) && (damage.ticksRemaining >= 1.0f))
		{
			damage.ticksRemaining--;
			damage.deltaSinceTick -= damage.interval;

			// Get the health component for the target entity and apply the damage to it's health modifier.
			auto& targetHealth = registry.get<ECS::Health>(sourceAndTarget.targetEntity);
			targetHealth.health.modifiers -= damage.quantity;
		}

		if (damage.ticksRemaining <= 0.0f)
		{
			// Remove just the component.
			registry.remove<ECS::DamageOverTime>(entity);
		}
	}
}


void SystemApplyHeal(entt::registry& registry)
{
	// Apply any heals to the health modifiers.
	auto view = registry.view<ECS::Heal, ECS::SourceAndTarget>();
	for (auto entity : view)
	{
		// Get the components..
		auto& heal = view.get<ECS::Heal>(entity);
		auto& sourceAndTarget = view.get<ECS::SourceAndTarget>(entity);
		auto& targetHealth = registry.get<ECS::Health>(sourceAndTarget.targetEntity);

		// Check for overheals.
		float newModifier = targetHealth.health.modifiers + heal.quantity;
		if (newModifier > 0.0f)
		{
			// It was an overheal.
			targetHealth.health.modifiers = 0.0f;
		}
		else
		{
			targetHealth.health.modifiers = newModifier;
		}

		// Remove just the component.
		registry.remove<ECS::Heal>(entity);
	}
}


void SystemApplyHealOverTime(float dt, entt::registry& registry)
{
	// Apply any heal to the health modifiers.
	auto view = registry.view<ECS::HealOverTime, ECS::SourceAndTarget>();
	for (auto entity : view)
	{
		// Get the components..
		auto& heal = view.get<ECS::HealOverTime>(entity);
		auto& sourceAndTarget = view.get<ECS::SourceAndTarget>(entity);

		heal.deltaSinceTick += dt;
		if ((heal.deltaSinceTick >= heal.interval) && (heal.ticksRemaining >= 1.0f))
		{
			heal.ticksRemaining--;
			heal.deltaSinceTick -= heal.interval;

			// Get the health component for the target entity and apply the heal to it's health modifier.
			auto& targetHealth = registry.get<ECS::Health>(sourceAndTarget.targetEntity);

			// Check for overheals.
			float newModifier = targetHealth.health.modifiers + heal.quantity;
			if (newModifier > 0.0f)
			{
				// It was an overheal.
				targetHealth.health.modifiers = 0.0f;
			}
			else
			{
				targetHealth.health.modifiers = newModifier;
			}

			if (heal.ticksRemaining <= 0.0f)
			{
				// Remove just the component.
				registry.remove<ECS::HealOverTime>(entity);
			}
		}
	}
}


void SystemHealthCheck(entt::registry& registry)
{
	// Update each health component, applying the modifier to it's base to calculate the current health.
	// Update death status if appropriate.
	auto view = registry.view<ECS::Health, ECS::SourceAndTarget>();
	for (auto entity : view)
	{
		// Get the components..
		auto& health = view.get<ECS::Health>(entity);

		// Check for overkill.
		float newHealth = health.health.GetBaseAttribute() + health.health.modifiers;
		if (newHealth <= 0.0f)
		{
			health.isDead = true;

			// TODO: Inform them they died with -newHealth being the amount of overkill.
		}
	}


	// ***
	// *** Chi System
	// ***


}void SystemApplyQiUtilisation(entt::registry& registry)
{
	// Apply any qi usage to the modifiers.
	auto view = registry.view<ECS::UtiliseQi, ECS::SourceAndTarget>();
	for (auto entity : view)
	{
		// Get the components..
		auto& qi = view.get<ECS::UtiliseQi>(entity);
		auto& sourceAndTarget = view.get<ECS::SourceAndTarget>(entity);
		auto& sourceQi = registry.get<ECS::Qi>(sourceAndTarget.sourceEntity);

		// Get the qi component for the target entity and apply the usage to it's modifier.
		sourceQi.qi.modifiers -= qi.quantity;

		// Remove just the component.
		registry.remove<ECS::UtiliseQi>(entity);
	}
}


void SystemApplyQiUtilisationOverTime(float dt, entt::registry& registry)
{
	// Apply any qi to the modifiers.
	auto view = registry.view<ECS::UtiliseQiOverTime, ECS::SourceAndTarget>();
	for (auto entity : view)
	{
		// Get the components..
		auto& qi = view.get<ECS::UtiliseQiOverTime>(entity);
		auto& sourceAndTarget = view.get<ECS::SourceAndTarget>(entity);

		qi.deltaSinceTick += dt;
		if ((qi.deltaSinceTick >= qi.interval) && (qi.ticksRemaining >= 1.0f))
		{
			qi.ticksRemaining--;
			qi.deltaSinceTick -= qi.interval;

			// Get the qi component for the target entity and apply the usage to it's modifier.
			auto& sourceQi = registry.get<ECS::Qi>(sourceAndTarget.sourceEntity);
			sourceQi.qi.modifiers -= qi.quantity;
		}

		if (qi.ticksRemaining <= 0.0f)
		{
			// Remove just the component.
			registry.remove<ECS::UtiliseQiOverTime>(entity);
		}
	}
}


void SystemApplyQiReplenishment(entt::registry& registry)
{
	// Apply any replenishment to the qi modifiers.
	auto view = registry.view<ECS::ReplenishQi, ECS::SourceAndTarget>();
	for (auto entity : view)
	{
		// Get the components..
		auto& replenish = view.get<ECS::ReplenishQi>(entity);
		auto& sourceAndTarget = view.get<ECS::SourceAndTarget>(entity);
		auto& targetQi = registry.get<ECS::Qi>(sourceAndTarget.targetEntity);

		// Check for over-replenishment.
		float newModifier = targetQi.qi.modifiers + replenish.quantity;
		if (newModifier > 0.0f)
		{
			// It was an over-replenishment.
			targetQi.qi.modifiers = 0.0f;
		}
		else
		{
			targetQi.qi.modifiers = newModifier;
		}

		// Remove just the component.
		registry.remove<ECS::ReplenishQi>(entity);
	}
}


void SystemApplyQiReplenishmentOverTime(float dt, entt::registry& registry)
{
	// Apply any replenishment to the qi modifiers.
	auto view = registry.view<ECS::ReplenishQiOverTime, ECS::SourceAndTarget>();
	for (auto entity : view)
	{
		// Get the components..
		auto& replenish = view.get<ECS::ReplenishQiOverTime>(entity);
		auto& sourceAndTarget = view.get<ECS::SourceAndTarget>(entity);

		replenish.deltaSinceTick += dt;
		if ((replenish.deltaSinceTick >= replenish.interval) && (replenish.ticksRemaining >= 1.0f))
		{
			replenish.ticksRemaining--;
			replenish.deltaSinceTick -= replenish.interval;

			// Get the qi component for the target entity and apply the replenishment to it's modifier.
			auto& targetQi = registry.get<ECS::Qi>(sourceAndTarget.targetEntity);

			// Check for over-replenishment.
			float newModifier = targetQi.qi.modifiers + replenish.quantity;
			if (newModifier > 0.0f)
			{
				// It was an over-replenishment.
				targetQi.qi.modifiers = 0.0f;
			}
			else
			{
				targetQi.qi.modifiers = newModifier;
			}

			if (replenish.ticksRemaining <= 0.0f)
			{
				// Remove just the component.
				registry.remove<ECS::ReplenishQiOverTime>(entity);
			}
		}
	}
}
}
