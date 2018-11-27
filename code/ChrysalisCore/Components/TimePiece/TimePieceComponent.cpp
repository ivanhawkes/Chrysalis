#include <StdAfx.h>

#include "TimePieceComponent.h"


namespace Chrysalis
{
void CTimePieceComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTimePieceComponent::SetMeshType, "{D75F9198-0EE3-46A5-B86A-2DDFCDEC6D2B}"_cry_guid, "SetType");
		pFunction->BindInput(1, 'type', "Type");
		pFunction->SetDescription("Changes the type of the object");
		pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member });
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTimePieceComponent::SetHour, "{2F4233B1-7CD0-406B-8CD6-1447F0EE157B}"_cry_guid, "SetNeedle");
		pFunction->SetDescription("Set the hour");
		pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member, Schematyc::EEnvFunctionFlags::Construction });
		pFunction->BindInput(1, 'set', "Set");
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTimePieceComponent::SetMinute, "{D24684D0-3FA5-4DD7-AB74-CBA3AC4F5B95}"_cry_guid, "SetMinute");
		pFunction->SetDescription("Set the minute");
		pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member, Schematyc::EEnvFunctionFlags::Construction });
		pFunction->BindInput(1, 'set', "Set");
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTimePieceComponent::SetSecond, "{B511FA50-D474-4AE9-AA7D-10748835FD39}"_cry_guid, "SetSecond");
		pFunction->SetDescription("Set the second");
		pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member, Schematyc::EEnvFunctionFlags::Construction });
		pFunction->BindInput(1, 'set', "Set");
		componentScope.Register(pFunction);
	}
}


void CTimePieceComponent::ReflectType(Schematyc::CTypeDesc<CTimePieceComponent>& desc)
{
	desc.SetGUID(CTimePieceComponent::IID());
	desc.SetEditorCategory("Miscellaneous");
	desc.SetLabel("TimePiece");
	desc.SetDescription("Watches, clocks and other time keeping devices.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });

	// Mesh related.
	desc.AddMember(&CTimePieceComponent::m_type, 'type', "Type", "Type", "Determines the behavior of the static mesh", Cry::DefaultComponents::EMeshType::RenderAndCollider);
	desc.AddMember(&CTimePieceComponent::m_filePath, 'file', "FilePath", "File", "Determines the animated mesh to load", "");
	desc.AddMember(&CTimePieceComponent::m_renderParameters, 'rend', "Render", "Rendering Settings", "Settings for the rendered representation of the component", Cry::DefaultComponents::SRenderParameters());
	desc.AddMember(&CTimePieceComponent::m_physics, 'phys', "Physics", "Physics", "Physical properties for the object, only used if a simple physics or character controller is applied to the entity.", Cry::DefaultComponents::SPhysicsParameters());
	
	// Specific for time pieces.
	desc.AddMember(&CTimePieceComponent::m_timePieceProperties, 'time', "Time", "Time", "Time components", CTimePieceComponent::STimePieceProperties());
}


void CTimePieceComponent::Initialize()
{
	LoadFromDisk();
	ResetObject();
}


void CTimePieceComponent::ProcessEvent(const SEntityEvent& event)
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


void CTimePieceComponent::LoadFromDisk()
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


void CTimePieceComponent::ResetObject()
{
	if (m_pCachedCharacter == nullptr)
	{
		FreeEntitySlot();
		return;
	}

	m_pEntity->SetCharacter(m_pCachedCharacter, GetOrMakeEntitySlotId() | ENTITY_SLOT_ACTUAL, false);
}


void CTimePieceComponent::Update(SEntityUpdateContext* pCtx)
{
	if (m_pCachedCharacter)
	{
		// If we can access the attachments, we can try and find the rotation for each hand.
		const IAttachmentManager* pAttachmentManager = m_pCachedCharacter->GetIAttachmentManager();
		if (pAttachmentManager)
		{
			// Can we find the hours hand?
			const auto hoursIndex = pAttachmentManager->GetIndexByName("hours");
			if (IAttachment* pHoursAttachment = pAttachmentManager->GetInterfaceByIndex(hoursIndex))
			{
				QuatT trans = pHoursAttachment->GetAttAbsoluteDefault();
				const float radians = DEG2RAD(m_timePieceProperties.hour * 30.0f + m_timePieceProperties.minute / 2.0f);
				trans.q = Quat::CreateRotationXYZ(m_timePieceProperties.axis * radians);
				pHoursAttachment->SetAttAbsoluteDefault(trans);
			}

			// Can we find the minutes hand?
			const auto minutesIndex = pAttachmentManager->GetIndexByName("minutes");
			if (IAttachment* pMinutesAttachment = pAttachmentManager->GetInterfaceByIndex(minutesIndex))
			{
				QuatT trans = pMinutesAttachment->GetAttAbsoluteDefault();
				const float radians = DEG2RAD(m_timePieceProperties.minute * 6.0f);
				trans.q = Quat::CreateRotationXYZ(m_timePieceProperties.axis * radians);
				pMinutesAttachment->SetAttAbsoluteDefault(trans);
			}

			// Can we find the seconds hand?
			const auto secondsIndex = pAttachmentManager->GetIndexByName("seconds");
			IAttachment* pSecondsAttachment = pAttachmentManager->GetInterfaceByIndex(secondsIndex);
			if (IAttachment* pSecondsAttachment = pAttachmentManager->GetInterfaceByIndex(secondsIndex))
			{
				QuatT trans = pSecondsAttachment->GetAttAbsoluteDefault();
				const float radians = DEG2RAD(m_timePieceProperties.second * 6.0f);
				trans.q = Quat::CreateRotationXYZ(m_timePieceProperties.axis * radians);
				pSecondsAttachment->SetAttAbsoluteDefault(trans);
			}
		}
	}
}


//bool CTimePieceComponent::SetMaterial(int slotId, const char* szMaterial)
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