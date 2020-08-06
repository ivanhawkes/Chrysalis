#include <StdAfx.h>

#include "Systems.h"
#include "ECS/Components/Health.h"
#include "ECS/Components/Inventory.h"
#include "ECS/Components/Items.h"
#include "ECS/Components/Qi.h"

#include <Actor/Animation/Actions/ActorAnimationActionCooperative.h>
#include <Components/Animation/ActorAnimationComponent.h>
#include <Components/Actor/ActorComponent.h>
#include <Components/Spells/SpellbookComponent.h>


namespace Chrysalis::ECS
{
// ***
// *** Health System
// ***


void SystemApplyDamage(entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Apply any damage to the damage modifiers.
	auto view = spellRegistry.view<Damage, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components.
		auto& damage = view.get<Damage>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		// Get the health component for the target entity and apply the damage to it's health modifier.
		Health* targetHealth {nullptr};
		if (damage.targetTargetType == TargetTargetType::target)
		{
			auto& health = actorRegistry.get<Health>(sourceAndTarget.targetEntity);
			targetHealth = &health;
		}
		else
		{
			auto& health = actorRegistry.get<Health>(sourceAndTarget.sourceEntity);
			targetHealth = &health;
		}

		targetHealth->health.modifiers -= damage.quantity;

		// Remove just the component.
		spellRegistry.remove<Damage>(entity);
	}
}


void SystemApplyDamageOverTime(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Apply any damage to the damage modifiers.
	auto view = spellRegistry.view<DamageOverTime, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components..
		auto& damage = view.get<DamageOverTime>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		damage.deltaSinceTick += dt;
		if ((damage.deltaSinceTick >= damage.interval) && (damage.ticksRemaining >= 1.0f))
		{
			damage.ticksRemaining--;
			damage.deltaSinceTick -= damage.interval;

			// Get the health component for the target entity and apply the damage to it's health modifier.
			Health* targetHealth {nullptr};
			if (damage.targetTargetType == TargetTargetType::target)
			{
				auto& health = actorRegistry.get<Health>(sourceAndTarget.targetEntity);
				targetHealth = &health;
			}
			else
			{
				auto& health = actorRegistry.get<Health>(sourceAndTarget.sourceEntity);
				targetHealth = &health;
			}
			targetHealth->health.modifiers -= damage.quantity;
		}

		if (damage.ticksRemaining <= 0.0f)
		{
			// Remove just the component.
			spellRegistry.remove<DamageOverTime>(entity);
		}
	}
}


void SystemApplyHeal(entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Apply any heals to the health modifiers.
	auto view = spellRegistry.view<Heal, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components..
		auto& heal = view.get<Heal>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		// Get the health component for the target entity and apply the heal to it's health modifier.
		Health* targetHealth {nullptr};
		if (heal.targetTargetType == TargetTargetType::target)
		{
			auto& health = actorRegistry.get<Health>(sourceAndTarget.targetEntity);
			targetHealth = &health;
		}
		else
		{
			auto& health = actorRegistry.get<Health>(sourceAndTarget.sourceEntity);
			targetHealth = &health;
		}

		// Check for overheals.
		float newModifier = targetHealth->health.modifiers + heal.quantity;
		if (newModifier > 0.0f)
		{
			// It was an overheal.
			targetHealth->health.modifiers = 0.0f;
		}
		else
		{
			targetHealth->health.modifiers = newModifier;
		}

		// Remove just the component.
		spellRegistry.remove<Heal>(entity);
	}
}


void SystemApplyHealOverTime(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Apply any heal to the health modifiers.
	auto view = spellRegistry.view<HealOverTime, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components..
		auto& heal = view.get<HealOverTime>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		heal.deltaSinceTick += dt;
		if ((heal.deltaSinceTick >= heal.interval) && (heal.ticksRemaining >= 1.0f))
		{
			heal.ticksRemaining--;
			heal.deltaSinceTick -= heal.interval;

			// Get the health component for the target entity and apply the heal to it's health modifier.
			Health* targetHealth {nullptr};
			if (heal.targetTargetType == TargetTargetType::target)
			{
				auto& health = actorRegistry.get<Health>(sourceAndTarget.targetEntity);
				targetHealth = &health;
			}
			else
			{
				auto& health = actorRegistry.get<Health>(sourceAndTarget.sourceEntity);
				targetHealth = &health;
			}


			// Check for overheals.
			float newModifier = targetHealth->health.modifiers + heal.quantity;
			if (newModifier > 0.0f)
			{
				// It was an overheal.
				targetHealth->health.modifiers = 0.0f;
			}
			else
			{
				targetHealth->health.modifiers = newModifier;
			}

			if (heal.ticksRemaining <= 0.0f)
			{
				// Remove just the component.
				spellRegistry.remove<HealOverTime>(entity);
			}
		}
	}
}


