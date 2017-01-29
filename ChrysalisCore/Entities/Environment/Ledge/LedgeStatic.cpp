#include <StdAfx.h>

#include "LedgeStatic.h"


CLedgeObjectStatic::CLedgeObjectStatic()
{
	CRY_ASSERT_MESSAGE(gEnv->IsEditor(), "Static ledge object should only be instantiated in the editor!");
}


CLedgeObjectStatic::~CLedgeObjectStatic()
{}
