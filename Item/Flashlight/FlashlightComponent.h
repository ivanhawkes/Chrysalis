#pragma once

#include <IGameObject.h>
#include <Item/Item.h>
#include <Item/Flashlight/ItemFlashlightParameter.h>
#include <Item/Flashlight/ItemFlashlightParameterShared.h>
#include <SharedParameters/DynamicLight.h>
#include <Entities/EntityEffects.h>
#include <Entities/Interaction/IEntityInteraction.h>
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>


class CEntityInteractionComponent;


/**
A flashlight component.

\sa CGameObjectExtensionHelper&lt;CFlashlightComponent, IGameObjectExtension&gt;
\sa IGameObjectView
\sa IInteractionSwitch
\sa IInteractionPickupAndDrop
\sa IInteractionInteract
**/
//class CFlashlightComponent : public CItem, public IEntityPropertyGroup, public IInteractionSwitch, public IInteractionPickupAndDrop, public IInteractionInteract
class CFlashlightComponent : public CGameObjectExtensionHelper <CFlashlightComponent, CItem>, public IEntityPropertyGroup, public IInteractionSwitch, public IInteractionPickupAndDrop, public IInteractionInteract
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CFlashlightComponent, "Flashlight", 0x6B4BFAC58E004F0E, 0xB09881AE0D6B2582)

public:

	// IEntityComponent
	void Initialize() override {};
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return CItem::GetEventMask() | BIT64(ENTITY_EVENT_START_LEVEL) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_EDITOR_PROPERTY_CHANGED) | BIT64(ENTITY_EVENT_XFORM_FINISHED_EDITOR); }
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const { return "Flashlight"; };
	void SerializeProperties(Serialization::IArchive& archive);
	// ~IEntityPropertyGroup

	// ISimpleItem
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
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

	/**
	This instance's default constructor.
	*/
	CFlashlightComponent();


	/**
	This instance's default destructor.
	*/
	~CFlashlightComponent();


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
	bool m_isSwitchedOn { true };

	/** The battery level - range 0.0f - 1.0f. */
	float m_batteryLevel { 1.0f };

	/** Identifier for the light once it's attached to this entity. */
	EntityEffects::TAttachedEffectId m_lightId { EntityEffects::EFFECTID_INVALID };

	int m_fpGeomSlotId { -1 };

	CEntityInteractionComponent* m_interactor { nullptr };
};