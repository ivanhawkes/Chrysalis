#include <StdAfx.h>

#include <CryGame/IGameStartup.h>
#include <CryGame/IGameFramework.h>
#include <IActorSystem.h>
#include <IGameRulesSystem.h>
#include <ILevelSystem.h>
#include <IItemSystem.h>
#include <IForceFeedbackSystem.h>
//#include <IMovieSystem.h>
#include <CryEntitySystem/IEntityPoolManager.h>
#include <Actor/Player/Player.h>
#include <Actor/Character/Character.h>
#include "EditorGame.h"
#include "Game.h"


#define EDITOR_SERVER_PORT 0xed17


static ICVar* g_pEditorGameMode = nullptr;


static CEditorGame* g_pEditorGame = nullptr;


extern "C"
{
	/// <summary>    Forward Declaration (SEE Startup.cpp For Implementation). </summary>
	GAME_API IGameStartup *CreateGameStartup();


	/**
	Automatically Called By The Sandbox Editor When It Is Time To Create An Instance Of The IEditorGame Interface.
	This Function Is Very Important And Must Exist In Order For The CRYENGINE Sandbox Editor To Create An IEditorGame
	Instance.  This Function Should Not Be Called Manually.

	\return    The Newly Created CEditorGame Instance.
	*/
	GAME_API IEditorGame *CreateEditorGame()
	{
		return new CEditorGame();
	}
};


CEditorGame::CEditorGame() :
m_pGameStartup(nullptr),
m_GameRef(nullptr),
m_bNetContext(false),
m_bGameMode(false),
m_bPlayer(false),
m_bMultiPlayerGameRules(false)
{
	g_pEditorGame = this;
}


CEditorGame::CEditorGame(const char* pBinDir) :
m_pGameStartup(nullptr),
m_GameRef(nullptr),
m_bNetContext(false),
m_bGameMode(false),
m_bPlayer(false),
m_bMultiPlayerGameRules(false)
{
	g_pEditorGame = this;
}


CEditorGame::~CEditorGame()
{
	// TODO: Add implmentation of IEquipmentSystemInterface. (SAFE_DELETE)
}


void CEditorGame::ResetClient(IConsoleCmdArgs* pConsoleCommandArgs)
{
	// Store whether the player is enabled or not.
	bool bOldPlayerValue = g_pEditorGame->m_bPlayer;

	// Disable the player.
	g_pEditorGame->EnablePlayer(false);

	// Find the Player class and load it's script.
	IEntityClass *pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Player");
	if (pClass)
		pClass->LoadScript(true);

	// If the player was originally enabled before the reset, then re-start the networked game context and game rules.
	if (bOldPlayerValue)
	{
		// Start A Networked Game Context.
		g_pEditorGame->EnableNetContext(true);

		// TODO: We obviously need to support more game rules here or at least support the ones we have.

		if (g_pEditorGame->m_bMultiPlayerGameRules)
		{
			// Create The MultiPlayer Game Rules.
			gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules("MultiPlayer");
			gEnv->pLog->LogAlways("Created MultiPlayer Game Rules!");
		}
		else
		{
			// Create The SinglePlayer Game Rules.
			//gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules("SinglePlayer");
			//gEnv->pLog->LogAlways("Created SinglePlayer Game Rules!");
			gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules("SoloExploration");
			gEnv->pLog->LogAlways("Created SoloExploration Game Rules!");
		}
	}

	// Re-enable/disable the player.
	g_pEditorGame->EnablePlayer(bOldPlayerValue);

	// Hide the player.
	g_pEditorGame->HidePlayer(true);
}


