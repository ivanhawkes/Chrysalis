#include <StdAfx.h>

#include "GameActionMaps.h"

// NOTE: This definition is different to the one in the header.
#define DECL_ACTION(name) name = #name;

CGameActionMaps::CGameActionMaps()
{
	// Initialise all the actions, using the text file as input.
#include "Actions.txt"
}


CGameActionMaps::~CGameActionMaps()
{
	IActionMapManager* pActionMapManager = g_pGame->GetIGameFramework()->GetIActionMapManager();

	if (pActionMapManager)
	{
		pActionMapManager->UnregisterActionMapEventListener(this);
	}
}


void CGameActionMaps::OnActionMapEvent(const SActionMapEvent& event)
{
	if (event.event == SActionMapEvent::eActionMapManagerEvent_ActionMapsInitialized)
	{
		CreateActionFilters();
	}
}


void CGameActionMaps::CreateActionFilters()
{
}
