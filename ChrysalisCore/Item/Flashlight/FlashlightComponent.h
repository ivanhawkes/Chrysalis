#pragma once

#include <IGameObject.h>
#include <Item/Item.h>
#include <Item/Flashlight/ItemFlashlightParameter.h>
#include <Item/Flashlight/ItemFlashlightParameterShared.h>
#include <SharedParameters/DynamicLight.h>
#include <Entities/EntityEffects.h>
#include <Entities/Interaction/IEntityInteraction.h>
#include "Helpers/DesignerEntityComponent.h"
#include <Components/Geometry/GeometryComponent.h>
#include <Components/Lights/DynamicLightComponent.h>


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
//class CFlashlightComponent : public CGameObjectExtensionHelper <CFlashlightComponent, CItem>, public IEntityPropertyGroup,
//	public IInteractionSwitch, public IInteractionPickupAndDrop, public IInteractionInteract
class CFlashlightComponent : public CDesignerEntityComponent<>, public IEntityPropertyGroup,
	public IInteractionSwitch, public IInteractionPickupAndDrop, public IInteractionInteract,
	public CGeometryComponent::IGeometryListener, public CDynamicLightComponent::IDynamicLightListener
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CFlashlightComponent, "FlashlightComponent", 0x7BFC30B2D9F541D4, 0x9A79F422E3B49400)

public:

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_START_LEVEL) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_EDITOR_PROPERTY_CHANGED) | BIT64(ENTITY_EVENT_XFORM_FINISHED_EDITOR); }
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Flashlight Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// CDynamicLightComponent::IDynamicLightListener
	void OnGeometryResetState() override;
	// ~CDynamicLightComponent::IDynamicLightListener

	// CGeometryComponent::IGeometryListener
	void OnDynamicLightResetState() override;
	// ~CGeometryComponent::IGeometryListener


	// ISimpleItem
	//bool Init(IGameObject * pGameObject) override;
	//void PostInit(IGameObject * pGameObject) override;
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


	//void GetSharedParameters(XmlNodeRef rootParams);
	//void LoadFromXML();


	/** Toggle flash light on switch. */
	void ToggleSwitch();


	/**
	Enables the flash light, creating and destroying light sources as needed.
	
	\param	isSwitchedOn true to enable, false to disable.
	**/
	void Switch(bool isSwitchedOn);


	/** true if a flash light is enabled, false if not. */
	bool IsSwitchedOn() { return m_isSwitchedOn; }

private:
	const Quat kRightToForward = Quat::CreateRotationXYZ(Ang3(0.0f, 0.0f, DEG2RAD(90.0f)));

	void OnResetState();

	/** Dynamic light. */
	CDynamicLightComponent* m_pDynamicLightComponent { nullptr };

	/** Model for the geometry. */
	CGeometryComponent* m_pGeometryComponent { nullptr };

	/** Instanced flashlight parameters. */
	CItemFlashlightParameter m_itemFlashlightParameter;

	/** true if this object is switched on. */
	bool m_isSwitchedOn { true };

	/** The battery level - range 0.0f - 1.0f. */
	float m_batteryLevel { 1.0f };

	CEntityInteractionComponent* m_interactor { nullptr };

	/** A local postion offset within the slot for the light. **/
	Vec3 m_lightLocalPosition { ZERO };

	/** A local rotation offset within the slot for the light. **/
	Quat m_lightLocalRotation { kRightToForward };

	/** Shared flashlight parameters. */
	//SItemFlashlightParameterSharedConstPtr m_itemFlashlightParameterShared;

	/** Parameters for the associated dynamic light. */
	//SDynamicLightConstPtr m_dynamicLightParameterShared;
};