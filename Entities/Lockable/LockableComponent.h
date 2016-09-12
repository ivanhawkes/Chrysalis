#pragma once

#include <Entities/Lockable/ILockableComponent.h>


/**
A lock extension.

\sa CGameObjectExtensionHelper&lt;CLockableComponent, CNativeEntityBase&gt;
\sa ILockExtension
**/
class CLockableComponent : public CGameObjectExtensionHelper <CLockableComponent, ILockableComponent>
{
public:
	enum EInputPorts
	{
		eInputPort_Unlock = 0,
		eInputPort_Lock
	};

	// Indices of the properties, registered in the Register function
	enum EProperties
	{
		ePropertyGroup_LockableBegin,
		eProperty_Lockable_IsLocked,
		ePropertyGroup_LockableEnd,

		eNumProperties
	};

	// CNativeEntityBase
	void PostInit(IGameObject * pGameObject) override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	// ~CNativeEntityBase


	// ***
	// *** CLockableComponent
	// ***

	CLockableComponent() {};
	virtual ~CLockableComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

	// Called to register the entity class and its properties
	static void Register();

	// Called when one of the input Flowgraph ports are activated in one of the entity instances tied to this class
	static void OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const class CFlowGameEntityNode *pNode);
};