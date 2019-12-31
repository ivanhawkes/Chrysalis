#include <StdAfx.h>

#include "ECSSimulation.h"
#include "Systems.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Health.h"
#include "ECS/Components/Inventory.h"
#include "ECS/Components/Items.h"
#include "ECS/Components/Qi.h"
#include "ECS/Components/Spell.h"
#include "ECS/Systems/Systems.h"
#include "ECS/Systems/XMLSerializer.h"
#include <entt/entt.hpp>
#include "Crymath/Random.h"


namespace Chrysalis::ECS
{
//void update(std::uint64_t dt, entt::registry& registry)
//{
//	registry.view<ECS::Position, ECS::Velocity>().each([dt](auto& pos, auto& vel) {
//		// gets all the components of the view at once ...
//		pos.x += vel.dx * dt;
//		pos.y += vel.dy * dt;

//		// ...
//		});
//}


const entt::entity GetHero(entt::registry& registry)
{
	auto view = registry.view<ECS::Name>();

	for (auto entity : view)
	{
		auto& name = view.get<ECS::Name>(entity);

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
	auto view = registry.view<ECS::Name>();

	for (auto entity : view)
	{
		auto& name = view.get<ECS::Name>(entity);

		if (strcmp(name.name, "Villain") == 0)
		{
			return entity;
		}
	}

	// Failed to find it.
	return entt::null;
}


void ECSSimulation::Init()
{
	ECS::RegisterComponentsWithMeta();
}


void ECSSimulation::Update(const float deltaTime)
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
}


void ECSSimulation::UpdateImmediate(const float deltaTime)
{
	// Simluate some direct heals and direct damage.
	ECS::SystemApplyDamage(m_actorRegistry);
	ECS::SystemApplyHeal(m_actorRegistry);
	ECS::SystemHealthCheck(m_actorRegistry);

	// Simluate some direct qi use and replenishment.
	ECS::SystemApplyQiUtilisation(m_actorRegistry);
	ECS::SystemApplyQiReplenishment(m_actorRegistry);
}


void ECSSimulation::UpdateTick(const float deltaTime)
{
	// Health ticks.
	ECS::SystemApplyDamageOverTime(deltaTime, m_actorRegistry);
	ECS::SystemApplyHealOverTime(deltaTime, m_actorRegistry);
	ECS::SystemHealthCheck(m_actorRegistry);

	// Qi ticks.
	ECS::SystemApplyQiUtilisationOverTime(deltaTime, m_actorRegistry);
	ECS::SystemApplyQiReplenishmentOverTime(deltaTime, m_actorRegistry);
}


void ECSSimulation::LoadSimulationData()
{
	//update(10, saveRegistry);
	//update(saveRegistry);

	// Load the actor registry.
	ECS::LoadECSFromXML("chrysalis/parameters/items/test.xml", m_actorRegistry);

	// Load the spell registry.
	ECS::LoadECSFromXML("chrysalis/parameters/spells/spells.xml", m_spellRegistry);

	//auto villain = GetVillain(m_actorRegistry);
	//auto hero = GetHero(m_actorRegistry);
	//if ((hero != entt::null) && (villain != entt::null))
	//{
	//	// Health test data.
	//	TestAddDamage(hero, villain, m_actorRegistry);
	//	TestAddDamageOverTime(hero, villain, m_actorRegistry);
	//	TestAddHeal(hero, villain, m_actorRegistry);
	//	TestAddHealOverTime(hero, villain, m_actorRegistry);

	//	// Qi test data.
	//	TestAddUtiliseQi(hero, villain, m_actorRegistry);
	//	TestAddUtiliseQiOverTime(hero, villain, m_actorRegistry);
	//	TestAddReplenishQi(hero, villain, m_actorRegistry);
	//	TestAddReplenishQiOverTime(hero, villain, m_actorRegistry);
	//}
}


void ECSSimulation::SaveSimulationData()
{
	// Actor related.
	ECS::SerialiseECS actorSerial;
	m_actorRegistry.snapshot()
		.entities(actorSerial)
		.component<ECS::Name,
		ECS::Health, ECS::Damage, ECS::DamageOverTime, ECS::Heal, ECS::HealOverTime,
		ECS::Qi, ECS::UtiliseQi, ECS::UtiliseQiOverTime, ECS::ReplenishQi, ECS::ReplenishQiOverTime,
		ECS::Spell,
		ECS::ItemClass>(actorSerial);
	actorSerial.SaveToFile("chrysalis/parameters/items/test-out-snapshot.xml");

	// Spell prototypes.
	ECS::SerialiseECS spellSerial;
	m_spellRegistry.snapshot()
		.entities(spellSerial)
		.component<ECS::Name,
		ECS::Health, ECS::Damage, ECS::DamageOverTime, ECS::Heal, ECS::HealOverTime,
		ECS::Qi, ECS::UtiliseQi, ECS::UtiliseQiOverTime, ECS::ReplenishQi, ECS::ReplenishQiOverTime,
		ECS::Spell>(spellSerial);

	spellSerial.SaveToFile("chrysalis/parameters/spells/spells-snapshot.xml");
}
}
