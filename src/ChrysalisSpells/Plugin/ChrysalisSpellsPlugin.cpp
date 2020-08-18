#include "StdAfx.h"

#include "ChrysalisSpellsPlugin.h"
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <IGameObjectSystem.h>
#include <IGameObject.h>

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>


namespace ChrysalisSpells
{
CChrysalisSpellsPlugin::~CChrysalisSpellsPlugin()
{
	// Remove any registered listeners before 'this' becomes invalid
	if (gEnv->pGameFramework != nullptr)
	{
		gEnv->pGameFramework->RemoveNetworkedClientListener(*this);
	}

	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CChrysalisSpellsPlugin::GetCID());
	}
}


bool CChrysalisSpellsPlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CChrysalisSpells");

	return true;
}


void CChrysalisSpellsPlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		// Called when the game framework has initialized and we are ready for game logic to start
		case ESYSTEM_EVENT_GAME_POST_INIT:
		{
			// Listen for client connection events, in order to create the local player
			gEnv->pGameFramework->AddNetworkedClientListener(*this);
		}
		break;

		case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
		{
			// Register all components that belong to this plug-in
			auto staticAutoRegisterLambda = [](Schematyc::IEnvRegistrar& registrar)
			{
				// Call all static callback registered with the CRY_STATIC_AUTO_REGISTER_WITH_PARAM
				Detail::CStaticAutoRegistrar<Schematyc::IEnvRegistrar&>::InvokeStaticCallbacks(registrar);
			};

			if (gEnv->pSchematyc)
			{
				gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
					stl::make_unique<Schematyc::CEnvPackage>(
						CChrysalisSpellsPlugin::GetCID(),
						"EntityComponents",
						"Tau Radius",
						"Components",
						staticAutoRegisterLambda
						)
				);
			}
		}
		break;
	}
}

CRYREGISTER_SINGLETON_CLASS(CChrysalisSpellsPlugin)
}