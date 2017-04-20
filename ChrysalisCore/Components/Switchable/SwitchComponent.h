#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Geometry/GeometryComponent.h>


class CSwitchComponent final : public CDesignerEntityComponent<>, public IEntityPropertyGroup, public IInteractionSwitch
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CSwitchComponent, "SwitchComponent", 0x814B5D9866864051, 0x948F23D33989D88D)

	virtual ~CSwitchComponent() {}

public:
	const string kQueueSignal { "interaction_switch" };
	const string kSwitchOnVerb { "interaction_switch_on" };
	const string kSwitchOffVerb { "interaction_switch_off" };

	struct ISwitchListener
	{
		virtual ~ISwitchListener() {};

		virtual void OnSwitchResetState() = 0;
	};

	void AddEventListener(ISwitchListener* pListener)
	{
		assert(pListener);
		if (pListener)
			stl::push_back_unique(m_ListenersList, pListener);
	}

	void RemoveEventListener(ISwitchListener* pListener)
	{
		assert(pListener);
		m_ListenersList.remove(pListener);
	}

public:
	// IEntityComponent
	virtual void Initialize() final;
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "Switch Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// IInteractionSwitch
	virtual void OnInteractionSwitchToggle() override;;
	virtual void OnInteractionSwitchOn() override;;
	virtual void OnInteractionSwitchOff() override;;
	// ~IInteractionSwitch

	// ISwitchEntityComponent
	virtual void OnResetState() final;
	// ISwitchEntityComponent

private:
	void InformAllLinkedEntities(string verb, bool isSwitchedOn);

	typedef std::list<ISwitchListener*> TListenersList;
	TListenersList m_ListenersList;

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

	/** Model for the geometry. */
	CGeometryComponent* m_pGeometryComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** Send an alternative queue signal to DRS if the string is not empty. */
	string m_queueSignal;

	/** Send this verb when switching on, if the string in not empty. */
	string m_switchOnVerb;

	/** Send this verb when switching off, if the string in not empty. */
	string m_switchOffVerb;
};
