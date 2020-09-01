#pragma once

#include <entt/entt.hpp>

namespace Chrysalis::ECS
{
// Timers.
void SystemUpdateAllTimers(float dt, entt::registry& spellcastingRegistry);
void SystemUpdateTimers(float dt, entt::registry& spellcastingRegistry);
void SystemUpdateDurations(float dt, entt::registry& spellcastingRegistry);
void SystemUpdateDelays(float dt, entt::registry& spellcastingRegistry);

// Health.
void SystemApplyDamage(entt::registry& spellcastingRegistry, entt::registry& actorRegistry);
void SystemApplyDamageOverTime(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry);
void SystemApplyHeal(entt::registry& spellcastingRegistry, entt::registry& actorRegistry);
void SystemApplyHealOverTime(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry);
void SystemHealthCheck(entt::registry& spellcastingRegistry, entt::registry& actorRegistry);

// Qi.
void SystemApplyQiUtilisation(entt::registry& spellcastingRegistry, entt::registry& actorRegistry);
void SystemApplyQiUtilisationOverTime(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry);
void SystemApplyQiReplenishment(entt::registry& spellcastingRegistry, entt::registry& actorRegistry);
void SystemApplyQiReplenishmentOverTime(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry);

// World Spellcasts.
void SystemWorldSpellCasts(float dt, entt::registry& spellcastingRegistry, entt::registry& actorRegistry);

// Actors.
void SystemUpdateActors(float dt, entt::registry& actorRegistry);
}