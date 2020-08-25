#include <StdAfx.h>

#include "Simulation.h"
#include "Systems.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Health.h"
#include "ECS/Components/Inventory.h"
#include "ECS/Components/Items.h"
#include "ECS/Components/Qi.h"
#include "ECS/Components/RenderLight.h"
#include "ECS/Components/Spells/Spell.h"
#include "ECS/Components/Spells/SpellActions.h"
#include "ECS/Systems/Systems.h"
#include "ECS/Systems/XMLSerializer.h"
#include <entt/entt.hpp>
#include "Crymath/Random.h"


namespace Chrysalis::ECS
{
/** Takes a reference to a spell and applies the needed fixups. This is mainly going to fix up the source and targets
	for now. */
void CSimulation::RewireSpell(entt::registry& spellcastingRegistry, entt::entity spellEntity, entt::entity sourceEntity, entt::entity targetEntity,
	EntityId crySourceEntityId, EntityId cryTargetEntityId)
{
	SpellFragment& spellFragment = spellcastingRegistry.get<SpellFragment>(spellEntity);

	entt::entity source {entt::null};
	entt::entity target {entt::null};
	EntityId sourceEntityId {INVALID_ENTITYID};
	EntityId targetEntityId {INVALID_ENTITYID};

	// The source should almost always be the real source of the spell.
	if (spellFragment.targetType != TargetType::none)
	{
		source = sourceEntity;
		sourceEntityId = crySourceEntityId;
	}

	// The target should be the target usually, unless there is no direct target.
	switch (spellFragment.targetType)
	{
		// Targetting the caster.
		case TargetType::self:
			target = sourceEntity;
			targetEntityId = crySourceEntityId;
			break;

			// Not targetted at an entity.
		case TargetType::none:
		case TargetType::cone:
		case TargetType::column:
		case TargetType::sourceBasedAOE:
		case TargetType::groundTargettedAOE:
			target = entt::null;
			targetEntityId = INVALID_ENTITYID;
			break;

			// Targetting the selected entity.
		default:
			target = targetEntity;
			targetEntityId = cryTargetEntityId;
			break;
	}

	// The source and target for the spell need to be added to the entity.
	spellcastingRegistry.emplace<SourceEntity>(spellEntity, source, sourceEntityId);
	spellcastingRegistry.emplace<TargetEntity>(spellEntity, target, targetEntityId);
}


/** Super dirty and slow way to locate a spell from the registry. */
entt::entity CSimulation::GetSpellByName(const char* spellName)
{
	auto view = m_spellRegistry.view<Name>();

	for (auto& entity : view)
	{
		auto& name = view.get<Name>(entity);

		if (strcmp(name.name, spellName) == 0)
		{
			return entity;
		}
	}

	// Failed to find it.
	return entt::null;
}


/** Queues a spell onto the spellcasting registry - where it will later be processed by the systems. */
void CSimulation::CastSpellByName(const char* spellName, entt::entity sourceEntity, entt::entity targetEntity,
	EntityId crySourceEntityId, EntityId cryTargetEntityId)
{
	auto spellEntity = GetSpellByName(spellName);
	if (spellEntity != entt::null)
	{
		auto newEntity = m_spellcastingRegistry.create();

		m_spellRegistry.visit(spellEntity, [this, spellEntity, newEntity](const auto type_id)
			{m_functionDispatchMap[type_id].stampFunction(m_spellRegistry, spellEntity, m_spellcastingRegistry, newEntity); });

		// Do fixups.
		RewireSpell(m_spellcastingRegistry, newEntity, sourceEntity, targetEntity, crySourceEntityId, cryTargetEntityId);

		// We supply them an execution context.
		m_spellcastingRegistry.emplace<SpellcastExecution>(newEntity);

		// Adjust their qi cast timer.
		auto& qi = m_actorRegistry.get<Qi>(sourceEntity);
		qi.timeSinceLastSpellcast = 0.0f;
	}
}


const entt::entity GetHero(entt::registry& registry)
{
	auto view = registry.view<Name>();

	for (auto& entity : view)
	{
		auto& name = view.get<Name>(entity);

		if (strcmp(name.name, "Hero") == 0)
		{
			return entity;
		}
	}

	// Failed to find it.
	return entt::null;
}


const entt::entity GetVillain(entt::registry& registry)
{
	auto view = registry.view<Name>();

	for (auto& entity : view)
	{
		auto& name = view.get<Name>(entity);

		if (strcmp(name.name, "Villain") == 0)
		{
			return entity;
		}
	}

	// Failed to find it.
	return entt::null;
}


void CSimulation::Init()
{
	RegisterComponentsWithMeta();
}


void CSimulation::Update(const float deltaTime)
{
	// Run the ticks no more often than this interval.
	static const float tickInterval {0.5f};

	// Track the amount of time that has gone since we last run the tick code.
	static float passedTime {0.0f};

	// Update the things which should be handled immediately e.g direct damage and heals.
	UpdateImmediate(deltaTime);

	// Check if we need to tick.
	passedTime += deltaTime;
	if (passedTime >= tickInterval)
	{
		// Perform a tick.
		// HACK: NOTE: This is just an approximation of how much time has passed. It will always be out by almost a frame's
		// worth of time. For now, it appears better to have the tick nice and steady, even if it lags behind reality a bit.
		UpdateTick(tickInterval);

		// HACK: We decrement by the interval size, so it will catch up if we miss some frames.
		passedTime -= tickInterval;
	}

	// Update the spell casts.
	UpdateWorldSpellcasts(deltaTime);
}


void CSimulation::UpdateImmediate(const float deltaTime)
{
	// Simluate some direct heals and direct damage.
	SystemApplyDamage(m_spellcastingRegistry, m_actorRegistry);
	SystemApplyHeal(m_spellcastingRegistry, m_actorRegistry);
	SystemHealthCheck(m_spellcastingRegistry, m_actorRegistry);

	// Simluate some direct qi use and replenishment.
	SystemApplyQiUtilisation(m_spellcastingRegistry, m_actorRegistry);
	SystemApplyQiReplenishment(m_spellcastingRegistry, m_actorRegistry);
}


void CSimulation::UpdateTick(const float deltaTime)
{
	// Health ticks.
	SystemApplyDamageOverTime(deltaTime, m_spellcastingRegistry, m_actorRegistry);
	SystemApplyHealOverTime(deltaTime, m_spellcastingRegistry, m_actorRegistry);
	SystemHealthCheck(m_spellcastingRegistry, m_actorRegistry);

	// Qi ticks.
	SystemApplyQiUtilisationOverTime(deltaTime, m_spellcastingRegistry, m_actorRegistry);
	SystemApplyQiReplenishmentOverTime(deltaTime, m_spellcastingRegistry, m_actorRegistry);

	// Update the actors qi, health, whatever.
	UpdateActors(deltaTime);
}


void CSimulation::UpdateWorldSpellcasts(const float deltaTime)
{
	SystemWorldSpellCasts(deltaTime, m_spellcastingRegistry, m_actorRegistry);
}


void CSimulation::UpdateActors(const float deltaTime)
{
	SystemUpdateActors(deltaTime, m_actorRegistry);
}


void CSimulation::LoadActorData()
{
	// Actor related.
	m_actorRegistry.clear();
	SerialiseECSInput actorSerial;
	actorSerial.LoadFromFile("chrysalis/parameters/actor/actor.xml");
	entt::snapshot_loader {m_actorRegistry}
		.entities(actorSerial)
		.component<Name, Prototype,
		Health, Damage, DamageOverTime, Heal, HealOverTime,
		Qi, UtiliseQi, UtiliseQiOverTime, ReplenishQi, ReplenishQiOverTime,
		SpellFragment, ItemClass, RenderLight
		>(actorSerial);
}


void CSimulation::SaveActorData()
{
	// Actor related.
	SerialiseECSOutput actorSerial;

	entt::snapshot {m_actorRegistry}
		.entities(actorSerial)
		.component<Name, Prototype,
		Health, Damage, DamageOverTime, Heal, HealOverTime,
		Qi, UtiliseQi, UtiliseQiOverTime, ReplenishQi, ReplenishQiOverTime,
		SpellFragment, ItemClass, RenderLight
		>(actorSerial);
	actorSerial.SaveToFile("chrysalis/parameters/actor/actor-snapshot.xml");
}


void CSimulation::LoadPrototypeData()
{
	// We might need to reload the data, so best to be sure and clear it's current data.
	m_spellRegistry.clear();

	// Load the definitions from disk into the serialiser.
	SerialiseECSInput spellSerial;
	spellSerial.LoadFromFile("chrysalis/parameters/spells/spell-prototype.xml");

	// NOTE: The load and save must match in the order and components or things will break.
	entt::snapshot_loader {m_spellRegistry}
		.entities(spellSerial)
		.component<Name, Prototype,
		Health, Damage, DamageOverTime, SelfHarm, Heal, HealOverTime,
		Qi, UtiliseQi, UtiliseQiOverTime, ReplenishQi, ReplenishQiOverTime,
		Spell, SpellFragment, SpellCastDuration,
		SpellActionSchematyc, SpellActionDRS,
		SpellActionInspect, SpellActionExamine,
		SpellActionTake, SpellActionDrop, SpellActionThrow,
		SpellActionSwitch,
		SpellActionOpen, SpellActionClose,
		SpellActionUnlock, SpellActionLock,
		RenderLight,
		Timer, Duration, Range, Aura, Buff, Debuff, Cooldown,
		Channelled, 
		AnimationFragmentSpellCast, AnimationFragmentEmote, AnimationTag,
		MovementFactor, CancelOnMovement, AreaOfEffect,
		CrowdControlBlind, CrowdControlDisarm, CrowdControlMovementRestricted, CrowdControlRotationRestricted, CrowdControlFlee, CrowdControlMindControl,
		CrowdControlPull, CrowdControlTaunt, CrowdControlThrow, CrowdControlKockback, CrowdControlKnockdown, CrowdControlPolymorph, CrowdControlSilence
		>(spellSerial);
}


void CSimulation::SavePrototypeData()
{
	// Spell prototypes.
	SerialiseECSOutput spellSerial;

	// NOTE: The load and save must match in the order and components or things will break.
	entt::snapshot {m_spellRegistry}
		.entities(spellSerial)
		.component<Name, Prototype,
		Health, Damage, DamageOverTime, SelfHarm, Heal, HealOverTime,
		Qi, UtiliseQi, UtiliseQiOverTime, ReplenishQi, ReplenishQiOverTime,
		Spell, SpellFragment, SpellCastDuration,
		SpellActionSchematyc, SpellActionDRS,
		SpellActionInspect, SpellActionExamine,
		SpellActionTake, SpellActionDrop, SpellActionThrow,
		SpellActionSwitch,
		SpellActionOpen, SpellActionClose,
		SpellActionUnlock, SpellActionLock,
		RenderLight,
		Timer, Duration, Range, Aura, Buff, Debuff, Cooldown,
		Channelled, 
		AnimationFragmentSpellCast, AnimationFragmentEmote, AnimationTag,
		MovementFactor, CancelOnMovement, AreaOfEffect,
		CrowdControlBlind, CrowdControlDisarm, CrowdControlMovementRestricted, CrowdControlRotationRestricted, CrowdControlFlee, CrowdControlMindControl,
		CrowdControlPull, CrowdControlTaunt, CrowdControlThrow, CrowdControlKockback, CrowdControlKnockdown, CrowdControlPolymorph, CrowdControlSilence
		>(spellSerial);

	// Push the contents of the serialiser out to disk.
	spellSerial.SaveToFile("chrysalis/parameters/spells/spell-prototype.xml");
}
}