void SystemHealthCheck(entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Update each health component, applying the modifier to it's base to calculate the current health.
	// Update death status if appropriate.
	auto view = actorRegistry.view<Health, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components..
		auto& health = view.get<Health>(entity);

		// Check for overkill.
		float newHealth = health.health.GetBaseAttribute() + health.health.modifiers;
		if (newHealth <= 0.0f)
		{
			health.isDead = true;

			// TODO: Inform them they died with -newHealth being the amount of overkill.
		}
	}
}


// ***
// *** Qi System
// ***


void SystemApplyQiUtilisation(entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Apply any qi usage to the modifiers.
	auto view = spellRegistry.view<UtiliseQi, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components..
		auto& qiUse = view.get<UtiliseQi>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		// Get the qi component for the target entity and apply the usage to it's modifier.
		Qi* targetQi {nullptr};
		if (qiUse.targetTargetType == TargetTargetType::target)
		{
			auto& qi = actorRegistry.get<Qi>(sourceAndTarget.targetEntity);
			targetQi = &qi;
		}
		else
		{
			auto& qi = actorRegistry.get<Qi>(sourceAndTarget.sourceEntity);
			targetQi = &qi;
		}

		// Get the qi component for the target entity and apply the usage to it's modifier.
		targetQi->qi.modifiers -= qiUse.quantity;

		// Remove just the component.
		spellRegistry.remove<UtiliseQi>(entity);
	}
}


void SystemApplyQiUtilisationOverTime(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Apply any qi to the modifiers.
	auto view = spellRegistry.view<UtiliseQiOverTime, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components..
		auto& qiUse = view.get<UtiliseQiOverTime>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		qiUse.deltaSinceTick += dt;
		if ((qiUse.deltaSinceTick >= qiUse.interval) && (qiUse.ticksRemaining >= 1.0f))
		{
			qiUse.ticksRemaining--;
			qiUse.deltaSinceTick -= qiUse.interval;

			// Get the qi component for the target entity and apply the usage to it's modifier.
			Qi* targetQi {nullptr};
			if (qiUse.targetTargetType == TargetTargetType::target)
			{
				auto& qi = actorRegistry.get<Qi>(sourceAndTarget.targetEntity);
				targetQi = &qi;
			}
			else
			{
				auto& qi = actorRegistry.get<Qi>(sourceAndTarget.sourceEntity);
				targetQi = &qi;
			}
			targetQi->qi.modifiers -= qiUse.quantity;
		}

		if (qiUse.ticksRemaining <= 0.0f)
		{
			// Remove just the component.
			spellRegistry.remove<UtiliseQiOverTime>(entity);
		}
	}
}


void SystemApplyQiReplenishment(entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Apply any replenishment to the qi modifiers.
	auto view = spellRegistry.view<ReplenishQi, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components..
		auto& replenish = view.get<ReplenishQi>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		// Get the qi component for the target entity and apply the replenishment to it's modifier.
		Qi* targetQi {nullptr};
		if (replenish.targetTargetType == TargetTargetType::target)
		{
			auto& qi = actorRegistry.get<Qi>(sourceAndTarget.targetEntity);
			targetQi = &qi;
		}
		else
		{
			auto& qi = actorRegistry.get<Qi>(sourceAndTarget.sourceEntity);
			targetQi = &qi;
		}

		// Check for over-replenishment.
		float newModifier = targetQi->qi.modifiers + replenish.quantity;
		if (newModifier > 0.0f)
		{
			// It was an over-replenishment.
			targetQi->qi.modifiers = 0.0f;
		}
		else
		{
			targetQi->qi.modifiers = newModifier;
		}

		// Remove just the component.
		spellRegistry.remove<ReplenishQi>(entity);
	}
}


