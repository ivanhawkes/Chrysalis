#pragma once

#include <Entities/Lockable/Keyring/IKeyringComponent.h>


/**
A key extension.

\sa CGameObjectExtensionHelper&lt;CKeyringComponent, CNativeEntityBase&gt;
\sa IKeyringComponent
**/
class CKeyringComponent : public CGameObjectExtensionHelper <CKeyringComponent, IKeyringComponent>
{
public:
	// Indices of the properties, registered in the Register function
	enum EProperties
	{
		ePropertyGroup_KeyringBegin,
		eProperty_Keyring_Keys,
		ePropertyGroup_KeyringEnd,

		eNumProperties
	};


	// CNativeEntityBase
	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	// ~CNativeEntityBase


	// ***
	// *** CKeyringComponent
	// ***

	CKeyringComponent() {};
	virtual ~CKeyringComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

	// Called to register the entity class and its properties
	static void Register();
};