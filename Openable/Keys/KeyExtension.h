#pragma once

#include <Openable/Keys/IKeyExtension.h>


/**
TODO: Document this class.

\sa	CGameObjectExtensionHelper&lt;CKeyExtension, IGameObjectExtension&gt;
\sa	IGameObjectView
*/
class CKeyExtension : public CGameObjectExtensionHelper < CKeyExtension, IKeyExtension >
{
public:
	// ***
	// *** IGameObjectExtension
	// ***

	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;


	// ***
	// *** CKeyExtension
	// ***
};