#pragma once

#include <Entities/Interaction/IEntityInteraction.h>


namespace Chrysalis
{
/**
An entity interaction component.

\sa IEntityComponent
\sa IEntityPropertyGroup
**/

class CEntityInteractionComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE); }
	// ~IEntityComponent

public:
	CEntityInteractionComponent() {}
	virtual ~CEntityInteractionComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CEntityInteractionComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{86E63BFB-CA1E-4365-8AD5-CD208F2A7010}"_cry_guid;
		return id;
	}

	/** Updates this instance. */
	void Update();

	std::vector<string> GetVerbs(bool includeHidden = false);

	void AddInteraction(IInteractionPtr interaction);
	void RemoveInteraction(string verb);
	IInteractionWeakPtr GetInteraction(string verb);
	IInteractionWeakPtr SelectInteractionVerb(string verb);
	void ClearInteractionVerb();

	void OnInteractionStart();
	void OnInteractionTick();
	void OnInteractionComplete();
	void OnInteractionCancel();

private:
	std::vector<IInteractionPtr> m_Interactions;
	IInteractionPtr m_selectedInteraction { IInteractionPtr() };
};
}