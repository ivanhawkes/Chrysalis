#include <StdAfx.h>

#include "EntityInventory.h"
#include "IItem.h"
//#include "IWeapon.h"


// ***
// *** IGameObjectExtension
// ***


void CEntityInventory::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}


bool CEntityInventory::Init(IGameObject * pGameObject)
{
	// Stores the specified IGameObject in this instance.
	SetGameObject(pGameObject);

	// Initialization successful.
	return true;
}


void CEntityInventory::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);
}


bool CEntityInventory::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// It's very important that this gets called. Restores the IGameObject of the instance.
	ResetGameObject();

	return true;
}

void CEntityInventory::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	auto pEntity = GetEntity();
	if (!pEntity)
		return;
}


// ***
// *** IInventory
// ***


bool CEntityInventory::AddItem(EntityId itemId)
{
	bool bWasItemAdded(false);
	// TODO:
	return bWasItemAdded;
}


bool CEntityInventory::RemoveItem(EntityId itemId)
{
	bool bWereItemsRemoved(false);
	
	// TODO:
	return bWereItemsRemoved;
}


void CEntityInventory::RemoveAllItems(bool forceClear)
{
	// TODO:
}


void CEntityInventory::Destroy()
{
	// TODO:
}


void CEntityInventory::Clear(bool forceClear)
{
	// TODO:
}


int CEntityInventory::FindItem(EntityId itemId) const
{
	// TODO:
	return 0;
}


void CEntityInventory::ActorBackpackInfo::SetExpansionCounter(int expansionCounter)
{
	// We don't want to allow more then 5 bag expansions. We can't have the character carrying anything they want.
	MAX(5, expansionCounter);
}


// ***
// *** CEntityInventory
// ***


CEntityInventory::CEntityInventory()
{}


CEntityInventory::~CEntityInventory()
{}


