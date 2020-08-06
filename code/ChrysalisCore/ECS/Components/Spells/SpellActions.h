#pragma once

#include "ECS/Components/Components.h"


namespace Chrysalis::ECS
{
using SpellActionSchematyc = FlagComponent<"schematyc"_hs, "Spell action - schematic"_hs>;
using SpellActionDRS = FlagComponent<"drs"_hs, "Spell action - drs"_hs>;
using SpellActionInspect = FlagComponent<"inspect"_hs, "Spell action - inspect"_hs>;
using SpellActionExamine = FlagComponent<"examine"_hs, "Spell action - examine"_hs>;
using SpellActionTake = FlagComponent<"take"_hs, "Spell action - take"_hs>;
using SpellActionDrop = FlagComponent<"drop"_hs, "Spell action - drop"_hs>;
using SpellActionThrow = FlagComponent<"throw"_hs, "Spell action - throw"_hs>;
using SpellActionSwitch = FlagComponent<"switch"_hs, "Spell action - switch"_hs>;
using SpellActionOpen = FlagComponent<"open"_hs, "Spell action - open"_hs>;
using SpellActionClose = FlagComponent<"close"_hs, "Spell action - close"_hs>;
using SpellActionUnlock = FlagComponent<"unlock"_hs, "Spell action - unlock"_hs>;
using SpellActionLock = FlagComponent<"lock"_hs, "Spell action - lock"_hs>;
}