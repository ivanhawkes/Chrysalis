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
	// IEntityComponent
	void Initialize() override;
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

	std::vector<string> GetVerbs(bool includeHidden = false);

	void AddInteraction(IInteractionPtr interaction);
	void RemoveInteraction(string verb);
	IInteractionWeakPtr GetInteraction(string verb);
	IInteractionWeakPtr SelectInteractionVerb(string verb);
	void ClearInteractionVerb();

	void OnInteractionStart(IActor& actor);
	void OnInteractionTick(IActor& actor);
	void OnInteractionComplete(IActor& actor);

private:
	std::vector<IInteractionPtr> m_interactionQueue;
	IInteractionPtr m_selectedInteraction {IInteractionPtr()};
};
}