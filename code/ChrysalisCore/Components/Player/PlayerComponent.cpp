#include <StdAfx.h>

#include "PlayerComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include "Camera/CameraManagerComponent.h"
#include <Components/Items/ItemComponent.h>
#include <Components/Player/Input/PlayerInputComponent.h>
#include <Item/Weapon/Weapon.h>
#include <Components/Actor/ActorComponent.h>


namespace Chrysalis
{
static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
		// Functions
		{
		}
	}
}


void CPlayerComponent::ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
{
	desc.SetGUID(CPlayerComponent::IID());
	desc.SetEditorCategory("Player");
	desc.SetLabel("Player");
	desc.SetDescription("A local / remote player.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});
}


// ***
// *** IEntityComponent
// ***


void CPlayerComponent::Initialize()
{
	CryLogAlways("The player Id is %d - should be 30583.", GetEntityId());

	// Create a camera manager for this player. We do this early, since character attachment code needs to make calls
	// to a functioning camera.
	m_pCameraManager = m_pEntity->CreateComponent<CCameraManagerComponent>();

	// Acquire a player input component. At a later time it will be useful to check if a network version is needed, or
	// perhaps AI / nullptr versions.
	// NOTE: This component requires a pointer to a camera manager - so it must always load after that component.
	m_pPlayerInput = m_pEntity->CreateComponent<CPlayerInputComponent>();
}


void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::GameplayStarted:
		{
			// Revive the entity when gameplay starts
			OnRevive();
		}
		break;

		case EEntityEvent::Update:
		{
			//SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam [0];
		}
		break;
	}
}


bool CPlayerComponent::IsViewFirstPerson() const
{
	return m_pCameraManager->GetCamera()->IsViewFirstPerson();
}


void CPlayerComponent::AttachToCharacter(EntityId characterId)
{
	// Ensure the object is an actor, then inform it we are now attached.
	if (auto pAttachedEntity = gEnv->pEntitySystem->GetEntity(characterId))
	{
		if (auto pActorComponent = pAttachedEntity->GetComponent<CActorComponent>())
		{
			// If the target was an actor then it's safe to complete the attachment.
			m_attachedCharacterId = m_cameraTargetId = characterId;
			
			// TODO: We need to detach from any previous entity before we attach to the new one.
			pActorComponent->OnPlayerAttach(*this);

			// Inform the camera system we are now watching a different entity.
			m_pCameraManager->AttachToEntity(m_cameraTargetId);
		}
	}
}


void CPlayerComponent::AttachToHero()
{
	// #HACK: hard coded effort to grab an entity to attach to as our pawn / character. It must occur after the camera has a chance to create itself.
	auto pCharacterEntity = gEnv->pEntitySystem->FindEntityByName("Hero");
	CRY_ASSERT_MESSAGE(pCharacterEntity, "Player is not attached to a character. Do not enter game mode without attaching to a character.");
	if (pCharacterEntity)
		AttachToCharacter(pCharacterEntity->GetId());
}


EntityId CPlayerComponent::GetAttachedEntityId() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "This player is not attached to an entity.");

	return m_attachedCharacterId;
}


IEntity* CPlayerComponent::GetAttachedEntity() const
{
	CRY_ASSERT_MESSAGE(m_attachedCharacterId != INVALID_ENTITYID, "This player is not attached to an entity.");

	return gEnv->pEntitySystem->GetEntity(m_attachedCharacterId);
}


CActorComponent* CPlayerComponent::GetAttachedActor() const
{
	if (auto pAttachedEntity = GetAttachedEntity())
	{
		return pAttachedEntity->GetComponent<CActorComponent>();
	}

	CRY_ASSERT_MESSAGE(false, "Attempt to retrieve attached actor failed. Check your logic.");

	return nullptr;
}


CActorComponent* CPlayerComponent::GetLocalActor()
{
	// Try and find a local player on this client.
	auto pPlayer = CPlayerComponent::GetLocalPlayer();
	if (pPlayer)
		return pPlayer->GetAttachedActor();

	CRY_ASSERT_MESSAGE(false, "Attempt to retrieve local character failed. Check your logic.");

	return nullptr;
}


void CPlayerComponent::OnRevive()
{
	if (auto pActorComponent = GetLocalActor())
	{
		pActorComponent->OnRevive();
	}
}


void CPlayerComponent::NetworkClientConnect()
{
	// We want to make it simple to attach to the default entity 'hero' when entering a game as the local player. This
	// behaviour isn't strictly correct, but it's good enough for now.
	if (IsLocalPlayer())
		AttachToHero();

	OnRevive();
}


EntityId CPlayerComponent::GetCameraTargetId() const
{
	return m_cameraTargetId;
}


CCameraManagerComponent* CPlayerComponent::GetCameraManager() const
{
	//CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager;
}


ICameraComponent* CPlayerComponent::GetCamera() const
{
	//	CRY_ASSERT_MESSAGE(m_pCameraManager, "A camera manager is critical when the level contains actors.");
	return m_pCameraManager ? m_pCameraManager->GetCamera() : nullptr;
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent)
}