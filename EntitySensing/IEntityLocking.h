#pragma once

#include <IGameObject.h>


struct IEntityLocking : public IGameObjectExtension
{
	virtual bool IsUsable(EntityId entityId) const = 0;
	virtual bool IsLocked() const = 0;
	virtual int GetLockedEntityId() const = 0;
	virtual void SetQueryMethods(char* pMethods) = 0;
	virtual int GetOverEntityId() const = 0;
};
