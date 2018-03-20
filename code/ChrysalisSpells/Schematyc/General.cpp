#include "StdAfx.h"

#include <CrySchematyc/CoreAPI.h>
#include <Cry3DEngine/I3DEngine.h>
#include <Cry3DEngine/ITimeOfDay.h>
#include <Schematyc/CoreEnv.h>
#include <CryGame/GameUtils.h>


namespace ChrysalisSpells
{
namespace Environment
{
using namespace ::Schematyc;

static void RegisterFunctions(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(g_generalModuleGUID);
}
} // Environment


namespace Math
{
using namespace ::Schematyc;

static void RegisterFunctions(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(GetTypeDesc<float>().GetGUID());
}
} // Math


static void RegisterGeneralFunctions(::Schematyc::IEnvRegistrar& registrar)
{
	Environment::RegisterFunctions(registrar);
	Math::RegisterFunctions(registrar);
}
} // Chrysalis

CRY_STATIC_AUTO_REGISTER_FUNCTION(&ChrysalisSpells::RegisterGeneralFunctions)