#include <StdAfx.h>

#include "ContainerExtension.h"


// ***
// *** IGameObjectExtension
// ***


void CContainerExtension::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);
}


void CContainerExtension::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// ***
// *** CContainerExtension
// ***
