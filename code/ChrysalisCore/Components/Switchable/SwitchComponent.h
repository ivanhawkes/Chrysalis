#pragma once

#include <Components/Interaction/EntityInteractionComponent.h>


namespace Chrysalis
{
class CSwitchComponent
	: public IEntityComponent
	, public IInteractionSwitch
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void Initialize() final;
	// ~IEntityComponent

public:
	CSwitchComponent() {}
	virtual ~CSwitchComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CSwitchComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{FB9ADD5D-D384-4414-877D-D670A3FE94E4}"_cry_guid;
		return id;
	}

	const string kQueueSignal { "interaction_switch" };
	const string kSwitchOnVerb { "interaction_switch_on" };
	const string kSwitchOffVerb { "interaction_switch_off" };


	struct SSwitchOnSignal
	{
		SSwitchOnSignal() = default;
	};


	struct SSwitchOffSignal
	{
		SSwitchOffSignal() = default;
	};


	struct SSwitchToggleSignal
	{
		SSwitchToggleSignal() = default;
	};


	// IInteractionSwitch
	virtual void OnInteractionSwitchToggle() override;;
	virtual void OnInteractionSwitchOn() override;;
	virtual void OnInteractionSwitchOff() override;;
	// ~IInteractionSwitch

protected:
	void InformAllLinkedEntities(string verb, bool isSwitchedOn);

	virtual void OnResetState();

	/** True if this switch is able to be used. */
	bool m_isEnabled { true };

	/** Indicates if the switch is in the 'on' position. */
	bool m_isSwitchedOn { false };

	/** True if this switch can only be used once. */
	bool m_isSingleUseOnly { false };

	/** Keep track of these so we can enable / disable as needed. */
	CInteractionSwitchTogglePtr m_switchTogglePtr { nullptr };
	CInteractionSwitchOnPtr m_switchOnPtr { nullptr };
	CInteractionSwitchOffPtr m_switchOffPtr { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** Send an alternative queue signal to DRS if the string is not empty. */
	Schematyc::CSharedString m_queueSignal;
};
}