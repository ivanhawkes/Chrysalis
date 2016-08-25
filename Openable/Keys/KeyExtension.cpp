#include <StdAfx.h>

#include "KeyExtension.h"


// ***
// *** IGameObjectExtension
// *** 


void CKeyExtension::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);
}


void CKeyExtension::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// ***
// *** CKeyExtension
// ***
