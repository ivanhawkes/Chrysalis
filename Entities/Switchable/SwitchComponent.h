#pragma once

#include <Entities/Switchable/ISwitchComponent.h>


/**
A switch component.

\sa CGameObjectExtensionHelper&lt;CSwitchComponent, CNativeEntityBase&gt;
\sa ILockExtension
**/
class CSwitchComponent : public CGameObjectExtensionHelper <CSwitchComponent, ISwitchComponent>
{
public:
	enum EInputPorts
	{
		eInputPort_Unlock = 0,
		eInputPort_Lock
	};

	// Indices of the properties, registered in the Register function
	enum EProperties
	{
		ePropertyGroup_SwitchBegin,
		eProperty_Switch_Enabled,
		eProperty_Switch_IsSwitchOn,
		ePropertyGroup_SwitchEnd,

		eNumProperties
	};

	// CNativeEntityBase
	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	// ~CNativeEntityBase

	// ISwitchComponent
	void SwitchOn() override { gEnv->pLog->LogAlways("SwitchOn requested."); };
	void SwitchOff() override { gEnv->pLog->LogAlways("SwitchOff requested."); };
	// ~ISwitchComponent


	// ***
	// *** CSwitchComponent
	// ***

	CSwitchComponent() {};
	virtual ~CSwitchComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

	// Called to register the entity class and its properties
	static void Register();
};