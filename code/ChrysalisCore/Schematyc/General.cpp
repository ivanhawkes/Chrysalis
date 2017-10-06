#include "StdAfx.h"

#include <CrySchematyc/CoreAPI.h>
#include <Cry3DEngine/I3DEngine.h>
#include <Cry3DEngine/ITimeOfDay.h>
#include <Schematyc/CoreEnv.h>
#include <CryGame/GameUtils.h>


namespace Chrysalis
{
namespace Environment
{
using namespace ::Schematyc;

float GetTimeOfDay()
{
	// Get the time of day, represented as hours24 (float).
	ITimeOfDay* pTimeOfDay = gEnv->p3DEngine->GetTimeOfDay();
	return pTimeOfDay->GetTime();
}


float GetFrameTime()
{
	return gEnv->pTimer->GetFrameTime();
}


void HoursMinutesSeconds(float timeOfDay, float& hours, float& minutes, float& seconds)
{
	const float hours24 = floor(timeOfDay);
	const float remainder = timeOfDay - hours24;
	const float totalSeconds = remainder * 3600;
	
	hours = hours24;
	minutes = floor(totalSeconds / 60.0f);
	seconds = totalSeconds - minutes * 60.0f;
}


static void RegisterFunctions(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(g_generalModuleGUID);
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&GetTimeOfDay, "{A876ACF2-1CD2-42B3-823B-F0BC1A0C31BE}"_cry_guid, "GetEnvironmentTime");
		pFunction->SetDescription("Environment time");
		pFunction->BindOutput(0, 'time', "Time");
		scope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&HoursMinutesSeconds, "{5D515A5B-7128-4652-809A-821B8DB68F57}"_cry_guid, "HoursMinutesSeconds");
		pFunction->SetDescription("Environment time expressed in hours, minutes and seconds");
		pFunction->BindInput(1, 'time', "Time");
		pFunction->BindOutput(2, 'hour', "Hours");
		pFunction->BindOutput(3, 'min', "Minutes");
		pFunction->BindOutput(4, 'sec', "Seconds");
		scope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&GetFrameTime, "{7B17363E-27D1-4AD2-A6E9-CC531BAA790E}"_cry_guid, "GetFrameTime");
		pFunction->SetDescription("Environment frame time");
		pFunction->BindOutput(0, 'ft', "FrameTime");
		scope.Register(pFunction);
	}
}
} // Environment


namespace Math
{
using namespace ::Schematyc;

float InterpolateFloat(float actual, float goal, float speed, float frameTime, float limit)
{
	float newActual = actual;
	Interpolate(newActual, goal, speed, frameTime, limit);
	
	return newActual;
}


static void RegisterFunctions(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(GetTypeDesc<float>().GetGUID());
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&InterpolateFloat, "{1B16293D-02BE-4A38-9FF6-FC2BF0EC8542}"_cry_guid, "Interpolate");
		pFunction->SetDescription("Interpolate between values");
		pFunction->BindOutput(0, 'res', "Result");
		pFunction->BindInput(1, 'actu', "Actual");
		pFunction->BindInput(2, 'goal', "Goal");
		pFunction->BindInput(3, 'spee', "Speed");
		pFunction->BindInput(4, 'ft', "FrameTime");
		pFunction->BindInput(5, 'limi', "Limit");
		scope.Register(pFunction);
	}
}
} // Math


static void RegisterGeneralFunctions(::Schematyc::IEnvRegistrar& registrar)
{
	Environment::RegisterFunctions(registrar);
	Math::RegisterFunctions(registrar);
}
} // Chrysalis

CRY_STATIC_AUTO_REGISTER_FUNCTION(&Chrysalis::RegisterGeneralFunctions)