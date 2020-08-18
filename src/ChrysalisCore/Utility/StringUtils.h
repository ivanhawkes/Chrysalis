// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

/*************************************************************************
   -------------------------------------------------------------------------
   StringUtils.h

   TODO: Move contents of this file into CryEngine/CryCommon/StringUtils.h
   and contents of StringUtils.cpp into CryEngine/CryCommon/StringUtils.cpp
*************************************************************************/

#pragma once

namespace Chrysalis
{
//--------------------------------------------------------------------------------
// cry_copyStringUntilFindChar: Parameter order is the same as strncpy;
// additional 'until' parameter defines which additional character should
// stop the copying. Return value is number of bytes (including nullptr)
// written into 'destination', or 0 if 'until' character not found in
// first 'bufferLength' bytes of 'source'.
size_t cry_copyStringUntilFindChar(char* destination, const char* source, size_t bufferLength, char until);

#if !defined(_DEBUG)
#define cry_displayMemInHexAndAscii(...)      (void)(0)
#else

class ITextOutputHandler
{
public:
	virtual void DoOutput(const char* text) = 0;
};

class CCryWatchOutputHandler :public ITextOutputHandler
{
	virtual void DoOutput(const char* text);
};

class CCryLogOutputHandler :public ITextOutputHandler
{
	virtual void DoOutput(const char* text);
};

class CCryLogAlwaysOutputHandler :public ITextOutputHandler
{
	virtual void DoOutput(const char* text);
};

//--------------------------------------------------------------------------------
// cry_displayMemInHexAndAscii outputs (using an ITextOutputHandler subclass) the
// contents of the first 'size' bytes starting at memory location 'data'.
void cry_displayMemInHexAndAscii(const char* startEachLineWith, const void* data, int size, ITextOutputHandler &output, const int bytesPerLine = 32);
#endif

//--------------------------------------------------------------------------------
// Generates a string in the format X days X hrs X mins X secs, or if useShortForm is set 00:00:00.
const char* GetTimeString(int secs, bool useShortForm = false, bool includeSeconds = true, bool useSingleLetters = false);
const char* GetTimeString(float secs, bool useShortForm = false, bool includeSeconds = true, bool useSingleLetters = false);

/**
Takes a number of seconds as an input and translates that into the corresponding number of days, hours, minutes
and seconds.

\param    secs               The number of seconds to be converted.
\param [in,out]    days       The days.
\param [in,out]    hours      The hours.
\param [in,out]    minutes    The minutes.
\param [in,out]    seconds    The seconds.
*/
static void ExpandTimeSeconds(int secs, int& days, int& hours, int& minutes, int& seconds);
}
