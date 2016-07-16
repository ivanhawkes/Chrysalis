#pragma once


class CConsoleVariables
{
public:
	void Register();
	void Unregister();


	/** Specifies Whether Lights Are Allowed To Spawn From Item Effects. 1 If Allowed, 0 Otherwise. */
	int	m_iilighteffects;


	/** The "sv_Pacifist" Console Variable Value. Required For Multi-Player. Not Needed, Only Here For Compatibility. */
	int m_isvPacifist;


	// ***
	// *** Client Variables
	// ***


	/**
	Should we invert the Y axis for camera movements? This is preferred by some players, particularly those using a
	flight yoke.
	*/
	int cl_invertPitch;

	
	int cl_isClientFocusWindow;


	// ***
	// *** Mouse Variables
	// ***


	/**
	The cl mouse sensitivity.

	Overall mouse sensitivity. This should be factored into any movements involving the mouse.
	*/
	float cl_mouseSensitivity;


	// ***
	// *** Camera Variables
	// ***


	/**
	Determine the level of zoom on the third person camera. Sensible values are from 0.0f up to 100.0f, although it
	is capable of zooming beyond that.
	*/
	float cl_tpvZoom;


	/** The minimum value for camera zoom. */
	float cl_tpvZoomMin;


	/** The maximum value for camera zoom. */
	float cl_tpvZoomMax;


	/**
	Each zoom event in or out will alter the zoom factor, cl_tpvZoom, by this amount. Use lower values for more steps
	and higher values to zoom in / out faster with less steps.
	*/
	float cl_tpvZoomStep;


	/**
	When the zoom changes we interpolate between it's last value and the goal value. This provides for smoother
	movement on camera zooms. Higher values will interpolate faster than lower values.
	*/
	float cl_tpvZoomSpeed;


	/** A delta value in degrees to apply to the camera's initial calculated pitch. */
	float cl_tpvViewPitch;


	/** A delta value in degrees to apply to the camera's initial calculated yaw. */
	float cl_tpvViewYaw;


	/**
	A delta value in degrees to apply to the camera's initial calculated pitch. This value comes from the XBox
	controller if one is present.
	*/
	float cl_tpvXIPitchDelta;


	/**
	A delta value in degrees to apply to the camera's initial calculated yaw. This value comes from the XBox
	controller if one is present.
	*/
	float cl_tpvXIYawDelta;


	/** Minimum pitch delta in degrees. */
	float cl_tpvPitchMin;


	/** Maximum pitch delta in degrees. */
	float cl_tpvPitchMax;


	/** Minimum yaw delta in degrees. */
	float cl_tpvYawMin;


	/** Maximum yaw delta in degrees. */
	float cl_tpvYawMax;


	/**
	Filter pitch adjustments below this threshold (radians). This is useful for removing slight amounts of jitter on
	mouse movements and XBox controllers, making it easier to perform precise movements in only one axis.
	*/
	float cl_tpvPitchFilter;


	/**
	Filter yaw adjustments below this threshold (radians). This is useful for removing slight amounts of jitter on
	mouse movements and XBox controllers, making it easier to perform precise movements in only one axis.
	*/
	float cl_tpvYawFilter;


	/** Mouse sensitivity for pitching and yawing. All pitch and yaw deltas will be factored by this value. */
	float cl_tpvPitchYawSensitivity;


	// ***
	// *** Ladder Variables
	// ***


	/** Should we render the player last when they're on a ladder? */
	int ladder_renderPlayerLast;


	/** Log verbosity level for ladder related events. */
	int	ladder_logVerbosity;


	// ***
	// *** Various
	// ***

	int game_rayCastQuota;


	// ***
	// *** Watch Debug
	// ***
	
	/** Is watch debug enabled? */
	int watch_enabled;

	/** The watch text render start position x coordinate. */
	float watch_text_render_start_pos_x;
	
	/** The watch text render start position y coordinate. */
	float watch_text_render_start_pos_y;

	/** Size at which the watch text will render. */
	float watch_text_render_size;
	
	/** Line spacing for watch text. */
	float watch_text_render_lineSpacing;
	
	/** The watch text render fxscale. */
	float watch_text_render_fxscale;
};