void CEditorGame::OnChangeEditorGameMode(ICVar* pCVar)
{
	CRY_ASSERT(pCVar == g_pEditorGameMode);
	if (g_pEditorGame)
		// Enable / disable editor "game mode" as requested.
		g_pEditorGame->SetGameMode(g_pEditorGame->m_bGameMode);
}


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
	if (!m_pGameStartup)
		return false;

	// Initializes the game startup instance which in turn creates and initializes a CGame instance. 
	m_GameRef = m_pGameStartup->Init(startupParams);
	if (!m_GameRef)
		return false;

	// Initialize the sandbox editor using the IGameFramework.
	m_GameRef->GetIGameFramework()->InitEditor(pGameToEditorInterface);

	// Lets all systems know that we are acting as a server.
	gEnv->bServer = true;

	// Lets all systems know that we are not in multi-player Mode.
	gEnv->bMultiplayer = false;

	// Lets all systems know that we are also acting as a client.
	gEnv->SetIsClient(true);

	// Registers The "net_gamemode" console variable and tells it what function to call to when the value changes.
	g_pEditorGameMode = REGISTER_INT_CB("net_gamemode", 0, VF_NULL, "Enables/Disables Editor Game Mode", &OnChangeEditorGameMode);

	// Makes sure we are not in game mode by setting it off.
	SetGameMode(false);

	// Registers the "net_reseteditorclient" console command and tells it what function to call to handle it.
	REGISTER_COMMAND("net_reseteditorclient", ResetClient, VF_NULL, "Resets the player and game rules!");

	// Starts a networked game context.
	EnableNetContext(true);

	// TODO: Add implmentation of IEquipmentSystemInterface. (init)

	// Initialization was successful.
	return true;
}


