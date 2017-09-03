#include <StdAfx.h>

#include "Actor.h"
#include <CryMath/Cry_Math.h>
#include <CryCore/Assert/CryAssert.h>
#include <ICryMannequin.h>
#include <IAnimatedCharacter.h>
#include <IGameObject.h>
#include <IItemSystem.h>
#include <Actor/Animation/ActorAnimation.h>
#include <Actor/Animation/Actions/ActorAnimationActionAiming.h>
#include <Actor/Animation/Actions/ActorAnimationActionAimPose.h>
#include <Actor/Animation/Actions/ActorAnimationActionCooperative.h>
#include <Actor/Animation/Actions/ActorAnimationActionEmote.h>
#include <Actor/Animation/Actions/ActorAnimationActionInteration.h>
#include <Actor/Animation/Actions/ActorAnimationActionLocomotion.h>
#include <Actor/Animation/Actions/ActorAnimationActionLooking.h>
#include <Actor/Animation/Actions/ActorAnimationActionLookPose.h>
#include <Actor/Movement/ActorMovementController.h>
#include <Actor/ActorPhysics.h>
#include <Components/Player/Input/IPlayerInputComponent.h>
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
// Definition of the state machine that controls character movement.
DEFINE_STATE_MACHINE(IActorComponent, Movement);


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
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

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

	// Release the movement state machine.
	//MovementHSMRelease();

	// We're over worrying about rotation.
	//SAFE_DELETE(m_characterRotation);
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

	// Initialise the movement state machine.
	//MovementHSMInit();

	// Default is for a character to be controlled by AI.
	//	m_isAIControlled = true;
	m_isAIControlled = false;

	// Tells this instance to trigger areas.
	pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS);

	// Are we the local player?
	if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
	{
		// Tells this instance to trigger areas and that it's the local player.
		pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS | ENTITY_FLAG_LOCAL_PLAYER);
		gEnv->pLog->LogAlways("CActorComponent::HandleEvent(): Entity \"%s\" became the local character!", pEntity->GetName());
	}

	// Resolve the animation tags.
	m_rotateTagId = m_pAdvancedAnimationComponent->GetTagId("Rotate");

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


// TODO: CRITICAL: HACK: BROKEN: !!

//void CActorComponent::FullSerialize(TSerialize ser)
//{
//	// Serialise the movement state machine.
//	MovementHSMSerialize(ser);
//}


void CActorComponent::Update(SEntityUpdateContext* pCtx)
{
	const float frameTime = pCtx->fFrameTime;

	UpdateMovementRequest(frameTime);
	UpdateLookDirectionRequest(frameTime);
	UpdateAnimation(frameTime);

	// Update the movement state machine.
	// TODO: CRITICAL: HACK: BROKEN: how do we get the ctx now?
	//MovementHSMUpdate(ctx, updateSlot);
}


void CActorComponent::UpdateMovementRequest(float frameTime)
{
	// Default is for them to not request movement.
	m_movementRequest = ZERO;

	// Don't handle input if we are in air.
	if (!m_pCharacterControllerComponent->IsOnGround())
	{
		m_movingDuration = 0.0f;
		return;
	}

	// If there's a player controlling us, we can query them for inputs and camera and apply that to our movement.
	if (m_pPlayer)
	{
		auto* pPlayerInput = m_pPlayer->GetPlayerInput();

		// The request needs to take into account both the direction of the camera and the direction of the movement i.e.
		// left is alway left relative to the camera.
		// TODO: What will it take to make this use AddVelocity instead? The values don't seem to match with what I'd
		// expect to input to it. 
		float moveSpeed = GetMovementBaseSpeed(pPlayerInput->GetMovementDirectionFlags());
		if (moveSpeed > FLT_EPSILON)
		{
			m_movingDuration += frameTime;
			m_movementRequest = pPlayerInput->GetMovement(m_pPlayer->GetCamera()->GetRotation()) * moveSpeed;
			SetVelocity(m_movementRequest);
		}
	}
}


