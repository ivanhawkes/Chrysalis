#include "StdAfx.h"

#include "ChrysalisCorePlugin.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CrySystem/ISystem.h>
#include <IGameObjectSystem.h>
#include <IGameObject.h>
#include "Components/Player/PlayerComponent.h"
#include "Console/CVars.h"
#include "DynamicResponseSystem/ConditionDistanceToEntity.h"
#include "DynamicResponseSystem/ActionClose.h"
#include "DynamicResponseSystem/ActionLock.h"
#include "DynamicResponseSystem/ActionOpen.h"
#include "DynamicResponseSystem/ActionPlayAnimation.h"
#include "DynamicResponseSystem/ActionSwitch.h"
#include "DynamicResponseSystem/ActionUnlock.h"
#include "ObjectID/ObjectIdMasterFactory.h"
#include "Schematyc/CoreEnv.h"
#include "ECS/ECS.h"

#ifdef IMGUI
#include "Imgui/Panels/EntityEditor.h"
#endif

// Testing functionality.
#include "Item/ItemSystem.h"

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>


namespace Chrysalis
{
#ifdef IMGUI
static std::shared_ptr<CImguiImpl> g_pImguiImpl;
static std::shared_ptr<CEntityEditor> g_pSpellPrototypeEditor;
static std::shared_ptr<CEntityEditor> g_pSpellcastEditor;
static std::shared_ptr<CEntityEditor> g_pActorEditor;
#endif


CChrysalisCorePlugin::~CChrysalisCorePlugin()
{
	// Remove any registered listeners before 'this' becomes invalid
	gEnv->pGameFramework->RemoveNetworkedClientListener(*this);
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
	gEnv->pGameFramework->UnregisterListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CChrysalisCorePlugin::GetCID());
	}

	// Unregister all the cvars.
	g_cvars.UnregisterVariables();
}


bool CChrysalisCorePlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CChrysalisCore");

	// Register all the cvars.
	g_cvars.RegisterVariables();

	// Create a valid master factory which can provide instance unique Ids for us.
	// #TODO: Get the InstanceId from the command line or cvars.
	m_pObjectIdMasterFactory = new CObjectIdMasterFactory(0);

#ifdef IMGUI
	g_pImguiImpl = std::make_shared<CImguiImpl>();
	g_pSpellPrototypeEditor = std::make_shared<CEntityEditor>(ECS::Simulation.GetSpellRegistry(), "Spell Prototype Editor");
	g_pSpellcastEditor = std::make_shared<CEntityEditor>(ECS::Simulation.GetSpellCastingRegistry(), "Spellcast Editor");
	//g_pActorEditor = std::make_shared<CEntityEditor>(ECS::Simulation.GetActorRegistry(), "Actor Editor");
#endif

	EnableUpdate(IEnginePlugin::EUpdateStep::MainUpdate, true);

	return true;
}


void CChrysalisCorePlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
		{
			// Register core environmental Schematyc code.
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
			};

			if (gEnv->pSchematyc)
			{
				gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
					stl::make_unique<Schematyc::CEnvPackage>(
						CChrysalisCorePlugin::GetCID(),
						"EntityComponents",
						"Tau Radius",
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
			// TEST: just seeing if the entt code works.
			//CItemSystem itemSystem;

			// Search the file system to find XML files with definitions for game items.
			// TODO: CRITICAL: HACK: BROKEN: !!
			//gEnv->pGameFramework->GetIItemSystem()->Scan("Parameters/Items");

			// Search the file system to find XML files with definitions for game weapons.
			// TODO: CRITICAL: HACK: BROKEN: !!
			//gEnv->pGameFramework->GetIActorSystem()->Scan("Parameters/Actors");

			// Need to listen for game framework events.
			gEnv->pGameFramework->RegisterListener(this, "CChrysalisCore", FRAMEWORKLISTENERPRIORITY_GAME);

			// Create a class to handle the Simulation work for the ECS.
			ECS::Simulation.Init();
			ECS::Simulation.LoadPrototypeData();

			// HACK: Get an output version of the data immediately for inspection / testing.
			//ECS::Simulation.SavePrototypeData();

			// Listen for client connection events, in order to create the local player
			gEnv->pGameFramework->AddNetworkedClientListener(*this);

			// We need to register the procedural contexts.
			IProceduralClipFactory& proceduralClipFactory = gEnv->pGameFramework->GetMannequinInterface().GetProceduralClipFactory();
			mannequin::RegisterProceduralClipsForModule(proceduralClipFactory);

			// #TODO: this is also null at this point in init...move as well.
			if (gEnv->pDynamicResponseSystem)
			{
				// Register the custom DRS actions and conditions.
				REGISTER_DRS_CUSTOM_CONDITION(CConditionDistanceToEntity);
				REGISTER_DRS_CUSTOM_ACTION(CActionClose);
				REGISTER_DRS_CUSTOM_ACTION(CActionLock);
				REGISTER_DRS_CUSTOM_ACTION(CActionOpen);
				REGISTER_DRS_CUSTOM_ACTION(CActionPlayAnimation);
				REGISTER_DRS_CUSTOM_ACTION(CActionSwitch);
				REGISTER_DRS_CUSTOM_ACTION(CActionUnlock);
			}

			// Don't need to load the map in editor
			if (!gEnv->IsEditor())
			{
				gEnv->pConsole->ExecuteString("map test", false, true);
			}
		}
		break;

		case ESYSTEM_EVENT_LEVEL_LOAD_END:
			// HACK: TEST: I need a convenient time to write back the Simulation so I can examine it. This will do for now.
			//ECS::Simulation.SavePrototypeData();

			// In the editor, we wait until now before attempting to connect to the local player. This is to ensure all the
			// entities are already loaded and initialised. It works differently in game mode. 
			if (gEnv->IsEditor())
			{
				if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
					pPlayer->NetworkClientConnect();
			}
			break;

		case ESYSTEM_EVENT_LEVEL_UNLOAD:
		{
			m_players.clear();
		}
		break;
	}
}


