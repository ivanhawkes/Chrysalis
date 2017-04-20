#include <StdAfx.h>

#include "AnimatedDoorComponent.h"
#include <CrySerialization/Decorators/Resources.h>
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Geometry/GeometryComponent.h>
#include <Components/Lockable/LockableComponent.h>
#include <Components/Animation/ControlledAnimationComponent.h>
#include <Components/Animation/SimpleAnimationComponent.h>


CRYREGISTER_CLASS(CAnimatedDoorComponent)

class CDoorRegistrator : public IEntityRegistrator
{
	void Register() override
	{
		RegisterEntityWithDefaultComponent<CAnimatedDoorComponent>("AnimatedDoor", "Doors", "door.bmp");
	}
};

CDoorRegistrator g_doorRegistrator;


void CAnimatedDoorComponent::Initialize()
{
	auto pEntity = GetEntity();

	// Get some geometry.
	m_pGeometryComponent = pEntity->CreateComponent<CGeometryComponent>();

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
				// Close the door, Hodor.
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


void CAnimatedDoorComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(m_IsOpen, "IsOpen", "Is Open?");
	archive.doc("Is the door open?");

	if (archive.isInput())
	{
		OnResetState();

		// #TODO: We should get the door into it's proper state e.g. open / closed.
	}
}


void CAnimatedDoorComponent::OnResetState()
{
}
