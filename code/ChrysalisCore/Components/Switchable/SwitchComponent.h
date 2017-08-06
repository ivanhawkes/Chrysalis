#pragma once

#include <Components/Interaction/EntityInteractionComponent.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>

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
		static CryGUID id = "00000000-0000-0000-0000-000000000000"_cry_guid;
		return id;
	}

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

	// IInteractionSwitch
	virtual void OnInteractionSwitchToggle() override;;
	virtual void OnInteractionSwitchOn() override;;
	virtual void OnInteractionSwitchOff() override;;
	// ~IInteractionSwitch

protected:
	void InformAllLinkedEntities(string verb, bool isSwitchedOn);

	virtual void OnResetState();

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
	Cry::DefaultComponents::CStaticMeshComponent* m_pGeometryComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** Send an alternative queue signal to DRS if the string is not empty. */
	string m_queueSignal;

	/** Send this verb when switching on, if the string in not empty. */
	string m_switchOnVerb;

	/** Send this verb when switching off, if the string in not empty. */
	string m_switchOffVerb;
};
}