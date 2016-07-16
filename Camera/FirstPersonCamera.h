#pragma once

#include <IGameObject.h>
#include <IViewSystem.h>
#include "ICamera.h"


/**
An first person camera suitable for use with games using a first person view.

\sa CGameObjectExtensionHelper&lt;CFirstPersonCamera, IGameObjectExtension&gt;
\sa IGameObjectView
**/
class CFirstPersonCamera : public CGameObjectExtensionHelper <CFirstPersonCamera, ICamera>,public IGameObjectView
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
	void Release() override;
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
	// *** IGameObjectView
	// ***


	/**
	Called when the view should provide an update of it's current state.
	
	\param	params A structure with members that are used to control view parameters.
	**/
	void UpdateView(SViewParams& params) override;


	/**
	Called when the view should provide a post update of it's current state.
	
	\param	params A variable-length parameters list containing view parameters.
	**/
	void PostUpdateView(SViewParams& params) override {};


	// ***
	// *** ICamera
	// ***


	/**
	Switch which entity the camera will treat as it's attachment.

	\param	entityID	If non-null, the entity.
	*/
	void AttachToEntity(EntityId entityID) override;

	/** Executes the activate action. */
	void OnActivate() override;

	/** Executes the deactivate action. */
	void OnDeactivate() override;


	// ***
	// *** CFirstPersonRPGCamera
	// ***

public:

	/** This instance's default constructor. */
	CFirstPersonCamera() {};

	/** This instance's default destructor. */
	virtual ~CFirstPersonCamera() {};


private:
	/** A static handler for the actions we are interested in hooking. */
	TActionHandler<CFirstPersonCamera> m_actionHandler;

	/** Identifier for the entity which this camera is targeted towards. */
	EntityId m_targetEntityID = INVALID_ENTITYID;

	/** The view camera for this instance. */
	IView* m_pView = nullptr;
};