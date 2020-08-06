#include <StdAfx.h>

#include "ActorComponent.h"
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryMath/Cry_Math.h>
#include <CryCore/Assert/CryAssert.h>
#include <ICryMannequin.h>
#include <IGameObject.h>
#include <Actor/Animation/ActorAnimation.h>
#include <Actor/Animation/Actions/ActorAnimationActionAiming.h>
#include <Actor/Animation/Actions/ActorAnimationActionAimPose.h>
#include <Actor/Animation/Actions/ActorAnimationActionCooperative.h>
#include <Actor/Animation/Actions/ActorAnimationActionEmote.h>
#include <Actor/Animation/Actions/ActorAnimationActionInteration.h>
#include <Actor/Animation/Actions/ActorAnimationActionLocomotion.h>
#include <Actor/Animation/Actions/ActorAnimationActionLooking.h>
#include <Actor/Animation/Actions/ActorAnimationActionLookPose.h>
#include <Actor/Movement/StateMachine/ActorStateEvents.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include <Animation/ProceduralContext/ProceduralContextAim.h>
#include <Animation/ProceduralContext/ProceduralContextLook.h>
#include <Components/Actor/ActorControllerComponent.h>
#include <Components/Spells/SpellbookComponent.h>
#include <Components/Spells/SpellParticipantComponent.h>
#include <Components/Player/PlayerComponent.h>
#include <Components/Player/Camera/ICameraComponent.h>
#include <Components/Interaction/EntityAwarenessComponent.h>
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Equipment/EquipmentComponent.h>
#include <Components/Snaplocks/SnaplockComponent.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include <entt/entt.hpp>
#include <ECS/Components/Components.h>
#include <ECS/Components/Health.h>
#include <ECS/Components/Qi.h>
#include <ECS/Components/Spells/Spell.h>
#include <ECS/ECS.h>
#include <ECS/Systems/Simulation.h>



namespace Chrysalis
{

static void RegisterActorComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CActorComponent));
		// Functions
		{
		}
	}
}


void CActorComponent::ReflectType(Schematyc::CTypeDesc<CActorComponent>& desc)
{
	desc.SetGUID(CActorComponent::IID());
	desc.SetEditorCategory("Actors");
	desc.SetLabel("Actor");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});

	desc.AddMember(&CActorComponent::m_geometryFirstPerson, 'geof', "GeometryFirstPerson", "First Person Geometry", "", "");
	desc.AddMember(&CActorComponent::m_geometryThirdPerson, 'geot', "GeometryThirdPerson", "Third Person Geometry", "", "");
}


void CActorComponent::Initialize()
{
	// Mesh and animation.
	m_pActorAnimationComponent = m_pEntity->GetOrCreateComponent<CActorAnimationComponent>();

	// Character movement controller.
	m_pCharacterControllerComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();

	// Contoller.
	m_pActorControllerComponent = m_pEntity->GetOrCreateComponent<CActorControllerComponent>();

	// Spell book.
	m_pSpellbookComponent = m_pEntity->GetOrCreateComponent<CSpellbookComponent>();

	// Spell participant.
	m_pSpellParticipantComponent = m_pEntity->GetOrCreateComponent<CSpellParticipantComponent>();
	
	// Inventory management.
	m_pInventoryComponent = m_pEntity->GetOrCreateComponent<CInventoryComponent>();

	// Equipment management.
	m_pEquipmentComponent = m_pEntity->GetOrCreateComponent<CEquipmentComponent>();

	// Give the actor a DRS proxy, since it will probably need one.
	m_pDrsComponent = crycomponent_cast<IEntityDynamicResponseComponent*> (m_pEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));

	// For now, all actors will have awareness built-in, but this should default to not having it at some stage unless they are
	// the player target.
	m_pAwarenessComponent = m_pEntity->GetOrCreateComponent<CEntityAwarenessComponent>();

	// Manage our snaplocks.
	m_pSnaplockComponent = m_pEntity->GetOrCreateComponent<CSnaplockComponent>();

	// HACK: Need a way to add the default snaplocks in place. For now, I'm going to hard code them to test.
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_HEAD, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_FACE, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_NECK, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_SHOULDERS, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_CHEST, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_BACK, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_LEFTARM, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_RIGHTARM, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_LEFTHAND, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_RIGHTHAND, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_WAIST, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_LEFTLEG, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_RIGHTLEG, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_LEFTFOOT, false));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_RIGHTFOOT, false));

	// Default is for a character to be controlled by AI.
	//	m_isAIControlled = true;
	//m_isAIControlled = false;

	// Tells this instance to trigger areas.
	m_pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS);

	// Are we the local player?
	if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
	{
		// Tells this instance to trigger areas and that it's the local player.
		// BUG: HACK: We may not be the local player! That flag likely needs toggling as we enter and exit control of the actor.
		m_pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS | ENTITY_FLAG_LOCAL_PLAYER);
		CryLogAlways("CActorComponent::HandleEvent(): Entity \"%s\" became the local character!", m_pEntity->GetName());
	}

	// Reset the entity.
	OnResetState();
}


void CActorComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		// All the other components should be initialised before this is called.
		case EEntityEvent::Initialize:
			// Make sure we have the same ECS EntityId as the spellbook.
			m_ecsEntity = m_pSpellParticipantComponent->GetECSEntity();
			break;

		// Physicalize on level start for Launcher
		case EEntityEvent::LevelStarted:

		// Editor specific, physicalize on reset, property change or transform change
		case EEntityEvent::Reset:
		case EEntityEvent::EditorPropertyChanged:
		case EEntityEvent::TransformChangeFinishedInEditor:
			OnResetState();
			break;

		case EEntityEvent::Update:
		{
			SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];
			Update(pCtx);
			break;
		}
	}
}


void CActorComponent::Update(SEntityUpdateContext* pCtx)
{
	const float frameTime = pCtx->fFrameTime;

	// HACK: This belongs in pre-physics...I think.
	SetIK();

	// Check for spellbook abilities that can be used by the player.
	// TODO: Advertise this to the player through a decent UI.
	// TODO: The test for being a player is clearly broken - need to fix player control testing.
	if ((m_pAwarenessComponent) && (IsPlayer()))
	{
		auto results = m_pAwarenessComponent->GetNearDotFiltered();
		if (results.size() > 0)
		{
			auto pTargetEntity = gEnv->pEntitySystem->GetEntity(results[0]);

			// HACK: Quick way to display spell UI. Comment out when handing out a build.
			if (auto pSpellbookComponent = pTargetEntity->GetComponent<CSpellbookComponent>())
			{
				auto spellCollection = pSpellbookComponent->GetSpellColllection();

				int spellId {1};
				for (auto& spell : spellCollection.spells)
				{
					CryWatch("Spell %d : %s", spellId, spell.spellName.c_str());
					spellId++;
				}
			}
		}
	}

	// DEBUG: Let's see some data.
	auto registry = ECS::Simulation.GetActorRegistry();
	auto& health = registry->get<ECS::Health>(m_ecsEntity);
	CryWatch("%s - health: %.2f", m_pEntity->GetName(), health.health.GetAttribute());
	auto& qi = registry->get<ECS::Qi>(m_ecsEntity);
	CryWatch("%s - qi: %.2f", m_pEntity->GetName(), qi.qi.GetAttribute());
}


ICharacterInstance* CActorComponent::GetCharacter() const
{
	CRY_ASSERT(m_pActorAnimationComponent);

	return m_pActorAnimationComponent->GetCharacter();
}


