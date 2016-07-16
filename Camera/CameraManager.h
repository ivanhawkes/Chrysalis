/**
\file	Source\Actor\PlayerCamera\PlayerCamera.h

This is a camera management class. It creates several cameras, one of each type, and then provides features
for switching between the cameras. While the name implies the cameras are just for the player, in general they will
be able to follow any entity in the game.

The host entity will be used as a default for the entity which the camera operates from.
*/
#pragma once

#include <IGameObject.h>
#include <IViewSystem.h>
#include "ICameraManager.h"
#include "ICamera.h"


class CCameraManager : public CGameObjectExtensionHelper <CCameraManager, ICameraManager>, public IGameObjectView
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
	// *** ICameraManager
	// ***

	/**
	Player cameras generally need to follow an actor. This allows us to switch which entity represents the actor that
	the camera is following.

	\param	entityID	If non-null, the entity.
	*/
	void AttachToEntity(EntityId entityID) override;


	/**
	Gets current camera mode.

	\return	The current camera mode.
	*/
	ECameraMode GetCameraMode() override;


	/**
	Sets camera mode.

	\param	mode  	The mode.
	\param	reason	The reason.
	*/
	void SetCameraMode(ECameraMode mode, const char* reason) override;


	/**
	Gets the currently active camera.

	\return	null if it fails, else the camera.
	*/
	ICamera* GetCamera() const override;


	// ***
	// *** CCameraManager
	// ***

	/**
	Constructor.

	\param [in,out]	playerOwner	The player that owns this item.
	*/
	CCameraManager();

	/** Destructor. */
	virtual ~CCameraManager();


private:
	/**	An array large enough to hold one of each defined camera mode. **/
	ICamera* m_cameraModes [ECameraMode::_LAST];

	/** The current camera mode. */
	ECameraMode m_cameraMode;

	/** The last camera mode. */
	ECameraMode m_lastCameraMode;

	/** The camera pose. */
	CCameraPose m_cameraPose { CCameraPose() };
};
