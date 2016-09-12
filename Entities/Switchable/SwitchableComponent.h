#pragma once

#include "ISwitchableComponent.h"


/**
A switchable component.

\sa CGameObjectExtensionHelper<CSwitchableComponent, ISwitchableComponent>
**/
class CSwitchableComponent : public CGameObjectExtensionHelper <CSwitchableComponent, ISwitchableComponent>
{
public:
	// Indices of the properties, registered in the Register function
	enum EProperties
	{
		ePropertyGroup_SwitchableBegin,
		eProperty_Switchable_Enabled,
		eProperty_Switchable_IsSwitchOn,
		ePropertyGroup_SwitchableEnd,

		eNumProperties
	};

	// CNativeEntityBase
	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	// ~CNativeEntityBase

	// ISwitchableComponent
	void OnSwitchOn() override { gEnv->pLog->LogAlways("OnSwitchOn fired."); ISwitchableComponent::OnSwitchOn(); }
	void OnSwitchOff() override { gEnv->pLog->LogAlways("OnSwitchOff fired."); ISwitchableComponent::OnSwitchOff(); }
	// ~ISwitchableComponent


	// ***
	// *** CSwitchableComponent
	// ***

	CSwitchableComponent() {};
	virtual ~CSwitchableComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

	// Called to register the entity class and its properties
	static void Register();
};