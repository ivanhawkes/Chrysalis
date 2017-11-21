#include <StdAfx.h>

#include "ActorComponent.h"
#include <CryMath/Cry_Math.h>
#include <CryCore/Assert/CryAssert.h>
#include <ICryMannequin.h>
#include <IAnimatedCharacter.h>
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
#include <Actor/ActorControllerComponent.h>
#include "Components/Player/PlayerComponent.h"
#include <Components/Player/Camera/ICameraComponent.h>
#include <Components/Interaction/EntityAwarenessComponent.h>
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Inventory/InventoryComponent.h>
#include <Components/Equipment/EquipmentComponent.h>
#include <Components/Snaplocks/SnaplockComponent.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


namespace Chrysalis
{
void IActorComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void IActorComponent::ReflectType(Schematyc::CTypeDesc<IActorComponent>& desc)
{
	desc.SetGUID(IActorComponent::IID());
	desc.SetEditorCategory("Actors");
	desc.SetLabel("IActor");
	desc.SetDescription("Actor base interface.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


void CActorComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CActorComponent::ReflectType(Schematyc::CTypeDesc<CActorComponent>& desc)
{
	desc.SetGUID(CActorComponent::IID());
	desc.SetEditorCategory("Actors");
	desc.SetLabel("Actor");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

	desc.AddMember(&CActorComponent::m_geometryFirstPerson, 'geof', "GeometryFirstPerson", "First Person Geometry", "", "");
	desc.AddMember(&CActorComponent::m_geometryThirdPerson, 'geot', "GeometryThirdPerson", "Third Person Geometry", "", "");
}


CActorComponent::~CActorComponent()
{
	// Inventory takes a little extra work to break down.
	// TODO: CRITICAL: HACK: BROKEN: !!
	//if (m_pInventory)
	//{
	//	if (IItem* item = GetCurrentItem())
	//	{
	//		if (item->IsUsed())
	//			item->StopUse(GetEntityId());
	//	}

	//	if (gEnv->bServer)
	//		m_pInventory->Destroy();
	//}
}


void CActorComponent::Initialize()
{
	const auto pEntity = GetEntity();

	// Mesh and animation.
	m_pAdvancedAnimationComponent = pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();

	// Character movement controller.
	m_pCharacterControllerComponent = pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();

	// Inventory management.
	m_pInventoryComponent = pEntity->GetOrCreateComponent<CInventoryComponent>();

	// Equipment management.
	m_pEquipmentComponent = pEntity->GetOrCreateComponent<CEquipmentComponent>();

	// Contoller.
	m_pActorControllerComponent = pEntity->GetOrCreateComponent<CActorControllerComponent>();

	// Give the actor a DRS proxy, since it will probably need one.
	m_pDrsComponent = crycomponent_cast<IEntityDynamicResponseComponent*> (pEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));

	// For now, all actors will have awareness built-in, but this should default to not having it at some stage unless they are
	// the player target.
	m_pAwareness = pEntity->GetOrCreateComponent<CEntityAwarenessComponent>();

	// Manage our snaplocks.
	m_pSnaplockComponent = pEntity->GetOrCreateComponent<CSnaplockComponent>();

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
	pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS);

	// Are we the local player?
	if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
	{
		// Tells this instance to trigger areas and that it's the local player.
		pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS | ENTITY_FLAG_LOCAL_PLAYER);
		gEnv->pLog->LogAlways("CActorComponent::HandleEvent(): Entity \"%s\" became the local character!", pEntity->GetName());
	}

	// Reset the entity.
	OnResetState();
}


void CActorComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		// Physicalize on level start for Launcher
		case ENTITY_EVENT_START_LEVEL:

			// Editor specific, physicalize on reset, property change or transform change
		case ENTITY_EVENT_RESET:
		case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
		case ENTITY_EVENT_XFORM_FINISHED_EDITOR:
			OnResetState();
			break;

		case ENTITY_EVENT_UPDATE:
		{
			SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam [0];
			Update(pCtx);
		}
		break;
	}
}


void CActorComponent::Update(SEntityUpdateContext* pCtx)
{
	const float frameTime = pCtx->fFrameTime;
}


// *** 
// *** IActor
// *** 


