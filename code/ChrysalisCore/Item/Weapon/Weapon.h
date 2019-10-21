#pragma once

#include <IWeapon.h>
#include "Components/Items/ItemComponent.h"


namespace Chrysalis
{
class CWeapon :
	public CItemComponent, public IWeapon
{
public:
	CWeapon();
	~CWeapon();

	virtual IWeapon *GetIWeapon() { return this; };
};
}