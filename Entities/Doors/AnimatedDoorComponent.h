#pragma once

#include <Entities/Helpers/NativeEntityBase.h>
#include <Entities/Interaction/IEntityInteractionComponent.h>


struct ILockableComponent;


/**
An animated door.

\sa CGameObjectExtensionHelper<CAnimatedDoorComponent, CNativeEntityBase>
**/
class CAnimatedDoorComponent : public CGameObjectExtensionHelper<CAnimatedDoorComponent, CNativeEntityBase>, public IInteractionContainer
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


	// ***
	// *** IInteractionContainer
	// ***

	void ContainerOpen() override { gEnv->pLog->LogAlways("Interation Toggle fired."); /*ToggleSwitch();*/ };
	void ContainerClose() override { gEnv->pLog->LogAlways("Interation Toggle fired."); /*ToggleSwitch();*/ };
	void ContainerLock() override { gEnv->pLog->LogAlways("Interation Toggle fired."); /*ToggleSwitch();*/ };
	void ContainerUnlock() override { gEnv->pLog->LogAlways("Interation Toggle fired."); /*ToggleSwitch();*/ };


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

	// ***
	// *** CAnimatedDoorComponent
	// ***

	ILockableComponent* m_lockableExtension { nullptr };

	IEntityInteractionComponent* m_interactor { nullptr };
};