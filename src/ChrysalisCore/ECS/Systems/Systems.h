#pragma once

#include <entt/entt.hpp>

namespace Chrysalis::ECS
{
// Health.
void SystemApplyDamage(entt::registry& spellRegistry, entt::registry& actorRegistry);
void SystemApplyDamageOverTime(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry);
void SystemApplyHeal(entt::registry& spellRegistry, entt::registry& actorRegistry);
void SystemApplyHealOverTime(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry);
void SystemHealthCheck(entt::registry& spellRegistry, entt::registry& actorRegistry);

// Qi.
void SystemApplyQiUtilisation(entt::registry& spellRegistry, entt::registry& actorRegistry);
void SystemApplyQiUtilisationOverTime(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry);
void SystemApplyQiReplenishment(entt::registry& spellRegistry, entt::registry& actorRegistry);
void SystemApplyQiReplenishmentOverTime(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry);

// World Spellcasts.
void SystemWorldSpellCasts(float dt, entt::registry& spellRegistry, entt::registry& actorRegistry);

// Actors.
void SystemUpdateActors(float dt, entt::registry& actorRegistry);
}