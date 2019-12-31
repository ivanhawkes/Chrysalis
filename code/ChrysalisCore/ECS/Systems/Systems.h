#pragma once

#include <entt/entt.hpp>

namespace Chrysalis::ECS
{
// Health.
void SystemApplyDamage(entt::registry& registry);

void SystemApplyDamageOverTime(float dt, entt::registry& registry);

void SystemApplyHeal(entt::registry& registry);

void SystemApplyHealOverTime(float dt, entt::registry& registry);

void SystemHealthCheck(entt::registry& registry);

// Qi.
void SystemApplyQiUtilisation(entt::registry& registry);

void SystemApplyQiUtilisationOverTime(float dt, entt::registry& registry);

void SystemApplyQiReplenishment(entt::registry& registry);

void SystemApplyQiReplenishmentOverTime(float dt, entt::registry& registry);
}