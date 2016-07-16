/**
\file	Utility\DesignerWarning.h

Display a basic message box in windows to allow designers to be told something is wrong with their setup.
*/
#pragma once

#if defined(WIN32) 
#define DESIGNER_WARNING_ENABLED 1  // needs a release build define to hook in here 
#elif defined(WIN64)
#define DESIGNER_WARNING_ENABLED 1  // needs a release build define to hook in here 
#else
#define DESIGNER_WARNING_ENABLED 0
#endif

#if defined(_RELEASE)
#undef DESIGNER_WARNING_ENABLED
#define DESIGNER_WARNING_ENABLED 0
#endif

#if DESIGNER_WARNING_ENABLED

#define DesignerWarning(cond, ...) ((!(cond)) && DesignerWarningFail(#cond, string().Format(__VA_ARGS__).c_str()))
#define DesignerWarningFail(condText, messageText) DesignerWarningFunc(string().Format("CONDITION:\n%s\n\nMESSAGE:\n%s", condText, messageText))
int DesignerWarningFunc(const char * message);
int GetNumDesignerWarningsHit();

#else // DESIGNER_WARNING_ENABLED

#define DesignerWarning(cond, ...) (0)
#define DesignerWarningFail(condText, messageText) (0)
#define GetNumDesignerWarningsHit() (0)

#endif // DESIGNER_WARNING_ENABLED
