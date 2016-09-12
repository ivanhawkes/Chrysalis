#include <StdAfx.h>

#include "EditorGame.h"
#include <Startup/GameStartup.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <IGameRulesSystem.h>
#include <Player/Player.h>
#include <Player/Camera/ICameraComponent.h>
#include <ILevelSystem.h>


extern "C"
{
	GAME_API IGameStartup* CreateGameStartup();
};


bool CEditorGame::Init(ISystem* pSystem, IGameToEditorInterface* pGameToEditorInterface)
{
	SSystemInitParams startupParams;

	// Specifies that we are in editor mode.
	startupParams.bEditor = true;

	// Lets all systems know about the created ISystem instance.
	startupParams.pSystem = pSystem;

	// Do not execute the command line at this moment.
	startupParams.bExecuteCommandLine = false;

	// Creates a game startup instance.
	m_pGameStartup = CreateGameStartup();

	// Initializes the game startup instance which in turn creates and initializes a CGame instance. 
	m_pGame = m_pGameStartup->Init(startupParams);

	// Initialize the sandbox editor using the IGameFramework.
	m_pGame->GetIGameFramework()->InitEditor(pGameToEditorInterface);

	// Lets all systems know that we are acting as a server.
	gEnv->bServer = true;

	// Lets all systems know that we are not in multi-player Mode.
	gEnv->bMultiplayer = false;

#if CRY_PLATFORM_DESKTOP
	// Lets all systems know that we are also acting as a client.
	gEnv->SetIsClient(true);
#endif

	// Makes sure we are not in game mode by setting it off.
	SetGameMode(false);

	// Starts a networked game context.
	ConfigureNetContext(true);

	return true;
}


int CEditorGame::Update(bool haveFocus, unsigned int updateFlags)
{
	return m_pGameStartup->Update(haveFocus, updateFlags);
}


void CEditorGame::Shutdown()
{
	// Disable the player.
	EnablePlayer(false);

	// Exit "game mode".
	SetGameMode(false);

	// Shut down the game startup instance, which in turn shuts down the CGame instance.
	m_pGameStartup->Shutdown();
}


void CEditorGame::EnablePlayer(bool bPlayer)
{
	bool spawnPlayer = false;

	if (m_bPlayer != bPlayer)
	{
		spawnPlayer = m_bPlayer = bPlayer;
	}

	if (!SetGameMode(m_bGameMode))
	{
		gEnv->pLog->LogWarning("Failed setting game mode");
	}
	else if (m_bEnabled && spawnPlayer)
	{
		if (!m_pGame->GetIGameFramework()->BlockingSpawnPlayer())
		{
			gEnv->pLog->LogWarning("Failed spawning player");
		}
	}
}


bool CEditorGame::SetGameMode(bool bGameMode)
{
	m_bGameMode = bGameMode;
	bool ok = ConfigureNetContext(m_bPlayer);

	if (ok)
	{
		if (gEnv->IsEditor())
		{
			m_pGame->EditorResetGame(bGameMode);
		}

		m_pGame->GetIGameFramework()->OnEditorSetGameMode(bGameMode);
	}
	else
	{
		gEnv->pLog->LogWarning("Failed configuring net context");
	}

	// For convenience, we will move the player to the camera location when exiting game mode.	
	//if (const auto pPlayer = CPlayer::GetLocalPlayer())
	//{
	//	if (auto pCamera = pPlayer->GetCamera())
	//	{
	//		if (!m_bGameMode)
	//		{
	//			pPlayer->GetEntity()->SetPos(pCamera->GetCameraPose().GetPosition());
	//			pPlayer->GetEntity()->SetRotation(pCamera->GetCameraPose().GetRotation());
	//		}

	//		//// HACK: Try and make a new actor here for testing...
	//		//// It spawns and we can control it, but oddly doesn't delete when asked.
	//		//auto *pActorSystem = gEnv->pGame->GetIGameFramework()->GetIActorSystem();
	//		//if (isGameMode)
	//		//{
	//		//	// Called when a new client connects to the server. We need to spawn a player class to get the ball rolling.				
	//		//	if (auto pActor = pActorSystem->CreateActor(0, "CharacterProxy", "Character", pCamera->GetCameraPose().GetPosition(),
	//		//		Quat (Ang3(0.0f, 0.0f, pCamera->GetCameraPose().GetRotation().GetRotZ())),
	//		//		Vec3(1, 1, 1)))
	//		//	{
	//		//		gEnv->pLog->LogAlways("Spawned a player pawn.");
	//		//		m_playerPawnId = pActor->GetEntityId();
	//		//		pPlayer->AttachToCharacter(m_playerPawnId);
	//		//	}
	//		//	else
	//		//	{
	//		//		gEnv->pLog->LogAlways("Failed to spawn a player pawn.");
	//		//	}
	//		//}
	//		//else
	//		//{
	//		//	// NOTE: this should remove the pawn, but it doesn't.
	//		//	if (m_playerPawnId != INVALID_ENTITYID)
	//		//	{
	//		//		pActorSystem->RemoveActor(m_playerPawnId);
	//		//		m_playerPawnId = INVALID_ENTITYID;
	//		//	}
	//		//}
	//	}
	//}

	return ok;
}


