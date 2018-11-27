#include <StdAfx.h>

#include "GaugeComponent.h"


namespace Chrysalis
{
void CGaugeComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CGaugeComponent::SetMeshType, "{EAA0A8D8-2788-4E05-9D5B-DD458689DBF3}"_cry_guid, "SetType");
		pFunction->BindInput(1, 'type', "Type");
		pFunction->SetDescription("Changes the type of the object");
		pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member });
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CGaugeComponent::SetNeedle, "{0F48EC15-72AC-4067-BF10-65F789D31C0B}"_cry_guid, "SetNeedle");
		pFunction->SetDescription("Set the needle");
		pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member, Schematyc::EEnvFunctionFlags::Construction });
		pFunction->BindInput(1, 'set', "Set");
		componentScope.Register(pFunction);
	}
}


void CGaugeComponent::ReflectType(Schematyc::CTypeDesc<CGaugeComponent>& desc)
{
	desc.SetGUID(CGaugeComponent::IID());
	desc.SetEditorCategory("Miscellaneous");
	desc.SetLabel("Gauge");
	desc.SetDescription("Watches, clocks and other time keeping devices.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });

	// Mesh related.
	desc.AddMember(&CGaugeComponent::m_type, 'type', "Type", "Type", "Determines the behavior of the static mesh", Cry::DefaultComponents::EMeshType::RenderAndCollider);
	desc.AddMember(&CGaugeComponent::m_filePath, 'file', "FilePath", "File", "Determines the animated mesh to load", "");
	desc.AddMember(&CGaugeComponent::m_renderParameters, 'rend', "Render", "Rendering Settings", "Settings for the rendered representation of the component", Cry::DefaultComponents::SRenderParameters());
	desc.AddMember(&CGaugeComponent::m_physics, 'phys', "Physics", "Physics", "Physical properties for the object, only used if a simple physics or character controller is applied to the entity.", Cry::DefaultComponents::SPhysicsParameters());
	desc.AddMember(&CGaugeComponent::m_materialPath, 'mat', "Material", "Material", "Specifies the override material for the selected object", "");

	// Specific for time pieces.
	desc.AddMember(&CGaugeComponent::m_gaugeProperties, 'time', "Time", "Time", "Time components", CGaugeComponent::SGaugeProperties());
}


void CGaugeComponent::Initialize()
{
	LoadFromDisk();
	ResetObject();
}


void CGaugeComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::EditorPropertyChanged:
		{
			m_pEntity->UpdateComponentEventMask(this);
			LoadFromDisk();
			ResetObject();
		}
		break;

		case EEntityEvent::Update:
		{
			SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam [0];
			Update(pCtx);
		}
		break;
	}

	CBaseMeshComponent::ProcessEvent(event);
}


void CGaugeComponent::LoadFromDisk()
{
	if (!m_filePath.value.IsEmpty())
	{
		m_pCachedCharacter = gEnv->pCharacterManager->CreateInstance(m_filePath.value);
	}
	else
	{
		m_pCachedCharacter = nullptr;
	}
}


void CGaugeComponent::ResetObject()
{
	if (m_pCachedCharacter == nullptr)
	{
		FreeEntitySlot();
		return;
	}

	m_pEntity->SetCharacter(m_pCachedCharacter, GetOrMakeEntitySlotId() | ENTITY_SLOT_ACTUAL, false);
}


void CGaugeComponent::Update(SEntityUpdateContext* pCtx)
{
	if (m_pCachedCharacter)
	{
		// If we can access the attachments, we can try and find the rotation for each hand.
		const IAttachmentManager* pAttachmentManager = m_pCachedCharacter->GetIAttachmentManager();
		if (pAttachmentManager)
		{
			// Is there a needle joint?
			const auto needleIndex = pAttachmentManager->GetIndexByName("hours"); // TODO: switch this to needle or have a widget to pick it out of a list
			if (IAttachment* pNeedleAttachment = pAttachmentManager->GetInterfaceByIndex(needleIndex))
			{
				QuatT trans = pNeedleAttachment->GetAttAbsoluteDefault();
				const float radians = DEG2RAD(m_gaugeProperties.needleValue);
				trans.q = Quat::CreateRotationXYZ(m_gaugeProperties.axis * radians);
				pNeedleAttachment->SetAttAbsoluteDefault(trans);
			}
		}
	}
}


//bool CGaugeComponent::SetMaterial(int slotId, const char* szMaterial)
//{
//	if (slotId == GetEntitySlotId())
//	{
//		if (IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(szMaterial, false))
//		{
//			m_materialPath = szMaterial;
//			m_pEntity->SetSlotMaterial(GetEntitySlotId(), pMaterial);
//		}
//		else if (szMaterial [0] == '\0')
//		{
//			m_materialPath.value.clear();
//			m_pEntity->SetSlotMaterial(GetEntitySlotId(), nullptr);
//		}
//
//		return true;
//	}
//
//	return false;
//}
}