const Vec3 CActorComponent::GetLocalEyePos() const
{
	// The default, in case we can't find the actual eye position, will be to use an average male's height.
	Vec3 eyePosition { 0.0f, 0.0f, 1.82f };

	// Get their character or bail early.
	ICharacterInstance* pCharacter = GetEntity()->GetCharacter(0);
	if (!pCharacter)
		return eyePosition;

	// Determine the position of the left and right eyes, using their average for eyePosition.
	const IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
	if (pAttachmentManager)
	{
		// Did the animators define a camera for us to use?
		const auto eyeCamera = pAttachmentManager->GetIndexByName("#camera");
		const IAttachment* pCameraAttachment = pAttachmentManager->GetInterfaceByIndex(eyeCamera);
		if (pCameraAttachment)
		{
			// Early out and use the camera.
			return GetEntity()->GetRotation() * pCameraAttachment->GetAttModelRelative().t;
		}

		const auto eyeLeft = pAttachmentManager->GetIndexByName("eye_left");
		const auto eyeRight = pAttachmentManager->GetIndexByName("eye_right");
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

		static bool alreadyWarned { false };
		switch (eyeFlags)
		{
			case 0:
				// Failure, didn't find any eyes.
				// This will most likely spam the log. Disable it if it's annoying.
				if (!alreadyWarned)
				{
					CryLogAlways("Character does not have '#camera', 'left_eye' or 'right_eye' defined.");
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
	// The default, in case we can't find the actual hand position.
	const Vec3 handPosition { -0.2f, 0.3f, 1.3f };

	// Get their character or bail early.
	ICharacterInstance* pCharacter = GetEntity()->GetCharacter(0);
	if (pCharacter)
	{
		// Determine the position of the left and right eyes, using their average for eyePosition.
		const IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
		if (pAttachmentManager)
		{
			// Did the animators define a hand bone for us to use?
			// #TODO: This is from SDK guys. Change this to a well defined name for our skeleton attachments.
			const auto handBone = pAttachmentManager->GetIndexByName("left_weapon");
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
	// The default, in case we can't find the actual hand position.
	const Vec3 handPosition { 0.2f, 0.3f, 1.3f };

	// Get their character or bail early.
	ICharacterInstance* pCharacter = GetEntity()->GetCharacter(0);
	if (pCharacter)
	{
		// Determine the position of the left and right eyes, using their average for eyePosition.
		const IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
		if (pAttachmentManager)
		{
			// Did the animators define a hand bone for us to use?
			// #TODO: This is from SDK guys. Change this to a well defined name for our skeleton attachments.
			const auto handBone = pAttachmentManager->GetIndexByName("weapon");
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


// ***
// *** IActorEventListener
// ***

void CActorComponent::OnSpecialMove(IActor* pActor, IActorEventListener::ESpecialMove move)
{}


void CActorComponent::OnDeath(IActor* pActor, bool bIsGod)
{
}


void CActorComponent::OnRevive(IActor* pActor, bool bIsGod)
{
	Revive();
	m_pActorControllerComponent->OnRevive();
}


void CActorComponent::OnEnterVehicle(IActor* pActor, const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson)
{}


void CActorComponent::OnExitVehicle(IActor* pActor)
{}


void CActorComponent::OnHealthChanged(IActor* pActor, float newHealth)
{}


void CActorComponent::OnItemPickedUp(IActor* pActor, EntityId itemId)
{}


void CActorComponent::OnItemUsed(IActor* pActor, EntityId itemId)
{}


void CActorComponent::OnItemDropped(IActor* pActor, EntityId itemId)
{}


void CActorComponent::OnSprintStaminaChanged(IActor* pActor, float newStamina)
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


IItem* CActorComponent::GetCurrentItem(bool includeVehicle) const
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
	// Make a note of the player for back reference.
	m_pPlayer = &player;

	// Default assumption is we now control the character.
	//m_isAIControlled = false;

	// We should reset the animations.
	OnResetState();
}


void CActorComponent::OnPlayerDetach()
{
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


void CActorComponent::OnResetState()
{
	const auto pEntity = GetEntity();

	// Select a character definition based on first / third person mode. Hard coding the default scope isn't a great
	// idea, but it's good enough for now. 
	if (IsViewFirstPerson())
	{
		m_pAdvancedAnimationComponent->SetCharacterFile(m_geometryFirstPerson.value);
		m_pAdvancedAnimationComponent->SetDefaultScopeContextName("Char1P");
		
		// TODO: In order to switch the models out, we need to load and reset - but at present that is not removing the
		// existing models. 
		
		//m_pAdvancedAnimationComponent->LoadFromDisk();
		//m_pAdvancedAnimationComponent->ResetCharacter();
	}
	else
	{
		m_pAdvancedAnimationComponent->SetCharacterFile(m_geometryThirdPerson.value);
		m_pAdvancedAnimationComponent->SetDefaultScopeContextName("Char3P");
		//m_pAdvancedAnimationComponent->LoadFromDisk();
		//m_pAdvancedAnimationComponent->ResetCharacter();
	}

	// You need to reset the character after changing the animation properties.
	m_pAdvancedAnimationComponent->ResetCharacter();

	// HACK: the CAdvancedAnimation doesn't allow us to queue actions yet, this is a workaround.
	// Queue the locomotion action, which switches fragments and tags as needed for actor locomotion.
	IActionController *pActionController = gEnv->pGameFramework->GetMannequinInterface().FindActionController(*GetEntity());
	if (pActionController != nullptr)
	{
		auto locomotionAction = new CActorAnimationActionLocomotion();
		pActionController->Queue(*locomotionAction);

		// HACK: quick way to get some debug info out. Need to filter it to only one entity to prevent overlays.
		//if (strcmp(GetEntity()->GetName(), "Hero") == 0)
		//	pActionController->SetFlag(AC_DebugDraw, true);
	}

	// Mannequin should also be reset.
	//ResetMannequin();
}


// HACK: NOTE: TODO: I removed this code during the 5.4 refactor because it's hard to see quite how it fits in again.
// Most of it will need to be added in at some point. 

//// TODO: Is this really needed? Perhaps there's a better way to handle it. Revive isn't getting called at present
//// so there must be a better place for this.
//
//void CActor::ResetMannequin()
//{
//	if (m_pActionController)
//	{
//		//if (IsPlayer() && !IsAIControlled())
//		//{
//		//	m_pActionController->Resume();
//
//		//	SAnimationContext &animContext = m_pActionController->GetContext();
//		//	animContext.state.Set(g_actorMannequinParams.tagIDs.localClient, IsClient());
//		//	animContext.state.SetGroup(g_actorMannequinParams.tagGroupIDs.playMode, gEnv->bMultiplayer ? g_actorMannequinParams.tagIDs.MP : g_actorMannequinParams.tagIDs.SP);
//		//	animContext.state.Set(g_actorMannequinParams.tagIDs.FP, IsViewFirstPerson());
//
//		//	SetStanceTag(m_pCharacterControllerComponent->GetStance(), animContext.state);
//
//		//	// Install persistent AimPose action
//		//	m_pActionController->Queue(*new CPlayerBackgroundAction(EActorActionPriority::eAAP_Movement, g_actorMannequinParams.fragmentIDs.AimPose));
//
//		//	// Install persistent WeaponPose action
//		//	m_pActionController->Queue(*new CPlayerBackgroundAction(EActorActionPriority::eAAP_Lowest, g_actorMannequinParams.fragmentIDs.WeaponPose));
//
//		//	CActionItemIdle *itemIdle = new CActionItemIdle(EActorActionPriority::eAAP_Lowest, g_actorMannequinParams.fragmentIDs.Idle, g_actorMannequinParams.fragmentIDs.IdleBreak, TAG_STATE_EMPTY, *this);
//		//	m_pActionController->Queue(*itemIdle);
//
//		//    CPlayerMovementAction *movementAction = new CPlayerMovementAction(EActorActionPriority::eAAP_Movement);
//		//    m_pActionController->Queue(*movementAction);
//
//		// Locomotion action.
//		auto locomotionAction = new CActorAnimationActionLocomotion();
//		m_pActionController->Queue(*locomotionAction);
//
//		// TODO: Get back to aiming and looking after some interaction is sorted.
//
//		// Aim actions.
//		//if (CActorAnimationActionAimPose::IsSupported(m_pActionController->GetContext())
//		//	&& CActorAnimationActionAiming::IsSupported(m_pActionController->GetContext()))
//		//{
//		//	// TODO: I presume these are needed to do things.
//		//	//m_pProceduralContextAim = static_cast<CProceduralContextAim*>(m_pActionController->FindOrCreateProceduralContext(PROCEDURAL_CONTEXT_AIM_NAME));
//
//		//	m_pActionController->Queue(*new CActorAnimationActionAimPose());
//		//	m_pActionController->Queue(*new CActorAnimationActionAiming());
//		//}
//
//		//// Look actions.
//		//if (CActorAnimationActionLookPose::IsSupported(m_pActionController->GetContext()) 
//		//	&& CActorAnimationActionLooking::IsSupported(m_pActionController->GetContext()))
//		//{
//		//	// TODO: I presume these are needed to do things.
//		//	//m_pProceduralContextLook = static_cast<CProceduralContextLook*>(m_pActionController->FindOrCreateProceduralContext(PROCEDURAL_CONTEXT_LOOK_NAME));
//
//		//	m_pActionController->Queue(*new CActorAnimationActionLookPose());
//		//	m_pActionController->Queue(*new CActorAnimationActionLooking());
//		//}
//
//		//	m_weaponFPAiming.ResetMannequin();
//		//}
//	}
//}


void CActorComponent::Kill()
{
	// #TODO: A *LOT* of code needs to be added here to handle reviving.
}


void CActorComponent::Revive(EReasonForRevive reasonForRevive)
{
	// #TODO: A *LOT* of code needs to be added here to handle reviving.

	// Mannequin should be reset.
	//ResetMannequin();
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
			if (auto pInteraction = pInteractor->GetInteraction("interaction_drop")._Get())
			{
				pInteraction->OnInteractionStart();
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
			if (auto pInteraction = pInteractor->GetInteraction("interaction_toss")._Get())
			{
				pInteraction->OnInteractionStart();
			}
		}
	}

	// We no longer have an entity to drop.
	m_interactionEntityId = INVALID_ENTITYID;
}


void CActorComponent::OnActionBarUse(int actionBarId)
{
	if (m_pAwareness)
	{
		auto results = m_pAwareness->GetNearDotFiltered();
		if (results.size() > 0)
		{
			auto pTargetEntity = gEnv->pEntitySystem->GetEntity(results [0]);

			if (auto pInteractor = pTargetEntity->GetComponent<CEntityInteractionComponent>())
			{
				// There's an interactor component, so this is an interactive entity.
				auto verbs = pInteractor->GetVerbs();
				if (verbs.size() >= actionBarId)
				{
					auto verb = verbs [actionBarId - 1];
					auto pInteraction = pInteractor->GetInteraction(verb)._Get();

					pInteraction->OnInteractionStart();
				}
				else
				{
					CryLogAlways("No action defined.");
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
	if (m_pAwareness)
	{
		auto results = m_pAwareness->GetNearDotFiltered();
		if (results.size() > 0)
		{
			m_interactionEntityId = results [0];
			auto pInteractionEntity = gEnv->pEntitySystem->GetEntity(m_interactionEntityId);

			if (auto pInteractor = pInteractionEntity->GetComponent<CEntityInteractionComponent>())
			{
				// There's an interactor component, so this is an interactive entity.
				auto verbs = pInteractor->GetVerbs();
				if (verbs.size() > 0)
				{
					auto verb = verbs [0];

					// HACK: TEST making a call to the DRS system
					auto pDrsProxy = crycomponent_cast<IEntityDynamicResponseComponent*> (pInteractionEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));
					pDrsProxy->GetResponseActor()->QueueSignal(verb);

					// #HACK: Another test - just calling the interaction directly instead.
					auto pInteraction = pInteractor->GetInteraction(verb)._Get();
					pInteraction->OnInteractionStart();
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
	if (m_pAwareness)
	{
		auto results = m_pAwareness->GetNearDotFiltered();
		if (results.size() > 0)
		{
			m_interactionEntityId = results [0];
			auto pInteractionEntity = gEnv->pEntitySystem->GetEntity(m_interactionEntityId);

			// HACK: Another test, this time of the slaved animation code.
			//TagState tagState { TAG_STATE_EMPTY };
			//auto pPlayerAction = new CActorAnimationActionCooperative(*this, m_interactionEntityId, g_actorMannequinParams.fragmentIDs.Interaction,
			//	tagState, g_actorMannequinParams.tagIDs.ScopeSlave);
			//m_pActionController->Queue(*pPlayerAction);

			// HACK: Another test - this time of setting an emote.
			//auto emoteAction = new CActorAnimationActionEmote(g_emoteMannequinParams.tagIDs.Awe);
			//m_pActionController->Queue(*emoteAction);

			if (auto pInteractor = pInteractionEntity->GetComponent<CEntityInteractionComponent>())
			{
				// There's an interactor component, so this is an interactive entity.
				// #TODO: We should really only process an 'interact' verb - not simply the first entry.
				auto verbs = pInteractor->GetVerbs();
				if (verbs.size() > 0)
				{
					// Display the verbs in a cheap manner.
					CryLogAlways("VERBS");
					int index { 1 };
					for (auto& verb : verbs)
					{
						CryLogAlways("%d) %s", index, verb);
						index++;
					}

					auto verb = verbs [0];

					// #HACK: Another test - just calling the interaction directly instead.
					m_pInteraction = pInteractor->GetInteraction(verb)._Get();
					CryLogAlways("Player started interacting with: %s", m_pInteraction->GetVerbUI());
					m_pInteraction->OnInteractionStart();

					// HACK: Doesn't belong here, test to see if we can queue an interaction action.
					//auto action = new CActorAnimationActionInteraction();
					//m_pActionController->Queue(*action);
				}
			}
		}
	}
}


void CActorComponent::OnActionInteraction()
{
	if (m_pInteraction)
	{
		CryWatch("Interacting with: %s", m_pInteraction->GetVerbUI());
		m_pInteraction->OnInteractionTick();
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
		CryLogAlways("Player stopped interacting with: %s", m_pInteraction->GetVerbUI());
		m_pInteraction->OnInteractionComplete();
	}
	else
	{
		CryLogAlways("Player stopped interacting with nothing");
	}

	// No longer valid.
	m_pInteraction = nullptr;
	m_interactionEntityId = INVALID_ENTITYID;
}
}