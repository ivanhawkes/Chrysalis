#include "StdAfx.h"

#include "ChrysalisSpellsPlugin.h"
#include <CryExtension/ICryPluginManager.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CrySystem/ISystem.h>
#include <IGameObjectSystem.h>
#include <IGameObject.h>
#include "Schematyc/CoreEnv.h"


// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

namespace ChrysalisSpells
{
CChrysalisSpellsPlugin::~CChrysalisSpellsPlugin()
{
	// Remove any registered listeners before 'this' becomes invalid
	gEnv->pGameFramework->RemoveNetworkedClientListener(*this);
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(GetSchematycPackageGUID());
	}
}


void CChrysalisSpellsPlugin::RegisterComponents(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		//{
		//	Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(Chrysalis::CPlayerComponent));
		//	Chrysalis::CPlayerComponent::Register(componentScope);
		//}
	}
}


bool CChrysalisSpellsPlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CChrysalisSpells");

	// Listen for client connection events, in order to create the local player
	gEnv->pGameFramework->AddNetworkedClientListener(*this);

	return true;
}


void CChrysalisSpellsPlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
		{
			if (gEnv->pSchematyc)
			{
				gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
					SCHEMATYC_MAKE_ENV_PACKAGE(g_coreEnvPackageGuid, "CoreEnv", "Chrysalis", "Core Schematyc environment", SCHEMATYC_DELEGATE(&RegisterCoreEnvPackage)));
			}

			// Register all components that belong to this plug-in
			auto staticAutoRegisterLambda = [this](Schematyc::IEnvRegistrar& registrar)
			{
				// Call all static callback registered with the CRY_STATIC_AUTO_REGISTER_WITH_PARAM
				Detail::CStaticAutoRegistrar<Schematyc::IEnvRegistrar&>::InvokeStaticCallbacks(registrar);

				// Register our components.
				RegisterComponents(registrar);
			};

			if (gEnv->pSchematyc)
			{
				gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
					stl::make_unique<Schematyc::CEnvPackage>(
					GetSchematycPackageGUID(),
					"EntityComponents",
					"Chrysalis",
					"Components",
					staticAutoRegisterLambda
				)
				);
			}
		}
		break;

		// Called when the game framework has initialized and we are ready for game logic to start
		case ESYSTEM_EVENT_GAME_POST_INIT:
		{
		}
		break;

		case ESYSTEM_EVENT_LEVEL_LOAD_END:
			break;
	}
}


bool CChrysalisSpellsPlugin::OnClientConnectionReceived(int channelId, bool bIsReset)
{
	return true;
}


bool CChrysalisSpellsPlugin::OnClientReadyForGameplay(int channelId, bool bIsReset)
{
	return true;
}


void CChrysalisSpellsPlugin::OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient)
{
}


CChrysalisSpellsPlugin* CChrysalisSpellsPlugin::Get()
{
	static CChrysalisSpellsPlugin* plugIn { nullptr };

	if (!plugIn)
		plugIn = gEnv->pSystem->GetIPluginManager()->QueryPlugin<CChrysalisSpellsPlugin>();

	if (!plugIn)
		CRY_ASSERT_MESSAGE(plugIn, "Chrysalis Core plugin was not found.");

	return plugIn;
}

CRYREGISTER_SINGLETON_CLASS(CChrysalisSpellsPlugin)
}