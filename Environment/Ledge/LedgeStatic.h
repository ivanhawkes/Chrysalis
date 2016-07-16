#pragma once

#include <IGameObject.h>
#include "Ledge.h"


class CLedgeObjectStatic : public CLedgeObject
{
public:
	CLedgeObjectStatic();
	virtual ~CLedgeObjectStatic();

protected:
	virtual bool IsStatic() const { return true; }
};
