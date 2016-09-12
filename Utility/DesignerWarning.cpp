#include <StdAfx.h>

#include "DesignerWarning.h"
/*#include "Testing/FeatureTester.h"
#include "Testing/AutoTester.h"*/


#if DESIGNER_WARNING_ENABLED

static int s_numDesignerWarningsHit = 0;

#ifdef WIN32
#include <CryCore/Platform/CryWindows.h>
#endif

int DesignerWarningFunc(const char * message)
{
	/*	if (g_pGame->GetCVars().m_designer_warning_enabled && (!gEnv->IsDedicated()))
		{
		GameWarning("!DESIGNER WARNING\n%s", message);
		}

		// Kept because autotests gather all designer warnings out of logs with this form.
		CryLogAlways("---DESIGNER_WARNING: %s", message);
		CryLogAlways("----------------------------------------");

		#if ENABLE_FEATURE_TESTER

		// If feature testing is in progress, write each designer warning out as a failed feature test.
		CFeatureTester * featureTester = CFeatureTester::GetInstance();
		if (featureTester)
		{
		CAutoTester * autoTestResultWriter = featureTester->GetAutoTesterIfActive();
		if (autoTestResultWriter)
		{
		CryFixedStringT<32> warningName;
		warningName.Format("DesignerWarning%04u", s_numDesignerWarningsHit);
		autoTestResultWriter->AddSimpleTestCase("DesignerWarnings", warningName.c_str(), 0.1f, message);
		}
		}
		#endif

		s_numDesignerWarningsHit++;
		return 0;*/

	// TODO: ILH removed this code for now. Either find a better way or place back in when dependencies are sorted.
	return 0;
}


int GetNumDesignerWarningsHit()
{
	return s_numDesignerWarningsHit;
}

#endif // DESIGNER_WARNING_ENABLED