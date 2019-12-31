#include <StdAfx.h>

#include "Spell.h"
#include "CrySerialization/Enum.h"


namespace Chrysalis::ECS
{
// Spell Rewire.
SERIALIZATION_ENUM_BEGIN(SpellRewire, "Spell Rewire")
SERIALIZATION_ENUM(SpellRewire::simple, "simple", "simple")
SERIALIZATION_ENUM(SpellRewire::damage, "damage", "damage")
SERIALIZATION_ENUM(SpellRewire::heal, "heal", "heal")
SERIALIZATION_ENUM(SpellRewire::regenerate, "regenerate", "regenerate")
SERIALIZATION_ENUM(SpellRewire::crowdControl, "crowd-control", "crowd-control")
SERIALIZATION_ENUM(SpellRewire::custom, "custom", "custom")
SERIALIZATION_ENUM_END()

// Spell Rewire.
SERIALIZATION_ENUM_BEGIN(SpellCastStyle, "Spell Cast Style")
SERIALIZATION_ENUM(SpellCastStyle::instant, "instant", "instant")
SERIALIZATION_ENUM(SpellCastStyle::movementAllowed, "movementAllowed", "movementAllowed")
SERIALIZATION_ENUM(SpellCastStyle::turret, "turret", "turret")
SERIALIZATION_ENUM(SpellCastStyle::channelled, "channelled", "channelled")
SERIALIZATION_ENUM_END()
}
