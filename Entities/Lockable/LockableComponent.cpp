#include <StdAfx.h>

#include "LockableComponent.h"
#include <Game/GameFactory.h>


class CLockableExtensionRegistrator
	: public IEntityRegistrator
	, public CLockableComponent::SExternalCVars
{
	virtual void Register() override
	{
		CLockableComponent::Register();
		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_lockable_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CLockableExtensionRegistrator g_LockableExtensionRegistrator;

const CLockableComponent::SExternalCVars& CLockableComponent::GetCVars() const
{
	return g_LockableExtensionRegistrator;
}


// ***
// *** IGameObjectExtension
// ***


void CLockableComponent::PostInit(IGameObject * pGameObject)
{
}


void CLockableComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// ***
// *** CLockableComponent
// ***


void CLockableComponent::Register()
{
	auto properties = new SNativeEntityPropertyInfo [eNumProperties];
	memset(properties, 0, sizeof(SNativeEntityPropertyInfo) * eNumProperties);

	{	// Lockable
		ENTITY_PROPERTY_GROUP("Lockable", ePropertyGroup_LockableBegin, ePropertyGroup_LockableEnd, properties);
		RegisterEntityProperty<bool>(properties, eProperty_Lockable_IsLocked, "IsLocked", "1", "Is this entity locked?", 0.0f, 1.0f);
	}	// ~Lockable

	// Finally, register the entity class so that instances can be created later on either via Launcher or Editor
	CGameFactory::RegisterNativeEntity<CLockableComponent>("Lockable", "Locks", "Light.bmp", CGameFactory::eGameObjectRegistrationFlags::eGORF_HiddenInEditor, properties, eNumProperties);
}


void CLockableComponent::OnFlowgraphActivation(EntityId entityId, IFlowNode::SActivationInfo *pActInfo, const class CFlowGameEntityNode *pNode)
{
	if (auto pExtension = static_cast<CLockableComponent *>(QueryExtension(entityId)))
	{
		if (IsPortActive(pActInfo, eInputPort_Unlock))
		{
			pExtension->SetPropertyBool(eProperty_Lockable_IsLocked, false);
		}
		else if (IsPortActive(pActInfo, eInputPort_Lock))
		{
			pExtension->SetPropertyBool(eProperty_Lockable_IsLocked, true);
		}
	}
}