IEntity* CEditorGame::GetPlayer()
{
	return gEnv->pEntitySystem->GetEntity(LOCAL_PLAYER_ENTITY_ID);
}


void CEditorGame::SetPlayerPosAng(Vec3 pos, Vec3 viewDir)
{
	IEntity* pPlayer = GetPlayer();
	if (pPlayer)
	{
		pPlayer->SetPosRotScale(pos, Quat::CreateRotationVDir(viewDir), Vec3(1.0f), ENTITY_XFORM_EDITOR);
	}
}


void CEditorGame::HidePlayer(bool bHide)
{
	auto *pPlayer = GetPlayerActor();
	if (pPlayer)
	{
		IEntity &playerEntity = *pPlayer->GetEntity();

		playerEntity.Hide(bHide);

		playerEntity.InvalidateTM();

		if (!bHide)
		{
			pPlayer->SetHealth(pPlayer->GetMaxHealth());
		}
	}
}


IActor *CEditorGame::GetPlayerActor()
{
	return gEnv->pGame->GetIGameFramework()->GetClientActor();
}


bool CEditorGame::ConfigureNetContext(bool on)
{
	bool ok = false;

	IGameFramework* pGameFramework = m_pGame->GetIGameFramework();

	if (on == m_bEnabled)
	{
		ok = true;
	}
	else if (on)
	{
		SGameContextParams ctx;

		SGameStartParams gameParams;
		gameParams.flags = eGSF_Server
			| eGSF_NoSpawnPlayer
			| eGSF_Client
			| eGSF_NoLevelLoading
			| eGSF_BlockingClientConnect
			| eGSF_NoGameRules
			| eGSF_NoQueries;

		gameParams.flags |= eGSF_LocalOnly;
		gameParams.connectionString = "";
		gameParams.hostname = "localhost";
		gameParams.port = 60695;
		gameParams.pContextParams = &ctx;
		gameParams.maxPlayers = 1;

		if (pGameFramework->StartGameContext(&gameParams))
			ok = true;
	}
	else
	{
		pGameFramework->EndGameContext();
		gEnv->pNetwork->SyncWithGame(eNGS_Shutdown);
		ok = true;
	}

	m_bEnabled = on && ok;

	return ok;
}


void CEditorGame::OnBeforeLevelLoad()
{
	EnablePlayer(false);
	ConfigureNetContext(true);

	// Load the default game rules.
	ICVar *pDefaultGameRulesVar = gEnv->pConsole->GetCVar("sv_gamerulesdefault");
	m_pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules(pDefaultGameRulesVar->GetString());

	// Dispatch to all ILevelSystemListeners that a level has started to load.
	gEnv->pGame->GetIGameFramework()->GetILevelSystem()->OnLoadingStart(0);
}


void CEditorGame::OnAfterLevelLoad(const char *levelName, const char *levelFolder)
{
	ILevelInfo* pLevel = m_pGame->GetIGameFramework()->GetILevelSystem()->SetEditorLoadedLevel(levelName);
	m_pGame->GetIGameFramework()->GetILevelSystem()->OnLoadingComplete(pLevel);
	EnablePlayer(true);
}


IFlowSystem* CEditorGame::GetIFlowSystem()
{
	return m_pGame->GetIGameFramework()->GetIFlowSystem();
}


IGameTokenSystem* CEditorGame::GetIGameTokenSystem()
{
	return m_pGame->GetIGameFramework()->GetIGameTokenSystem();
}
