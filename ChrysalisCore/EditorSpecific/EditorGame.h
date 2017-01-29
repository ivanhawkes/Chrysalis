#pragma once

#include <CryGame/IGameRef.h>
#include <CrySandbox/IEditorGame.h>


struct IGameStartup;
struct IActor;


class CEditorGame : public IEditorGame
{
public:
	CEditorGame() {};
	virtual ~CEditorGame() {};

	// IEditorGame
	virtual bool Init(ISystem* pSystem, IGameToEditorInterface* pGameToEditorInterface) override;
	virtual int Update(bool haveFocus, unsigned int updateFlags) override;
	virtual void Shutdown() override;
	virtual bool SetGameMode(bool bGameMode) override;
	virtual IEntity* GetPlayer() override;
	virtual void SetPlayerPosAng(Vec3 pos, Vec3 viewDir) override;
	virtual void HidePlayer(bool bHide) override;
	virtual void OnBeforeLevelLoad() override;
	virtual void OnAfterLevelLoad(const char* levelName, const char* levelFolder) override;
	virtual void OnCloseLevel() override {}
	virtual void OnSaveLevel() override {}
	virtual bool BuildEntitySerializationList(XmlNodeRef output) override { return true; }
	virtual bool GetAdditionalMinimapData(XmlNodeRef output) override { return false; }
	virtual IFlowSystem* GetIFlowSystem() override;
	virtual IGameTokenSystem* GetIGameTokenSystem() override;
	virtual IEquipmentSystemInterface* GetIEquipmentSystemInterface() override { return nullptr; }
	virtual bool SupportsMultiplayerGameRules() override { return false; }
	virtual void ToggleMultiplayerGameRules() override {}
	virtual void RegisterTelemetryTimelineRenderers(Telemetry::ITelemetryRepository* pRepository) override {}
	virtual void OnDisplayRenderUpdated(bool displayHelpers) override {}
	virtual void OnAfterLevelInit(const char* levelName, const char* levelFolder) override {}
	virtual void OnEntitySelectionChanged(EntityId entityId, bool isSelected) override {}
	virtual void OnReloadScripts(EReloadScriptsType scriptsType) override {}
	// ~IEditorGame

protected:
	IActor *GetPlayerActor();

private:
	bool ConfigureNetContext(bool bEnable);
	void EnablePlayer(bool bPlayer);

	IGameRef m_pGame { nullptr };
	IGameStartup* m_pGameStartup { nullptr };
	bool m_bEnabled { false };
	bool m_bGameMode { false };
	bool m_bPlayer { false };
};