const Vec3 CActorComponent::GetLocalEyePos() const
{
	// The default, in case we can't find the actual eye position, will be to use an average male's height.
	Vec3 eyePosition {0.0f, 0.0f, 1.82f};

	// Get their character or bail early.
	auto pCharacter = m_pActorAnimationComponent->GetCharacter();
	if (!pCharacter)
		return eyePosition;

	// Determine the position of the left and right eyes, using their average for eyePosition.
	const IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
	if (pAttachmentManager)
	{
		// Did the animators define a camera for us to use?
		const auto eyeCamera = pAttachmentManager->GetIndexByName("Camera");
		const IAttachment* pCameraAttachment = pAttachmentManager->GetInterfaceByIndex(eyeCamera);
		if (pCameraAttachment)
		{
			// Early out and use the camera.
			return GetEntity()->GetRotation() * pCameraAttachment->GetAttModelRelative().t;
		}

		const auto eyeLeft = pAttachmentManager->GetIndexByName("LeftEye");
		const auto eyeRight = pAttachmentManager->GetIndexByName("RightEye");
		Vec3 eyeLeftPosition;
		Vec3 eyeRightPosition;
		int eyeFlags = 0;

		// Is there a left eye?
		const IAttachment* pEyeLeftAttachment = pAttachmentManager->GetInterfaceByIndex(eyeLeft);
		if (pEyeLeftAttachment)
		{
			eyeLeftPosition = GetEntity()->GetRotation() * pEyeLeftAttachment->GetAttModelRelative().t;
			eyeFlags |= 0x01;
		}

		// Is there a right eye?
		const IAttachment* pEyeRightAttachment = pAttachmentManager->GetInterfaceByIndex(eyeRight);
		if (pEyeRightAttachment)
		{
			eyeRightPosition = GetEntity()->GetRotation() * pEyeRightAttachment->GetAttModelRelative().t;
			eyeFlags |= 0x02;
		}

		static bool alreadyWarned {false};
		switch (eyeFlags)
		{
			case 0:
				// Failure, didn't find any eyes.
				// This will most likely spam the log. Disable it if it's annoying.
				if (!alreadyWarned)
				{
					CryLogAlways("Character does not have 'Camera', 'left_eye' or 'right_eye' defined.");
					alreadyWarned = true;
				}
				break;

			case 1:
				// Left eye only.
				eyePosition = eyeLeftPosition;
				break;

			case 2:
				// Right eye only.
				eyePosition = eyeRightPosition;
				break;

			case 3:
				// Both eyes, position between the two points.
				eyePosition = (eyeLeftPosition + eyeRightPosition) / 2.0f;
				break;
		}
	}

	return eyePosition;
}


Vec3 CActorComponent::GetLocalLeftHandPos() const
{
	// A terrible default, in case we can't find the actual hand position.
	const Vec3 handPosition {-0.2f, 0.3f, 1.3f};

	// Get their character or bail early.
	auto pCharacter = m_pActorAnimationComponent->GetCharacter();
	if (pCharacter)
	{
		const IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
		if (pAttachmentManager)
		{
			// Did the animators define a hand bone for us to use?
			const auto handBone = pAttachmentManager->GetIndexByName("LeftHand");
			const IAttachment* pAttachment = pAttachmentManager->GetInterfaceByIndex(handBone);
			if (pAttachment)
			{
				// We have an exact position to return.
				return GetEntity()->GetRotation() * pAttachment->GetAttModelRelative().t;
			}
		}
	}

	return handPosition;
}


Vec3 CActorComponent::GetLocalRightHandPos() const
{
	// A terrible default, in case we can't find the actual hand position.
	const Vec3 handPosition {0.2f, 0.3f, 1.3f};

	// Get their character or bail early.
	auto pCharacter = m_pActorAnimationComponent->GetCharacter();
	if (pCharacter)
	{
		const IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
		if (pAttachmentManager)
		{
			// Did the animators define a hand bone for us to use?
			const auto handBone = pAttachmentManager->GetIndexByName("RightHand");
			const IAttachment* pAttachment = pAttachmentManager->GetInterfaceByIndex(handBone);
			if (pAttachment)
			{
				// We have an exact position to return.
				return GetEntity()->GetRotation() * pAttachment->GetAttModelRelative().t;
			}
		}
	}

	return handPosition;
}


bool CActorComponent::IsViewFirstPerson() const
{
	// The view is always considered third person, unless the local player is controlling this actor, and their view is
	// set to a first person view. 
	if (m_pPlayer && m_pPlayer->IsLocalPlayer())
		return m_pPlayer->GetCamera()->IsViewFirstPerson();

	return false;
}


void CActorComponent::OnKill()
{
}


void CActorComponent::OnRevive()
{
	// Mannequin should be reset.
	OnResetState();

	// Controller needs to be reset.
	m_pActorControllerComponent->OnRevive();
}


void CActorComponent::OnDeath()
{
}


void CActorComponent::OnJump()
{}


void CActorComponent::OnEnterVehicle(const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson)
{}


void CActorComponent::OnExitVehicle()
{}


void CActorComponent::OnHealthChanged(float newHealth)
{}


void CActorComponent::OnItemPickedUp(EntityId itemId)
{}


void CActorComponent::OnItemUsed(EntityId itemId)
{}