void CActorComponent::UpdateLookDirectionRequest(float frameTime)
{
	const float angularVelocityMax = g_PI / 1.0f; // Radians / sec
	const float catchupSpeed = g_PI2 / 0.25f; // Full rotations / second.

	// If there's a player controlling us, we can query them for inputs and camera and apply that to our rotation.
	if (m_pPlayer)
	{
		auto* pPlayerInput = m_pPlayer->GetPlayerInput();

		// Only allow the character to rotate in first person, and third person if they are moving.
		if ((!m_pPlayer->IsThirdPerson()) || (m_pPlayer->IsThirdPerson() && m_movementRequest.len() > FLT_EPSILON))
		{
			// Take the direction they are facing as a target.
			Ang3 facingDir;
			if (m_pPlayer->IsThirdPerson())
				facingDir = CCamera::CreateAnglesYPR(m_movementRequest);
			else
				facingDir = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));

			// Use their last orientation as their present direction.
			// NOTE: I tried it with GetEntity()->GetWorldTM() but that caused crazy jitter issues.
			Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));

			// We add in some extra rotation to 'catch up' to the direction they are being moved. This will perform a gradual
			// turn on the actor over several frames. 
			float rotationDelta { 0.0f };
			if (std::abs(facingDir.x - ypr.x) > std::abs(ypr.x - facingDir.x))
				rotationDelta = ypr.x - facingDir.x;
			else
				rotationDelta = facingDir.x - ypr.x;

			//float catchUp = rotationDelta / catchupSpeed * frameTime;
			float catchUp = std::min(rotationDelta * frameTime, catchupSpeed * frameTime);

			// Update angular velocity metrics.
			//m_yawAngularVelocity = CLAMP(pPlayerInput->GetMouseYawDelta() + catchUp, -angularVelocityMax * frameTime, angularVelocityMax * frameTime);
			m_yawAngularVelocity = pPlayerInput->GetMouseYawDelta() + catchUp;
			//CryWatch("m_yawAngularVelocity = %0.2f", m_yawAngularVelocity / frameTime);

			// Yaw.
			ypr.x += m_yawAngularVelocity;

			// Roll (zero it).
			ypr.z = 0;

			// Update the preferred direction we face.
			m_lookOrientation = Quat(CCamera::CreateOrientationYPR(ypr));
		}
	}
}


void CActorComponent::UpdateAnimation(float frameTime)
{
	if (m_pPlayer)
	{
		const float angularVelocityMin = 0.174f; // Radians / sec

		// Update tags and motion parameters used for turning
		const bool isTurning = std::abs(m_yawAngularVelocity) > angularVelocityMin;
		m_pAdvancedAnimationComponent->SetTagWithId(m_rotateTagId, isTurning);
		if (isTurning)
		{
			// Expect the turning motion to take approximately one second.
			// TODO: Get to work on making this happen more like Blade and Soul.
			const float turnDuration = 1.0f;
			m_pAdvancedAnimationComponent->SetMotionParameter(eMotionParamID_TurnAngle, m_yawAngularVelocity * turnDuration);
		}

		// Update entity rotation as the player turns. We only want to affect Z-axis rotation, zero pitch and roll.
		// TODO: is there a case where we want to avoid zeroing out pitch and roll?
		Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
		ypr.y = 0;
		ypr.z = 0;
		const Quat correctedOrientation = Quat(CCamera::CreateOrientationYPR(ypr));

		// Send updated transform to the entity, only orientation changes.
		GetEntity()->SetPosRotScale(GetEntity()->GetWorldPos(), correctedOrientation, Vec3(1, 1, 1));
	}
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


void CActorComponent::OnStanceChanged(IActor* pActor, EStance stance)
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
	if (EntityId itemId = GetCurrentItemId(includeVehicle))
		return gEnv->pGameFramework->GetIItemSystem()->GetItem(itemId);

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
	m_isAIControlled = false;

	// We should reset the animations.
	OnResetState();
}


