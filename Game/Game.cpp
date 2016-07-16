#include <StdAfx.h>

#include "Game.h"
#include <map>
#include <CryMath/Cry_Math.h>
#include <CryGame/IGameFramework.h>
#include <CryRenderer/IRenderer.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include <Cry3DEngine/I3DEngine.h>
#include <CryInput/IHardwareMouse.h>
#include <CryCore/Platform/CryWindows.h>
#include <IAnimatedCharacter.h>
#include <ILevelSystem.h>
#include <Game/Rules/GameRules.h>
#include <Actor/Player/Player.h>
#include <ActionMaps/GameActionMaps.h>
#include <CryCore/Platform/IPlatformOS.h>
#include <IPlayerProfiles.h>
#include <CryFlowGraph/IFlowSystem.h>
#include <IItemSystem.h>
#include <Environment/Ledge/LedgeManager.h>
#include <Game/Physics/GamePhysicsSettings.h>
#include <Camera/FirstPersonCamera.h>
#include <Camera/ActionRPGCamera.h>
#include <Actor/Character/Character.h>
#include <ConsoleCommands/ConsoleCommands.h>
#include <ConsoleVariables/ConsoleVariables.h>
#include <Entity/EntityScriptCalls.h>
#include <ObjectID/ObjectIdMasterFactory.h>
#include <ScriptBinds/ScriptBinds.h>
#include <Game/Cache/GameCache.h>
//#include <IPluginManager_impl.h>
#include <Game/Registration/GameRegistration.h>


//CG2AutoRegFlowNodeBase *CG2AutoRegFlowNodeBase::m_pFirst = 0;
//CG2AutoRegFlowNodeBase *CG2AutoRegFlowNodeBase::m_pLast = 0;



CGame::CGame()
{
	// Get the global set first.
	g_pGame = this;

	// Provide console variables.
	m_pConsoleVariables = new (CConsoleVariables);
	g_pGameCVars = m_pConsoleVariables;

	// Register For Platform-Specific Events
	gEnv->pSystem->GetPlatformOS()->AddListener(this, "CGame");

	// Register For System Events
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);

	// Notifies All CRYENGINE Systems About Our Game Class.
	GetISystem()->SetIGame(this);
}


CGame::~CGame()
{
	// Un-Registers From Platform Specific Events
	gEnv->pSystem->GetPlatformOS()->RemoveListener(this);

	// Un-Registers From System Events
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	// When The Game Has Ended We Need To End Any Currently Active Game Context.
	m_pGameFramework->EndGameContext();

	// Un-Registers Game-Specific Console Variables.
	UnRegisterGameCVars();

	// Un-Registers Game-Specific Console Commands.
	UnRegisterGameConsoleCommands();

	// Dispose of the action maps.
	SAFE_DELETE(m_pGameActionMaps);

	// Deletes The Game-Specific Physics Settings;
	SAFE_DELETE(m_pGamePhysicsSettings);

	// Remove our ledge manager.
	SAFE_DELETE(m_pLedgeManager);

	// Deletes All Game-Specific Script Binds.
	m_pScriptBinds->Release();
	SAFE_DELETE(m_pScriptBinds);

	// Un-Registers From IGameFramework Event Notifications.
	m_pGameFramework->UnregisterListener(this);

	// Very Important This Gets Called. Shuts Down And Releases All Loaded Plug-ins.
	//if (gPluginManager)
	//{
	//	CRY_ASSERT(gPluginManager->GetBase()->Release(true));
	//	gPluginManager = nullptr;
	//}

	// Clean up anything else that needs it.
	SAFE_DELETE(m_pObjectIdMasterFactory);
	SAFE_DELETE(m_pRayCaster);
	SAFE_DELETE(m_pGameCache);

	// Notifies All CRYENGINE Systems That Our Game Has Ended.
	GetISystem()->SetIGame(nullptr);
}