void CActorComponent::OnItemDropped(EntityId itemId)
{}


void CActorComponent::OnSprintStaminaChanged(float newStamina)
{}


// ***
// *** Item System
// ***


EntityId CActorComponent::GetCurrentItemId(bool includeVehicle) const
{
	// #TODO: Add handling of vehicles in this routine.

	// TODO: CRITICAL: HACK: BROKEN: !!
	// Let the inventory extension handle the hard work.
	//return m_pInventory ? m_pInventory->GetCurrentItem() : INVALID_ENTITYID;
	return INVALID_ENTITYID;
}


IItemReceipt* CActorComponent::GetCurrentItem(bool includeVehicle) const
{
	//if (EntityId itemId = GetCurrentItemId(includeVehicle))
	//	return gEnv->pGameFramework->GetIItemSystem()->GetItem(itemId);

	return nullptr;
}


// ***
// *** AI / Player Control
// ***


void CActorComponent::OnPlayerAttach(CPlayerComponent& player)
{
	// Make a note of the player for future reference.
	m_pPlayer = &player;

	// Request player input messages be sent to us now.
	m_pPlayer->GetPlayerInput()->AddEventListener(this);

	// Default assumption is we now control the character.
	//m_isAIControlled = false;

	// We should reset the animations.
	OnResetState();
}


void CActorComponent::OnPlayerDetach()
{
	// Request player input messages stop being sent to us now.
	m_pPlayer->GetPlayerInput()->RemoveEventListener(this);

	// That's it, make the player illegal to use again.
	m_pPlayer = nullptr;

	// #TODO: Detach the camera.

	// #TODO: We can remove the entity awareness component if that's desirable.

	// #TODO: handle transitioning this character back into the loving hands of the AI.
	//m_isAIControlled = true;
}


const bool CActorComponent::IsPlayer() const
{
	if (m_pPlayer)
		return m_pPlayer->IsLocalPlayer();

	return false;
}


const bool CActorComponent::IsClient() const
{
	if (m_pPlayer)
		return m_pPlayer->IsLocalPlayer();

	return false;
}


void CActorComponent::OnToggleFirstPerson()
{
	// We might need to switch character models or scopes.
	OnResetState();
}


// HACK: NOTE: I have removed some parts of the code in the following function because it's causing problems with
// animation playback. This needs to be dropped back in at some point when the correct method is found.

void CActorComponent::OnResetState()
{
	// HACK: This prevents a weird crash when getting the context a second time.
	m_pProceduralContextLook = nullptr;

	if (m_pActorAnimationComponent && m_pActorAnimationComponent->HasActionController())
	{
		// The actor animation component is responsible for maintaining the context.
		const auto& pContext = m_pActorAnimationComponent->GetContext();

		// The mannequin tags for an actor will need to be loaded. Because these are found in the controller definition,
		// they are potentially different for every actor. 
		m_actorMannequinParams = GetMannequinUserParams<SActorMannequinParams>(pContext);

		// It's a good idea to flush out any current actions, then resume processing.
		m_pActorAnimationComponent->FlushActions();
		m_pActorAnimationComponent->ResumeActions();

		// Select a character definition based on first / third person mode. Hard coding the default scope isn't a great
		// idea, but it's good enough for now. 
		if (IsViewFirstPerson())
		{
			m_pActorAnimationComponent->SetDefaultScopeContextName("Char1P");
			//m_pActorAnimationComponent->SetCharacterFile(m_geometryFirstPerson.value);
		}
		else
		{
			m_pActorAnimationComponent->SetDefaultScopeContextName("Char3P");
			//m_pActorAnimationComponent->SetCharacterFile(m_geometryThirdPerson.value);
		}

		// Queue the locomotion action, which switches fragments and tags as needed for actor locomotion.
		auto locomotionAction = new CActorAnimationActionLocomotion();
		QueueAction(*locomotionAction);

		// Third person views allow a little extra control.
		if (!IsViewFirstPerson())
		{
			// Aim actions.
			//if (CActorAnimationActionAimPose::IsSupported(pContext)
			//	&& CActorAnimationActionAiming::IsSupported(pContext))
			//{
			//	m_pProceduralContextAim = static_cast<CProceduralContextAim*>(m_pActorAnimationComponent->FindOrCreateProceduralContext(CProceduralContextAim::GetCID()));
			//	QueueAction(*new CActorAnimationActionAimPose());
			//	QueueAction(*new CActorAnimationActionAiming());
			//}

			//// Set the scope tag for look pose.
			//auto& animContext = m_pActorAnimationComponent->GetContext();
			//animContext.state.Set(m_actorMannequinParams->tagIDs.ScopeLookPose, true);

			//// Look actions.
			////if (CActorAnimationActionLookPose::IsSupported(pContext) // HACK: These tests are causing crashes on the second run through.
			//	//&& CActorAnimationActionLooking::IsSupported(pContext))
			//{
			//	const auto pX = m_pActorAnimationComponent->FindOrCreateProceduralContext(CProceduralContextLook::GetCID());
			//	m_pProceduralContextLook = static_cast<CProceduralContextLook*>(pX);

			//	QueueAction(*new CActorAnimationActionLookPose());
			//	QueueAction(*new CActorAnimationActionLooking());
			//}
		}
	}
}