int CEditorGame::Update(bool haveFocus, unsigned int updateFlags)
{
	// Update the CGame instance.
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


bool CEditorGame::SetGameMode(bool bGameMode)
{
	if (bGameMode)
		// Log that we are entering "game mode".
		gEnv->pLog->LogAlways("Entering Game Mode...");
	else
		// Log that we are exiting "game mode".
		gEnv->pLog->LogAlways("Exiting Game Mode...");

	m_bGameMode = bGameMode;
	bool on = bGameMode;
	if (g_pEditorGameMode->GetIVal() == 0)
		on = m_bPlayer;

	// Start or stop a networked game context.
	bool ok = EnableNetContext(on);
	if (ok)
	{
		// Successfully started a networked game context.

		// If we are in the sandbox editor, notify the currently active game that the editor's "game mode" has been switched.
		if (gEnv->IsEditor())
			gEnv->pGame->EditorResetGame(bGameMode);

		// Notify the IGameFramework that the editor's "game mode" has been switched.
		IGameFramework * pGameFramework = gEnv->pGame->GetIGameFramework();
		pGameFramework->OnEditorSetGameMode(bGameMode);

		if (m_bGameMode)
			// Log that we have successfully entered "game mode".
			gEnv->pLog->LogAlways("Successfully entered game mode!");
		else
			// Log that we have successfully exited "game mode".
			gEnv->pLog->LogAlways("Successfully exited game mode!");
	}
	else
	{
		if (on)
			// Log that we have failed to enable a networked game context.
			gEnv->pLog->LogWarning("Failed to enable net game context");
		else
			// Log that we have failed to disable a networked game context.
			gEnv->pLog->LogWarning("Failed to disable net game context");

		if (m_bGameMode)
			// Log that we have failed to enter "game mode".
			gEnv->pLog->LogWarning("Failed to enter game mode!");
		else
			// Log that we have failed to exit "game mode".
			gEnv->pLog->LogWarning("Failed to exit game mode!");
	}

	return ok;
}


IEntity* CEditorGame::GetPlayer()
{
	// Gets the local player
	auto pPlayer = CPlayer::GetLocalPlayer();

	return pPlayer ? pPlayer->GetEntity() : nullptr;
}


void CEditorGame::SetPlayerPosAng(Vec3 pos, Vec3 viewDir)
{
	// Gets the client actor.
	auto pPlayer = m_GameRef->GetIGameFramework()->GetClientActor();
	if (!pPlayer)
		return;

	// Gets the client actor's entity.
	auto pEntity = pPlayer->GetEntity();
	if (!pEntity)
		return;

	// Create a rotation from the specified view direction.
	auto CamRot = Ang3(Quat::CreateRotationVDir(viewDir));

	// Do not rotate the player in the x direction.
	CamRot.x = 0;

	// Set the players position, rotation, and scale at the same time. It is very important that you specify the "Why"
	// flags so that the sandbox editor can get notified about the transformation changes.
	// This has a hard-coded value for player height, which it uses to translate downwards - presumably because
	// the input pos is a camera position.
	pEntity->SetPosRotScale(pos /*- Vec3(0.0f, 0.0f, 1.88f)*/, Quat(CamRot), pEntity->GetScale(),
		ENTITY_XFORM_EDITOR | ENTITY_XFORM_POS | ENTITY_XFORM_ROT | ENTITY_XFORM_SCL);
}


void CEditorGame::HidePlayer(bool bHide)
{
	// TODO: We don't really have to hide the player, since we never display them.
	// Instead, this should give up control of the attached character, if there is one by this stage.

	// Gets the local player.
	//auto pEntity = CPlayer::GetLocalCharacter()->GetEntity();

	//// Hide or unhide the player entity.
	//if (pEntity)
	//	pEntity->Hide(bHide);
}


void CEditorGame::OnBeforeLevelLoad()
{
	// Disable the player.
	EnablePlayer(false);

	// Start a networked game context.
	EnableNetContext(true);

	// TODO: Is this the best place for it, since we create these rules in other places as well?
	// Create the specified game rules.
	//gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules("SinglePlayer");
	gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules("SoloExploration");
	
	// Dispatch to all ILevelSystemListeners that a level has started to load.
	gEnv->pGame->GetIGameFramework()->GetILevelSystem()->OnLoadingStart(0);
}


void CEditorGame::OnAfterLevelInit(const char *levelName, const char *levelFolder)
{
	// TODO: needs implementation.
	//InitEntityArchetypeEnums(m_pGTE, levelFolder, levelName);

	// Enable the player.
	EnablePlayer(true);
}


void CEditorGame::OnAfterLevelLoad(const char *levelName, const char *levelFolder)
{
	gEnv->pGame->GetIGameFramework()->GetILevelSystem()->Rescan("levels", ILevelSystem::TAG_MAIN);

	ILevelInfo* pLevel = gEnv->pGame->GetIGameFramework()->GetILevelSystem()->SetEditorLoadedLevel(levelName);
	gEnv->pGame->GetIGameFramework()->GetILevelSystem()->OnLoadingComplete(pLevel);
}


void CEditorGame::OnCloseLevel()
{}


void CEditorGame::OnSaveLevel()
{}


bool CEditorGame::BuildEntitySerializationList(XmlNodeRef output)
{
	return true;
}


bool CEditorGame::GetAdditionalMinimapData(XmlNodeRef output)
{
	return true;
}


IFlowSystem* CEditorGame::GetIFlowSystem()
{
	return m_GameRef ? m_GameRef->GetIGameFramework()->GetIFlowSystem() : nullptr;
}


IGameTokenSystem* CEditorGame::GetIGameTokenSystem()
{
	return m_GameRef ? m_GameRef->GetIGameFramework()->GetIGameTokenSystem() : nullptr;
}


IEquipmentSystemInterface* CEditorGame::GetIEquipmentSystemInterface()
{
	return nullptr;
}


bool CEditorGame::SupportsMultiplayerGameRules()
{
	return gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->HaveGameRules("MultiPlayer");
}


void CEditorGame::ToggleMultiplayerGameRules()
{
	m_bMultiPlayerGameRules = !m_bMultiPlayerGameRules;

	// Store whether the player is enabled or not.
	bool bOldPlayerSetting = m_bPlayer;

	// Disable the player.
	EnablePlayer(false);

	// Start a networked game context.
	EnableNetContext(true);

	// Find the player class and load it's script.
	IEntityClass *pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Player");
	if (pClass)
		pClass->LoadScript(true);

	
	// TODO: WTF - we're creating game rules again. Need to DRY principle this stuff.
	if (m_bMultiPlayerGameRules)
	{
		// Create the multiplayer game rules.
		gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules("MultiPlayer");
		gEnv->pLog->LogAlways("Created multiplayer game rules!");
	}
	else
	{
		// Create the singleplayer game rules.
		//gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules("SinglePlayer");
		//gEnv->pLog->LogAlways("Created singleplayer game rules!");
		gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules("SoloExploration");
		gEnv->pLog->LogAlways("Created SoloExploration game rules!");
	}

	// Re-enable / disable the player.
	EnablePlayer(bOldPlayerSetting);

	// Hide the player.
	HidePlayer(true);

	// Hide the console if it is showing.
	gEnv->pConsole->ShowConsole(false);
}


void CEditorGame::RegisterTelemetryTimelineRenderers(Telemetry::ITelemetryRepository* pRepository)
{}


void CEditorGame::OnDisplayRenderUpdated(bool displayHelpers)
{}


bool CEditorGame::EnableNetContext(bool bEnable)
{
	bool ok = false;

	IGameFramework * pGameFramework = gEnv->pGame->GetIGameFramework();
	if (bEnable == m_bNetContext)
		ok = true;
	else if (bEnable)
	{
		SGameContextParams ctx;

		SGameStartParams gameParams;
		gameParams.flags = eGSF_Server		// We are the server.
			| eGSF_NoSpawnPlayer			// The player should not be spawned automatically,  leave it up to us to do manually.
			| eGSF_Client					// We are the client.
			| eGSF_NoLevelLoading			// Dynamic level loading is not allowed.
			| eGSF_BlockingClientConnect	// When connecting to the server / client, block. Don't continue until a connection is made.
			| eGSF_NoGameRules				// The game rules should not be spawned automatically,  leave it up to use to do manually.
			| eGSF_NoQueries;				// Do not allow queries.

		if (m_bMultiPlayerGameRules)
		{
			// We are using multiplayer mode.
			gameParams.flags |= eGSF_ImmersiveMultiplayer;
			gEnv->pLog->LogAlways("Using multi-player game rules!");
		}
		else
		{
			// We are using singleplayer mode.
			gameParams.flags |= eGSF_LocalOnly;
			gEnv->pLog->LogAlways("Using single player game rules!");
		}

		gameParams.connectionString = "";

		// We are hosting the server on our local machine.
		gameParams.hostname = "localhost";
		gameParams.port = EDITOR_SERVER_PORT;
		gameParams.pContextParams = &ctx;

		// Do not allow other clients to join our server.
		gameParams.maxPlayers = 1;

		// Start the networked game context.
		gEnv->pLog->LogAlways("Starting Game Context...");
		if (pGameFramework->StartGameContext(&gameParams))
		{
			gEnv->pLog->LogAlways("Successfully Started Game Context!");
			ok = true;
		}
		else
			gEnv->pLog->LogWarning("Failed To Start Game Context!");
	}
	else
	{
		// End the networked game context.
		gEnv->pLog->LogAlways("Ending Game Context...");
		pGameFramework->EndGameContext();

		// Sync this change with the game.
		gEnv->pNetwork->SyncWithGame(eNGS_Shutdown);
		gEnv->pLog->LogAlways("Successfully Ended Game Context!");
		ok = true;
	}

	m_bNetContext = bEnable && ok;
	return ok;
}


void CEditorGame::EnablePlayer(bool bEnable)
{
	bool spawnPlayer = false;
	if (m_bPlayer != bEnable)
		spawnPlayer = m_bPlayer = bEnable;

	// Sets the editor's "game mode".
	if (!SetGameMode(m_bGameMode))
		gEnv->pLog->LogAlways("Failed To Set Game Mode!");
	else if (m_bNetContext && spawnPlayer)
	{
		// Spawn the player.
		gEnv->pLog->LogAlways("Spawning Player...");
		if (!gEnv->pGame->GetIGameFramework()->BlockingSpawnPlayer())
			gEnv->pLog->LogWarning("Failed To Spawn Player!");
		else
			gEnv->pLog->LogAlways("Successfully Spawned Player!");
	}
}