bool CGame::Init(IGameFramework *pFramework)
{
	// Store The IGameFramework.
	m_pGameFramework = pFramework;

	// Create a valid master factory which can provide instance unique Ids for us.
	// TODO: Get the InstanceId from the command line or cvars.
	m_pObjectIdMasterFactory = new CObjectIdMasterFactory(0);

	// Registers For IGameFramework Event Notifications.
	m_pGameFramework->RegisterListener(this, "CGame", FRAMEWORKLISTENERPRIORITY_GAME);

	// Create a set of action maps.
	m_pGameActionMaps = new CGameActionMaps();

	// Plugin manager needs to initialise fairly early.
	//PluginManager::InitPluginManager();
	//PluginManager::InitPluginsBeforeFramework();

	// Register Game-Specific Console Variables.
	RegisterGameCVars();

	// Register Game-Specific Console Commands.
	RegisterGameConsoleCommands();

	// Registers All Custom Game-Specific IGameObjects
	RegisterGameObjects();

	// Initialize The IPlatformOS
	InitPlatformOS();

	// Used To Load The Required Default ActionMap.
	LoadActionMaps();

	// Creates The Game-Specific Physics Settings.
	m_pGamePhysicsSettings = new CGamePhysicsSettings();

	// Creates And Initializes All Game-Specific Script Binds.
	m_pScriptBinds = new CScriptBinds();
	m_pScriptBinds->Init();

	// Stores The IPlayerProfileManager.
	m_pPlayerProfileManager = m_pGameFramework->GetIPlayerProfileManager();

	// Allocate a ledge manager.
	m_pLedgeManager = new CLedgeManager();

	// Signs In The Required Default User.
	if (!gEnv->pSystem->GetPlatformOS()->UserDoSignIn(0))
		return false;// Initialization Was NOT Successful.

	// Loads The "GameSettings Script"
	bool bMultiplayer = false;
	if (gEnv->pScriptSystem->ReloadScript(GamePathLuaScripts + "GameSettings.lua"))
	{
		// Gets The "GameSettings" Table.
		ScriptAnyValue GameSettingsTable;
		gEnv->pScriptSystem->GetGlobalAny("GameSettings", GameSettingsTable);

		// Gets Whether MultiPlayer Mode Needs To Be Enabled Or Not.
		if (GameSettingsTable.table)
			GameSettingsTable.table->GetValue("bMultiPlayer", bMultiplayer);
	}

	// Initializes The Game Type (Multi-player/Single-player).
	m_pGameFramework->InitGameType(bMultiplayer, true);

	// Create a new cache for the game.
	m_pGameCache = new CGameCache;
	m_pGameCache->Init();

	// Create a new deferred ray-cast object and set it's quota limit.
	m_pRayCaster = new GlobalRayCaster;
	m_pRayCaster->SetQuota(g_pGameCVars->game_rayCastQuota);

	// Search the file system to find XML files with definitions for game items.
	m_pGameFramework->GetIItemSystem()->Scan("Parameters/Items");

	// Search the file system to find XML files with definitions for game weapons.
	m_pGameFramework->GetIActorSystem()->Scan("Parameters/Actors");

	// TODO: When dropping in new weapon system, need to enable scan of weapon parameter files.

	// Search the file system to find XML files with definitions for game weapons.
	//m_pWeaponSystem = new CWeaponSystem(this, GetISystem());
	//m_pWeaponSystem->Scan("Parameters/Weapons");

	// Initialization Was Successful.
	return true;
}


void CGame::InitEditor(IGameToEditorInterface* pGameToEditor)
{
}


void CGame::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	m_pScriptBinds->GetMemoryUsage(s);
	m_pGameCache->GetMemoryUsage(s);
}


void CGame::Shutdown()
{
	// Delete This Instance (We Are Not Allocated On The Heap).
	this->~CGame();
}


