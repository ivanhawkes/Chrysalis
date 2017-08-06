#pragma once

#include "ICryMannequin.h"

namespace Chrysalis
{
typedef TAction<SAnimationContext> CAnimationAction;


template< typename T >
const T* GetMannequinUserParams(const SAnimationContext& context)
{
	CMannequinUserParamsManager& mannequinUserParams = gEnv->pGameFramework->GetMannequinInterface().GetMannequinUserParamsManager();
	const T* const pUserParams = mannequinUserParams.FindOrCreateParams<T>(context.controllerDef);
	CRY_ASSERT(pUserParams);

	return pUserParams;
}
}