#pragma once

#include <IGameObject.h>
#include <IViewSystem.h>
#include <Openable/Keys/IKeyExtension.h>


/**
TODO: Document this class.

\sa	CGameObjectExtensionHelper&lt;CKeyExtension, IGameObjectExtension&gt;
\sa	IGameObjectView
*/
class CKeyExtension : public CGameObjectExtensionHelper < CKeyExtension, IKeyExtension >
{
public:
	// ***
	// *** IGameObjectExtension
	// ***

	void GetMemoryUsage(ICrySizer *pSizer) const override;
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
	void InitClient(int channelId) override {};
	void PostInitClient(int channelId) override {};
	bool ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	void PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override {};
	bool GetEntityPoolSignature(TSerialize signature) override { return true; };
	void Release() override { delete this; };
	void FullSerialize(TSerialize ser) override {};
	bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) override { return true; };
	void PostSerialize() override {};
	void SerializeSpawnInfo(TSerialize ser) override {};
	ISerializableInfoPtr GetSpawnInfo() override { return nullptr; };
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void HandleEvent(const SGameObjectEvent& event) override {};
	void ProcessEvent(SEntityEvent& event) override {};
	void SetChannelId(uint16 id) override {};
	void SetAuthority(bool auth) override {};
	void PostUpdate(float frameTime) override {};
	void PostRemoteSpawn() override {};


	// ***
	// *** CKeyExtension
	// ***

	/**
	This instance's default constructor.
	*/
	CKeyExtension();


	/**
	This instance's default destructor.
	*/
	~CKeyExtension();
};