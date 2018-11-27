#include "StdAfx.h"

#include "ControlledAnimationComponent.h"
#include <Cry3DEngine/IRenderNode.h>


namespace Chrysalis
{
void CControlledAnimationComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
	// Functions
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControlledAnimationComponent::PlayAnimation, "{479DD227-E758-487B-AB8C-30C6DF4BDDBF}"_cry_guid, "PlayAnimation");
		pFunction->SetDescription("Plays a low-level animation on the animated mesh");
		pFunction->SetFlags(Schematyc::EEnvFunctionFlags::None);
		pFunction->BindInput(1, 'name', "Name");
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControlledAnimationComponent::SetPlaybackSpeed, "{BAB3BAB6-6D26-47F2-8663-5BBFFE88E066}"_cry_guid, "SetPlaybackSpeed");
		pFunction->SetDescription("Sets the speed at which animations are played");
		pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
		pFunction->BindInput(1, 'sped', "Speed");
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControlledAnimationComponent::SetPlaybackWeight, "{32B3F3E5-F245-4050-853A-6818ED02721B}"_cry_guid, "SetPlaybackWeight");
		pFunction->SetDescription("Sets the weight at which animations are played");
		pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
		pFunction->BindInput(1, 'weig', "Weight");
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControlledAnimationComponent::SetLayer, "{B9C3DAB0-7998-4572-95F7-056612434151}"_cry_guid, "SetLayer");
		pFunction->SetDescription("Sets the layer at which animations are played");
		pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
		pFunction->BindInput(1, 'layr', "Layer");
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControlledAnimationComponent::SetMeshType, "{9D5182CF-C754-41EE-AAA2-F0A8833462B4}"_cry_guid, "SetType");
		pFunction->BindInput(1, 'type', "Type");
		pFunction->SetDescription("Changes the type of the object");
		pFunction->SetFlags({ Schematyc::EEnvFunctionFlags::Member });
		componentScope.Register(pFunction);
	}
	{
		auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CControlledAnimationComponent::SeekFrame, "{A27A52CF-67A8-4ABF-9BCE-123DD83F4AAF}"_cry_guid, "SeekToFrame");
		pFunction->SetDescription("Seeks to the requested frame within the animation");
		pFunction->SetFlags(Schematyc::EEnvFunctionFlags::None);
		pFunction->BindInput(1, 'fram', "Frame");
		componentScope.Register(pFunction);
	}
}


void CControlledAnimationComponent::ReflectType(Schematyc::CTypeDesc<CControlledAnimationComponent>& desc)
{
	desc.SetGUID(CControlledAnimationComponent::IID());
	desc.SetEditorCategory("Geometry");
	desc.SetLabel("Controlled Animation");
	desc.SetDescription("A component containing a simple mesh that can be animated using direct control");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });

	desc.AddMember(&CControlledAnimationComponent::m_type, 'type', "Type", "Type", "Determines the behavior of the static mesh", Cry::DefaultComponents::EMeshType::RenderAndCollider);

	desc.AddMember(&CControlledAnimationComponent::m_filePath, 'file', "FilePath", "File", "Determines the animated mesh to load", "");
	desc.AddMember(&CControlledAnimationComponent::m_renderParameters, 'rend', "Render", "Rendering Settings", "Settings for the rendered representation of the component", Cry::DefaultComponents::SRenderParameters());

	desc.AddMember(&CControlledAnimationComponent::m_defaultAnimation, 'anim', "Animation", "Default Animation", "Specifies the animation we want to play by default", "");
	desc.AddMember(&CControlledAnimationComponent::m_physics, 'phys', "Physics", "Physics", "Physical properties for the object, only used if a simple physics or character controller is applied to the entity.", Cry::DefaultComponents::SPhysicsParameters());
}


void CControlledAnimationComponent::PlayAnimation(Schematyc::LowLevelAnimationName name)
{
	if (ICharacterInstance* pCharacter = m_pEntity->GetCharacter(GetEntitySlotId()))
	{
		string animationName = PathUtil::GetFileName(name.value);
		pCharacter->GetISkeletonAnim()->StartAnimation(animationName, m_animationParams);
	}
}


void CControlledAnimationComponent::Initialize()
{
	LoadFromDisk();
	ResetObject();

	// Set the animation to require manual updating.
	m_animationParams.m_fPlaybackSpeed = 0.f;
	m_animationParams.m_nFlags = CA_MANUAL_UPDATE | CA_LOOP_ANIMATION;
}


void CControlledAnimationComponent::LoadFromDisk()
{
	if (m_filePath.value.size() > 0)
	{
		m_pCachedCharacter = gEnv->pCharacterManager->CreateInstance(m_filePath.value);
	}
	else
	{
		m_pCachedCharacter = nullptr;
	}
}


void CControlledAnimationComponent::ResetObject()
{
	if (m_pCachedCharacter == nullptr)
	{
		FreeEntitySlot();
		return;
	}

	m_pEntity->SetCharacter(m_pCachedCharacter, GetOrMakeEntitySlotId(), false);
}


void CControlledAnimationComponent::ProcessEvent(const SEntityEvent& event)
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


void CControlledAnimationComponent::Update(SEntityUpdateContext* pCtx)
{
	const float frameTime = pCtx->fFrameTime;
}


void CControlledAnimationComponent::SeekFrame(float frameTime)
{
	// Hang on to this, we may want to lerp in future.
	m_frameTime = frameTime;

	if (ICharacterInstance* pCharacter = m_pEntity->GetCharacter(GetEntitySlotId()))
	{
		if (auto skeletonAnim = pCharacter->GetISkeletonAnim())
		{
			skeletonAnim->StartAnimation(PathUtil::GetFileName(m_defaultAnimation.value), m_animationParams);
			skeletonAnim->ManualSeekAnimationInFIFO(m_animationParams.m_nLayerID, 0, m_frameTime, true);
		}
	}
}


void CControlledAnimationComponent::SetCharacterFile(const char* szPath)
{
	m_filePath = szPath;

	// Force a reload at this point.
	LoadFromDisk();
	ResetObject();
}


void CControlledAnimationComponent::SetDefaultAnimationName(const char* szPath)
{
	m_defaultAnimation = szPath;
}


//bool CControlledAnimationComponent::SetMaterial(int slotId, const char* szMaterial)
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
