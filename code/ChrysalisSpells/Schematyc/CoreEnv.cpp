#include "StdAfx.h"

#include "CoreEnv.h"
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvModule.h>


namespace ChrysalisSpells
{
using namespace ::Schematyc;

void RegisterCoreEnvPackage(IEnvRegistrar& registrar)
{
	registrar.RootScope().Register(SCHEMATYC_MAKE_ENV_MODULE(g_chrysalisModuleGUID, "ChrysalisSpells"));
	{
		CEnvRegistrationScope scope = registrar.Scope(g_chrysalisModuleGUID);
		scope.Register(SCHEMATYC_MAKE_ENV_MODULE(g_generalModuleGUID, "General"));
	}
}
}