int CGame::Update(bool haveFocus, unsigned int updateFlags)
{
	float frameTime = gEnv->pTimer->GetFrameTime();

	// Pre-updates the IGameFramework (which in-turn updates other various needed systems).
	bool m_bRun = m_pGameFramework->PreUpdate(haveFocus, updateFlags);

	// Draw some example text to the screen.
	gEnv->pRenderer->Draw2dLabel(0, 0, 2, ColorF(1, 1, 1, 1), false, "Game Is Running!");

	// Allow the deferred ray-caster to perform it's update.
	if (m_pRayCaster)
	{
		FRAME_PROFILER("GlobalRayCaster", gEnv->pSystem, PROFILE_AI);
		m_pRayCaster->SetQuota(g_pGameCVars->game_rayCastQuota);
		m_pRayCaster->Update(frameTime);
	}

	// Post-updates the IGameFramework (which in-turn updates other various needed systems).
	m_pGameFramework->PostUpdate(haveFocus, updateFlags);

	// Return whether to exit the game of not. False will exit.
	return m_bRun;
}


void CGame::EditorResetGame(bool bStart)
{}


void CGame::PlayerIdSet(EntityId playerId)
{
	// Store the player's EntityID.
	m_PlayerEntityId = playerId;
}


const char *CGame::GetLongName()
{
	// Return long name of the game.
	return ProjectName + " - ";
}


const char *CGame::GetName()
{
	// Return short name of the game.
	return ProjectName;
}


void CGame::OnClearPlayerIds()
{}


IGame::TSaveGameName CGame::CreateSaveGameName()
{
	// Create a save game name.
	return string(ProjectName + "Save") + CRY_SAVEGAME_FILE_EXT;
}


IGameFramework *CGame::GetIGameFramework()
{
	return m_pGameFramework;
}


const char* CGame::GetMappedLevelName(const char *levelName) const
{
	// Create a user friendly version of the specified level name (used for displaying in UI's).
	return levelName;
}


const bool CGame::DoInitialSavegame() const
{
	return true;
}


uint32 CGame::AddGameWarning(const char* stringId, const char* paramMessage, IGameWarningsListener* pListener)
{
	return 0;
}


void CGame::OnRenderScene(const SRenderingPassInfo &passInfo)
{}


void CGame::RenderGameWarnings()
{}


void CGame::RemoveGameWarning(const char* stringId)
{}


bool CGame::GameEndLevel(const char* stringId)
{
	// IGameFramework should handle this event.
	return true;
}


IGameStateRecorder* CGame::CreateGameStateRecorder(IGameplayListener* pL)
{
	return nullptr;
}


void CGame::FullSerialize(TSerialize ser)
{}


IGame::ExportFilesInfo CGame::ExportLevelData(const char* levelName, const char* missionName) const
{
	return ExportFilesInfo("", 0);
}


void CGame::LoadExportedLevelData(const char* levelName, const char* missionName)
{}


void CGame::RegisterGameFlowNodes()
{
	// TODO: New implementation for this or we don't get flownodes.

	// Gets the IFlowSystem.
	//if (IFlowSystem *pFlowSystem = m_pGameFramework->GetIFlowSystem())
	//{
	//	// Gets the flownode factory used to create the about-to-be-registered flownode.
	//	CG2AutoRegFlowNodeBase *pFactory = CG2AutoRegFlowNodeBase::m_pFirst;

	//	while (pFactory)
	//	{
	//		// Register the flownode.
	//		pFlowSystem->RegisterType(pFactory->m_sClassName, pFactory);

	//		// Go to the next flownode.
	//		pFactory = pFactory->m_pNext;
	//	}
	//}

	//PluginManager::RegisterPluginFlownodes();
}


IGamePhysicsSettings* CGame::GetIGamePhysicsSettings()
{
	return m_pGamePhysicsSettings;
}


void CGame::LoadActionMaps(const char* filename)
{
	IActionMapManager* pActionMapManager = m_pGameFramework->GetIActionMapManager();

	if (pActionMapManager)
	{
		pActionMapManager->RegisterActionMapEventListener(m_pGameActionMaps);
		if (pActionMapManager->InitActionMaps(filename))
		{
			pActionMapManager->EnableActionMap("default", true);
			pActionMapManager->EnableActionMap("player", true);
			pActionMapManager->Enable(true);

		}
		else
		{
			CryFatalError("CGame::LoadActionMaps() Invalid action maps setup");
		}
	}
}


