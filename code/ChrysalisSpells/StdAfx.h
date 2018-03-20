#pragma once

#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_EnginePlugin
#define GAME_API DLL_EXPORT

#include <CryCore/Platform/platform.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <CrySchematyc/CoreAPI.h>
//#include "Plugin/ChrysalisCorePlugin.h"
//#include "Utility/CryWatch.h"


namespace Chrysalis
{
#if !defined(_RELEASE)
#define INCLUDE_DEFAULT_PLUGINS_PRODUCTION_CODE
#endif // _RELEASE

template<class T>
static IEntityClass* RegisterEntityWithDefaultComponent(const char* name, const char* editorCategory = "", const char* editorIcon = "", bool bIconOnTop = false)
{
	IEntityClassRegistry::SEntityClassDesc clsDesc;
	clsDesc.sName = name;

	clsDesc.editorClassInfo.sCategory = editorCategory;
	clsDesc.editorClassInfo.sIcon = editorIcon;
	clsDesc.editorClassInfo.bIconOnTop = bIconOnTop;

	struct CObjectCreator
	{
		static IEntityComponent* Create(IEntity* pEntity, SEntitySpawnParams& params, void* pUserData)
		{
			return pEntity->GetOrCreateComponentClass<T>();
		}
	};

	clsDesc.pUserProxyCreateFunc = &CObjectCreator::Create;
	clsDesc.flags |= ECLF_INVISIBLE;

	return gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(clsDesc);
}


////////////////////////////////////////////////////////////////////////////
////! Reports a Game Warning to validator with WARNING severity.
//// #TODO: I'm not keen on having this here the way it is. Either move it, or find a better way to handle warnings.
//// Also, it turns out that this will cause an error when compiling in Linux on Clang as it somehow becomes defined twice.
//
//// HACK: FIX: Replace this.
//
//#ifndef WHY_THE_HELL_IS_THIS_BEING_INCLUDED_TWICE
//#define WHY_THE_HELL_IS_THIS_BEING_INCLUDED_TWICE
//inline void GameWarning(const char *, ...) PRINTF_PARAMS(1, 2);
//inline void GameWarning(const char *format, ...)
//{
//	if (!format)
//		return;
//
//	va_list args;
//	va_start(args, format);
//	GetISystem()->WarningV(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, 0, 0, format, args);
//	va_end(args);
//}
//#endif
//
//
//#define CODE_TRACE CryLogAlways("[TRACE] %s - Line %d - %s", __func__, __LINE__, __FILE__);
}