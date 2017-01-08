#pragma once

#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_EnginePlugin
#define GAME_API DLL_EXPORT

#include <CryCore/Platform/platform.h>
#include <CrySystem/ISystem.h>
#include <Cry3DEngine/I3DEngine.h>
#include <CryNetwork/ISerialize.h>


//////////////////////////////////////////////////////////////////////////
//! Reports a Game Warning to validator with WARNING severity.
// TODO: I'm not keen on having this here the way it is. Either move it, or find a better way to handle warnings.
// Also, it turns out that this will cause an error when compiling in Linux on Clang as it somehow becomes defined twice.
 #ifndef WHY_THE_HELL_IS_THIS_BEING_INCLUDED_TWICE
 #define WHY_THE_HELL_IS_THIS_BEING_INCLUDED_TWICE
 inline void GameWarning(const char *, ...) PRINTF_PARAMS(1, 2);
 inline void GameWarning(const char *format, ...)
 {
 	if (!format)
 		return;

 	va_list args;
 	va_start(args, format);
 	GetISystem()->WarningV(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, 0, 0, format, args);
 	va_end(args);
 }
 #endif


#define CODE_TRACE CryLogAlways("[TRACE] %s - Line %d - %s", __func__, __LINE__, __FILE__);
