#include <StdAfx.h>

#include "ConsoleVariables.h"


void CConsoleVariables::Register(void)
{
	// Needed For Particle System.....Bad Design On Crytek's Part.
	gEnv->pConsole->Register("i_lighteffects", &m_iilighteffects, 1, VF_DUMPTODISK, "Enable/Disable Lights Spawned During Item Effects.");

	// Needed For Multi-Player.....Bad Design On Crytek's Part.
	gEnv->pConsole->Register("sv_pacifist", &m_isvPacifist, 0, VF_DUMPTODISK, "Enable/Disable Pacifist Multi-player Mode.");

	// Client.
	REGISTER_CVAR(cl_invertPitch, 0, 0, "Invert Y axis on the mouse.");
	REGISTER_CVAR(cl_isClientFocusWindow, true, 0, "Is this client the current focus window.");

	// Mouse.
	REGISTER_CVAR(cl_mouseSensitivity, 1.0f, 0, "Sensitivity of the mouse.");

	// Camera.
	REGISTER_CVAR(cl_tpvZoom, 0.5f, 0, "3rd person camera - zoom level (0.0 - 1.0)");
	REGISTER_CVAR(cl_tpvZoomMin, 0.0f, 0, "3rd person camera - minimum amount we allow the camera to zoom");
	REGISTER_CVAR(cl_tpvZoomMax, 1.0f, 0, "3rd person camera - maximum amount we allow the camera to zoom");
	REGISTER_CVAR(cl_tpvZoomStep, 0.05f, 0, "3rd person camera - zoom step increments");
	REGISTER_CVAR(cl_tpvZoomSpeed, 20.0f, 0, "3rd person camera - interpolation speed for zoom movements.");
	REGISTER_CVAR(cl_tpvViewPitch, 0.0f, 0, "3rd person camera - player mouse controlled pitch");
	REGISTER_CVAR(cl_tpvViewYaw, 0.0f, 0, "3rd person camera - player mouse controlled yaw");
	REGISTER_CVAR(cl_tpvXIPitchDelta, 0.0f, 0, "3rd person camera - player Xbox controller pitch");
	REGISTER_CVAR(cl_tpvXIYawDelta, 0.0f, 0, "3rd person camera - player Xbox controller yaw");
	REGISTER_CVAR(cl_tpvPitchFilter, 0.015f, 0, "3rd person camera - pitch filter");
	REGISTER_CVAR(cl_tpvYawFilter, 0.015f, 0, "3rd person camera - yaw filter");
	REGISTER_CVAR(cl_tpvPitchYawSensitivity, 8.0f, 0, "3rd person camera - pitch / yaw mouse sensitivity");

	// Ladder.
	REGISTER_CVAR(ladder_renderPlayerLast, 0, 0, "Should we render the player last when they're on a ladder?");
	REGISTER_CVAR(ladder_logVerbosity, 0, 0, "Log verbosity level for ladder related events.");

	// Various.
	REGISTER_CVAR(game_rayCastQuota, 16, VF_CHEAT, "Amount of deferred rays allowed to be cast per frame by Game");

	// Watch debug.
	REGISTER_CVAR(watch_enabled, 1, 0, "On-screen watch text is enabled/disabled");
	REGISTER_CVAR(watch_text_render_start_pos_x, 35.0f, 0, "On-screen watch text render start x position");
	REGISTER_CVAR(watch_text_render_start_pos_y, 180.0f, 0, "On-screen watch text render start y position");
	REGISTER_CVAR(watch_text_render_size, 1.75f, 0, "On-screen watch text render size");
	REGISTER_CVAR(watch_text_render_lineSpacing, 9.3f, 0, "On-screen watch text line spacing (to cram more text on screen without shrinking the font)");
	REGISTER_CVAR(watch_text_render_fxscale, 13.0f, 0, "Draw2d label to IFFont x scale value (for calcing sizes).");
}


void CConsoleVariables::Unregister(void)
{
	// Un-Registers The "i_lighteffects" Console Variable.
	gEnv->pConsole->UnregisterVariable("i_lighteffects", true);

	// Un-Registers The "sv_pacifist" Console Variable.
	gEnv->pConsole->UnregisterVariable("sv_pacifist", true);

	// Client.
	gEnv->pConsole->UnregisterVariable("cl_invertPitch", true);
	gEnv->pConsole->UnregisterVariable("cl_isClientFocusWindow", true);

	// Mouse.
	gEnv->pConsole->UnregisterVariable("cl_mouseSensitivity", true);

	// Camera.
	gEnv->pConsole->UnregisterVariable("cl_tpvZoom", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvZoomMin", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvZoomMax", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvZoomStep", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvZoomSpeed", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvViewPitch", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvViewYaw", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvXIPitchDelta", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvXIYawDelta", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvPitchFilter", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvYawFilter", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvPitchFilter", true);
	gEnv->pConsole->UnregisterVariable("cl_tpvPitchYawSensitivity", true);

	// Ladder.
	gEnv->pConsole->UnregisterVariable("ladder_renderPlayerLast", true);
	gEnv->pConsole->UnregisterVariable("ladder_logVerbosity", true);

	// Various.
	gEnv->pConsole->UnregisterVariable("game_rayCastQuota", true);

	// Watch debug.
	gEnv->pConsole->UnregisterVariable("watch_enabled", true);
	gEnv->pConsole->UnregisterVariable("watch_text_render_start_pos_x", true);
	gEnv->pConsole->UnregisterVariable("watch_text_render_start_pos_y", true);
	gEnv->pConsole->UnregisterVariable("watch_text_render_size", true);
	gEnv->pConsole->UnregisterVariable("watch_text_render_lineSpacing", true);
	gEnv->pConsole->UnregisterVariable("watch_text_render_fxscale", true);
}
