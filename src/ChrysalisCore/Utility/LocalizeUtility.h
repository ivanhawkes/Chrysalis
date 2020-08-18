/**
\file	Utility\LocalizeUtility.h

A set of utility functions that allow you to Localize strings to the user's language.
Initially this was embedded into the HUD code, but it makes more sense to break it out.
*/
#pragma once


namespace Chrysalis
{
namespace LocalizeUtility
{
// ***
// *** Localize strings.
//  ***

const char * LocalizeString(const char *text, const char *arg1 = 0, const char *arg2 = 0, const char *arg3 = 0, const char *arg4 = 0);
void LocalizeString(string &out, const char *text, const char *arg1, const char *arg2, const char *arg3, const char *arg4);
void LocalizeStringn(char* dest, size_t bufferSizeInBytes, const char *text, const char *arg1 = 0, const char *arg2 = 0, const char *arg3 = 0, const char *arg4 = 0);


// ***
// *** Localize numbers.
// ***

const char* LocalizeNumber(const int number);
void LocalizeNumber(string &out, const int number);
void LocalizeNumbern(const char* dest, size_t bufferSizeInBytes, const int number);
const char* LocalizeNumber(const float number, int decimals);
void LocalizeNumber(string &out, const float number, int decimals);
void LocalizeNumbern(const char* dest, size_t bufferSizeInBytes, const float number, int decimals);
}
}