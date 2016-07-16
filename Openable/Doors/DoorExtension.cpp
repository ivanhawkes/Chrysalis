#include <StdAfx.h>

#include "DoorExtension.h"


// ***
// *** IGameObjectExtension
// ***


void CDoorExtension::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}


bool CDoorExtension::Init(IGameObject * pGameObject)
{
	// Stores the specified IGameObject in this instance.
	SetGameObject(pGameObject);

	// Initialization successful.
	return true;
}


void CDoorExtension::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);
}


bool CDoorExtension::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// It's very important that this gets called. Restores the IGameObject of the instance.
	ResetGameObject();

	return true;
}

void CDoorExtension::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	auto pEntity = GetEntity();
	if (!pEntity)
		return;
}


// ***
// *** CDoorExtension
// ***


CDoorExtension::CDoorExtension()
{}


CDoorExtension::~CDoorExtension()
{}