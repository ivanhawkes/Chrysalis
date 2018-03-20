#pragma once

#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Interaction/InteractComponent.h>


namespace Chrysalis
{
class CSwitchComponent
	: public CInteractComponent
	, public IInteractionSwitch
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void Initialize() override final;
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
	virtual void OnInteractionSwitchToggle(IInteraction& pInteraction, IActorComponent& actor) override;
	virtual void OnInteractionSwitchOn(IInteraction& pInteraction, IActorComponent& actor) override;
	virtual void OnInteractionSwitchOff(IInteraction& pInteraction, IActorComponent& actor) override;
	// ~IInteractionSwitch

protected:
	/** Sends the Schematyc switch toggle signal. */
	virtual void ProcessSchematycSignalStart() override { GetEntity()->GetSchematycObject()->ProcessSignal(SSwitchToggleSignal(), GetGUID()); };

	void InformAllLinkedEntities(string verb, bool isSwitchedOn) override;

	virtual void OnResetState() override;

	/** Indicates if the switch is in the 'on' position. */
	bool m_isSwitchedOn { false };

	/** Keep track of these so we can enable / disable as needed. */
	CInteractionSwitchTogglePtr m_switchTogglePtr { nullptr };
	CInteractionSwitchOnPtr m_switchOnPtr { nullptr };
	CInteractionSwitchOffPtr m_switchOffPtr { nullptr };
};


static void ReflectType(Schematyc::CTypeDesc<CSwitchComponent::SSwitchOnSignal>& desc)
{
	desc.SetGUID("{D250B16F-D529-4A79-9268-FFBFA4252A80}"_cry_guid);
	desc.SetLabel("Switch On");
}


static void ReflectType(Schematyc::CTypeDesc<CSwitchComponent::SSwitchOffSignal>& desc)
{
	desc.SetGUID("{70C42CEE-ED6B-437C-A18D-C9441BBB6C99}"_cry_guid);
	desc.SetLabel("Switch Off");
}


static void ReflectType(Schematyc::CTypeDesc<CSwitchComponent::SSwitchToggleSignal>& desc)
{
	desc.SetGUID("{71497D0B-6600-4862-B8E1-29016D92E606}"_cry_guid);
	desc.SetLabel("Switch Toggle");
}
}