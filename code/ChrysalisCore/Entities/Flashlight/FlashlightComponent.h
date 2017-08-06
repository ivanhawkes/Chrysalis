#pragma once

#include <IGameObject.h>
#include <Components/Items/ItemComponent.h>
#include <Item/Parameters/ItemFlashlightParameter.h>
#include <Item/Parameters/ItemFlashlightParameterShared.h>
#include <SharedParameters/DynamicLight.h>
#include <Entities/EntityEffects.h>
#include <Entities/Interaction/IEntityInteraction.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>
#include <Components/Lights/DynamicLightComponent.h>


namespace Chrysalis
{
class CEntityInteractionComponent;
class CItemInteractionComponent;
class CSnaplockComponent;


/**
A flashlight component.

**/
class CFlashlightComponent
	: public IEntityComponent
	, public IInteractionInteract
	, public IInteractionSwitch
	//, public Cry::DefaultComponents::CStaticMeshComponent::IGeometryListener
	//, public CDynamicLightComponent::IDynamicLightListener
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	// ~IEntityComponent

public:
	CFlashlightComponent() {}
	virtual ~CFlashlightComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CFlashlightComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{9BED57A9-8CA3-4588-AF4A-60A3AE797336}"_cry_guid;
		return id;
	}

	// Cry::DefaultComponents::CStaticMeshComponent::IGeometryListener
	//void OnGeometryResetState() override;
	// ~Cry::DefaultComponents::CStaticMeshComponent::IGeometryListener

	// CDynamicLightComponent::IDynamicLightListener
	//void OnDynamicLightResetState() override;
	// ~CDynamicLightComponent::IDynamicLightListener

	// IInteractionInteract
	void OnInteractionInteract() override { gEnv->pLog->LogAlways("OnInteractionInteract fired."); };
	// ~IInteractionInteract

	// IInteractionSwitch
	void OnInteractionSwitchToggle() override { gEnv->pLog->LogAlways("OnInteractionSwitchToggle fired."); ToggleSwitch(); };
	void OnInteractionSwitchOn() override { gEnv->pLog->LogAlways("OnInteractionSwitchOn fired."); Switch(true); };
	void OnInteractionSwitchOff() override { gEnv->pLog->LogAlways("OnInteractionSwitchOff fired."); Switch(false); };
	// ~IInteractionSwitch

	// ***
	// *** CFlashlightComponent
	// ***

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

	virtual void OnResetState();

	/** Dynamic light. */
	CDynamicLightComponent* m_pDynamicLightComponent { nullptr };

	/** Model for the geometry. */
	Cry::DefaultComponents::CStaticMeshComponent* m_pGeometryComponent { nullptr };

	/** Instanced flashlight parameters. */
	CItemFlashlightParameter m_itemFlashlightParameter;

	/**  Provide ability to offer interactions. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** Standard interactions for an item. */
	CItemInteractionComponent* m_pItemInteractionComponent { nullptr };

	/** true if this object is switched on. */
	bool m_isSwitchedOn { false };

	/** The battery level - range 0.0f - 1.0f. */
	float m_batteryLevel { 1.0f };

	/** A local postion offset within the slot for the light. **/
	Vec3 m_lightLocalPosition { ZERO };

	/** A local rotation offset within the slot for the light. **/
	Quat m_lightLocalRotation { kRightToForward };

	// HACK: This shouldn't be needed once we move to being derived from CItemComponent.

	/** A component that allows for management of snaplocks. */
	CSnaplockComponent* m_pSnaplockComponent { nullptr };

	/** Shared flashlight parameters. */
	//SItemFlashlightParameterSharedConstPtr m_itemFlashlightParameterShared;

	/** Parameters for the associated dynamic light. */
	//SDynamicLightConstPtr m_dynamicLightParameterShared;
};
}