void CActorComponent::SetIK() const
{
	// TEST: If the actor is looking at something, let's apply the IK.
	if (m_pAwarenessComponent && m_pAwarenessComponent->GetRayHit())
	{
		// TEST: Just look straight ahead.
		SetLookingIK(true, m_pAwarenessComponent->GetRayHitPosition());
	}
	else
	{
		// Don't allow look IK.
		SetLookingIK(false, Vec3 {ZERO});
	}
}


bool CActorComponent::SetLookingIK(const bool isLooking, const Vec3& lookTarget) const
{
	const bool shouldHandle = (m_pProceduralContextLook != nullptr);

	if (shouldHandle)
	{
		m_pProceduralContextLook->SetLookTarget(lookTarget);
		m_pProceduralContextLook->SetIsLookingGame(isLooking);
	}

	return shouldHandle;
}


// ***
// *** Handle interactions with other entities.
// ***


void CActorComponent::OnActionItemUse()
{
	CryLogAlways("Player tried to use an item");
}


void CActorComponent::OnActionItemPickup()
{
	CryLogAlways("Player picked up an item");
}


void CActorComponent::OnActionItemDrop()
{
	if (m_interactionEntityId != INVALID_ENTITYID)
	{
		CryLogAlways("Player dropped an item");

		auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_interactionEntityId);

		if (auto pInteractor = pTargetEntity->GetComponent<CEntityInteractionComponent>())
		{
			if (auto pInteraction = pInteractor->GetInteraction("interaction_drop").lock())
			{
				pInteraction->OnInteractionStart(*this);
			}
		}
	}

	// We no longer have an entity to drop.
	m_interactionEntityId = INVALID_ENTITYID;
}


void CActorComponent::OnActionItemToss()
{
	if (m_interactionEntityId != INVALID_ENTITYID)
	{
		CryLogAlways("Player tossed an item");
		auto pTargetEntity = gEnv->pEntitySystem->GetEntity(m_interactionEntityId);

		if (auto pInteractor = pTargetEntity->GetComponent<CEntityInteractionComponent>())
		{
			if (auto pInteraction = pInteractor->GetInteraction("interaction_toss").lock())
			{
				pInteraction->OnInteractionStart(*this);
			}
		}
	}

	// We no longer have an entity to drop.
	m_interactionEntityId = INVALID_ENTITYID;
}


//void CActorComponent::OnActionBarUse(int actionBarId)
//{
//	if (m_pAwarenessComponent)
//	{
//		auto results = m_pAwarenessComponent->GetNearDotFiltered();
//		if (results.size() > 0)
//		{
//			auto pTargetEntity = gEnv->pEntitySystem->GetEntity(results[0]);
//
//			if (auto pInteractor = pTargetEntity->GetComponent<CEntityInteractionComponent>())
//			{
//				// There's an interactor component, so this is an interactive entity.
//				auto verbs = pInteractor->GetVerbs();
//				if (verbs.size() >= actionBarId)
//				{
//					auto verb = verbs[actionBarId - 1];
//					auto pInteraction = pInteractor->GetInteraction(verb).lock();
//
//					pInteraction->OnInteractionStart(*this);
//				}
//				else
//				{
//					CryLogAlways("No action defined.");
//				}
//			}
//		}
//	}
//}


