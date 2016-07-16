#include "StdAfx.h"

#include "GameObjectExtensionTemplate.h"


// ***
// *** IGameObjectExtension
// ***


void CGameObjectExtensionTemplate::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}


bool CGameObjectExtensionTemplate::Init(IGameObject * pGameObject)
{
	// Stores the specified IGameObject in this instance.
	SetGameObject(pGameObject);

	// Initialization successful.
	return true;
}


void CGameObjectExtensionTemplate::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);
}


bool CGameObjectExtensionTemplate::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// It's very important that this gets called. Restores the IGameObject of the instance.
	ResetGameObject();

	return true;
}

void CGameObjectExtensionTemplate::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	auto pEntity = GetEntity();
	if (!pEntity)
		return;
}


// ***
// *** CGameObjectExtensionTemplate
// ***


CGameObjectExtensionTemplate::CGameObjectExtensionTemplate()
{}


CGameObjectExtensionTemplate::~CGameObjectExtensionTemplate()
{}