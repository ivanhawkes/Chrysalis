#pragma once

#include <IGameObject.h>
#include <Entities/Helpers/NativeEntityBase.h>
#include <Item/Item.h>
#include <Item/Flashlight/ItemFlashlightParameter.h>
#include <Item/Flashlight/ItemFlashlightParameterShared.h>
#include <SharedParameters/DynamicLight.h>
#include <Entities/EntityEffects.h>
#include <Entities/Interaction/IEntityInteractionComponent.h>


class IEntityInteractionComponent;


/**
\sa	CGameObjectExtensionHelper&lt;CFlashlightComponent, IGameObjectExtension&gt;
\sa	IGameObjectView
*/
class CFlashlightComponent : public CGameObjectExtensionHelper <CFlashlightComponent, CItem>, public IInteractionSwitch, public IInteractionPickupAndDrop, public IInteractionInteract
{
public:

	// ISimpleItem
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
	void ProcessEvent(SEntityEvent& event) override;
	// ~ISimpleItem


	// ***
	// *** IInteractionInteract
	// ***

	void Interact() override { gEnv->pLog->LogAlways("Interation Interact fired."); };

	// ***
	// *** IInteractionSwitch
	// ***

	void SwitchToggle() override { gEnv->pLog->LogAlways("Interation Toggle fired."); ToggleSwitch(); };
	void SwitchOn() override { gEnv->pLog->LogAlways("Interation SwitchOn fired."); ToggleSwitch(); };
	void SwitchOff() override { gEnv->pLog->LogAlways("Interation SwitchOff fired."); ToggleSwitch(); };


	// ***
	// *** IInteractionPickupAndDrop
	// ***

	void Pickup() override { gEnv->pLog->LogAlways("Interation Pickup fired."); };
	void Drop() override { gEnv->pLog->LogAlways("Interation Drop fired."); };
	void Inspect() override { gEnv->pLog->LogAlways("Interation Inspect fired."); };


	// ***
	// *** CFlashlightComponent
	// ***

	enum EInputPorts
	{
		eInputPort_SwitchOn = 0,
		eInputPort_SwitchOff
	};


	/**
	This instance's default constructor.
	*/
	CFlashlightComponent();


	/**
	This instance's default destructor.
	*/
	~CFlashlightComponent();

	// Called to register the entity class and its properties
	static void Register();


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
	 Queries if a flash light is enabled.

	 \return	true if a flash light is enabled, false if not.
	 */

	bool IsSwitchedOn() { return m_isSwitchedOn; }

private:

	void Reset();

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
	EntityEffects::TAttachedEffectId m_lightId { EntityEffects::EFFECTID_INVALID };

	int m_fpGeomSlotId = -1;

	IEntityInteractionComponent* m_interactor { nullptr };
};