void CActorComponent::OnActionBarUse(int actionBarId)
{
	if (m_pAwarenessComponent)
	{
		// For now, we are just going to use the nearest match according to my dot product algorithm.
		auto results = m_pAwarenessComponent->GetNearDotFiltered();
		if (results.size() > 0)
		{
			// Figure out which entity is being targetted.
			auto pTargetEntity = gEnv->pEntitySystem->GetEntity(results[0]);

			// Spell casts can only land on spell participants.
			if (auto pSpellParticipantComponent = pTargetEntity->GetComponent<CSpellParticipantComponent>())
			{
				// Do they have a spell book? We're using their spellbook for this cast instead of our own.
				if (auto pSpellbookComponent = pTargetEntity->GetComponent<CSpellbookComponent>())
				{
					auto spellCollection = pSpellbookComponent->GetSpellColllection();

					// Make sure there is a matching spell for this action bar.
					if (spellCollection.spells.size() >= actionBarId)
					{
						auto spell = spellCollection.spells[actionBarId - 1];

						CryLogAlways("Casting world spell %s.", spell.spellName.c_str());

						ECS::Simulation.CastSpellByName(spell.spellName,
							GetECSEntity(), pSpellParticipantComponent->GetECSEntity(),
							GetEntityId(), pSpellbookComponent->GetEntityId());
					}
					else
					{
						CryLogAlways("No spell defined.");
					}
				}
			}
		}
	}
}


void CActorComponent::OnFunctionBarUse(int actionBarId)
{
	if (m_pAwarenessComponent)
	{
		auto results = m_pAwarenessComponent->GetNearDotFiltered();
		if (results.size() > 0)
		{
			auto pTargetEntity = gEnv->pEntitySystem->GetEntity(results[0]);

			if (auto pTargetActor = pTargetEntity->GetComponent<CActorComponent>())
			{
				switch (actionBarId)
				{
					case 1:
						ECS::Simulation.CastSpellByName("Fireball", GetECSEntity(), pTargetActor->GetECSEntity(),
							GetEntityId(), pTargetActor->GetEntityId());
						break;

					case 2:
						ECS::Simulation.CastSpellByName("Shadow Word Pain", GetECSEntity(), pTargetActor->GetECSEntity(),
							GetEntityId(), pTargetActor->GetEntityId());
						break;

					case 3:
						ECS::Simulation.CastSpellByName("Scorch", GetECSEntity(), pTargetActor->GetECSEntity(),
							GetEntityId(), pTargetActor->GetEntityId());
						break;

					case 4:
						ECS::Simulation.CastSpellByName("Heal", GetECSEntity(), pTargetActor->GetECSEntity(),
							GetEntityId(), pTargetActor->GetEntityId());
						break;

					case 5:
						ECS::Simulation.CastSpellByName("Renew", GetECSEntity(), pTargetActor->GetECSEntity(),
							GetEntityId(), pTargetActor->GetEntityId());
						break;

					case 6:
						ECS::Simulation.CastSpellByName("HealAndRenew", GetECSEntity(), pTargetActor->GetECSEntity(),
							GetEntityId(), pTargetActor->GetEntityId());
						break;

					case 7:
						ECS::Simulation.CastSpellByName("Innervate", GetECSEntity(), pTargetActor->GetECSEntity(),
							GetEntityId(), pTargetActor->GetEntityId());
						break;

					case 8:
						ECS::Simulation.CastSpellByName("Mana Burn", GetECSEntity(), pTargetActor->GetECSEntity(),
							GetEntityId(), pTargetActor->GetEntityId());
						break;

					case 9:
						ECS::Simulation.CastSpellByName("Life Steal", GetECSEntity(), pTargetActor->GetECSEntity(),
							GetEntityId(), pTargetActor->GetEntityId());
						break;
				}
			}
		}
	}
}


void CActorComponent::OnActionInspectStart()
{
	CryLogAlways("Player started inspecting things.");
}