IGameRules* CGame::GetGameRules()
{
	// Store and return the current GameRules.
	m_pGameRules = m_pGameFramework->GetIGameRulesSystem()->GetCurrentGameRules();

	return m_pGameRules;
}


void CGame::RegisterGameObjects()
{
	CGameRegistration::RegisterGameObjects(m_pGameFramework);
}


bool CGame::LoginUser(unsigned int UserId)
{
	if (!m_pPlayerProfileManager)
		return false;

	// Gets The Specified User's Name.
	IPlatformOS::TUserName strUserName;
	gEnv->pSystem->GetPlatformOS()->UserGetName(UserId, strUserName);
	if (gEnv->pSystem->GetPlatformOS()->UserIsSignedIn(UserId))
	{
		// The User Is Already Signed In, So Just Activate His/Hers Profile.
		auto pDefaultProfile = m_pPlayerProfileManager->ActivateProfile(strUserName, "default");
		if (!pDefaultProfile)
			return false;
		else
			return true;
	}

	// Login The Specified User.
	bool bIsFirstTime = false;
	if (!m_pPlayerProfileManager->LoginUser(strUserName, bIsFirstTime))
		return false;

	// Activate The Specified User's Profile.
	auto pDefaultProfile = m_pPlayerProfileManager->ActivateProfile(strUserName, "default");
	if (!pDefaultProfile)
		return false;

	// User Logged-In Successfully. 
	return true;
}


void CGame::InitPlatformOS()
{
	// Arbitrary Encryption Magic.
	static const char s_encryptionMagic [] = { 'C', 'R', 'Y', '3', 'S', 'D', 'K' };    // 2 for release, 1 for demo

	// Arbitrary Encryption Key.
	static const uint64 s_saveEncryptionKey [] = {
		0x000000000000000000, 0x000000000000000000, 0x000000000000000000, 0x000000000000000000,
		0x000000000000000000, 0x000000000000000000, 0x000000000000000000, 0x000000000000000000,
		0x000000000000000000, 0x000000000000000000, 0x000000000000000000, 0x000000000000000000,
		0x000000000000000000, 0x000000000000000000, 0x000000000000000000, 0x000000000000000000,
		0x000000000000000000, 0x000000000000000000, 0x000000000000000000, 0x000000000000000000,
		0x000000000000000000, 0x000000000000000000, 0x000000000000000000, 0x000000000000000000,
		0x000000000000000000, 0x000000000000000000, 0x000000000000000000, 0x000000000000000000,
		0x000000000000000000, 0x000000000000000000, 0x000000000000000000, 0x000000000000000000,
	};

	// Initialize the platform's encryption key. (Used for PS3 game DRM etc)
	CRY_ASSERT(GetISystem()->GetPlatformOS());
	GetISystem()->GetPlatformOS()->InitEncryptionKey(s_encryptionMagic, sizeof(s_encryptionMagic), (uint8*) s_saveEncryptionKey, sizeof(s_saveEncryptionKey));
}


EntityId CGame::GetClientActorID()
{
	return m_PlayerEntityId;
}


void CGame::SetSpawnPoint(IEntity* pSpawnPoint)
{
	m_pSpawnPoint = pSpawnPoint;
}


IEntity* CGame::GetSpawnPoint()
{
	return m_pSpawnPoint;
}


CLedgeManager* CGame::GetLedgeManager()
{
	return m_pLedgeManager;
}


CGameCache& CGame::GetGameCache()
{
	CRY_ASSERT_MESSAGE(m_pGameCache, "Can't obtain GameCache object until CGame::Init() is called!");
	return *m_pGameCache;
}


const CGameCache& CGame::GetGameCache() const
{
	CRY_ASSERT_MESSAGE(m_pGameCache, "Can't obtain GameCache object until CGame::Init() is called!");
	return *m_pGameCache;
}


