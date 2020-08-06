#pragma once

#include <entt/entt.hpp>
#include <ECS/Systems/Simulation.h>


namespace Chrysalis::ECS
{
// Globals are considered bad form, but then, so are singletons. I prefer the simplest form
// where there is no clear advantage from one over the other.
extern CSimulation Simulation;
}