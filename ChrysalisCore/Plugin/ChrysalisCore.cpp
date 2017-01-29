#include "StdAfx.h"

#include "Plugin/ChrysalisCore.h"
#include <CrySystem/ISystem.h>
#include <CryGame/IGameFramework.h>
#include <IItemSystem.h>
#include <CryExtension/ICryPluginManager.h>
#include <CryCore/Assert/CryAssert.h>
#include <ObjectID/ObjectIdMasterFactory.h>
#include "Player/Player.h"
#include "ConsoleCommands/ConsoleCommands.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include "DynamicResponseSystem/ConditionDistanceToEntity.h"
#include "DynamicResponseSystem/ActionClose.h"
#include "DynamicResponseSystem/ActionLock.h"
#include "DynamicResponseSystem/ActionOpen.h"
#include "DynamicResponseSystem/ActionPlayAnimation.h"
#include "DynamicResponseSystem/ActionSwitch.h"
#include "DynamicResponseSystem/ActionUnlock.h"


const CChrysalisCore::SExternalCVars& CChrysalisCore::GetCVars() const { return m_cvars; }
CChrysalisCore::SExternalCVars& CChrysalisCore::SetCVars() { return m_cvars; }


CChrysalisCore::CChrysalisCore()
{
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);

	RegisterCVars();

	// Create a valid master factory which can provide instance unique Ids for us.
	// #TODO: Get the InstanceId from the command line or cvars.
	m_pObjectIdMasterFactory = new CObjectIdMasterFactory(0);

	// Register Game-Specific Console Commands.
	RegisterGameConsoleCommands();

}


CChrysalisCore::~CChrysalisCore()
{
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	// Un-Registers Game-Specific Console Commands.
	UnRegisterGameConsoleCommands();

	SAFE_DELETE(m_pObjectIdMasterFactory);
}


void CChrysalisCore::RegisterCVars()
{
	// #TODO: Maybe this is deprecated now?
	REGISTER_CVAR2("game_rayCastQuota", &m_cvars.m_rayCastQuota, 64, VF_CHEAT, "Number of allowed deferred raycasts.");

	// Mouse movements.
	REGISTER_CVAR2("cl_invertPitch", &m_cvars.m_cl_invertPitch, false, VF_CHEAT, "Should we invert the Y axis for camera movements? This is preferred by some players, particularly those using a flight yoke.");
	REGISTER_CVAR2("cl_mouseSensitivity", &m_cvars.m_cl_mouseSensitivity, 1.0f, VF_CHEAT, "Overall mouse sensitivity. This should be factored into any movements involving the mouse.");

	// Watch Debug
	REGISTER_CVAR2("watch_enabled", &m_cvars.m_watch_enabled, true, VF_CHEAT, "Is watch debug enabled?");
	REGISTER_CVAR2("watch_text_render_start_pos_x", &m_cvars.m_watch_text_render_start_pos_x, 35.0f, VF_CHEAT, "The watch text render start position x coordinate.");
	REGISTER_CVAR2("watch_text_render_start_pos_y", &m_cvars.m_watch_text_render_start_pos_y, 18.0f, VF_CHEAT, " The watch text render start position y coordinate.");
	REGISTER_CVAR2("watch_text_render_size", &m_cvars.m_watch_text_render_size, 1.75f, VF_CHEAT, "Size at which the watch text will render.");
	REGISTER_CVAR2("watch_text_render_lineSpacing", &m_cvars.m_watch_text_render_lineSpacing, 9.3f, VF_CHEAT, "Line spacing for watch text.");
	REGISTER_CVAR2("watch_text_render_fxscale", &m_cvars.m_watch_text_render_fxscale, 13.0f, VF_CHEAT, "The watch text render fxscale.");

	// Deprecate this or move it.
	REGISTER_CVAR2("ladder_logVerbosity", &m_cvars.m_ladder_logVerbosity, 0, VF_CHEAT, "Ladder logging.");
}


void CChrysalisCore::RegisterGameConsoleCommands()
{
	// Registers all the console commands.
	m_pConsoleCommands = new CConsoleCommands();
	m_pConsoleCommands->Register();
}


void CChrysalisCore::UnRegisterGameConsoleCommands()
{
	// Un-registers all the console commands.
	m_pConsoleCommands->Unregister();
	SAFE_DELETE(m_pConsoleCommands);
}


void CChrysalisCore::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		case ESYSTEM_EVENT_GAME_POST_INIT:
			// Search the file system to find XML files with definitions for game items.
			gEnv->pGameFramework->GetIItemSystem()->Scan("Parameters/Items");

			// Search the file system to find XML files with definitions for game weapons.
			gEnv->pGameFramework->GetIActorSystem()->Scan("Parameters/Actors");

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

			gEnv->pConsole->ExecuteString("map example", false, true);
			break;

		case ESYSTEM_EVENT_LEVEL_LOAD_END:
		{
			auto pPlayer = CPlayer::GetLocalPlayer();
			if (pPlayer)
				pPlayer->AttachToCharacter();
		}
			break;
	}
}
