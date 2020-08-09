#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntityClass.h>
#include <CryNetwork/INetwork.h>
#include <CryExtension/ClassWeaver.h>


namespace Chrysalis
{
class CPlayerComponent;
class CObjectIdMasterFactory;


/**
The entry-point of the application An instance of CChrysalisCorePlugin is automatically created when the library is
loaded We then construct the local player entity and CPlayerComponent instance when OnClientConnectionReceived is
first called.
**/

class CChrysalisCorePlugin
	: public Cry::IEnginePlugin
	, public ISystemEventListener
	, public INetworkedClientListener
	, public IGameFrameworkListener
{
public:
	CRYINTERFACE_SIMPLE(Cry::IEnginePlugin)
		CRYGENERATE_SINGLETONCLASS_GUID(CChrysalisCorePlugin, "ChrysalisCore", "{CA15546E-9C8A-4554-9EE6-426DEEF423EA}"_cry_guid);

	virtual ~CChrysalisCorePlugin();

	PLUGIN_FLOWNODE_REGISTER;
	PLUGIN_FLOWNODE_UNREGISTER;

	// Cry::IEnginePlugin
	virtual const char* GetName() const override { return "ChrysalisCore"; }
	virtual const char* GetCategory() const override { return "Game"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Cry::IEnginePlugin

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// IGameFrameworkListener
	virtual void OnPostUpdate(float fDeltaTime) override;
	virtual void OnSaveGame(ISaveGame* pSaveGame) override {};
	virtual void OnLoadGame(ILoadGame* pLoadGame) override {};
	virtual void OnLevelEnd(const char* nextLevel) override {};
	virtual void OnActionEvent(const SActionEvent& event) override {};
	// ~IGameFrameworkListener

	// INetworkedClientListener
	virtual void OnLocalClientDisconnected(EDisconnectionCause cause, const char* description) override {}
	virtual bool OnClientConnectionReceived(int channelId, bool bIsReset) override;
	virtual bool OnClientReadyForGameplay(int channelId, bool bIsReset) override;
	virtual void OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient) override;
	virtual bool OnClientTimingOut(int channelId, EDisconnectionCause cause, const char* description) override { return true; }
	// ~INetworkedClientListener

	// Helper function to call the specified callback for every player in the game
	void IterateOverPlayers(std::function<void(CPlayerComponent& player)> func) const;


	// Helper function to get the CGamePlugin instance
	// Note that CGamePlugin is declared as a singleton, so the CreateClassInstance will always return the same pointer
	static CChrysalisCorePlugin* GetInstance()
	{
		return cryinterface_cast<CChrysalisCorePlugin>(CChrysalisCorePlugin::s_factory.CreateClassInstance().get());
	}

	CObjectIdMasterFactory* GetObjectId() { return m_pObjectIdMasterFactory; }

protected:
	// Map containing player components, key is the channel id received in OnClientConnectionReceived
	std::unordered_map<int, EntityId> m_players;


private:
	/** The object identifier master factory. */
	CObjectIdMasterFactory* m_pObjectIdMasterFactory {nullptr};
};
}