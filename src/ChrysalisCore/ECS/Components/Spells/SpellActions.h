#pragma once

#include "ECS/Components/Components.h"


namespace Chrysalis::ECS
{
using SpellActionSchematyc = FlagComponent<"spell-action-schematyc"_hs>;
using SpellActionDRS = FlagComponent<"spell-action-drs"_hs>;
using SpellActionInspect = FlagComponent<"spell-action-inspect"_hs>;
using SpellActionExamine = FlagComponent<"spell-action-examine"_hs>;
using SpellActionTake = FlagComponent<"spell-action-take"_hs>;
using SpellActionDrop = FlagComponent<"spell-action-drop"_hs>;
using SpellActionThrow = FlagComponent<"spell-action-throw"_hs>;
using SpellActionSwitch = FlagComponent<"spell-action-switch"_hs>;
using SpellActionOpen = FlagComponent<"spell-action-open"_hs>;
using SpellActionClose = FlagComponent<"spell-action-close"_hs>;
using SpellActionUnlock = FlagComponent<"spell-action-unlock"_hs>;
using SpellActionLock = FlagComponent<"spell-action-lock"_hs>;
}