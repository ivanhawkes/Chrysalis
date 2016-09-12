#pragma once

#include <IGameObject.h>
#include <Entities/Interaction/IEntityInteractionComponent.h>


/**
TODO: Document this class.

\sa	CGameObjectExtensionHelper&lt;CEntityInteractionComponent, IGameObjectExtension&gt;
\sa	IGameObjectView
*/
class CEntityInteractionComponent : public CGameObjectExtensionHelper <CEntityInteractionComponent, IEntityInteractionComponent>
{
public:
	// ***
	// *** IGameObjectExtension
	// ***

	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;


	// ***
	// *** CEntityInteractionComponent
	// ***

	CEntityInteractionComponent() {};
	virtual ~CEntityInteractionComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

	IInteraction* m_selectedInteraction { nullptr };
};