#include <StdAfx.h>

#include "AnimatedDoorComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>
#include <Components/Lockable/LockableComponent.h>
#include <Components/Animation/ControlledAnimationComponent.h>
#include <Components/Animation/SimpleAnimationComponent.h>


namespace Chrysalis
{
void CAnimatedDoorComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CAnimatedDoorComponent::ReflectType(Schematyc::CTypeDesc<CAnimatedDoorComponent>& desc)
{
	desc.SetGUID(CAnimatedDoorComponent::IID());
	desc.SetEditorCategory("Doors");
	desc.SetLabel("AnimatedDoor");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

	desc.AddMember(&CAnimatedDoorComponent::m_IsOpen, 'open', "IsOpen", "Is Open?", "Is the door open?", false);
}


void CAnimatedDoorComponent::Initialize()
{
	const auto pEntity = GetEntity();

	// Get some geometry.
	m_pGeometryComponent = pEntity->CreateComponent<Cry::DefaultComponents::CAnimatedMeshComponent>();

	// Get a controllable animation component.
	//m_pAnimationComponent = pEntity->CreateComponent<CControlledAnimationComponent>();
	m_pAnimationComponent = pEntity->CreateComponent<CSimpleAnimationComponent>();

	// Allow locking.
	m_pLockableComponent = pEntity->CreateComponent<CLockableComponent>();

	// We want to supply interaction verbs.
	auto m_interactor = pEntity->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactor->AddInteraction(std::make_shared<CInteractionInteract>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionOpenableOpen>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionOpenableClose>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionLockableLock>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionLockableUnlock>(this));
	}

	OnResetState();
}


void CAnimatedDoorComponent::OnInteractionInteract()
{
	if (m_pLockableComponent)
	{
		if (m_pLockableComponent->IsLocked())
		{
			// #TODO: Notify player door is locked.
			gEnv->pLog->LogAlways("Door is locked.");
		}
		else
		{
			// #TODO: Add handling for door being open / closed. This should include forcing re-calc of the nav-mesh if needed.
			// Consider forcing the geom to have an 'Open' anim instead of just a 'default' one, since we are overriding it anyway.
			if (m_IsOpen)
			{
				// Close the door.
				gEnv->pLog->LogAlways("Door was open.");
				m_IsOpen = false;
				m_pAnimationComponent->OnPlayAnimation(kDoorAnimationClose);
			}
			else
			{
				// Open the door.
				gEnv->pLog->LogAlways("Door was closed.");
				m_IsOpen = true;
				m_pAnimationComponent->OnPlayAnimation(kDoorAnimationOpen);
			}
		}
	}
}


void CAnimatedDoorComponent::OnResetState()
{
}
}