#include <StdAfx.h>

#include "DoorExtension.h"


// ***
// *** IGameObjectExtension
// ***


void CDoorExtension::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);
}


void CDoorExtension::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// ***
// *** CDoorExtension
// ***
