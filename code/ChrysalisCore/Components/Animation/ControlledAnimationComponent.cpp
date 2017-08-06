#include "StdAfx.h"

#include "ControlledAnimationComponent.h"
#include <CryAnimation/ICryAnimation.h>


namespace Chrysalis
{
void CControlledAnimationComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CControlledAnimationComponent::ReflectType(Schematyc::CTypeDesc<CControlledAnimationComponent>& desc)
{
	desc.SetGUID(CControlledAnimationComponent::IID());
	desc.SetEditorCategory("Animation");
	desc.SetLabel("Controlled Animation Component");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

	desc.AddMember(&CControlledAnimationComponent::m_animation, 'anim', "Animation", "Animation", "Animation to play.", "");
	desc.AddMember(&CControlledAnimationComponent::m_animationSpeed, 'spee', "Speed", "Speed", "Speed at which animation will play.", 1.0f);
	desc.AddMember(&CControlledAnimationComponent::m_bLoopAnimation, 'loop', "IsLooped", "Is Looped", "Should the animation loop?", false);
}


void CControlledAnimationComponent::Initialize()
{
	// Look to see if there's some geometry. If so, we will assume it's the one this animation is paired with. All
	// geometry components will need to be added to the entity before any animation that depends on them.
	m_pGeometryComponent = GetEntity()->GetComponent<Cry::DefaultComponents::CStaticMeshComponent>();
	if (m_pGeometryComponent)
	{
		// TODO: Replace this functionality with new method from 5.4.
		//m_pGeometryComponent->AddEventListener(this);
		//m_slotId = m_pGeometryComponent->GetSlotId();
	}
}


void CControlledAnimationComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_HIDE:
			break;

		case ENTITY_EVENT_UNHIDE:
			break;
	}
}


void CControlledAnimationComponent::OnGeometryResetState()
{
	//if (m_pGeometryComponent)
	//{
	//	m_slotId = m_pGeometryComponent->GetSlotId();
	//}
}


void CControlledAnimationComponent::OnResetState()
{
	// If they requested animation, give them simple animation playback.
	if (m_animation.value.size() > 0)
	{
		if (auto* pCharacter = GetEntity()->GetCharacter(m_slotId))
		{
			CryCharAnimationParams animParams;
			animParams.m_fPlaybackSpeed = m_animationSpeed;
			animParams.m_nFlags = m_bLoopAnimation ? CA_LOOP_ANIMATION : 0;
			pCharacter->GetISkeletonAnim()->StartAnimation(m_animation.value.c_str(), animParams);
		}
		else
		{
			gEnv->pLog->LogWarning("Animation playback on %s entity failed. No character found.", m_animation.value.c_str());
		}
	}

	// Notify listeners.
	for (auto& pListener : m_ListenersList)
	{
		pListener->OnControlledAnimationResetState();
	}
}
}