#pragma once

#include <IItemSystem.h>
#include <IWeapon.h>
#include "Item/Item.h"


class CWeapon :
	public CItem, public IWeapon
{
public:
	CWeapon();
	~CWeapon();

	virtual IWeapon *GetIWeapon() { return this; };
};

