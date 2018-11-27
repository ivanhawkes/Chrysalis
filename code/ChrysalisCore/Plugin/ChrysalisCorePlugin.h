#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntityClass.h>
#include <CryNetwork/INetwork.h>
#include <CryExtension/ClassWeaver.h>


namespace Chrysalis
{
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
{
public:
	CRYINTERFACE_SIMPLE(Cry::IEnginePlugin)
	CRYGENERATE_SINGLETONCLASS_GUID(CChrysalisCorePlugin, "ChrysalisCore", "{CA15546E-9C8A-4554-9EE6-426DEEF423EA}"_cry_guid);
	static CryGUID GetSchematycPackageGUID() { return "{A403DB90-2A16-434A-8603-9B35098F6364}"_cry_guid; }

	void RegisterComponents(Schematyc::IEnvRegistrar& registrar);

	PLUGIN_FLOWNODE_REGISTER
	PLUGIN_FLOWNODE_UNREGISTER

	virtual ~CChrysalisCorePlugin();

	// ICryPlugin
	virtual const char* GetName() const override { return "ChrysalisCore"; }
	virtual const char* GetCategory() const override { return "Game"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~ICryPlugin

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// INetworkedClientListener
	// Sent to the local client on disconnect
	virtual void OnLocalClientDisconnected(EDisconnectionCause cause, const char* description) override {}

	// Sent to the server when a new client has started connecting
	// Return false to disallow the connection
	virtual bool OnClientConnectionReceived(int channelId, bool bIsReset) override;

	// Sent to the server when a new client has finished connecting and is ready for gameplay
	// Return false to disallow the connection and kick the player
	virtual bool OnClientReadyForGameplay(int channelId, bool bIsReset) override;

	// Sent to the server when a client is disconnected
	virtual void OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient) override;

	// Sent to the server when a client is timing out (no packets for X seconds)
	// Return true to allow disconnection, otherwise false to keep client.

	virtual bool OnClientTimingOut(int channelId, EDisconnectionCause cause, const char* description) override { return true; }
	// ~INetworkedClientListener

	static CChrysalisCorePlugin* Get();

	CObjectIdMasterFactory* GetObjectId() { return m_pObjectIdMasterFactory; }

protected:
	// Map containing player components, key is the channel id received in OnClientConnectionReceived
	std::unordered_map<int, EntityId> m_players;


private:
	/** The object identifier master factory. */
	CObjectIdMasterFactory* m_pObjectIdMasterFactory { nullptr };
};
}