#pragma once

#include <IGameObject.h>
#include <Openable/Locks/ILockExtension.h>


/**
TODO: Document this class.

\sa	CGameObjectExtensionHelper&lt;CLockExtension, IGameObjectExtension&gt;
\sa	IGameObjectView
*/
class CLockExtension : public CGameObjectExtensionHelper < CLockExtension, ILockExtension >
{
public:
	// ***
	// *** IGameObjectExtension
	// ***

	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;


	// ***
	// *** CLockExtension
	// ***
};