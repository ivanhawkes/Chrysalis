#include <StdAfx.h>

#include <CrySystem/ILocalizationManager.h>
#include <CryString/StringUtils.h>
#include <Utility/StringUtils.h>
#include "LocalizeUtility.h"


namespace Chrysalis
{
namespace LocalizeUtility
{
	// ***
	// *** Localize strings.
	//  ***


	void LocalizeString(string &out, const char *text, const char *arg1, const char *arg2, const char *arg3, const char *arg4)
	{
#if ENABLE_HUD_EXTRA_DEBUG
		const int numberOfWs = gEnv->pGameFramework->GetHUD()->GetCVars()->hud_localize_ws_instead;
		if (numberOfWs > 0)
		{
			static int lastNumberOfWs = 0;
			if (lastNumberOfWs != numberOfWs)
			{
				for (int i = 0; i < numberOfWs; ++i)
				{
					out.append("W");
				}

				lastNumberOfWs = numberOfWs;
			}
			return;
		}
#endif

		if (!text)
		{
			out = "";
			return;
		}

		string localizedString, param1, param2, param3, param4;
		ILocalizationManager* pLocMgr = gEnv->pSystem->GetLocalizationManager();

		if (text [0] == '@')
			pLocMgr->LocalizeString(text, localizedString);
		else
			localizedString = text;

		if (arg1)
		{
			if (arg1 [0] == '@')
				pLocMgr->LocalizeString(arg1, param1);
			else
				param1 = arg1;
		}

		if (arg2)
		{
			if (arg2 [0] == '@')
				pLocMgr->LocalizeString(arg2, param2);
			else
				param2 = arg2;
		}

		if (arg3)
		{
			if (arg3 [0] == '@')
				pLocMgr->LocalizeString(arg3, param3);
			else
				param3 = arg3;
		}

		if (arg4)
		{
			if (arg4 [0] == '@')
				pLocMgr->LocalizeString(arg4, param4);
			else
				param4 = arg4;
		}

		out.resize(0);
		pLocMgr->FormatStringMessage(out, localizedString, param1.c_str(), param2.c_str(), param3.c_str(), param4.c_str());
	}


	const char * LocalizeString(const char *text, const char *arg1, const char *arg2, const char *arg3, const char *arg4)
	{
		//ScopedSwitchToGlobalHeap globalHeap;
		static string charstr;
		LocalizeString(charstr, text, arg1, arg2, arg3, arg4);

		return charstr.c_str();
	}


	void LocalizeStringn(char* dest, size_t bufferSizeInBytes, const char *text, const char *arg1 /*= nullptr*/, const char *arg2 /*= nullptr*/, const char *arg3 /*= nullptr*/, const char *arg4 /*= nullptr*/)
	{
		cry_strcpy(dest, bufferSizeInBytes, LocalizeString(text, arg1, arg2, arg3, arg4));
	}


	// ***
	// *** Localize numbers.
	// ***

	const char* LocalizeNumber(const int number)
	{
		ILocalizationManager* pLocMgr = gEnv->pSystem->GetLocalizationManager();

		//ScopedSwitchToGlobalHeap globalHeap;
		static string charstr;
		pLocMgr->LocalizeNumber(number, charstr);

		return charstr.c_str();
	}


	void LocalizeNumber(string& out, const int number)
	{
		ILocalizationManager* pLocMgr = gEnv->pSystem->GetLocalizationManager();

		pLocMgr->LocalizeNumber(number, out);
	}


	void LocalizeNumbern(char* dest, size_t bufferSizeInBytes, const int number)
	{
		cry_strcpy(dest, bufferSizeInBytes, LocalizeNumber(number));
	}


	const char* LocalizeNumber(const float number, int decimals)
	{
		ILocalizationManager* pLocMgr = gEnv->pSystem->GetLocalizationManager();

		//ScopedSwitchToGlobalHeap globalHeap;
		static string charstr;
		pLocMgr->LocalizeNumber(number, decimals, charstr);

		return charstr.c_str();
	}


	void LocalizeNumber(string& out, const float number, int decimals)
	{
		ILocalizationManager* pLocMgr = gEnv->pSystem->GetLocalizationManager();

		pLocMgr->LocalizeNumber(number, decimals, out);
	}


	void LocalizeNumbern(char* dest, size_t bufferSizeInBytes, const float number, int decimals)
	{
		cry_strcpy(dest, bufferSizeInBytes, LocalizeNumber(number, decimals));
	}
}
}