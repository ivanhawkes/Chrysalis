#include <StdAfx.h>

#include "ScriptBindItem.h"
#include <Game/Game.h>
#include "Item/Item.h"


CScriptBindItem::CScriptBindItem()
{
	// Initializes this instance by storing the passed-in values and creating a table that will hold this instance's
	// registered methods and variables.
	Init(gEnv->pSystem->GetIScriptSystem(), gEnv->pSystem);

	// Creates the "Item" table in Lua that will hold all this instance's registered methods and variables.
	SetGlobalName("Item");
	RegisterGlobals();
	RegisterMethods();
}


CScriptBindItem::~CScriptBindItem()
{
}


void CScriptBindItem::RegisterGlobals()
{
}


void CScriptBindItem::RegisterMethods()
{
	// TODO: Doesn't the compiler provide a means of getting the class name automatically?
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBindItem::

//	RegisterTemplateFunction("SetSpawnPoint", "EntityId", *this, &CScriptBindItem::SetSpawnPoint);
//	RegisterFunction("SetSpawnPointToNull", functor_ret(*this, &CScriptBindItem::SetSpawnPointToNull));
//	SCRIPT_REG_TEMPLFUNC(CacheResource, "whoIsRequesting, resourceName, resourceType, resourceFlags");

#undef SCRIPT_REG_CLASSNAME
}


void CScriptBindItem::AttachTo(CItem *pItem)
{
	auto pScriptTable = pItem->GetEntity()->GetScriptTable();
	if (pScriptTable)
	{
		SmartScriptTable thisTable(m_pSS);
		thisTable->SetValue("__this", ScriptHandle(pItem->GetEntityId()));
		thisTable->Delegate(GetMethodsTable());
		pScriptTable->SetValue("item", thisTable);
	}
}
