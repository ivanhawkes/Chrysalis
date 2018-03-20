#include "StdAfx.h"

#include "SimpleAnimationComponent.h"
#include <CryAnimation/ICryAnimation.h>
#include <CrySchematyc/Utils/SharedString.h>


namespace Chrysalis
{
void CSimpleAnimationComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CSimpleAnimationComponent::ReflectType(Schematyc::CTypeDesc<CSimpleAnimationComponent>& desc)
{
	desc.SetGUID(CSimpleAnimationComponent::IID());
	desc.SetEditorCategory("Animation");
	desc.SetLabel("Simple Animation");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

	desc.AddMember(&CSimpleAnimationComponent::m_animation, 'anim', "Animation", "Animation", "Animation to play.", "");
	desc.AddMember(&CSimpleAnimationComponent::m_animationSpeed, 'spee', "Speed", "Speed", "Speed at which animation will play.", 1.0f);
	desc.AddMember(&CSimpleAnimationComponent::m_bLoopAnimation, 'loop', "IsLooped", "Is Looped", "Should the animation loop?", false);
	desc.AddMember(&CSimpleAnimationComponent::m_bPlayOnLevelStart, 'shad', "PlayOnLevelStart", "Play On Level ", "Start playing when the level starts?", false);
}


void CSimpleAnimationComponent::Initialize()
{
	// Look to see if there's some geometry. If so, we will assume it's the one this animation is paired with. All
	// geometry components will need to be added to the entity before any animation that depends on them.
	m_pGeometryComponent = GetEntity()->GetComponent<Cry::DefaultComponents::CStaticMeshComponent>();
}


void CSimpleAnimationComponent::OnResetState()
{
	// If they requested animation, give them simple animation playback.
	if (m_bPlayOnLevelStart)
		OnPlayAnimation();
}


void CSimpleAnimationComponent::OnGeometryResetState()
{
}


void CSimpleAnimationComponent::OnPlayAnimation(Schematyc::LowLevelAnimationName overrideAnimation)
{
	// We'll use the default animation if there is none given.
	auto animation = overrideAnimation.value.IsEmpty() ? (m_animation.value.IsEmpty() ? "default" : m_animation) : overrideAnimation;

	if (auto* pCharacter = GetEntity()->GetCharacter(m_slotId))
	{
		CryCharAnimationParams animParams;
		animParams.m_fPlaybackSpeed = m_animationSpeed;
		animParams.m_nFlags = m_bLoopAnimation ? CA_LOOP_ANIMATION : 0;
		//animParams.m_nFlags = CA_FADEOUT | CA_REPEAT_LAST_KEY;
		pCharacter->GetISkeletonAnim()->StartAnimation(animation.value, animParams);
	}
	else
	{
		gEnv->pLog->LogWarning("Animation playback on %s entity failed. No character found.", animation.value.c_str());
	}
}
}