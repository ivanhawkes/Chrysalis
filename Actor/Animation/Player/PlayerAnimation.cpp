#include <StdAfx.h>

#include "PlayerAnimation.h"
#include "Game/Game.h"


SMannequinPlayerParams PlayerMannequin;


void InitPlayerMannequin(IActionController* pActionController)
{
	CRY_ASSERT(g_pGame);
	IGameFramework* pGameFramework = g_pGame->GetIGameFramework();

	CRY_ASSERT(pGameFramework);
	CMannequinUserParamsManager& mannequinUserParams = pGameFramework->GetMannequinInterface().GetMannequinUserParamsManager();
	mannequinUserParams.RegisterParams< SMannequinPlayerParams >(pActionController, &PlayerMannequin);
}