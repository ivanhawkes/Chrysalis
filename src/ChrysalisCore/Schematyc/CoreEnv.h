#pragma once

namespace Chrysalis
{
static constexpr CryGUID g_coreEnvPackageGuid = "{D37343D4-6658-43F0-AF1D-D6446CF3452D}"_cry_guid;
static constexpr CryGUID g_chrysalisModuleGUID = "{BD1E5ABF-E57B-49D0-AB3C-25277FBE0ED6}"_cry_guid;
static constexpr CryGUID g_generalModuleGUID = "{5A19FE8F-7D8F-4C75-ADED-D07F4D14AFEB}"_cry_guid;

struct ::Schematyc::IEnvRegistrar;

void RegisterCoreEnvPackage(::Schematyc::IEnvRegistrar& registrar);

} // ~Schematyc
