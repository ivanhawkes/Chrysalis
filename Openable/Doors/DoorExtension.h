#pragma once

#include <Openable/Doors/IDoorExtension.h>


/**
TODO: Document this class.

\sa	CGameObjectExtensionHelper&lt;CDoorExtension, IGameObjectExtension&gt;
\sa	IGameObjectView
*/
class CDoorExtension : public CGameObjectExtensionHelper < CDoorExtension, IDoorExtension >
{
public:
	// ***
	// *** IGameObjectExtension
	// ***

	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;


	// ***
	// *** CDoorExtension
	// ***
};