void CActorComponent::OnPlayerDetach()
{
	m_pPlayer = nullptr;

	// #TODO: Detach the camera.

	// #TODO: We can remove the entity awareness component if that's desirable.

	// #TODO: handle transitioning this character back into the loving hands of the AI.
	m_isAIControlled = true;
}


// ***
// *** Life-cycle
// ***


void CActorComponent::OnToggleThirdPerson()
{
	// We might need to switch character models or scopes.
	OnResetState();
}


void CActorComponent::OnResetState()
{
	const auto pEntity = GetEntity();

	// Select a character definition based on first / third person mode. Hard coding the default scope isn't a great
	// idea, but it's good enough for now. 
	if (IsThirdPerson())
	{
		m_pAdvancedAnimationComponent->SetCharacterFile(m_geometryThirdPerson.value);
		m_pAdvancedAnimationComponent->SetDefaultScopeContextName("Char3P");
	}
	else
	{
		m_pAdvancedAnimationComponent->SetCharacterFile(m_geometryFirstPerson.value);
		m_pAdvancedAnimationComponent->SetDefaultScopeContextName("Char1P");
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

	//// Reset the HSM for movement.
	//MovementHSMReset();

	//// Select which HSM to use for our actor's movement. This relies on it's AI status being correctly set first.
	//SelectMovementHierarchy();

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
//		//	animContext.state.Set(g_actorMannequinParams.tagIDs.FP, !IsThirdPerson());
//
//		//	SetStanceTag(GetStance(), animContext.state);
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

	//// Reset the HSM for character movement.
	//MovementHSMReset();

	//// Select which HSM to use for our character's movement. This relies on it's AI status being
	//// correctly set first.
	//SelectMovementHierarchy();

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
	CryLogAlways("Player started interacting with things.");

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
					auto verb = verbs [0];

					// #HACK: Another test - just calling the interaction directly instead.
					auto pInteraction = pInteractor->GetInteraction(verb)._Get();
					pInteraction->OnInteractionStart();

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
}


void CActorComponent::OnActionInteractionEnd()
{
	CryLogAlways("Player stopped interacting with things.");
}


// TODO: So wrong in every way. Need some more state checking before deciding which stance to move to when exiting
// crouching, though this is a pretty decent default. Needs checks to ensure you can't crouch while swimming / falling /
// etc - maybe put this into the class itself? 

void CActorComponent::OnActionCrouchToggle()
{
	if (GetStance() == EActorStance::eAS_Crouching)
	{
		SetStance(EActorStance::eAS_Standing);
	}
	else
	{
		SetStance(EActorStance::eAS_Crouching);
	}
}


void CActorComponent::OnActionCrawlToggle()
{
	if (GetStance() == EActorStance::eAS_Crawling)
	{
		SetStance(EActorStance::eAS_Standing);
	}
	else
	{
		SetStance(EActorStance::eAS_Crawling);
	}
}


void CActorComponent::OnActionKneelToggle()
{
	if (GetStance() == EActorStance::eAS_Kneeling)
	{
		SetStance(EActorStance::eAS_Standing);
	}
	else
	{
		SetStance(EActorStance::eAS_Kneeling);
	}
}


void CActorComponent::OnActionSitToggle()
{
	if (GetStance() == EActorStance::eAS_SittingFloor)
	{
		SetStance(EActorStance::eAS_Standing);
	}
	else
	{
		SetStance(EActorStance::eAS_SittingFloor);
	}
}


float CActorComponent::GetMovementBaseSpeed(TInputFlags movementDirectionFlags) const
{
	const static float walkBaseSpeed { 2.1f };
	const static float jogBaseSpeed { 4.2f };
	const static float runBaseSpeed { 6.3f };
	const static float crawlBaseSpeed { 1.2f };
	const static float crouchBaseSpeed { 1.2f };
	float baseSpeed { 0.0f };
	float dirScale { 1.0f };

	switch (GetStance())
	{
		case EActorStance::eAS_Standing:
			// Work out a base for walking, jogging or sprinting.
			if (IsSprinting())
			{
				baseSpeed = runBaseSpeed;
			}
			else
			{
				if (IsJogging())
					baseSpeed = jogBaseSpeed;
				else
					baseSpeed = walkBaseSpeed;
			}
			break;

		case EActorStance::eAS_Crawling:
			baseSpeed = crawlBaseSpeed;
			break;

		case EActorStance::eAS_Crouching:
			baseSpeed = crouchBaseSpeed;
			break;

		case EActorStance::eAS_Swimming:
			baseSpeed = walkBaseSpeed;
			break;

		case EActorStance::eAS_Flying:
			baseSpeed = jogBaseSpeed;
			break;

		case EActorStance::eAS_Spellcasting:
			baseSpeed = walkBaseSpeed;
			break;

		default:
			// Don't let them control movement.
			baseSpeed = 0.0f;
			break;
	}

	// Scale it based on their movement direction.
	switch (movementDirectionFlags)
	{
		case (TInputFlags)EInputFlag::Forward:
			dirScale = 1.0f;
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Right):
			dirScale = 0.9f;
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Left):
			dirScale = 0.9f;
			break;

		case (TInputFlags)EInputFlag::Right:
			dirScale = 0.85f;
			break;

		case (TInputFlags)EInputFlag::Left:
			dirScale = 0.85f;
			break;

		case (TInputFlags)EInputFlag::Backward:
			dirScale = 0.71f;
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Right):
			dirScale = 0.71f;
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Left):
			dirScale = 0.71f;
			break;

		default:
			dirScale = 0.0f;
			break;
	}

	return baseSpeed * dirScale;
}


