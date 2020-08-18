#pragma once

namespace Chrysalis
{
class CCVars final
{
public:
	CCVars() = default;
	CCVars(CCVars const&) = delete;
	CCVars(CCVars&&) = delete;
	CCVars& operator=(CCVars const&) = delete;
	CCVars& operator=(CCVars&&) = delete;

	void RegisterVariables();
	void UnregisterVariables();

	// Game.
	int m_rayCastQuota {64};
	int m_ladder_logVerbosity {0};
	int m_cl_invertPitch {0};
	float m_cl_mouseSensitivity {1.0f};
	int m_watch_enabled {true};
	float m_watch_text_render_start_pos_x {35.0f};
	float m_watch_text_render_start_pos_y {180.0f};
	float m_watch_text_render_size {1.75f};
	float m_watch_text_render_lineSpacing {9.3f};
	float m_watch_text_render_fxscale {13.0f};

	// Camera general
	//float m_cameraFoV {75.0f};

	// Camera manager.
	ICVar* m_cameraManagerDebugViewOffset {nullptr};
	int m_cameraManagerDefaultCamera {1};

	// Action RPG Camera.
	int m_actionRPGCameraDebug;
	float m_actionRPGCameraPitchMin;
	float m_actionRPGCameraPitchMax;
	float m_actionRPGCameraTargetDistance;
	float m_actionRPGCameraReversePitchTilt;
	float m_actionRPGCameraSlerpSpeed;
	float m_actionRPGCameraZoomMin;
	float m_actionRPGCameraZoomMax;
	float m_actionRPGCameraZoomStep;
	float m_actionRPGCameraZoomSpeed;
	ICVar* m_actionRPGCameraViewPositionOffset;
	ICVar* m_actionRPGCameraAimPositionOffset;

	// First Person Camera.
	int m_firstPersonCameraDebug {0};
	float m_firstPersonCameraPitchMin {-85.0f};
	float m_firstPersonCameraPitchMax {85.0f};

	// Examine Camera.
	int m_examineCameraDebug {0};
	float m_examineCameraYawMin {-85.0f};
	float m_examineCameraYawMax {85.0f};
	float m_examineCameraPitchMin {-85.0f};
	float m_examineCameraPitchMax {85.0f};

	// Components.
	int m_componentKeyringDebug {0};
	int m_componentEquipmentDebug {0};
	int m_componentCharacterAttributesDebug {0};
	int m_componentAwarenessDebug {0};
	int m_componentInventoryDebug {0};


	/**
	Attaches the currently player to an entity.

	\param [in,out]	pConsoleCommandArgs If non-null, the console command arguments.
	**/
	static void OnAttach(IConsoleCmdArgs* pConsoleCommandArgs);


	/**
	Requests a new ObjectId to be created, and output to the log.

	\param [in,out]	pConsoleCommandArgs If non-null, the console command arguments.
	**/
	static void OnCreateObjectId(IConsoleCmdArgs* pConsoleCommandArgs);


	/**
	Makes a request for the character under player command to perform an emote.

	\param [in,out]	pConsoleCommandArgs If non-null, the console command arguments.
	**/
	static void OnEmote(IConsoleCmdArgs* pConsoleCommandArgs);

	/**
		Attaches the currently player to an entity.

		\param [in,out]	pConsoleCommandArgs If non-null, the console command arguments.
		**/
	static void OnCameraSetFov(IConsoleCmdArgs* pConsoleCommandArgs);
}
;

extern CCVars g_cvars;
}