void SystemApplyQiReplenishmentOverTime(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Apply any replenishment to the qi modifiers.
	auto view = spellRegistry.view<ReplenishQiOverTime, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components..
		auto& replenish = view.get<ReplenishQiOverTime>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		replenish.deltaSinceTick += dt;
		if ((replenish.deltaSinceTick >= replenish.interval) && (replenish.ticksRemaining >= 1.0f))
		{
			replenish.ticksRemaining--;
			replenish.deltaSinceTick -= replenish.interval;

			// Get the qi component for the target entity and apply the replenishment to it's modifier.
			Qi* targetQi {nullptr};
			if (replenish.targetTargetType == TargetTargetType::target)
			{
				auto& qi = actorRegistry.get<Qi>(sourceAndTarget.targetEntity);
				targetQi = &qi;
			}
			else
			{
				auto& qi = actorRegistry.get<Qi>(sourceAndTarget.sourceEntity);
				targetQi = &qi;
			}

			// Check for over-replenishment.
			float newModifier = targetQi->qi.modifiers + replenish.quantity;
			if (newModifier > 0.0f)
			{
				// It was an over-replenishment.
				targetQi->qi.modifiers = 0.0f;
			}
			else
			{
				targetQi->qi.modifiers = newModifier;
			}

			if (replenish.ticksRemaining <= 0.0f)
			{
				// Remove just the component.
				spellRegistry.remove<ReplenishQiOverTime>(entity);
			}
		}
	}
}


// ***
// *** World Spellcasts
// ***


bool IsSpellCastable(const Spell& spell, const SourceAndTarget& sourceAndTarget)
{
	// TODO: Range and LoS checks, etc.

	return true;
}


void SpellCastOpen(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Check for spell cast components.
	auto view = spellRegistry.view<SpellActionOpen, Name, Spell, SpellcastExecution, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components.
		auto& name = view.get<Name>(entity);
		auto& spell = view.get<Spell>(entity);
		auto& spellcastExecution = view.get<SpellcastExecution>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		// Check validity of the spell cast request.
		if (IsSpellCastable(spell, sourceAndTarget))
		{
			switch (spellcastExecution.castExecutionStatus)
			{
				case SpellCastExecutionStatus::initialised:
					// The spell is now considered to be in a casting state.
					spellcastExecution.castExecutionStatus = SpellCastExecutionStatus::casting;
					CryLogAlways("Spellcast: %s, Source: %d, target: %d", name.displayName.c_str(), sourceAndTarget.sourceEntity, sourceAndTarget.targetEntity);
					break;

				case SpellCastExecutionStatus::casting:
					spellcastExecution.executionTime += dt;
					break;
			}

			// Does the source actor have a spellbook for this cast?
			if (auto* pSourceEntity = gEnv->pEntitySystem->GetEntity(sourceAndTarget.crySourceEntityId))
			{
				if (auto* pSpellbookComponent = pSourceEntity->GetComponent<CSpellbookComponent>())
				{
					// Toss the spell onto the source entity's queue.
					auto spellcast = std::make_shared<Chrysalis::SpellCastOpen>(name, spell, sourceAndTarget);
					pSpellbookComponent->QueueSpellCast(spellcast);
				}
			}
		}

		if (spell.castDuration <= spellcastExecution.executionTime)
		{
			// TODO: The spell should cast now if it's not immediate cast.

			// Assumption, it succeeded, yay team!
			spellcastExecution.castExecutionStatus = SpellCastExecutionStatus::success;
		}

		// HACK: Destroy the entity. Assumption is each entity only has one of these sorts of spell components on it.
		// Doing this here for convenience for now.
		if (spellcastExecution.castExecutionStatus == SpellCastExecutionStatus::success)
		{
			CryLogAlways("Spellcast Finished: %s, Source: %d, target: %d", name.displayName.c_str(), sourceAndTarget.sourceEntity, sourceAndTarget.targetEntity);
			spellRegistry.destroy(entity);
		}
	}
}


void SpellCastTake(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Check for spell cast components.
	auto view = spellRegistry.view<SpellActionTake, Name, Spell, SpellcastExecution, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components.
		auto& name = view.get<Name>(entity);
		auto& spell = view.get<Spell>(entity);
		//auto& spellcastExecution = view.get<SpellcastExecution>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		// Check validity of the spell cast request.
		if (IsSpellCastable(spell, sourceAndTarget))
		{
			// Do something.
			CryLogAlways("Spellcast: %s, Source: %d, target: %d", name.displayName.c_str(), sourceAndTarget.sourceEntity, sourceAndTarget.targetEntity);
		}

		// Destroy the entity. Assumption is each entity only has one of these sorts of spell components on it. 
		spellRegistry.destroy(entity);
	}
}


