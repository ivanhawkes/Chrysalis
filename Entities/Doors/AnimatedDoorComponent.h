#pragma once

#include <Entities/Helpers/NativeEntityBase.h>

struct ILockableComponent;


/**
An animated door.

\sa CGameObjectExtensionHelper<CAnimatedDoorComponent, CNativeEntityBase>
**/
class CAnimatedDoorComponent : public CGameObjectExtensionHelper<CAnimatedDoorComponent, CNativeEntityBase>
{
public:
	enum EInputPorts
	{
		eInputPort_Open = 0,
		eInputPort_Close
	};

	// Indices of the properties, registered in the Register function
	enum EProperties
	{
		eProperty_Model = 0,
		eProperty_Mass,

		eNumProperties
	};


	CAnimatedDoorComponent();
	virtual ~CAnimatedDoorComponent() {}

	// CNativeEntityBase
	void PostInit(IGameObject* pGameObject) override;
	void ProcessEvent(SEntityEvent& event) override;
	// ~CNativeEntityBase

	// Called to register the entity class and its properties
	static void Register();

	// Called when one of the input Flowgraph ports are activated in one of the entity instances tied to this class
	static void OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo* pActInfo, const class CFlowGameEntityNode *pNode);

protected:
	// Called on entity spawn, or when the state of the entity changes in Editor
	void Reset();

	// Specifies the entity geometry slot in which the light is loaded, -1 if not currently loaded
	// We default to using slot 1 for this light sample, in case the user would want to put geometry into slot 0.
	int m_lightSlot { -1 };

	// Light parameters, updated in the Reset function
	CDLight m_light;


	// ***
	// *** CAnimatedDoorComponent
	// ***

	ILockableComponent* m_lockableExtension { nullptr };
};