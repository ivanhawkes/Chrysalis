#include <StdAfx.h>

#include "AnimatedDoorComponent.h"
#include <Game/GameFactory.h>
#include <FlowNodes/Helpers/FlowGameEntityNode.h>
#include <Entities/Lockable/ILockableComponent.h>


class CDoorRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		CAnimatedDoorComponent::Register();
	}
};

CDoorRegistrator g_doorRegistrator;


CAnimatedDoorComponent::CAnimatedDoorComponent()
{
}


void CAnimatedDoorComponent::PostInit(IGameObject* pGameObject)
{
	m_lockableExtension = static_cast<ILockableComponent*> (GetGameObject()->AcquireExtension("Lockable"));

	// We want to supply interaction verbs.
	m_interactor = static_cast<IEntityInteractionComponent*> (GetGameObject()->AcquireExtension("EntityInteraction"));
	if (m_interactor)
	{
		auto openInteractPtr = std::make_shared<CInteractionOpen>(this);
		m_interactor->AddInteraction(openInteractPtr);

		auto closeInteractPtr = std::make_shared<CInteractionClose>(this);
		m_interactor->AddInteraction(closeInteractPtr);
	}

}


void CAnimatedDoorComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		// Physicalize on level start for Launcher
		case ENTITY_EVENT_START_LEVEL:

		// Editor specific, physicalize on reset, property change or transform change
		case ENTITY_EVENT_RESET:
		case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
		case ENTITY_EVENT_XFORM_FINISHED_EDITOR:
			Reset();
			break;
	}
}

void CAnimatedDoorComponent::Reset()
{
	const char *modelPath = GetPropertyValue(eProperty_Model);
	if (strlen(modelPath) > 0)
	{
		auto &gameObject = *GetGameObject();

		const int geometrySlot = 0;
		LoadMesh(geometrySlot, modelPath);

		SEntityPhysicalizeParams physicalizationParams;
		physicalizationParams.type = PE_RIGID;
		physicalizationParams.mass = GetPropertyFloat(eProperty_Mass);

		GetEntity()->Physicalize(physicalizationParams);
	}
}


void CAnimatedDoorComponent::Register()
{
	auto properties = new SNativeEntityPropertyInfo [eNumProperties];
	memset(properties, 0, sizeof(SNativeEntityPropertyInfo) * eNumProperties);

	RegisterEntityPropertyObject(properties, eProperty_Model, "Model", "", "Sets the object of the entity");
	RegisterEntityProperty<float>(properties, eProperty_Mass, "Mass", "", "Sets the object's mass", 0, 10000);

	// Finally, register the entity class so that instances can be created later on either via Launcher or Editor
	CGameFactory::RegisterNativeEntity<CAnimatedDoorComponent>("AnimatedDoor", "Doors", "Light.bmp", 0u, properties, eNumProperties);

	// Create flownode
	CGameEntityNodeFactory &nodeFactory = CGameFactory::RegisterEntityFlowNode("AnimatedDoor");

	// Define input ports, and the callback function for when they are triggered
	std::vector<SInputPortConfig> inputs;
	inputs.push_back(InputPortConfig_Void("Open", "Open the door"));
	inputs.push_back(InputPortConfig_Void("Close", "Close the door"));
	nodeFactory.AddInputs(inputs, OnFlowgraphActivation);

	// Mark the factory as complete, indicating that there will be no additional ports
	nodeFactory.Close();
}


void CAnimatedDoorComponent::OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo *pActInfo, const class CFlowGameEntityNode *pNode)
{
	if (auto pExtension = static_cast<CAnimatedDoorComponent *>(QueryExtension(entityId)))
	{
		if (IsPortActive(pActInfo, eInputPort_Open))
		{
			//			pExtension->SetPropertyBool(eProperty_IsOpen, true);
		}
		else if (IsPortActive(pActInfo, eInputPort_Close))
		{
			//			pExtension->SetPropertyBool(eProperty_IsOpen, false);
		}
	}
}
