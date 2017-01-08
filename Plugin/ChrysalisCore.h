#pragma once

#include <CrySystem/ISystem.h>


class CObjectIdMasterFactory;
class CConsoleCommands;


class CChrysalisCore : public ISystemEventListener
{
public:
	CChrysalisCore();
	virtual ~CChrysalisCore();

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	struct SExternalCVars
	{
		int m_rayCastQuota { 64 };
		int m_ladder_logVerbosity { 0 };

		// Mouse movements.
		int m_cl_invertPitch { 0 };
		float m_cl_mouseSensitivity { 1.0f };

		// Watch Debug

		/** Is watch debug enabled? */
		int m_watch_enabled { true };

		/** The watch text render start position x coordinate. */
		float m_watch_text_render_start_pos_x { 35.0f };

		/** The watch text render start position y coordinate. */
		float m_watch_text_render_start_pos_y { 180.0f };

		/** Size at which the watch text will render. */
		float m_watch_text_render_size { 1.75f };

		/** Line spacing for watch text. */
		float m_watch_text_render_lineSpacing { 9.3f };

		/** The watch text render fxscale. */
		float m_watch_text_render_fxscale { 13.0f };
	};
	const SExternalCVars &GetCVars() const;
	SExternalCVars &SetCVars();

	CObjectIdMasterFactory* GetObjectId() { return m_pObjectIdMasterFactory; }

private:
	CChrysalisCore::SExternalCVars m_cvars;

	void RegisterCVars();

	/** Registers Game-Specific Console Commands. */
	void RegisterGameConsoleCommands();

	/** Un-Registers Game-Specific Console Commands. */
	void UnRegisterGameConsoleCommands();

	/** The console commands. Used to register and unregister the console commands. */
	CConsoleCommands* m_pConsoleCommands;

	/** The object identifier master factory. */
	CObjectIdMasterFactory* m_pObjectIdMasterFactory { nullptr };
};
