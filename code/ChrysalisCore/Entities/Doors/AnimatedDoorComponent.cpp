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

	desc.AddMember(&CAnimatedDoorComponent::m_isOpen, 'open', "IsOpen", "Is Open?", "Is the door open?", false);
}


void CAnimatedDoorComponent::Initialize()
{
	const auto pEntity = GetEntity();

	// Get some geometry and give it physics.
	m_pGeometryComponent = pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAnimatedMeshComponent>();
	m_pRigidBodyComponent = pEntity->GetOrCreateComponent<Cry::DefaultComponents::CRigidBodyComponent>();
	m_pRigidBodyComponent->m_bSendCollisionSignal = true;
		
	// Get a controllable animation component.
	//m_pAnimationComponent = pEntity->GetOrCreateComponent<CControlledAnimationComponent>();
	m_pAnimationComponent = pEntity->GetOrCreateComponent<CSimpleAnimationComponent>();

	// Allow locking.
	m_pLockableComponent = pEntity->GetOrCreateComponent<CLockableComponent>();

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


void CAnimatedDoorComponent::OnInteractionInteractStart()
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
			if (m_isOpen)
			{
				// Close the door.
				gEnv->pLog->LogAlways("Door was open.");
				m_isOpen = false;
				m_pAnimationComponent->OnPlayAnimation(kDoorAnimationClose);
			}
			else
			{
				// Open the door.
				gEnv->pLog->LogAlways("Door was closed.");
				m_isOpen = true;
				m_pAnimationComponent->OnPlayAnimation(kDoorAnimationOpen);
			}
		}
	}
}


void CAnimatedDoorComponent::OnResetState()
{
}
}