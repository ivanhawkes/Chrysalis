#include <StdAfx.h>

#include "ActorComponent.h"
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
#include <Actor/ActorControllerComponent.h>
#include <Animation/ProceduralContext/ProceduralContextAim.h>
#include <Animation/ProceduralContext/ProceduralContextLook.h>
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

	// Contoller.
	m_pActorControllerComponent = pEntity->GetOrCreateComponent<CActorControllerComponent>();

	// Inventory management.
	m_pInventoryComponent = pEntity->GetOrCreateComponent<CInventoryComponent>();

	// Equipment management.
	m_pEquipmentComponent = pEntity->GetOrCreateComponent<CEquipmentComponent>();

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
		CryLogAlways("CActorComponent::HandleEvent(): Entity \"%s\" became the local character!", pEntity->GetName());
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


ICharacterInstance* CActorComponent::GetCharacter() const
{
	CRY_ASSERT(m_pAdvancedAnimationComponent);
	return m_pAdvancedAnimationComponent->GetCharacter();
}


const Vec3 CActorComponent::GetLocalEyePos() const
{
	// The default, in case we can't find the actual eye position, will be to use an average male's height.
	Vec3 eyePosition { 0.0f, 0.0f, 1.82f };

	// Get their character or bail early.
	auto pCharacter = m_pAdvancedAnimationComponent->GetCharacter();
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

		static bool alreadyWarned { false };
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
	// The default, in case we can't find the actual hand position.
	const Vec3 handPosition { -0.2f, 0.3f, 1.3f };

	// Get their character or bail early.
	auto pCharacter = m_pAdvancedAnimationComponent->GetCharacter();
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
	auto pCharacter = m_pAdvancedAnimationComponent->GetCharacter();
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

	// Queue the locomotion action, which switches fragments and tags as needed for actor locomotion.
	//auto locomotionAction = new CActorAnimationActionLocomotion();
	//QueueAction(*locomotionAction);

	// HACK: the CAdvancedAnimation doesn't allow us access to the action controller. This is a workaround.
	m_pActionController = gEnv->pGameFramework->GetMannequinInterface().FindActionController(*GetEntity());

	if (m_pActionController)
	{
		// The mannequin tags for an actor will need to be loaded. Because these are found in the controller definition,
		// they are potentially different for every actor. 
		m_actorMannequinParams = GetMannequinUserParams<SActorMannequinParams>(m_pActionController->GetContext());

		// HACK: quick way to get some debug info out. Need to filter it to only one entity to prevent overlays.
		if (strcmp(GetEntity()->GetName(), "Hero") == 0)
			m_pActionController->SetFlag(AC_DebugDraw, true);
	}

	// Mannequin should also be reset.
	ResetMannequin();
}


// HACK: NOTE: TODO: I removed this code during the 5.4 refactor because it's hard to see quite how it fits in again.
// Most of it will need to be added in at some point. 

// TODO: Is this really needed? Perhaps there's a better way to handle it. Revive isn't getting called at present
// so there must be a better place for this.

