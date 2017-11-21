#pragma once

#include <Components/Interaction/EntityInteractionComponent.h>


namespace Chrysalis
{
class CInteractComponent
	: public IEntityComponent
	, public IInteractionInteract
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void Initialize() final;
	// ~IEntityComponent

public:
	CInteractComponent() {}
	virtual ~CInteractComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CInteractComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{C03D46F4-D1C9-45BE-AAC6-2362EFD3ABCC}"_cry_guid;
		return id;
	}

	const string kQueueSignal { "interaction_interact" };
	const string kInteractStartVerb { "interaction_interact_start" };
	const string kInteractTickVerb { "interaction_interact_tick" };
	const string kInteractCompleteVerb { "interaction_interact_complete" };
	const string kInteractCancelVerb { "interaction_interact_cancel" };


	struct SInteractStartSignal
	{
		SInteractStartSignal() = default;
	};


	struct SInteractTickSignal
	{
		SInteractTickSignal() = default;
		SInteractTickSignal(float deltaPitch, float deltaYaw) : m_deltaPitch (deltaPitch), m_deltaYaw(deltaYaw) {}

		float m_deltaPitch { 0.0f };
		float m_deltaYaw { 0.0f };
	};


	struct SInteractCompleteSignal
	{
		SInteractCompleteSignal() = default;
	};


	struct SInteractCancelSignal
	{
		SInteractCancelSignal() = default;
	};


	// IInteractionInteract
	virtual void OnInteractionInteractStart() override;
	virtual void OnInteractionInteractTick() override;
	virtual void OnInteractionInteractComplete() override;
	virtual void OnInteractionInteractCancel() override;
	// ~IInteractionInteract

protected:
	void InformAllLinkedEntities(string verb, bool isInteractedOn);

	virtual void OnResetState();

	/** True if this Interact is able to be used. */
	bool m_isEnabled { true };

	/** True if this Interact can only be used once. */
	bool m_isSingleUseOnly { false };

	/** Keep track of these so we can enable / disable as needed. */
	CInteractionInteractPtr m_interactPtr { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** Send an alternative queue signal to DRS if the string is not empty. */
	Schematyc::CSharedString m_queueSignal;
};
}