//// ***
//// *** Hierarchical State Machine Support
//// ***
//
//
//void CCharacterComponent::SelectMovementHierarchy()
//{
//	// Force the state machine in the proper hierarchy
//	//if (IsAIControlled ())
//	//{
//	//	CRY_ASSERT (!IsPlayer ());
//
//	//	StateMachineHandleEventMovement (ACTOR_EVENT_ENTRY_AI);
//	//}
//	//else
//	//{
//	//	StateMachineHandleEventMovement (ACTOR_EVENT_ENTRY_PLAYER);
//	//}
//
//	// #HACK: set it to always be player for now.
//	// #TODO: NEED THIS!!!
//	StateMachineHandleEventMovement(ACTOR_EVENT_ENTRY);
//}
//
//
//void CCharacterComponent::MovementHSMRelease()
//{
//	StateMachineReleaseMovement();
//}
//
//
//void CCharacterComponent::MovementHSMInit()
//{
//	StateMachineInitMovement();
//}
//
//
//void CCharacterComponent::MovementHSMSerialize(TSerialize ser)
//{
//	StateMachineSerializeMovement(SStateEventSerialize(ser));
//}
//
//
//void CCharacterComponent::MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot)
//{
//	//#ifdef STATE_DEBUG
//	//	const bool shouldDebug = (s_StateMachineDebugEntityID == GetEntityId ());
//	//#else
//	//	const bool shouldDebug = false;
//	//#endif
//	const bool shouldDebug = false;
//	//const bool shouldDebug = true;
//
//	StateMachineUpdateMovement(ctx.fFrameTime, shouldDebug);
//
//	// Pass the update into the movement state machine.
//	// #TODO: make this happen.
//	StateMachineHandleEventMovement(SStateEventUpdate(ctx.fFrameTime));
//}
//
//
//void CCharacterComponent::MovementHSMReset()
//{
//	StateMachineResetMovement();
//}
}