/**
\file	Source\stdafx.h

Declares the stdafx class; an include file for standard system include files, or project specific include
files that are used frequently, but are changed infrequently.
*/
#pragma once

// Let various code paths know this is the game module. It seems mostly to be used for debugging information and must
// be declared before your includes.
#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_Game
#define GAME_API   DLL_EXPORT

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN


#include <CryCore/Platform/platform.h>
#include <CrySystem/ISystem.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <limits>
#include <CryCore/smartptr.h>
#include <CryAction.h>
#include <CryThreading/CryThread.h>
#include <CryMath/Cry_Math.h>
#include <CrySystem/ISystem.h>
#include <Cry3DEngine/I3DEngine.h>
#include <CryInput/IInput.h>
#include <CrySystem/IConsole.h>
#include <CrySystem/ITimer.h>
#include <CrySystem/ILog.h>
#include <IGameplayRecorder.h>
#include <CryNetwork/ISerialize.h>
#include <CryCore/Assert/CryAssert.h>
#include <Utility/CryWatch.h>
#include <Game/Game.h>


// Using symbol from Platform.h to determine if we need dllexport specs on the entry points.
#define GAME_API DLL_EXPORT

// Define the memory we allocate to be game memory.
#define eCryModule eCryM_Game


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