void CActorComponent::ResetMannequin()
{
	if (m_pActionController)
	{
		//if (IsPlayer() && !IsAIControlled())
		//{
		//	m_pActionController->Resume();
		//m_pActionController->Reset();

		//	SAnimationContext &animContext = m_pActionController->GetContext();
		//	animContext.state.Set(m_actorMannequinParams->tagIDs.localClient, IsClient());
		//	animContext.state.SetGroup(m_actorMannequinParams->tagGroupIDs.playMode, gEnv->bMultiplayer ? m_actorMannequinParams->tagIDs.MP : m_actorMannequinParams->tagIDs.SP);
		//	animContext.state.Set(m_actorMannequinParams->tagIDs.FP, IsViewFirstPerson());

		//	SetStanceTag(m_pCharacterControllerComponent->GetStance(), animContext.state);

		//	// Install persistent AimPose action
		//	QueueAction(*new CPlayerBackgroundAction(EActorActionPriority::eAAP_Movement, m_actorMannequinParams->fragmentIDs.AimPose));

		//	// Install persistent WeaponPose action
		//	QueueAction(*new CPlayerBackgroundAction(EActorActionPriority::eAAP_Lowest, m_actorMannequinParams->fragmentIDs.WeaponPose));

		//	CActionItemIdle *itemIdle = new CActionItemIdle(EActorActionPriority::eAAP_Lowest, m_actorMannequinParams->fragmentIDs.Idle, m_actorMannequinParams->fragmentIDs.IdleBreak, TAG_STATE_EMPTY, *this);
		//	QueueAction(*itemIdle);

		//	CPlayerMovementAction *movementAction = new CPlayerMovementAction(EActorActionPriority::eAAP_Movement);
		//	QueueAction(*movementAction);

		// Queue the locomotion action, which switches fragments and tags as needed for actor locomotion.
		auto locomotionAction = new CActorAnimationActionLocomotion();
		QueueAction(*locomotionAction);

		// Aim actions.
		//if (CActorAnimationActionAimPose::IsSupported(m_pActionController->GetContext())
		//	&& CActorAnimationActionAiming::IsSupported(m_pActionController->GetContext()))
		//{
		//	m_pProceduralContextAim = static_cast<CProceduralContextAim*>(m_pActionController->FindOrCreateProceduralContext(CProceduralContextAim::GetCID()));
		//	QueueAction(*new CActorAnimationActionAimPose());
		//	QueueAction(*new CActorAnimationActionAiming());
		//}

		//// Look actions.
		if (CActorAnimationActionLookPose::IsSupported(m_pActionController->GetContext())
			&& CActorAnimationActionLooking::IsSupported(m_pActionController->GetContext()))
		{
			m_pProceduralContextLook = static_cast<CProceduralContextLook*>(m_pActionController->FindOrCreateProceduralContext(CProceduralContextLook::GetCID()));
			QueueAction(*new CActorAnimationActionLookPose());
			QueueAction(*new CActorAnimationActionLooking());
		}

		//	m_weaponFPAiming.ResetMannequin();
		//}
	}
}


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
					auto pInteraction = pInteractor->GetInteraction(verb).lock();

					pInteraction->OnInteractionStart(*this);
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
	//if (m_pInteraction != nullptr)
	if (isBusyInInteraction)
		return;

	if (m_pAwareness)
	{
		auto results = m_pAwareness->GetNearDotFiltered();
		if (results.size() > 0)
		{
			m_interactionEntityId = results [0];
			auto pInteractionEntity = gEnv->pEntitySystem->GetEntity(m_interactionEntityId);

			// HACK: Another test, this time of the slaved animation code.
			//TagState tagState { TAG_STATE_EMPTY };
			//auto pPlayerAction = new CActorAnimationActionCooperative(*this, m_interactionEntityId, m_actorMannequinParams->fragmentIDs.Interaction,
			//	tagState, m_actorMannequinParams->tagIDs.ScopeSlave);
			//QueueAction(*pPlayerAction);

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
					int index { 1 };
					for (auto& verb : verbs)
					{
						CryLogAlways("%d) %s", index, verb);
						index++;
					}

					auto verb = verbs [0];

					// #HACK: Another test - just calling the interaction directly instead.
					m_pInteraction = pInteractor->GetInteraction(verb).lock();
					CryLogAlways("Player started interacting with: %s", m_pInteraction->GetVerbUI());
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
		CryWatch("Interacting with: %s", m_pInteraction->GetVerbUI());
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
		CryLogAlways("Player stopped interacting with: %s", m_pInteraction->GetVerbUI());
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
// *** Allow control of the actor's animations / fragments / etc.
// ***

IActionController* CActorComponent::GetActionController() const
{
	return m_pActionController;
}


TagID CActorComponent::GetStanceTagId(EActorStance actorStance)
{
	TagID tagId { TAG_ID_INVALID };

	switch (actorStance)
	{
		case EActorStance::eAS_Crawling:
			tagId = m_actorMannequinParams->tagIDs.Crawling;
			break;

		case EActorStance::eAS_Prone:
			tagId = m_actorMannequinParams->tagIDs.Prone;
			break;

		case EActorStance::eAS_Crouching:
			tagId = m_actorMannequinParams->tagIDs.Crouching;
			break;

		case EActorStance::eAS_Falling:
			tagId = m_actorMannequinParams->tagIDs.Falling;
			break;

		case EActorStance::eAS_Flying:
			tagId = m_actorMannequinParams->tagIDs.Flying;
			break;

		case EActorStance::eAS_Kneeling:
			tagId = m_actorMannequinParams->tagIDs.Kneeling;
			break;

		case EActorStance::eAS_Landing:
			tagId = m_actorMannequinParams->tagIDs.Landing;
			break;

		case EActorStance::eAS_SittingChair:
			tagId = m_actorMannequinParams->tagIDs.SittingChair;
			break;

		case EActorStance::eAS_SittingFloor:
			tagId = m_actorMannequinParams->tagIDs.SittingFloor;
			break;

		case EActorStance::eAS_Sleeping:
			tagId = m_actorMannequinParams->tagIDs.Sleeping;
			break;

		case EActorStance::eAS_Spellcasting:
			tagId = m_actorMannequinParams->tagIDs.Spellcasting;
			break;

		case EActorStance::eAS_Standing:
			tagId = m_actorMannequinParams->tagIDs.Standing;
			break;

		case EActorStance::eAS_Swimming:
			tagId = m_actorMannequinParams->tagIDs.Swimming;
			break;
	}

	return tagId;
}


TagID CActorComponent::GetPostureTagId(EActorPosture actorPosture)
{
	TagID tagId { TAG_ID_INVALID };

	switch (actorPosture)
	{
		case EActorPosture::eAP_Aggressive:
			tagId = m_actorMannequinParams->tagIDs.Aggressive;
			break;

		case EActorPosture::eAP_Alerted:
			tagId = m_actorMannequinParams->tagIDs.Alerted;
			break;

		case EActorPosture::eAP_Bored:
			tagId = m_actorMannequinParams->tagIDs.Bored;
			break;

		case EActorPosture::eAP_Dazed:
			tagId = m_actorMannequinParams->tagIDs.Dazed;
			break;

		case EActorPosture::eAP_Depressed:
			tagId = m_actorMannequinParams->tagIDs.Depressed;
			break;

		case EActorPosture::eAP_Distracted:
			tagId = m_actorMannequinParams->tagIDs.Distracted;
			break;

		case EActorPosture::eAP_Excited:
			tagId = m_actorMannequinParams->tagIDs.Excited;
			break;

		case EActorPosture::eAP_Interested:
			tagId = m_actorMannequinParams->tagIDs.Interested;
			break;

		case EActorPosture::eAP_Neutral:
			tagId = m_actorMannequinParams->tagIDs.Neutral;
			break;

		case EActorPosture::eAP_Passive:
			tagId = m_actorMannequinParams->tagIDs.Passive;
			break;

		case EActorPosture::eAP_Suspicious:
			tagId = m_actorMannequinParams->tagIDs.Suspicious;
			break;

		case EActorPosture::eAP_Unaware:
			tagId = m_actorMannequinParams->tagIDs.Unaware;
			break;
	}

	return tagId;
}
}