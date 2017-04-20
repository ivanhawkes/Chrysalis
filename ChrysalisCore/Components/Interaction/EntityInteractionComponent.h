#pragma once

#include <Entities/Interaction/IEntityInteraction.h>


/**
An entity interaction component.

\sa IEntityComponent
\sa IEntityPropertyGroup
**/
class CEntityInteractionComponent : public IEntityComponent, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CEntityInteractionComponent, "EntityInteraction", 0xE2F360C6F7FF41DE, 0xA1A2D7C7C64AD730)

public:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE); }
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Entity Interaction Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup


	// ***
	// *** CEntityInteractionComponent
	// ***

	CEntityInteractionComponent() {};
	virtual ~CEntityInteractionComponent() {};

	/** Updates this instance. */
	void Update();

	std::vector<string> GetVerbs(bool includeHidden = false);

	void AddInteraction(IInteractionPtr interaction);
	void RemoveInteraction(string verb);
	IInteractionWeakPtr GetInteraction(string verb);
	IInteractionWeakPtr SelectInteractionVerb(string verb);
	void ClearInteractionVerb();

	void OnInteractionStart();
	void OnInteractionComplete();
	void OnInteractionCancel();

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

private:
	std::vector<IInteractionPtr> m_Interactions;
	IInteractionPtr m_selectedInteraction { IInteractionPtr() };
};