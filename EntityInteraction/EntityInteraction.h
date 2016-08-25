#pragma once

#include <IGameObject.h>
#include <EntityInteraction/IEntityInteraction.h>


/**
TODO: Document this class.

\sa	CGameObjectExtensionHelper&lt;CEntityInteraction, IGameObjectExtension&gt;
\sa	IGameObjectView
*/
class CEntityInteraction : public CGameObjectExtensionHelper <CEntityInteraction, IEntityInteraction>
{
public:
	// ***
	// *** IGameObjectExtension
	// ***

	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;


	// ***
	// *** CEntityInteraction
	// ***

	IInteraction* m_selectedInteraction { nullptr };
};