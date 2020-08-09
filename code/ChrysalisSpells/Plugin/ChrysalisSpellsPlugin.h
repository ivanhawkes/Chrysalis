#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntityClass.h>
#include <CryNetwork/INetwork.h>


namespace ChrysalisSpells
{
class CChrysalisSpellsPlugin
	: public Cry::IEnginePlugin
	, public ISystemEventListener
	, public INetworkedClientListener
{
public:
	CRYINTERFACE_SIMPLE(Cry::IEnginePlugin)
	CRYGENERATE_SINGLETONCLASS_GUID(CChrysalisSpellsPlugin, "ChrysalisSpells", "{8B63CD64-F07C-45A7-A5D1-8E35E4BE7090}"_cry_guid);

	virtual ~CChrysalisSpellsPlugin();

	PLUGIN_FLOWNODE_REGISTER;
	PLUGIN_FLOWNODE_UNREGISTER;
		
	// Cry::IEnginePlugin
	virtual const char* GetName() const override { return "ChrysalisSpells"; }
	virtual const char* GetCategory() const override { return "Plugin"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Cry::IEnginePlugin

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// INetworkedClientListener
	virtual void OnLocalClientDisconnected(EDisconnectionCause cause, const char* description) override {};
	virtual bool OnClientConnectionReceived(int channelId, bool bIsReset) override {return true;}
	virtual bool OnClientReadyForGameplay(int channelId, bool bIsReset) { return true; }
	virtual void OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient) override {};
	virtual bool OnClientTimingOut(int channelId, EDisconnectionCause cause, const char* description) override { return true; }
	// ~INetworkedClientListener

	// Helper function to get the CGamePlugin instance
	// Note that CGamePlugin is declared as a singleton, so the CreateClassInstance will always return the same pointer
	static CChrysalisSpellsPlugin* GetInstance()
	{
		return cryinterface_cast<CChrysalisSpellsPlugin>(CChrysalisSpellsPlugin::s_factory.CreateClassInstance().get());
	}
};
}