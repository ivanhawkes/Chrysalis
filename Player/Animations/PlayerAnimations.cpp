#include <StdAfx.h>

#include "PlayerAnimations.h"
#include <CryAnimation/ICryAnimation.h>
#include <ICryMannequin.h>
#include <CryGame\IGameFramework.h>

SMannequinPlayerParams PlayerMannequin;


void InitPlayerMannequin(IActionController* pActionController)
{
	// TODO: Update to newer code from TPS or other template.
	auto& mannequinUserParams = gEnv->pGameFramework->GetMannequinInterface().GetMannequinUserParamsManager();
	mannequinUserParams.RegisterParams< SMannequinPlayerParams >(pActionController, &PlayerMannequin);
}