void CActorComponent::OnActionInspect()
{
	if (m_pAwarenessComponent)
	{
		auto results = m_pAwarenessComponent->GetNearDotFiltered();
		if (results.size() > 0)
		{
			m_interactionEntityId = results[0];
			auto pInteractionEntity = gEnv->pEntitySystem->GetEntity(m_interactionEntityId);

			if (auto pInteractor = pInteractionEntity->GetComponent<CEntityInteractionComponent>())
			{
				// There's an interactor component, so this is an interactive entity.
				auto verbs = pInteractor->GetVerbs();
				if (verbs.size() > 0)
				{
					auto verb = verbs[0];

					// HACK: TEST making a call to the DRS system
					auto pDrsProxy = crycomponent_cast<IEntityDynamicResponseComponent*> (pInteractionEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));
					pDrsProxy->GetResponseActor()->QueueSignal(verb);

					// #HACK: Another test - just calling the interaction directly instead.
					auto pInteraction = pInteractor->GetInteraction(verb).lock();
					pInteraction->OnInteractionStart(*this);
				}
			}
		}
	}
}


void CActorComponent::OnActionInspectEnd()
{
	CryLogAlways("Player stopped inspecting things.");
}


void CActorComponent::OnActionInteractionStart()
{
	// You shouldn't be allowed to start another interaction before the last one is completed.
	if (isBusyInInteraction)
		return;

	if (m_pAwarenessComponent)
	{
		auto results = m_pAwarenessComponent->GetNearDotFiltered();
		if (results.size() > 0)
		{
			m_interactionEntityId = results[0];
			auto pInteractionEntity = gEnv->pEntitySystem->GetEntity(m_interactionEntityId);

			// HACK: Another test - this time of setting an emote.
			//auto emoteAction = new CActorAnimationActionEmote(g_emoteMannequinParams.tagIDs.Awe);
			//QueueAction(*emoteAction);

			if (auto pInteractor = pInteractionEntity->GetComponent<CEntityInteractionComponent>())
			{
				// There's an interactor component, so this is an interactive entity.
				// #TODO: We should really only process an 'interact' verb - not simply the first entry.
				auto verbs = pInteractor->GetVerbs();
				if (verbs.size() > 0)
				{
					// Display the verbs in a cheap manner.
					CryLogAlways("VERBS");
					int index {1};
					for (auto& verb : verbs)
					{
						CryLogAlways("%d) %s", index, verb.c_str());
						index++;
					}

					auto verb = verbs[0];

					// #HACK: Another test - just calling the interaction directly instead.
					m_pInteraction = pInteractor->GetInteraction(verb).lock();
					CryLogAlways("Player started interacting with: %s", m_pInteraction->GetVerbUI().c_str());
					m_pInteraction->OnInteractionStart(*this);
				}
			}
		}
	}
}


void CActorComponent::OnActionInteractionTick()
{
	if (m_pInteraction)
	{
		CryWatch("Interacting with: %s", m_pInteraction->GetVerbUI().c_str());
		m_pInteraction->OnInteractionTick(*this);
	}
	else
	{
		CryWatch("Interacting with nothing");
	}
}


void CActorComponent::OnActionInteractionEnd()
{
	if (m_pInteraction)
	{
		CryLogAlways("Player stopped interacting with: %s", m_pInteraction->GetVerbUI().c_str());
		m_pInteraction->OnInteractionComplete(*this);
	}
	else
	{
		CryLogAlways("Player stopped interacting with nothing");
	}
}


void CActorComponent::InteractionStart(IInteraction* pInteraction)
{
	isBusyInInteraction = true;
}


void CActorComponent::InteractionTick(IInteraction* pInteraction)
{
}


void CActorComponent::InteractionEnd(IInteraction* pInteraction)
{
	// No longer valid.
	isBusyInInteraction = false;
	m_pInteraction = nullptr;
	m_interactionEntityId = INVALID_ENTITYID; // HACK: FIX: This seems weak, look for a better way to handle keeping an entity Id for later.
}


// ***
// *** CPlayerInputComponent::IInputEventListener
// ***


void CActorComponent::OnInputEscape(int activationMode)
{
	CryLogAlways("CActorComponent::OnInputEscape");
}


void CActorComponent::OnInputInteraction(int activationMode)
{
	switch (activationMode)
	{
		case eAAM_OnPress:
			CryLogAlways("CActorComponent::OnInputInteraction - Press");
			break;

		case eAAM_OnHold:
			break;

		case eAAM_OnRelease:
			CryLogAlways("CActorComponent::OnInputInteraction - Release");
			break;
	}
}


