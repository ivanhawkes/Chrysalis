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
#include <Actor/Animation/Actions/ActorAnimationActionEmote.h>
#include <Actor/Animation/ActorAnimation.h>
#include <CryAnimation/ICryMannequin.h>



namespace Chrysalis::ECS
{

// ***
// *** Timers
// ***


void SystemUpdateAllTimers(float dt, entt::registry& spellcastingRegistry)
{
	SystemUpdateTimers(dt, spellcastingRegistry);
	SystemUpdateDurations(dt, spellcastingRegistry);
	SystemUpdateDelays(dt, spellcastingRegistry);
}


void SystemUpdateTimers(float dt, entt::registry& spellcastingRegistry)
{
	spellcastingRegistry.view<Timer>().each
	([&spellcastingRegistry, dt](auto entity, auto& timer) {
		timer.value += dt;
		});
}


void SystemUpdateDurations(float dt, entt::registry& spellcastingRegistry)
{
	spellcastingRegistry.view<Duration>().each
	([&spellcastingRegistry, dt](auto entity, auto& duration) {
		duration.value -= dt;
		
		// TODO: Work out how this is meant to work or delete this component.
		});
}


void SystemUpdateDelays(float dt, entt::registry& spellcastingRegistry)
{
	spellcastingRegistry.view<Delay>().each
	([&spellcastingRegistry, dt](auto entity, auto& delay) {
		delay.value -= dt;

		// Remove just the component.
		if (delay.value <= 0.0f)
			spellcastingRegistry.remove<Delay>(entity);
		});
}


// ***
// *** Health System
// ***


void SystemApplyDamage(entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Apply any damage to the damage modifiers.
	spellcastingRegistry.view<Damage, SourceEntity, TargetEntity>(entt::exclude<Delay, TickEachInterval>).each
	([&spellcastingRegistry, &actorRegistry](auto entity, auto& damage, auto& sourceEntity, auto& targetEntity) {
		// Get the health component for the target entity and apply the damage to it's health modifier.
		Health& targetHealth = actorRegistry.get<Health>(targetEntity.targetEntityId);
		targetHealth.health.modifiers -= damage.quantity;

		// Destroy the entity.
		spellcastingRegistry.destroy(entity);
		});
}


void SystemApplyDamageOverTime(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Apply any damage to the damage modifiers.
	spellcastingRegistry.view<Damage, TickEachInterval, SourceEntity, TargetEntity>().each
	([dt, &spellcastingRegistry, &actorRegistry](auto entity, auto& damage, auto& tickEachInterval, auto& sourceEntity, auto& targetEntity) {
		tickEachInterval.deltaSinceTick += dt;
		if ((tickEachInterval.deltaSinceTick >= tickEachInterval.interval) && (tickEachInterval.ticksRemaining >= 1.0f))
		{
			tickEachInterval.ticksRemaining--;
			tickEachInterval.deltaSinceTick -= tickEachInterval.interval;

			// Get the health component for the target entity and apply the damage to it's health modifier.
			Health& targetHealth = actorRegistry.get<Health>(targetEntity.targetEntityId);
			targetHealth.health.modifiers -= damage.quantity;
		}

		if (tickEachInterval.ticksRemaining <= 0.0f)
		{
			// Destroy the entity.
			spellcastingRegistry.destroy(entity);
		}
		});
}


void SystemApplyHeal(entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Apply any heals to the health modifiers.
	spellcastingRegistry.view<Heal, SourceEntity, TargetEntity>(entt::exclude<Delay, TickEachInterval>).each
	([&spellcastingRegistry, &actorRegistry](auto entity, auto& heal, auto& sourceEntity, auto& targetEntity) {
		// Get the health component for the target entity and apply the heal to it's health modifier.
		Health& targetHealth = actorRegistry.get<Health>(targetEntity.targetEntityId);

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

		// Destroy the entity.
		spellcastingRegistry.destroy(entity);
		});
}


void SystemApplyHealOverTime(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Apply any heal to the health modifiers.
	spellcastingRegistry.view<Heal, TickEachInterval, SourceEntity, TargetEntity>().each
	([dt, &spellcastingRegistry, &actorRegistry](auto entity, auto& heal, auto& tickEachInterval, auto& sourceEntity, auto& targetEntity) {
		tickEachInterval.deltaSinceTick += dt;
		if ((tickEachInterval.deltaSinceTick >= tickEachInterval.interval) && (tickEachInterval.ticksRemaining >= 1.0f))
		{
			tickEachInterval.ticksRemaining--;
			tickEachInterval.deltaSinceTick -= tickEachInterval.interval;

			// Get the health component for the target entity and apply the heal to it's health modifier.
			Health& targetHealth = actorRegistry.get<Health>(targetEntity.targetEntityId);

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

			if (tickEachInterval.ticksRemaining <= 0.0f)
			{
				// Destroy the entity.
				spellcastingRegistry.destroy(entity);
			}
		}
		});
}


void SystemHealthCheck(entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Update each health component, applying the modifier to it's base to calculate the current health.
	// Update death status if appropriate.
	spellcastingRegistry.view<Health, SourceEntity, TargetEntity>().each
	([&spellcastingRegistry, &actorRegistry](auto entity, auto& health, auto& sourceEntity, auto& targetEntity) {
		// Check for overkill.
		float newHealth = health.health.GetBaseAttribute() + health.health.modifiers;
		if (newHealth <= 0.0f)
		{
			health.isDead = true;

			// TODO: Inform them they died with -newHealth being the amount of overkill.
		}
		});
}


// ***
// *** Qi System
// ***


void SystemApplyQiUtilisation(entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Apply any qi usage to the modifiers.
	spellcastingRegistry.view<UtiliseQi, SourceEntity, TargetEntity>(entt::exclude<Delay, TickEachInterval>).each
	([&spellcastingRegistry, &actorRegistry](auto entity, auto& qiUse, auto& sourceEntity, auto& targetEntity) {
		// Get the qi component for the target entity and apply the usage to it's modifier.
		Qi& targetQi = actorRegistry.get<Qi>(targetEntity.targetEntityId);
		targetQi.qi.modifiers -= qiUse.quantity;

		// Destroy the entity.
		spellcastingRegistry.destroy(entity);
		});
}


void SystemApplyQiUtilisationOverTime(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Apply any qi to the modifiers.
	spellcastingRegistry.view<UtiliseQi, TickEachInterval, SourceEntity, TargetEntity>().each
	([dt, &spellcastingRegistry, &actorRegistry](auto entity, auto& utiliseQi, auto& tickEachInterval, auto& sourceEntity, auto& targetEntity) {
		tickEachInterval.deltaSinceTick += dt;
		if ((tickEachInterval.deltaSinceTick >= tickEachInterval.interval) && (tickEachInterval.ticksRemaining >= 1.0f))
		{
			tickEachInterval.ticksRemaining--;
			tickEachInterval.deltaSinceTick -= tickEachInterval.interval;

			// Get the qi component for the target entity and apply the usage to it's modifier.
			Qi& targetQi = actorRegistry.get<Qi>(targetEntity.targetEntityId);
			targetQi.qi.modifiers -= utiliseQi.quantity;
		}

		if (tickEachInterval.ticksRemaining <= 0.0f)
		{
			// Destroy the entity.
			spellcastingRegistry.destroy(entity);
		}
		});
}


void SystemApplyQiReplenishment(entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Apply any replenishment to the qi modifiers.
	spellcastingRegistry.view<ReplenishQi, SourceEntity, TargetEntity>(entt::exclude<Delay, TickEachInterval>).each
	([&spellcastingRegistry, &actorRegistry](auto entity, auto& replenish, auto& sourceEntity, auto& targetEntity) {
		// Get the qi component for the target entity and apply the replenishment to it's modifier.
		Qi& targetQi = actorRegistry.get<Qi>(targetEntity.targetEntityId);

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

		// Destroy the entity.
		spellcastingRegistry.destroy(entity);
		});
}


void SystemApplyQiReplenishmentOverTime(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Apply any replenishment to the qi modifiers.
	spellcastingRegistry.view<ReplenishQi, TickEachInterval, SourceEntity, TargetEntity>().each
	([dt, &spellcastingRegistry, &actorRegistry](auto entity, auto& replenishQi, auto& tickEachInterval, auto& sourceEntity, auto& targetEntity) {
		tickEachInterval.deltaSinceTick += dt;
		if ((tickEachInterval.deltaSinceTick >= tickEachInterval.interval) && (tickEachInterval.ticksRemaining >= 1.0f))
		{
			tickEachInterval.ticksRemaining--;
			tickEachInterval.deltaSinceTick -= tickEachInterval.interval;

			// Get the qi component for the target entity and apply the replenishment to it's modifier.
			Qi& targetQi = actorRegistry.get<Qi>(targetEntity.targetEntityId);

			// Check for over-replenishment.
			float newModifier = targetQi.qi.modifiers + replenishQi.quantity;
			if (newModifier > 0.0f)
			{
				// It was an over-replenishment.
				targetQi.qi.modifiers = 0.0f;
			}
			else
			{
				targetQi.qi.modifiers = newModifier;
			}

			if (tickEachInterval.ticksRemaining <= 0.0f)
			{
				// Destroy the entity.
				spellcastingRegistry.destroy(entity);
			}
		}
		});
}


// ***
// *** World Spellcasts
// ***


bool IsSpellCastable(const SpellFragment& spell, const SourceEntity& sourceEntity, const TargetEntity& targetEntity)
{
	// TODO: Range and LoS checks, etc.

	return true;
}


void SpellCastOpen(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Query the registry.
	spellcastingRegistry.view<SpellActionOpen, Name, SpellFragment, SpellcastExecution, SourceEntity, TargetEntity>().each
	([dt, &spellcastingRegistry, &actorRegistry](auto entity, auto& spellActionOpen, auto& name, auto& spell, auto& spellcastExecution, auto& sourceEntity, auto& targetEntity) {
		// Check validity of the spell cast request.
		if (IsSpellCastable(spell, sourceEntity, targetEntity))
		{
			switch (spellcastExecution.castExecutionStatus)
			{
				case SpellCastExecutionStatus::queued:
					// Move it from queued casting to casting.
					spellcastExecution.castExecutionStatus = SpellCastExecutionStatus::casting;
					CryLogAlways("Starting: %s, Source: %d, target: %d", name.displayName.c_str(), sourceEntity.sourceEntityId, targetEntity.targetEntityId);
					break;

				case SpellCastExecutionStatus::casting:
					spellcastExecution.executionTime += dt;
					break;
			}

			// Does the source actor have a spellbook for this cast?
			if (auto* pSourceEntity = gEnv->pEntitySystem->GetEntity(sourceEntity.crySourceEntityId))
			{
				if (auto* pSpellbookComponent = pSourceEntity->GetComponent<CSpellbookComponent>())
				{
					// Toss the spell onto the source entity's queue.
					auto spellcast = std::make_shared<Chrysalis::SpellCastOpen>(name, spell, sourceEntity, targetEntity);
					pSpellbookComponent->QueueSpellCast(spellcast);
				}
			}
		}

//		if (spell.castDuration <= spellcastExecution.executionTime)
		{
			// TODO: The spell should cast now if it's not immediate cast.

			// Assumption, it succeeded, yay team!
			spellcastExecution.castExecutionStatus = SpellCastExecutionStatus::success;
		}

		// HACK: Destroy the entity. Assumption is each entity only has one of these sorts of spell components on it.
		// Doing this here for convenience for now.
		if (spellcastExecution.castExecutionStatus == SpellCastExecutionStatus::success)
		{
			CryLogAlways("Spellcast Finished: %s, Source: %d, target: %d", name.displayName.c_str(), sourceEntity.sourceEntityId, targetEntity.targetEntityId);
			spellcastingRegistry.destroy(entity);
		}
		});
}


void SpellCastTake(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Query the registry.
	spellcastingRegistry.view<SpellActionTake, Name, SpellFragment, SpellcastExecution, SourceEntity, TargetEntity>().each
	([dt, &spellcastingRegistry, &actorRegistry](auto entity, auto& spellActionTake, auto& name, auto& spell, auto& spellcastExecution, auto& sourceEntity, auto& targetEntity) {
		// Check validity of the spell cast request.
		if (IsSpellCastable(spell, sourceEntity, targetEntity))
		{
			// Do something.
			CryLogAlways("Spellcast: %s, Source: %d, target: %d", name.displayName.c_str(), sourceEntity.sourceEntityId, targetEntity.targetEntityId);
		}

		// Destroy the entity. Assumption is each entity only has one of these sorts of spell components on it. 
		spellcastingRegistry.destroy(entity);
		});
}


void SpellCastDrop(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Query the registry.
	spellcastingRegistry.view<SpellActionDrop, Name, SpellFragment, SpellcastExecution, SourceEntity, TargetEntity>().each
	([dt, &spellcastingRegistry, &actorRegistry](auto entity, auto& spellActionDrop, auto& name, auto& spell, auto& spellcastExecution, auto& sourceEntity, auto& targetEntity) {
		// Check validity of the spell cast request.
		if (IsSpellCastable(spell, sourceEntity, targetEntity))
		{
			// Do something.
			CryLogAlways("Spellcast: %s, Source: %d, target: %d", name.displayName.c_str(), sourceEntity.sourceEntityId, targetEntity.targetEntityId);
		}

		// Destroy the entity. Assumption is each entity only has one of these sorts of spell components on it. 
		spellcastingRegistry.destroy(entity);
		});
}


void SpellCastSwitch(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Query the registry.
	spellcastingRegistry.view<SpellActionSwitch, Name, SpellFragment, SpellcastExecution, SourceEntity, TargetEntity>().each
	([dt, &spellcastingRegistry, &actorRegistry](auto entity, auto& spellActionSwitch, auto& name, auto& spell, auto& spellcastExecution, auto& sourceEntity, auto& targetEntity) {
		// Check validity of the spell cast request.
		if (IsSpellCastable(spell, sourceEntity, targetEntity))
		{
			// Do something.
			CryLogAlways("Spellcast: %s, Source: %d, Target: %d", name.displayName.c_str(), sourceEntity.sourceEntityId, targetEntity.targetEntityId);
			CryLogAlways("SourceId: %d, TargetId: %d", sourceEntity.crySourceEntityId, targetEntity.cryTargetEntityId);

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

			if (auto* pSourceEntity = gEnv->pEntitySystem->GetEntity(sourceEntity.crySourceEntityId))
			{
				// Animation is handled by this component for most things. If it exists we can use it to find the animation control we need.
				if (auto* pActorAnimationComponent = pSourceEntity->GetComponent<CActorAnimationComponent>())
				{
					if (IActor* pActor = pSourceEntity->GetComponent<CActorComponent>())
					{
						// We prefer to place the actor into a co-operative animation if possible.
						auto action = new CActorAnimationActionCooperative(*pActor,
							pActorAnimationComponent,
							targetEntity.cryTargetEntityId,
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
		spellcastingRegistry.destroy(entity);
		});
}


void SpellCastAnimationFragmentEmote(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	// Query the registry.
	spellcastingRegistry.view<AnimationFragmentEmote, AnimationTag, TargetEntity>().each
	([dt, &spellcastingRegistry, &actorRegistry](auto entity, auto& animationFragmentEmote, auto& animationTag, auto& targetEntity) {
		if (auto pTargetEntity = gEnv->pEntitySystem->GetEntity(targetEntity.cryTargetEntityId))
		{
			if (auto pActorComponent = pTargetEntity->GetComponent<CActorComponent>())
			{
				// Queue an action and provide a tag for the specific emotion.
				auto emoteAction = new CActorAnimationActionEmote(string(animationTag.value).MakeLower());
				pActorComponent->QueueAction(*emoteAction);
			}
		}

		// Destroy the entity. Assumption is each entity only has one of these sorts of spell components on it. 
		spellcastingRegistry.destroy(entity);
		});
}


void SystemWorldSpellCasts(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry)
{
	SpellCastOpen(dt, spellcastingRegistry, actorRegistry);
	SpellCastTake(dt, spellcastingRegistry, actorRegistry);
	SpellCastDrop(dt, spellcastingRegistry, actorRegistry);
	SpellCastSwitch(dt, spellcastingRegistry, actorRegistry);
	SpellCastAnimationFragmentEmote(dt, spellcastingRegistry, actorRegistry);
}


void SystemUpdateActors(float dt, entt::registry& actorRegistry)
{
	// Allow the qi to regenerate naturally.
	actorRegistry.view<Qi>().each
	([dt, &actorRegistry](auto entity, auto& qi) {
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
		});
}
}
