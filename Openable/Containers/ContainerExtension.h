#pragma once

#include <Openable/Containers/IContainerExtension.h>


/**
TODO: Document this class.

\sa	CGameObjectExtensionHelper&lt;CContainerExtension, IGameObjectExtension&gt;
\sa	IGameObjectView
*/
class CContainerExtension : public CGameObjectExtensionHelper <CContainerExtension, IContainerExtension>
{
public:
	// ***
	// *** IGameObjectExtension
	// ***

	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;


	// ***
	// *** CContainerExtension
	// ***
};