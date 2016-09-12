#pragma once

#include <Entities/Helpers/NativeEntityBase.h>


struct ISwitchComponent : public CNativeEntityBase
{
	virtual void SwitchOn() = 0;
	virtual void SwitchOff() = 0;
};
