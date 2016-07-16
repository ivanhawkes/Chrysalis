#include <StdAfx.h>

#include "LockExtension.h"


// ***
// *** IGameObjectExtension
// ***


void CLockExtension::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}


bool CLockExtension::Init(IGameObject * pGameObject)
{
	// Stores the specified IGameObject in this instance.
	SetGameObject(pGameObject);

	// Initialization successful.
	return true;
}


void CLockExtension::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);
}


bool CLockExtension::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// It's very important that this gets called. Restores the IGameObject of the instance.
	ResetGameObject();

	return true;
}

void CLockExtension::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	auto pEntity = GetEntity();
	if (!pEntity)
		return;
}


// ***
// *** CLockExtension
// ***


CLockExtension::CLockExtension()
{}


CLockExtension::~CLockExtension()
{}