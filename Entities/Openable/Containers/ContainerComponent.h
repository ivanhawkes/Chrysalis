#pragma once

#include <Entities/Openable/Containers/IContainerComponent.h>


/**
A container extension.

\sa CGameObjectExtensionHelper&lt;CContainerComponent, CNativeEntityBase&gt;
\sa IContainerComponent
**/
class CContainerComponent : public CGameObjectExtensionHelper <CContainerComponent, IContainerComponent>
{
public:
	// CNativeEntityBase
	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	// ~CNativeEntityBase


	// ***
	// *** CContainerComponent
	// ***

	CContainerComponent() {};
	virtual ~CContainerComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;
};