bool CChrysalisCorePlugin::OnClientConnectionReceived(int channelId, bool bIsReset)
{
	// Connection received from a client, create a player entity and component
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams.sName = "Player";

	// Determine if this channel is the local player channel. The first player to connect is considered the local
	// player, and this will rollover to new maps when they are loaded. 
	bool isLocal {false};
	if (!gEnv->IsDedicated())
	{
		if (m_players.size() == 0)
		{
			// We're the first channel to connect, we must be the local player.
			isLocal = true;
		}
		else
		{
			// If the Id we previously assigned the channel matches the LOCAL_PLAYER_ENTITY_ID, then we know this channel is
			// the local player. 
			const auto player = m_players.find(channelId);
			if ((player != m_players.end()) && (player->second == LOCAL_PLAYER_ENTITY_ID))
				isLocal = true;
		}
	}

	// If this is the local player, then it needs to be tagged and given the local player Id in order for calls to
	// CPlayerComponent::GetLocalPlayer() to work.
	if (isLocal)
	{
		spawnParams.id = LOCAL_PLAYER_ENTITY_ID;
		spawnParams.nFlags |= ENTITY_FLAG_LOCAL_PLAYER;
	}

	// Spawn the player entity
	if (IEntity* pPlayerEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		// Set the local player entity channel id, and bind it to the network so that it can support Multiplayer contexts
		pPlayerEntity->GetNetEntity()->SetChannelId(channelId);
		pPlayerEntity->GetNetEntity()->BindToNetwork();

		// Create the player component instance
		pPlayerEntity->GetOrCreateComponent<CPlayerComponent>();

		// Push the component into our map, with the channel id as the key
		m_players.emplace(std::make_pair(channelId, pPlayerEntity->GetId()));
	}

	return true;
}


bool CChrysalisCorePlugin::OnClientReadyForGameplay(int channelId, bool bIsReset)
{
	// In game mode, we can attempt to connect to the local player at this point. This is because all the entities
	// should already be loaded and initialised by now. It works differently in the editor. 
	if (!gEnv->IsEditor())
	{
		if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
			pPlayer->NetworkClientConnect();
	}

	return true;
}


void CChrysalisCorePlugin::OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient)
{
	// Client disconnected, remove the entity and from map.
	auto it = m_players.find(channelId);
	if (it != m_players.end())
	{
		gEnv->pEntitySystem->RemoveEntity(it->second);

		m_players.erase(it);
	}
}


void CChrysalisCorePlugin::MainUpdate(float frameTime)
{
	if (gEnv->IsDedicated())
		return;

#ifdef IMGUI
	if (g_pImguiImpl)
		g_pImguiImpl->Update();

	if (g_pSpellPrototypeEditor)
		g_pSpellPrototypeEditor->Draw();

	if (g_pSpellcastEditor)
		g_pSpellcastEditor->Draw();

	if (g_pActorEditor)
		g_pActorEditor->Draw();
#endif
}


void CChrysalisCorePlugin::OnPostUpdate(float deltaTime)
{
	ECS::Simulation.Update(deltaTime);
}


void CChrysalisCorePlugin::IterateOverPlayers(std::function<void(CPlayerComponent& player)> func) const
{
	for (const std::pair<int, EntityId>& playerPair : m_players)
	{
		if (IEntity* pPlayerEntity = gEnv->pEntitySystem->GetEntity(playerPair.second))
		{
			if (CPlayerComponent* pPlayer = pPlayerEntity->GetComponent<CPlayerComponent>())
			{
				func(*pPlayer);
			}
		}
	}
}


CRYREGISTER_SINGLETON_CLASS(CChrysalisCorePlugin)
}