void CGame::RegisterGameCVars()
{
	// Registers all the console variables.
	g_pGameCVars = m_pConsoleVariables = new CConsoleVariables();
	m_pConsoleVariables->Register();
}


void CGame::UnRegisterGameCVars()
{
	// Un-registers all the console variables.
	m_pConsoleVariables->Unregister();
	SAFE_DELETE(m_pConsoleVariables);
	g_pGameCVars = nullptr;
}


void CGame::RegisterGameConsoleCommands()
{
	// Registers all the console commands.
	m_pConsoleCommands = new CConsoleCommands();
	m_pConsoleCommands->Register();
}


void CGame::UnRegisterGameConsoleCommands()
{
	// Un-registers all the console commands.
	m_pConsoleCommands->Unregister();
	delete m_pConsoleCommands;
}


void CGame::OnClientEnteredGame(IActor* pClientActor)
{}


void CGame::OnClientActorSpawned(IActor* pClientActor)
{
	// Only process if we are not in the editor.
	if (!gEnv->IsEditor())
	{
		// Hides the mouse cursor.
		gEnv->pHardwareMouse->DecrementCounter();

		if (!pClientActor)
		{
			gEnv->pLog->LogWarning("No Player To Spawn. Aborting Spawn.");
			return;
		}

		// If a spawn point is set, use it if it is enabled.
		if (m_pSpawnPoint)
		{
			// We don't know if the spawn point is enabled or not yet, so set to false by default.
			bool bEnabled = false;

			// Get the spawn point's entity table.
			auto pSelfTable = m_pSpawnPoint->GetScriptTable();
			if (pSelfTable)
			{
				// Get the spawn point's property table.
				ScriptAnyValue PropsTable;
				pSelfTable->GetValueAny("Properties", PropsTable);

				// Get whether the spawnpoint is enabled or not
				if (PropsTable.table)
					PropsTable.table->GetValue("bEnabled", bEnabled);
			}

			// If the spawn point is enabled, use it.
			if (bEnabled)
			{
				auto SpawnPos = m_pSpawnPoint->GetWorldPos();
				auto SpawnRot = m_pSpawnPoint->GetWorldAngles();

				// do not use the x-axis and y-axis of the spawn point.
				SpawnRot.x = SpawnRot.y = 0;

				// Set the player's position, rotation, and scale.
				pClientActor->GetEntity()->SetPosRotScale(SpawnPos, Quat(SpawnRot), pClientActor->GetEntity()->GetScale(), ENTITY_XFORM_POS | ENTITY_XFORM_ROT | ENTITY_XFORM_SCL);
				gEnv->pLog->LogAlways("Player \"%s\" Successfully Spawned At Location: (%0.2f, %0.2f, %0.2f).", pClientActor->GetEntity()->GetName(), SpawnPos.x, SpawnPos.y, SpawnPos.z);
			}

			else
			{
				// The spawn point is not enabled, so spawn at position and rotation (0, 0, 0).
				gEnv->pLog->LogWarning("No Spawn Point Set. Will Spawn Player At World (0, 0, 0).");
				pClientActor->GetEntity()->SetPosRotScale(Vec3(0, 0, 0), Quat(Ang3(0, 0, 0)), Vec3(1, 1, 1), ENTITY_XFORM_POS | ENTITY_XFORM_ROT | ENTITY_XFORM_SCL);
				gEnv->pLog->LogAlways("Player \"%s\" Successfully Spawned At Location: (%0.2f, %0.2f, %0.2f).", pClientActor->GetEntity()->GetName(), 0, 0, 0);
			}
		}
		else
		{
			// No spawn point is set, so spawn at position and rotation (0, 0, 0).
			gEnv->pLog->LogWarning("No Spawn Point Set. Will Spawn Player At World (0, 0, 0).");
			pClientActor->GetEntity()->SetPosRotScale(Vec3(0, 0, 0), Quat(Ang3(0, 0, 0)), Vec3(1, 1, 1), ENTITY_XFORM_POS | ENTITY_XFORM_ROT | ENTITY_XFORM_SCL);
			gEnv->pLog->LogAlways("Player \"%s\" Successfully Spawned At Location: (%0.2f, %0.2f, %0.2f).", pClientActor->GetEntity()->GetName(), 0, 0, 0);
		}
	}
}


