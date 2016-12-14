#pragma once

#include <CryGame/IGame.h>
#include <CryGame/IGameFramework.h>
#include <../CryAction/IActionMapManager.h>
#include <CryPhysics/RayCastQueue.h>


static const char* GAME_GUID = "{98B6FC74-2245-4968-B1BD-E382FA71E318}";
const string ProjectName = "Chrysalis";

class CConsoleCommands;
class CPlayer;
struct IActor;
class CObjectIdMasterFactory;


// A default value for times when an actor isn't available to find eye height.
static const Vec3 AverageEyePosition { 0.0f, 0.0f, 1.76f };


class CGame : public IGame
{
public:
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


	CGame();
	virtual ~CGame();

	// IGame
	virtual bool Init(IGameFramework *pFramework) override;
	virtual bool CompleteInit() override { return true; };
	virtual void Shutdown() override { this->~CGame(); }
	virtual int Update(bool haveFocus, unsigned int updateFlags) override;
	virtual void EditorResetGame(bool bStart) override {}
	virtual void PlayerIdSet(EntityId playerId) override {}
	virtual IGameFramework* GetIGameFramework() override { return m_pGameFramework; }
	virtual const char* GetLongName() override { return GetName(); }
	virtual const char* GetName() override;
	virtual void GetMemoryStatistics(ICrySizer* s) override { s->Add(*this); }
	virtual void OnClearPlayerIds() override {}
	virtual IGame::TSaveGameName CreateSaveGameName() override { return TSaveGameName(); }
	virtual const char* GetMappedLevelName(const char *levelName) const override { return ""; }
	virtual IGameStateRecorder* CreateGameStateRecorder(IGameplayListener* pL) override { return nullptr; }
	virtual const bool DoInitialSavegame() const override { return true; }
	virtual void LoadActionMaps(const char* filename) override {}
	virtual uint32 AddGameWarning(const char* stringId, const char* paramMessage, IGameWarningsListener* pListener = nullptr) override { return 0; }
	virtual void RenderGameWarnings() override {}
	virtual void RemoveGameWarning(const char* stringId) override {}
	virtual bool GameEndLevel(const char* stringId) override { return false; }
	virtual void OnRenderScene(const SRenderingPassInfo &passInfo) override {}
	virtual void RegisterGameFlowNodes() override;
	virtual void FullSerialize(TSerialize ser) override {}
	virtual void PostSerialize() override {}
	virtual IGame::ExportFilesInfo ExportLevelData(const char* levelName, const char* missionName) const override { return IGame::ExportFilesInfo(levelName, 0); }
	virtual void LoadExportedLevelData(const char* levelName, const char* missionName) override {}
	virtual IGamePhysicsSettings* GetIGamePhysicsSettings() override { return nullptr; }
	virtual void InitEditor(IGameToEditorInterface* pGameToEditor) override {}
	virtual void* GetGameInterface() override {return nullptr;}
	// ~IGame

	const SExternalCVars &GetCVars() const;


	/**
	Gets local player, if there is one for this client.	It is possible there is no local player. Convenience function.

	\return	null if it fails, else the local player.
	*/
	static CPlayer* GetLocalPlayer();

	CObjectIdMasterFactory* GetObjectId() { return m_pObjectIdMasterFactory; }


protected:
	void InitializePlayerProfile();

private:
	/** Registers Game-Specific Console Commands. */
	void RegisterGameConsoleCommands();

	/** Un-Registers Game-Specific Console Commands. */
	void UnRegisterGameConsoleCommands();

	/** The console commands. Used to register and unregister the console commands. */
	CConsoleCommands* m_pConsoleCommands { nullptr };

	IGameFramework* m_pGameFramework { nullptr };

	/** The object identifier master factory. */
	CObjectIdMasterFactory* m_pObjectIdMasterFactory { nullptr };


	// ***	
	// *** Global ray-casting. Provides support for a ray-cast test that is queue based and global in scope.
	// ***

public:
	// Provides a type to use for all global ray-casting operations.
	typedef RayCastQueue<64> GlobalRayCaster;


	/**
	Gets access to the global ray caster.

	\return	The ray caster.
	*/
	GlobalRayCaster& GetRayCaster() { assert(m_pRayCaster); return *m_pRayCaster; }


private:
	GlobalRayCaster* m_pRayCaster { nullptr };
};


/** A global pointer to the game. **/
extern CGame *g_pGame;
