#include <StdAfx.h>

#include "LockExtension.h"


// ***
// *** IGameObjectExtension
// ***


void CLockExtension::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);
}


void CLockExtension::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// ***
// *** CLockExtension
// ***