void CGame::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		// Called automatically by the sandbox editor after switching between game and edit mode.
		case ESYSTEM_EVENT_EDITOR_GAME_MODE_CHANGED:
		{
			// If we are in editor game mode then use the spawn point if it exists.
			if (gEnv->IsEditor() && gEnv->IsEditorGameMode())
			{
				auto pClientActor = m_pGameFramework->GetClientActor();
				if (!pClientActor)
				{
					gEnv->pLog->LogWarning("No Player To Spawn. Aborting Spawn.");
					return;
				}

				// If a spawn point is set, use it if it is enabled.
				if (m_pSpawnPoint)
				{
					// We don't know if the spawn point is enabled or not yet, so set to false by default.
					bool bEnabled = false;

					// Get the spawn point's entity table.
					auto pSelfTable = m_pSpawnPoint->GetScriptTable();
					if (pSelfTable)
					{
						// Get the spawn point's property table.
						ScriptAnyValue PropsTable;
						pSelfTable->GetValueAny("Properties", PropsTable);

						// Get whether the spawnpoint is enabled or not
						if (PropsTable.table)
							PropsTable.table->GetValue("bEnabled", bEnabled);
					}

					// If the spawn point is enabled, use it.
					if (bEnabled)
					{
						auto SpawnPos = m_pSpawnPoint->GetWorldPos();
						auto SpawnRot = m_pSpawnPoint->GetWorldAngles();

						// Do not use the x-axis and y-axis of the spawn point.
						SpawnRot.x = SpawnRot.y = 0;

						// Set the player's position, rotation, and scale.
						pClientActor->GetEntity()->SetPosRotScale(SpawnPos, Quat(SpawnRot), pClientActor->GetEntity()->GetScale(), ENTITY_XFORM_POS | ENTITY_XFORM_ROT | ENTITY_XFORM_SCL);
						gEnv->pLog->LogAlways("Player \"%s\" Successfully Spawned At Location: (%0.2f, %0.2f, %0.2f).", pClientActor->GetEntity()->GetName(), SpawnPos.x, SpawnPos.y, SpawnPos.z);
					}
					else
					{
						// The spawn point is not enabled, so spawn at position and rotation (0, 0, 0).
						gEnv->pLog->LogWarning("No Spawn Point Set. Will Spawn Player At World (0, 0, 0).");
						pClientActor->GetEntity()->SetPosRotScale(Vec3(0, 0, 0), Quat(Ang3(0, 0, 0)), Vec3(1, 1, 1), ENTITY_XFORM_POS | ENTITY_XFORM_ROT | ENTITY_XFORM_SCL);
						gEnv->pLog->LogAlways("Player \"%s\" Successfully Spawned At Location: (%0.2f, %0.2f, %0.2f).", pClientActor->GetEntity()->GetName(), 0, 0, 0);
					}
				}
				else
				{
					// No spawn point is set, so spawn at position and rotation (0, 0, 0).
					gEnv->pLog->LogWarning("No Spawn Point Set. Will Spawn Player At World (0, 0, 0).");
					pClientActor->GetEntity()->SetPosRotScale(Vec3(0, 0, 0), Quat(Ang3(0, 0, 0)), Vec3(1, 1, 1), ENTITY_XFORM_POS | ENTITY_XFORM_ROT | ENTITY_XFORM_SCL);
					gEnv->pLog->LogAlways("Player \"%s\" Successfully Spawned At Location: (%0.2f, %0.2f, %0.2f).", pClientActor->GetEntity()->GetName(), 0, 0, 0);
				}
			}

			if (gEnv->IsEditor())
			{
				auto pClientActor = m_pGameFramework->GetClientActor();
				if (!pClientActor)
				{
					gEnv->pLog->LogWarning("No Player To Spawn. Aborting Spawn.");
					return;
				}

				// Physicalize the player.
				// TODO: Surely we need to physicalise at some point. Maybe not here, but somewhere.
				//if (!(static_cast<CPlayer*>(pClientActor)->Physicalize()))
				//	gEnv->pLog->LogWarning("CGame::OnSystemEvent(): Failed To Physicalize The Player!");
			}
		}
		break;

		case ESYSTEM_EVENT_LEVEL_LOAD_START:
		{
			// Check if we have a deferred ray-caster, and if not, allocate one and set it's quota.
			// TODO: Is it even possible for this to occur?
			if (!m_pRayCaster)
			{
				m_pRayCaster = new GlobalRayCaster;
				m_pRayCaster->SetQuota(g_pGameCVars->game_rayCastQuota);
			}
		}
		break;

		case ESYSTEM_EVENT_LEVEL_LOAD_END:
			break;

		case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
			break;

		default:
			break;
	}
}