void SpellCastDrop(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Check for spell cast components.
	auto view = spellRegistry.view<SpellActionDrop, Name, Spell, SpellcastExecution, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components.
		auto& name = view.get<Name>(entity);
		auto& spell = view.get<Spell>(entity);
		//auto& spellcastExecution = view.get<SpellcastExecution>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		// Check validity of the spell cast request.
		if (IsSpellCastable(spell, sourceAndTarget))
		{
			// Do something.
			CryLogAlways("Spellcast: %s, Source: %d, target: %d", name.displayName.c_str(), sourceAndTarget.sourceEntity, sourceAndTarget.targetEntity);
		}

		// Destroy the entity. Assumption is each entity only has one of these sorts of spell components on it. 
		spellRegistry.destroy(entity);
	}
}


void SpellCastSwitch(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	// Check for spell cast components.
	auto view = spellRegistry.view<SpellActionSwitch, Name, Spell, SpellcastExecution, SourceAndTarget>();
	for (auto& entity : view)
	{
		// Get the components.
		auto& name = view.get<Name>(entity);
		auto& spell = view.get<Spell>(entity);
		//auto& spellcastExecution = view.get<SpellcastExecution>(entity);
		auto& sourceAndTarget = view.get<SourceAndTarget>(entity);

		// Check validity of the spell cast request.
		if (IsSpellCastable(spell, sourceAndTarget))
		{
			// Do something.
			CryLogAlways("Spellcast: %s, Source: %d, Target: %d", name.displayName.c_str(), sourceAndTarget.sourceEntity, sourceAndTarget.targetEntity);
			CryLogAlways("SourceId: %d, TargetId: %d", sourceAndTarget.crySourceEntityId, sourceAndTarget.cryTargetEntityId);


			// TODO: All this really belongs somewhere else more related to what it's doing so it doesn't pollute the
			// clean code for systems.


			//m_pInteractionActor = &actor;
			//m_interaction = &pInteraction;

			//// Inform the actor we are taking control of an interaction.
			//m_pInteractionActor->InteractionStart(m_interaction);

			// HACK: We need to convert the tags from a Schematyc format to a more general one. In 5.5 / 5.6 we should be able
			// to use the generic containers instead.
			std::vector<string> tags;
			//for (int i = 0; i < m_tags.Size(); i++)
			//{
			//	tags.push_back(m_tags.At(i).tag.c_str());
			//}

			if (auto* pSourceEntity = gEnv->pEntitySystem->GetEntity(sourceAndTarget.crySourceEntityId))
			{
				// Animation is handled by this component for most things. If it exists we can use it to find the animation control we need.
				if (auto* pActorAnimationComponent = pSourceEntity->GetComponent<CActorAnimationComponent>())
				{
					if (IActor* pActor = pSourceEntity->GetComponent<CActorComponent>())
					{
						// We prefer to place the actor into a co-operative animation if possible.
						auto action = new CActorAnimationActionCooperative(*pActor,
							pActorAnimationComponent,
							sourceAndTarget.cryTargetEntityId,
							pActor->GetMannequinParams()->fragmentIDs.Interaction, TagState {TAG_STATE_EMPTY}, pActor->GetMannequinParams()->tagIDs.ScopeSlave, tags);
						//action->AddEventListener(this);

						pActor->QueueAction(*action);

						//// Disable after a single use.
						//if (m_isSingleUseOnly)
						//	m_isEnabled = false;
					}
				}
			}
		}

		// Destroy the entity. Assumption is each entity only has one of these sorts of spell components on it. 
		spellRegistry.destroy(entity);
	}
}


void SystemWorldSpellCasts(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry)
{
	SpellCastOpen(dt, spellRegistry, actorRegistry);
	SpellCastTake(dt, spellRegistry, actorRegistry);
	SpellCastDrop(dt, spellRegistry, actorRegistry);
	SpellCastSwitch(dt, spellRegistry, actorRegistry);
}


void SystemUpdateActors(float dt, entt::registry& actorRegistry)
{
	// Allow the qi to regenerate naturally.
	auto view = actorRegistry.view<Qi>();
	for (auto& entity : view)
	{
		// Get the components..
		auto& qi = view.get<Qi>(entity);

		// Accumulate the time since the last spell cast.
		qi.timeSinceLastSpellcast += dt;

		// Give them a 5 second timeout after casting a spell before they can regenerate.
		if (qi.timeSinceLastSpellcast >= 5.0f)
		{
			// Check for over-replenishment.
			float newModifier = qi.qi.modifiers + (qi.qi.base * qi.qiRegenerationPerSecond * dt);
			if (newModifier > 0.0f)
			{
				// It was an over-replenishment.
				qi.qi.modifiers = 0.0f;
			}
			else
			{
				qi.qi.modifiers = newModifier;
			}
		}
	}
}
}