void CActorComponent::OnInputActionBarUse(int activationMode, int buttonId)
{
	switch (activationMode)
	{
		case eAAM_OnPress:
			CryLogAlways("CActorComponent::OnInputActionBarUse - Press: %d", buttonId);
			OnActionBarUse(buttonId);
			break;

		case eAAM_OnHold:
			break;

		case eAAM_OnRelease:
			CryLogAlways("CActorComponent::OnInputActionBarUse - Release: %d", buttonId);
			break;
	}
}


void CActorComponent::OnInputFunctionBarUse(int activationMode, int buttonId)
{
	switch (activationMode)
	{
		case eAAM_OnPress:
			CryLogAlways("CActorComponent::OnInputFunctionBarUse - Press: %d", buttonId);
			break;

		case eAAM_OnHold:
			break;

		case eAAM_OnRelease:
			CryLogAlways("CActorComponent::OnInputFunctionBarUse - Release: %d", buttonId);
			break;
	}
}


// ***
// *** Allow control of the actor's animations / fragments / etc.
// ***


TagID CActorComponent::GetStanceTagId(EActorStance actorStance) const
{
	TagID tagId {TAG_ID_INVALID};

	switch (actorStance)
	{
		case EActorStance::crawling:
			tagId = m_actorMannequinParams->tagIDs.Crawling;
			break;

		case EActorStance::prone:
			tagId = m_actorMannequinParams->tagIDs.Prone;
			break;

		case EActorStance::crouching:
			tagId = m_actorMannequinParams->tagIDs.Crouching;
			break;

		case EActorStance::falling:
			tagId = m_actorMannequinParams->tagIDs.Falling;
			break;

		case EActorStance::flying:
			tagId = m_actorMannequinParams->tagIDs.Flying;
			break;

		case EActorStance::kneeling:
			tagId = m_actorMannequinParams->tagIDs.Kneeling;
			break;

		case EActorStance::landing:
			tagId = m_actorMannequinParams->tagIDs.Landing;
			break;

		case EActorStance::sittingChair:
			tagId = m_actorMannequinParams->tagIDs.SittingChair;
			break;

		case EActorStance::sittingFloor:
			tagId = m_actorMannequinParams->tagIDs.SittingFloor;
			break;

		case EActorStance::sleeping:
			tagId = m_actorMannequinParams->tagIDs.Sleeping;
			break;

		case EActorStance::spellcasting:
			tagId = m_actorMannequinParams->tagIDs.Spellcasting;
			break;

		case EActorStance::standing:
			tagId = m_actorMannequinParams->tagIDs.Standing;
			break;

		case EActorStance::swimming:
			tagId = m_actorMannequinParams->tagIDs.Swimming;
			break;
	}

	return tagId;
}


TagID CActorComponent::GetPostureTagId(EActorPosture actorPosture) const
{
	TagID tagId {TAG_ID_INVALID};

	switch (actorPosture)
	{
		case EActorPosture::aggressive:
			tagId = m_actorMannequinParams->tagIDs.Aggressive;
			break;

		case EActorPosture::alerted:
			tagId = m_actorMannequinParams->tagIDs.Alerted;
			break;

		case EActorPosture::bored:
			tagId = m_actorMannequinParams->tagIDs.Bored;
			break;

		case EActorPosture::dazed:
			tagId = m_actorMannequinParams->tagIDs.Dazed;
			break;

		case EActorPosture::depressed:
			tagId = m_actorMannequinParams->tagIDs.Depressed;
			break;

		case EActorPosture::distracted:
			tagId = m_actorMannequinParams->tagIDs.Distracted;
			break;

		case EActorPosture::excited:
			tagId = m_actorMannequinParams->tagIDs.Excited;
			break;

		case EActorPosture::interested:
			tagId = m_actorMannequinParams->tagIDs.Interested;
			break;

		case EActorPosture::neutral:
			tagId = m_actorMannequinParams->tagIDs.Neutral;
			break;

		case EActorPosture::passive:
			tagId = m_actorMannequinParams->tagIDs.Passive;
			break;

		case EActorPosture::suspicious:
			tagId = m_actorMannequinParams->tagIDs.Suspicious;
			break;

		case EActorPosture::unaware:
			tagId = m_actorMannequinParams->tagIDs.Unaware;
			break;
	}

	return tagId;
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActorComponent)
}