void CGame::OnPlatformEvent(const IPlatformOS::SPlatformEvent& event)
{
	const IPlatformOS::SPlatformEvent Event = event;

	IPlatformOS::SPlatformEvent::EEventType EventType = Event.m_eEventType;
	switch (EventType)
	{
		case IPlatformOS::SPlatformEvent::eET_None:
			break;

		case IPlatformOS::SPlatformEvent::eET_PlatformSpecific:
			break;

		case IPlatformOS::SPlatformEvent::eET_SignIn:
			break;

		case IPlatformOS::SPlatformEvent::eET_StorageMounted:
		{
			// The storage device has just now been mounted, so we can go ahead and login the specified user and load his/hers profile.
			// This event was most likely triggered by the IPlatformOS::UserDoSignIn() call.
			if (!event.m_uParams.m_storageMounted.m_bOnlyUpdateMediaState)
				CRY_ASSERT(!LoginUser(event.m_user));
		}
		break;

		case IPlatformOS::SPlatformEvent::eET_StorageRemoved:
			break;

		case IPlatformOS::SPlatformEvent::eET_StorageCancelled:
			break;

		case IPlatformOS::SPlatformEvent::eET_FileError:
			break;

		case IPlatformOS::SPlatformEvent::eET_FileWrite:
			break;

		case IPlatformOS::SPlatformEvent::eET_ContentInstalled:
			break;

		case IPlatformOS::SPlatformEvent::eET_InstallBegin:
			break;

		case IPlatformOS::SPlatformEvent::eET_InstallComplete:
			break;

		case IPlatformOS::SPlatformEvent::eET_PostLocalisationBootChecksDone:
			break;

		case IPlatformOS::SPlatformEvent::eET_PIIRetrieved:
			break;

		case IPlatformOS::SPlatformEvent::eET_SystemMenu:
			break;

		case IPlatformOS::SPlatformEvent::eET_ContentRemoved:
			break;

		default:
			break;
	}
}


void CGame::OnPostUpdate(float fDeltaTime)
{}


void CGame::OnSaveGame(ISaveGame* pSaveGame)
{}


void CGame::OnLoadGame(ILoadGame* pLoadGame)
{}


void CGame::OnLevelEnd(const char* nextLevel)
{}


void CGame::OnActionEvent(const SActionEvent& event)
{
	switch (event.m_event)
	{
		// Set player's ID to zero when a level unloads,
		case eAE_unloadLevel:
			m_PlayerEntityId = INVALID_ENTITYID;

			// It's time to reset the cache.
			m_pGameCache->Reset();
			break;

		default:
			break;
	}
}


void CGame::OnPreRender()
{}


void CGame::OnSavegameFileLoadedInMemory(const char* pLevelName)
{}


void CGame::OnForceLoadingWithFlash()
{}


// Global pointer for the game.
CGame* g_pGame = nullptr;

// Global for console variables.
CConsoleVariables* g_pGameCVars = nullptr;