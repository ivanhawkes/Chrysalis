// CryEngine Source File
// Copyright (C), Crytek, 1999-2016


#include "StdAfx.h"
#include "Game.h"
#include "GameFactory.h"
#include <IPlayerProfiles.h>
#include <IItemSystem.h>


CGame* g_pGame { nullptr };


class CGameRegistrator
	: public IEntityRegistrator
	, public CGame::SExternalCVars
{
	virtual void Register() override
	{
		REGISTER_CVAR2("game_rayCastQuota", &m_rayCastQuota, 64, VF_CHEAT, "Number of allowed deferred raycasts.");

		// Mouse movements.
		REGISTER_CVAR2("cl_invertPitch", &m_cl_invertPitch, false, VF_CHEAT, "Should we invert the Y axis for camera movements? This is preferred by some players, particularly those using a flight yoke.");
		REGISTER_CVAR2("cl_mouseSensitivity", &m_cl_mouseSensitivity, 1.0f, VF_CHEAT, "Overall mouse sensitivity. This should be factored into any movements involving the mouse.");

		// Watch Debug
		REGISTER_CVAR2("watch_enabled", &m_watch_enabled, true, VF_CHEAT, "Is watch debug enabled?");
		REGISTER_CVAR2("watch_text_render_start_pos_x", &m_watch_text_render_start_pos_x, 35.0f, VF_CHEAT, "The watch text render start position x coordinate.");
		REGISTER_CVAR2("watch_text_render_start_pos_y", &m_watch_text_render_start_pos_y, 18.0f, VF_CHEAT, " The watch text render start position y coordinate.");
		REGISTER_CVAR2("watch_text_render_size", &m_watch_text_render_size, 1.75f, VF_CHEAT, "Size at which the watch text will render.");
		REGISTER_CVAR2("watch_text_render_lineSpacing", &m_watch_text_render_lineSpacing, 9.3f, VF_CHEAT, "Line spacing for watch text.");
		REGISTER_CVAR2("watch_text_render_fxscale", &m_watch_text_render_fxscale, 13.0f, VF_CHEAT, "The watch text render fxscale.");

		// Deprecate this or move it.
		REGISTER_CVAR2("ladder_logVerbosity", &m_ladder_logVerbosity, 0, VF_CHEAT, "Ladder logging.");
	}
};

CGameRegistrator g_gameRegistrator;



CGame::CGame()
{
	// notifies all CRYENGINE systems about our game class.
	GetISystem()->SetIGame(this);

	// Set our global game pointer.
	g_pGame = this;
}


CGame::~CGame()
{
	if (m_pGameFramework->StartedGameContext())
	{
		m_pGameFramework->EndGameContext();
	}

	if (m_pRayCaster)
	{
		m_pRayCaster->Reset();
		SAFE_DELETE(m_pRayCaster);
	}


	GetISystem()->SetIGame(nullptr);
	g_pGame = nullptr;
}


bool CGame::Init(IGameFramework* pFramework)
{
	m_pGameFramework = pFramework;
	CGameFactory::Init();
	m_pGameFramework->SetGameGUID(GAME_GUID);

	// We hafve to initialize the player profile system
	// Otherwise level load will fail in action map initialization
	InitializePlayerProfile();

	// Create a new deferred ray-cast object and set it's quota limit.
	m_pRayCaster = new GlobalRayCaster;
	m_pRayCaster->SetQuota(GetCVars().m_rayCastQuota);

	// Search the file system to find XML files with definitions for game items.
	m_pGameFramework->GetIItemSystem()->Scan("Parameters/Items");

	// Search the file system to find XML files with definitions for game weapons.
	//m_pGameFramework->GetIActorSystem()->Scan("Parameters/Actors");

	// Load and init the default action map profile.
	IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	pActionMapManager->InitActionMaps("libs/config/defaultprofile.xml");
	pActionMapManager->Enable(true);

	return true;
}


void CGame::InitializePlayerProfile()
{
	auto &profileManager = *m_pGameFramework->GetIPlayerProfileManager();

	profileManager.Initialize();

	bool bFirstLogin = true;
	if (!profileManager.LoginUser("Player", bFirstLogin))
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to login player profile user.");
		return;
	}

	profileManager.ActivateProfile("Player", "default");
}


void CGame::RegisterGameFlowNodes()
{
	CGameFactory::RegisterFlowNodes();
}


int CGame::Update(bool haveFocus, unsigned int updateFlags)
{
	float frameTime = gEnv->pTimer->GetFrameTime();
	const bool bRun = m_pGameFramework->PreUpdate(haveFocus, updateFlags);

	// Allow the deferred ray-caster to perform it's update.
	if (m_pRayCaster)
	{
		FRAME_PROFILER("GlobalRayCaster", gEnv->pSystem, PROFILE_AI);
		m_pRayCaster->Update(frameTime);
	}

	// Post update.
	m_pGameFramework->PostUpdate(haveFocus, updateFlags);

	return bRun ? 1 : 0;
}


const char* CGame::GetName()
{
	ICVar *pGameName = gEnv->pConsole->GetCVar("sys_game_name");

	return pGameName->GetName();
}


const CGame::SExternalCVars &CGame::GetCVars() const
{
	return g_gameRegistrator;
}


CPlayer* CGame::GetLocalPlayer()
{
	CRY_ASSERT_MESSAGE(g_pGame, "There is no game.");
	return static_cast<CPlayer*>(g_pGame->GetIGameFramework()->GetClientActor());
}
