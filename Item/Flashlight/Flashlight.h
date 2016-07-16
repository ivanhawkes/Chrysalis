#pragma once

#include <IGameObject.h>
#include <IViewSystem.h>
#include <Item/Item.h>
#include <Item/Flashlight/ItemFlashlightParameter.h>
#include <Item/Flashlight/ItemFlashlightParameterShared.h>
#include <SharedParameters/DynamicLight.h>
#include <Entity/EntityEffects.h>


/**
\sa	CGameObjectExtensionHelper&lt;CFlashlight, IGameObjectExtension&gt;
\sa	IGameObjectView
*/
class CFlashlight : public CGameObjectExtensionHelper < CFlashlight, CItem >
{
public:
	// ***
	// *** IGameObjectExtension
	// ***

	void GetMemoryUsage(ICrySizer *pSizer) const override;
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
	void InitClient(int channelId) override;
	void PostInitClient(int channelId) override;
	bool ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	void PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	bool GetEntityPoolSignature(TSerialize signature) override;
	void Release() override;
	void FullSerialize(TSerialize ser) override;
	bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) override;
	void PostSerialize() override {};
	void SerializeSpawnInfo(TSerialize ser) override {};
	ISerializableInfoPtr GetSpawnInfo() override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void HandleEvent(const SGameObjectEvent& event) override;
	void ProcessEvent(SEntityEvent& event) override;
	void SetChannelId(uint16 id) override;
	void SetAuthority(bool auth) override;
	void PostUpdate(float frameTime) override;
	void PostRemoteSpawn() override;


	// ***
	// *** IItem
	// ***

	//void PreResetParams() override {};
	//bool ResetParams() override;


	// ***
	// *** CFlashlight
	// ***

	/**
	This instance's default constructor.
	*/
	CFlashlight();


	/**
	This instance's default destructor.
	*/
	~CFlashlight();


	void GetSharedParameters(XmlNodeRef rootParams);
	void LoadFromXML();


	/** Toggle flash light on switch. */
	void ToggleSwitch();


	/**
	 Enables the flash light, creating and destroying light sources as needed.
	
	 \param	on	true to enable, false to disable.
	 */

	void Switch(bool isSwitchedOn);


	/**
	Physicalies this instance.

	\return	True if physicalization was successful. False otherwise.
	*/
	void Physicalize(bool enable, bool rigid) override;


	/**
	 Queries if a flash light is enabled.
	
	 \return	true if a flash light is enabled, false if not.
	 */

	bool IsSwitchedOn();

private:

	/**
	Registers this instance for GameObject event notifications (will receive HandleEvent() calls).
	*/
	void RegisterEvents();


	/**
	Scripts are able to raise an event which is passed back to the c++ code though the ENTITY_EVENT_SCRIPT_EVENT.
	This method handles those events.
	
	\param	eventName	  	Name of the event.
	\param	eventValueType	Type of the event value.
	\param	pEventValue   	The event value.
	*/
	void OnScriptEvent(SEntityEvent& event);
	

	/** Whenever a property in the editor is changed, this function is called. */
	void OnEditorPropertyChanged();


	/** Called to indicate the entity must reset itself. This is often done during PostInit() and
	additionally by the editor when you both enter and leave game mode. */
	void OnReset();

	/** Instanced flashlight parameters. */
	CItemFlashlightParameter m_itemFlashlightParameter;

	/** Shared flashlight parameters. */
	SItemFlashlightParameterSharedConstPtr m_itemFlashlightParameterShared;

	/** Parameters for the associated dynamic light. */
	SDynamicLightConstPtr m_dynamicLightParameterShared;
	
	/** true if this object is switched on. */
	bool m_isSwitchedOn = true;

	/** The battery level - range 0.0f - 1.0f. */
	float m_batteryLevel = 1.0f;

	/** Identifier for the light once it's attached to this entity. */
	EntityEffects::TAttachedEffectId m_lightId;

	int m_fpGeomSlotId = -1;
};