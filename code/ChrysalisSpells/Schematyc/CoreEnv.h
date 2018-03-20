#pragma once

namespace ChrysalisSpells
{
static constexpr CryGUID g_coreEnvPackageGuid = "{0991CF0C-AD69-4640-BA5D-55AD2EC39ACC}"_cry_guid;
static constexpr CryGUID g_chrysalisModuleGUID = "{E0B93E1D-AE65-4E6F-8B63-3E25213D51E0}"_cry_guid;
static constexpr CryGUID g_generalModuleGUID = "{DB3EF45A-87D6-4416-AE82-F159719C4C06}"_cry_guid;

struct ::Schematyc::IEnvRegistrar;

void RegisterCoreEnvPackage(::Schematyc::IEnvRegistrar& registrar);

} // ~Schematyc
