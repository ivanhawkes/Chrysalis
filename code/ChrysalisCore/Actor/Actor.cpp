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
#include <Actor/Character/Character.h>
#include <Actor/Movement/ActorMovementController.h>
#include <Actor/ActorPhysics.h>
#include <Player/Input/IPlayerInputComponent.h>
#include <Player/Player.h>
#include <Components/Interaction/EntityAwarenessComponent.h>
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Utility/CryWatch.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


const string MANNEQUIN_FOLDER = "Animations/Mannequin/ADB/";


CActor::CActor()
{
	//m_characterRotation = new CCharacterRotation(*this);
}


CActor::~CActor()
{
	// Very important that this gets called. Removes the character from the system.
	gEnv->pGameFramework->GetIActorSystem()->RemoveActor(GetEntityId());

	// Inventory takes a little extra work to break down.
	if (m_pInventory)
	{
		if (IItem* item = GetCurrentItem())
		{
			if (item->IsUsed())
				item->StopUse(GetEntityId());
		}

		if (gEnv->bServer)
			m_pInventory->Destroy();
	}

	// Release the controller first, then the animation context. Errors will occur if done the other way.
	SAFE_DELETE(m_pMovementController);
	SAFE_RELEASE(m_pActionController);
	SAFE_DELETE(m_pAnimationContext);

	// Release the movement state machine.
	MovementHSMRelease();

	// We're over worrying about rotation.
	//SAFE_DELETE(m_characterRotation);
}


void CActor::Initialize()
{
	// #TODO: Move the PostInit into here once we are free of GameObjects.
	//PostInit(gEnv->pGameFramework->GetGameObject(GetEntityId()));
}


// ***
// *** IGameObjectExtension
// ***


bool CActor::Init(IGameObject * pGameObject)
{
	ISimpleActor::Init(pGameObject);

	// Add this instance to the network.
	return pGameObject->BindToNetwork();
}


void CActor::PostInit(IGameObject * pGameObject)
{
	// Required for 5.3 to call update.
	GetEntity()->Activate(true);

	// We need until we can remove the old style of update.
	pGameObject->EnableUpdateSlot(this, 0);

	auto pEntity = GetEntity();

	// Register for pre-physics update.
	//RegisterEvent(ENTITY_EVENT_PREPHYSICSUPDATE, IComponent::EComponentFlags_Enable);

	// We will require pre-physics updates.
	pGameObject->EnablePrePhysicsUpdate(ePPU_Always);

	// Animated characters need to register for the post step immediate event.
	pGameObject->EnablePhysicsEvent(true, eEPE_OnPostStepImmediate);

	// Initialise the movement state machine.
	MovementHSMInit();

	// Create a movement controller and set it as the active controller for this game object.
	// Attempt to acquire an animated character component.
	m_pMovementController = new CActorMovementController(this);
	if (m_pMovementController)
		GetGameObject()->SetMovementController(m_pMovementController);

	// #TODO: Always false, for now, since the player is the actual client. Find out if it's ever
	// right for an actor to be the client and simplify this code if it's not.
	m_isClient = false;

	// Registers this instance to the actor system.
	gEnv->pGameFramework->GetIActorSystem()->AddActor(GetEntityId(), this);

	// Default is for a character to be controlled by AI.
	//	m_isAIControlled = true;
	m_isAIControlled = false;

	// Attempt to acquire an animated character component.
	m_pAnimatedCharacter = static_cast<IAnimatedCharacter*> (GetGameObject()->AcquireExtension("AnimatedCharacter"));

	// Attempt to acquire an inventory component.
	m_pInventory = static_cast<IInventory*>(GetGameObject()->AcquireExtension("Inventory"));

	// Tells this instance to trigger areas.
	pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS);

	// Since we are the client always update this instance's character.
	if (ICharacterInstance * pCharacter = pEntity->GetCharacter(0))
		pCharacter->SetFlags(pCharacter->GetFlags() | CS_FLAG_UPDATE_ALWAYS);

	// For now, all actors will have awareness built-in, but this should default to not having it at some stage unless they are
	// the player target.
	m_pAwareness = pEntity->GetOrCreateComponent<CEntityAwarenessComponent>();

	// Give the actor a DRS proxy, since it will probably need one.
	m_pDrsComponent = crycomponent_cast<IEntityDynamicResponseComponent*> (pEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));

	// Are we the local player?
	if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
	{
		// Tells this instance to trigger areas and that it's the local player.
		auto pEntity = GetEntity();
		pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS | ENTITY_FLAG_LOCAL_PLAYER);

		//// Since we are the client always update this instance's character.
		//if (ICharacterInstance * pCharacter = pEntity->GetCharacter(0))
		//	pCharacter->SetFlags(pCharacter->GetFlags() | CS_FLAG_UPDATE_ALWAYS);

		gEnv->pLog->LogAlways("CActor::HandleEvent(): Entity \"%s\" became the local character!", pEntity->GetName());
	}

	// Reset the entity.
	OnResetState();
}


void CActor::FullSerialize(TSerialize ser)
{
	// Serialise the movement state machine.
	MovementHSMSerialize(ser);
}


void CActor::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	if (m_pActionController)
		m_pActionController->Update(ctx.fFrameTime);

	// #HACK: Process this in player instead of here which will affect ALL characters.
	// Alternatively, and perhaps safer due to uncertain entity sorting, place a check around the character to ensure it's the
	// player's current attach target.
	if (m_pMovementController)
		m_pMovementController->Compute();

	// Update the movement state machine.
	MovementHSMUpdate(ctx, updateSlot);
}


void CActor::ProcessEvent(SEntityEvent& event)
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
	}
}


IEntityComponent::ComponentEventPriority CActor::GetEventPriority(const int eventID) const
{
	switch (eventID)
	{
		case ENTITY_EVENT_PREPHYSICSUPDATE:
			return ENTITY_PROXY_LAST - ENTITY_PROXY_USER + EEntityEventPriority_Actor + EEntityEventPriority_Client; // #HACK: only used for when we are the client, fix later.
	}

	return IGameObjectExtension::GetEventPriority(eventID);
}


void CActor::UpdateAnimationState(const SActorMovementRequest& movementRequest)
{
	if (m_pActionController)
	{
		// NOTE: should use actor physics to get the veloctiy, last velocity, movement flags, etc.
		// 	auto physics = GetActorPhysics();

		// #HACK: get some debug. NOTE: This draws a big sphere over the character's head.
		if (strcmp(GetEntity()->GetName(), "Hero") == 0)	// HACK: Need to filter it to only one entity to prevent overlays.
			m_pActionController->SetFlag(AC_DebugDraw, true);

		// #HACK: Tracking if we moved this frame / last frame. Move this into a more cohesive solution.
		if (movementRequest.desiredVelocity.len() > FLT_EPSILON)
		{
			m_wasMovingLastFrame = true;
		}
		else
		{
			m_wasMovingLastFrame = false;
		}
	}

	/*	// #TODO: get this working.

		// Update variable scope contexts.
		CWeapon *pWeapon = GetWeapon(GetCurrentItemId());
		ICharacterInstance *pICharInst = pWeapon ? pWeapon->GetEntity()->GetCharacter(0) : nullptr;
		IActionController *pActionController = GetAnimatedCharacter()->GetActionController();
		IMannequin &mannequinSys = gEnv->pGameFramework->GetMannequinInterface();

		if (IsAIControlled())
		{
			UpdateAIAnimationState(movement, pWeapon, pICharInst, pActionController, mannequinSys);
		}
		else if (IsPlayer() && pActionController)
		{
			SAnimationContext &animContext = pActionController->GetContext();

			// Update tags.
			bool isOutOfAmmo = false;

			if (pWeapon)
			{
				int firemodeIdx = pWeapon->GetCurrentFireMode();
				IFireMode *pFireMode = pWeapon->GetFireMode(firemodeIdx);
				if (pFireMode)
				{

					IEntityClass* pAmmoClass = pFireMode->GetAmmoType();
					int weaponAmmoCount = pWeapon->GetAmmoCount(pAmmoClass);
					int inventoryAmmoCount = pWeapon->GetInventoryAmmoCount(pAmmoClass);

					isOutOfAmmo = ((weaponAmmoCount + inventoryAmmoCount) == 0);
				}
			}

			if (!animContext.state.GetDef().IsGroupSet(animContext.state.GetMask(), g_actorMannequinParams.tagGroupIDs.item))
			{
				animContext.state.Set(g_actorMannequinParams.tagIDs.nw, true);
				animContext.state.SetGroup(g_actorMannequinParams.tagGroupIDs.zoom, TAG_ID_INVALID);
				animContext.state.SetGroup(g_actorMannequinParams.tagGroupIDs.firemode, TAG_ID_INVALID);
			}

			animContext.state.Set(g_actorMannequinParams.tagIDs.outOfAmmo, isOutOfAmmo);
			const bool aimEnabled = !IsSprinting() || (pWeapon && pWeapon->IsReloading());
			animContext.state.Set(g_actorMannequinParams.tagIDs.aiming, (movement.aimIK || m_isPlayer) && aimEnabled);

			const Vec3 referenceVel = GetActorPhysics().velocity;
			float speedXY = referenceVel.GetLength2D();

			//		CryWatch("Vel (%f, %f, %f)", m_actorState.velocity.x, m_actorState.velocity.y, m_actorState.velocity.z);
			//		CryWatch("LastRequestedVel (%f, %f, %f)", m_lastRequestedVelocity.x, m_lastRequestedVelocity.y, m_lastRequestedVelocity.z);
			//		CryWatch("DesVel (%f, %f, %f)", desiredVelocity.x, desiredVelocity.y, desiredVelocity.z);

			TagID movementTag = IsSprinting() ? g_actorMannequinParams.tagIDs.sprint : TAG_ID_INVALID;
			TagID moveDir = TAG_ID_INVALID;
			if (speedXY > 0.5f)
			{
				Vec3 velXY;
				velXY.Set(referenceVel.x / speedXY, referenceVel.y / speedXY, 0.0f);

				const float signedAngle = Ang3::CreateRadZ(velXY, GetEntity()->GetForwardDir());
				const float unsignedAngle = fabs_tpl(signedAngle);
				if (unsignedAngle < gf_PI*0.25f)
				{
					moveDir = g_actorMannequinParams.tagIDs.forward;
				}
				else if (unsignedAngle > gf_PI*0.75f)
				{
					moveDir = g_actorMannequinParams.tagIDs.backward;
				}
				else if (signedAngle > 0.0f)
				{
					moveDir = g_actorMannequinParams.tagIDs.right;
				}
				else
				{
					moveDir = g_actorMannequinParams.tagIDs.left;
				}
			}

			animContext.state.SetGroup(g_actorMannequinParams.tagGroupIDs.moveDir, moveDir);
			animContext.state.SetGroup(g_actorMannequinParams.tagGroupIDs.moveSpeed, movementTag);
		}*/
}


// *** 
// *** IActor
// *** 


Vec3 CActor::GetLocalEyePos() const
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
					CryLogAlways("Character class %s does not have '#camera', 'left_eye' or 'right_eye' defined.", GetActorClassName());
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


Vec3 CActor::GetLocalLeftHandPos() const
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


Vec3 CActor::GetLocalRightHandPos() const
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


IMovementController* CActor::GetMovementController() const
{
	return m_pMovementController;
}


bool CActor::IsPlayer() const
{
	//CryLogAlways("[Warning] IsPlayer () tested, possible cast problems if they cast CCharacter to a CPlayer");
	return m_pAttachedPlayer != nullptr;
}


bool CActor::IsClient() const
{
	return m_isClient;
}


bool CActor::Physicalize()
{
	// #TODO: We are not making good use of pe_player_dimensions yet.

	// We are going to be physicalizing this instance (the player), so setup the physics dimensions to use for physics calculations.
	// See http://docs.cryengine.com/display/SDKDOC4/Physics+System+Programming
	pe_player_dimensions PDim;

	// We want a capsule, not a cylinder.
	PDim.bUseCapsule = 1;

	// Specifies the z-coordinate of a point in the entity frame that is considered to be at the feet level (usually 0).
	PDim.heightPivot = 0.0f;

	// Added recently, testing if they work.
	PDim.heightCollider = 0.0f;
	PDim.heightEye = 1.75f;
	PDim.type = PE_RIGID;
	PDim.sizeCollider = Vec3(0.45f, 0.45f, 2.0f);

	//PDim.heightEye = x; // The z-coordinate of the camera attached to the entity.

	// sizeCollider specifies the size of the cylinder (x is radius, z is half - height, y is unused).
	//PDim.sizeCollider = Vec3(rad, half height, 0.0f);

	// heightColliders is the cylinder's center z-coordinate.
	//PDim.heightCollider = 

	// The head is an auxiliary sphere that is checked for collisions with objects above the cylinder. Head collisions
	// don't affect movement but they make the camera position go down. headRadius is the radius of this sphere and
	// headHeight is the z-coordinate of its center in the topmost state (i.e. when it doesn't touch anything).
	// NOTE: Docs seem out of date at this point.
	//PDim.headRadius = xxx;

	// #TODO: grab player dimensions based on the stance they are in. I presume this needs updating when their stance
	// changes. It might be worth making a function to fill a ref to a dim for the character.

	// Player Dimensions
	//if (stance != STANCE_NULL)
	//{
	//	const SStanceInfo *sInfo = GetStanceInfo (stance);
	//	PDim.bUseCapsule = sInfo->useCapsule;
	//	PDim.heightCollider = sInfo->heightCollider;
	//	PDim.sizeCollider = sInfo->size;
	//	PDim.heightPivot = sInfo->heightPivot;
	//	PDim.maxUnproj = max (0.0f, sInfo->heightPivot);
	//}

	// We are going to be physicalizing this instance, so setup the physics simulation parameters to use for physics calculations.
	pe_player_dynamics PDyn;

	// Setting bActive to false puts the living entity to a special 'inactive' state where it does not check collisions
	// with the environment and only moves with the requested velocity (other entities can still collide with it,
	// though; note that this applies only to the entities of the same or higher simulation classes). 
	PDyn.bActive = true;

	// #TODO: See why rigid is recommended for this.
	PDyn.type = PE_RIGID;

	// A flag that indicates if the entity is allowed to attempt to move in all directions (gravity might still pull it
	// down though). If not set, the requested velocity will always be projected on the ground if the entity is not
	// flying. 
	PDyn.bSwimming = false;

	// We should collide with all entity types and areas/triggers.
	PDyn.collTypes = ent_all | ent_areas | ent_triggers;

	// We should use earth's normal gravity on this instance (the player).
	PDyn.gravity = Vec3(0.0f, 0.0f, -9.81f);

	// We should have almost all control while in the air.
	PDyn.kAirControl = 0.9f;

	// We should have relatively low air resistance.
	PDyn.kAirResistance = 0.2f;

	// We should have a standard amount of inertia.
	PDyn.kInertia = 8.0f;

	// We should have a standard amount of acceleration.
	PDyn.kInertiaAccel = 11.0f;

	// We should weigh x kilograms.
	PDyn.mass = 88.0f;

	// We should not be able to climb up hills that are steeper then 50 degrees.
	PDyn.maxClimbAngle = 50;

	// We should not be able to jump down-hill while on hills that are steeper then 50 degrees.
	PDyn.maxJumpAngle = 50;

	// We should start falling down-hill if we are on hills steeper then 50 degrees.
	PDyn.minFallAngle = 50;

	// We should start sliding down-hill if we are on hills steeper then 45 degrees.
	PDyn.minSlideAngle = DEG2RAD(45);

	// Player cannot stand on ground moving faster than this.
	PDyn.maxVelGround = 16.0f;

	// Sets the strength of camera reaction to landings.
	PDyn.nodSpeed = 60.0f;

	// Makes the entity allocate a much longer movement history array which might be required for synchronization (if
	// not set, this array will be allocated the first time network-related actions are requested, such as performing a
	// step back).
	PDyn.bNetwork = true;

	// Setup the physics parameters this instance (the player) should use.
	SEntityPhysicalizeParams PhysParams;

	// Animated character will need the poststep.
	PhysParams.nFlagsOR = pef_log_poststep;

	// We are not using density, we are using mass.
	PhysParams.density = -1;

	// We should weigh x kilograms. (doesn't matter because this value will come from the pe_player_dynamics).
	PhysParams.mass = PDyn.mass;

	// All entity slots in this instance should be physicalized using these settings.
	PhysParams.nSlot = -1;

	// Sets the player dimensions.
	PhysParams.pPlayerDimensions = &PDim;

	// Sets the player dynamics.
	PhysParams.pPlayerDynamics = &PDyn;

	// We are going to use "living" physics type, hence the use of pe_player_dimensions and pe_player_dynamics.
	PhysParams.type = PE_LIVING;

	// We should use standard human joint springiness.
	PhysParams.fStiffnessScale = 73.0f;

	// Actually physicalize this instance (the player) with the specified parameters.
	GetEntity()->Physicalize(PhysParams);

	// NOTE: This function isn't super useful now I've removed the change it makes to time impulse recover. Have to
	// find out what it was for.
	if (m_pAnimatedCharacter)
	{
		SAnimatedCharacterParams params = m_pAnimatedCharacter->GetParams();
		//params.timeImpulseRecover = GetTimeImpulseRecover(); // #TODO: ILH figure out if this was useful.
		m_pAnimatedCharacter->SetParams(params);
		m_pAnimatedCharacter->ResetInertiaCache();
	}

	return GetEntity()->GetPhysics() != nullptr;
}


// ***
// *** IActorEventListener
// ***

void CActor::OnSpecialMove(IActor* pActor, IActorEventListener::ESpecialMove move)
{}


void CActor::OnDeath(IActor* pActor, bool bIsGod)
{
}


void CActor::OnRevive(IActor* pActor, bool bIsGod)
{
}


void CActor::OnEnterVehicle(IActor* pActor, const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson)
{}


void CActor::OnExitVehicle(IActor* pActor)
{}


void CActor::OnHealthChanged(IActor* pActor, float newHealth)
{}


void CActor::OnItemPickedUp(IActor* pActor, EntityId itemId)
{}


void CActor::OnItemUsed(IActor* pActor, EntityId itemId)
{}


void CActor::OnItemDropped(IActor* pActor, EntityId itemId)
{}


void CActor::OnStanceChanged(IActor* pActor, EStance stance)
{}


void CActor::OnToggleThirdPerson(IActor* pActor, bool isThirdPerson)
{
	// Track first / third person for each actor.
	m_isThirdPerson = isThirdPerson;

	// If we switched view modes, we may need to load a new animation context.
	OnResetState();
}


void CActor::OnSprintStaminaChanged(IActor* pActor, float newStamina)
{}


// ***
// *** CActor
// ***


EntityId CActor::GetCurrentItemId(bool includeVehicle) const
{
	// #TODO: Add handling of vehicles in this routine.

	// Let the inventory extension handle the hard work.
	return m_pInventory ? m_pInventory->GetCurrentItem() : INVALID_ENTITYID;
}


IItem* CActor::GetCurrentItem(bool includeVehicle) const
{
	if (EntityId itemId = GetCurrentItemId(includeVehicle))
		return gEnv->pGameFramework->GetIItemSystem()->GetItem(itemId);

	return nullptr;
}


// ***
// *** AI / Player Control
// ***


void CActor::OnPlayerAttach(CPlayer& player)
{
	// Make a note of the player for back reference.
	m_pAttachedPlayer = &player;

	// Default assumption is we now control the character.
	m_isAIControlled = false;

	// We sdhould reset the animations.
	OnResetState();
}


void CActor::OnPlayerDetach()
{
	m_pAttachedPlayer = nullptr;

	// #TODO: Detach the camera.

	// #TODO: We can remove the entity awareness component if that's desirable.

	// #TODO: handle transitioning this character back into the loving hands of the AI.
	m_isAIControlled = true;
}


// ***
// *** Life-cycle
// ***


void CActor::OnResetState()
{
	auto pEntity = GetEntity();

	// Reset AnimatedCharacter
	if (m_pAnimatedCharacter)
	{
		m_pAnimatedCharacter->ResetState();
		m_pAnimatedCharacter->Init(GetGameObject());
		m_pAnimatedCharacter->SetMovementControlMethods(eMCMSlot_Animation, eMCM_Animation, eMCM_Animation);

		if (IActionController* pActionController = m_pAnimatedCharacter->GetActionController())
		{
			pActionController->GetContext().state.Clear();
		}
	}

	if (!m_geometry.IsEmpty())
	{
		// Load character
		pEntity->LoadCharacter(0, m_geometry);

		// Mannequin Initialization
		IMannequin& mannequin = gEnv->pGameFramework->GetMannequinInterface();
		IAnimationDatabaseManager& animationDatabaseManager = mannequin.GetAnimationDatabaseManager();

		// Loading the controller definition that we previously created.
		// This is owned by the animation database manager
		const SControllerDef* const pControllerDef = animationDatabaseManager.LoadControllerDef(MANNEQUIN_FOLDER + m_controllerDefinition);
		if (pControllerDef == nullptr)
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load controller definition for actor.");
			return;
		}

		// Creation of the animation context and action controller. Don't re-order these lines.
		SAFE_RELEASE(m_pActionController);
		SAFE_DELETE(m_pAnimationContext);
		m_pAnimationContext = new SAnimationContext(*pControllerDef);
		m_pActionController = mannequin.CreateActionController(pEntity, *m_pAnimationContext);

		// Scope Context Setup. In our controller definition we have a scope context that we called MainCharacter. The Scope
		// Context Setup will associate this entity, the character instance we loaded at the beginning, and the animation
		// database where we saved our fragments to this scope context.
		//const TagID scopeContextId = m_pAnimationContext->controllerDef.m_scopeContexts.Find(m_scopeContext);

		// HACK: Switching first / third person view modes should also switch animation contexts (I think). Hard coding the
		// contexts is really bad form, but it useful for testing. Look into having a more flexible way of determining the name
		// of the scope contexts.
		TagID scopeContextId;
		if (IsThirdPerson())
			scopeContextId = m_pAnimationContext->controllerDef.m_scopeContexts.Find("Char3P");
		else
			scopeContextId = m_pAnimationContext->controllerDef.m_scopeContexts.Find("Char1P");

		// Let them know we don't support that scope context.
		if (scopeContextId == TAG_ID_INVALID)
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to find %s scope context id for MannequinSample in controller definition.", m_scopeContext);
			return;
		}

		ICharacterInstance* const pCharacterInstance = pEntity->GetCharacter(0);
		CRY_ASSERT(pCharacterInstance != nullptr);

		// Loading a database
		const IAnimationDatabase* const pAnimationDatabase = animationDatabaseManager.Load(MANNEQUIN_FOLDER + m_animationDatabase);
		if (pAnimationDatabase == nullptr)
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load animation database for MannequinSample.");
			return;
		}

		// Set up the mannequin fragments and tags.
		if (m_pActionController)
		{
			// Setting Scope contexts can happen at any time, and what entity or character instance we have bound to a
			// particular scope context can change during the lifetime of an action controller. 
			if (pCharacterInstance)
			{
				m_pActionController->SetScopeContext(scopeContextId, *pEntity, pCharacterInstance, pAnimationDatabase);
			}

			// Any mannequin parameters which are needed in code where an action controller is not available should register
			// themselves here. 
			auto& mannequinUserParams = gEnv->pGameFramework->GetMannequinInterface().GetMannequinUserParamsManager();
			mannequinUserParams.RegisterParams<SActorMannequinParams>(m_pActionController, &g_actorMannequinParams);
			mannequinUserParams.RegisterParams<SMannequinEmoteParams>(m_pActionController, &g_emoteMannequinParams);
			
		}

		// Invalidates this instance's transformation matrix in order to force an update of the area manager and other
		// location sensitive systems. 
		pEntity->InvalidateTM(ENTITY_XFORM_POS);

		// The assumption is I need to physicalise on each change - though this might not be true.
		// #TODO: check if this has to happen every reset. 
		Physicalize();

		// Reset the HSM for character movement.
		MovementHSMReset();

		// Select which HSM to use for our character's movement. This relies on it's AI status being
		// correctly set first.
		SelectMovementHierarchy();

		// Mannequin should also be reset.
		ResetMannequin();

		//SetLastTimeInLedge (0.0f);
		//SetupAimIKProperties ();
		//DisableStumbling ();
		//m_playerStateSwim_WaterTestProxy.Reset (true);
		//GetSpectacularKill ().Reset ();
	}
}


void CActor::Kill()
{
	// #TODO: A *LOT* of code needs to be added here to handle reviving.
}


void CActor::Revive(EReasonForRevive reasonForRevive)
{
	// #TODO: A *LOT* of code needs to be added here to handle reviving.

	// Reset the HSM for character movement.
	MovementHSMReset();

	// Select which HSM to use for our character's movement. This relies on it's AI status being
	// correctly set first.
	SelectMovementHierarchy();

	// Mannequin should be reset.
	ResetMannequin();
}


// TODO: Is this really needed? Perhaps there's a better way to handle it. Revive isn't getting called at present
// so there must be a better place for this.

void CActor::ResetMannequin()
{
	if (m_pActionController)
	{
		//if (IsPlayer() && !IsAIControlled())
		//{
		//	m_pActionController->Resume();

		//	SAnimationContext &animContext = m_pActionController->GetContext();
		//	animContext.state.Set(g_actorMannequinParams.tagIDs.localClient, IsClient());
		//	animContext.state.SetGroup(g_actorMannequinParams.tagGroupIDs.playMode, gEnv->bMultiplayer ? g_actorMannequinParams.tagIDs.MP : g_actorMannequinParams.tagIDs.SP);
		//	animContext.state.Set(g_actorMannequinParams.tagIDs.FP, !IsThirdPerson());

		//	SetStanceTag(GetStance(), animContext.state);

		//	// Install persistent AimPose action
		//	m_pActionController->Queue(*new CPlayerBackgroundAction(EActorActionPriority::eAAP_Movement, g_actorMannequinParams.fragmentIDs.AimPose));

		//	// Install persistent WeaponPose action
		//	m_pActionController->Queue(*new CPlayerBackgroundAction(EActorActionPriority::eAAP_Lowest, g_actorMannequinParams.fragmentIDs.WeaponPose));

		//	CActionItemIdle *itemIdle = new CActionItemIdle(EActorActionPriority::eAAP_Lowest, g_actorMannequinParams.fragmentIDs.Idle, g_actorMannequinParams.fragmentIDs.IdleBreak, TAG_STATE_EMPTY, *this);
		//	m_pActionController->Queue(*itemIdle);

		//    CPlayerMovementAction *movementAction = new CPlayerMovementAction(EActorActionPriority::eAAP_Movement);
		//    m_pActionController->Queue(*movementAction);

		// Locomotion action.
		auto locomotionAction = new CActorAnimationActionLocomotion();
		m_pActionController->Queue(*locomotionAction);

		// TODO: Get back to aiming and looking after some interaction is sorted.

		// Aim actions.
		//if (CActorAnimationActionAimPose::IsSupported(m_pActionController->GetContext())
		//	&& CActorAnimationActionAiming::IsSupported(m_pActionController->GetContext()))
		//{
		//	// TODO: I presume these are needed to do things.
		//	//m_pProceduralContextAim = static_cast<CProceduralContextAim*>(m_pActionController->FindOrCreateProceduralContext(PROCEDURAL_CONTEXT_AIM_NAME));

		//	m_pActionController->Queue(*new CActorAnimationActionAimPose());
		//	m_pActionController->Queue(*new CActorAnimationActionAiming());
		//}

		//// Look actions.
		//if (CActorAnimationActionLookPose::IsSupported(m_pActionController->GetContext()) 
		//	&& CActorAnimationActionLooking::IsSupported(m_pActionController->GetContext()))
		//{
		//	// TODO: I presume these are needed to do things.
		//	//m_pProceduralContextLook = static_cast<CProceduralContextLook*>(m_pActionController->FindOrCreateProceduralContext(PROCEDURAL_CONTEXT_LOOK_NAME));

		//	m_pActionController->Queue(*new CActorAnimationActionLookPose());
		//	m_pActionController->Queue(*new CActorAnimationActionLooking());
		//}

		//	m_weaponFPAiming.ResetMannequin();
		//}
	}
}


// ***
// *** Handle interactions with other entities.
// ***


void CActor::OnActionItemUse(EntityId playerId)
{
	CryLogAlways("Player tried to use an item");
}


void CActor::OnActionItemPickup(EntityId playerId)
{
	CryLogAlways("Player picked up an item");
}


void CActor::OnActionItemDrop(EntityId playerId)
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


void CActor::OnActionItemToss(EntityId playerId)
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


void CActor::OnActionBarUse(EntityId playerId, int actionBarId)
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


void CActor::OnActionInspectStart(EntityId playerId)
{
	CryLogAlways("Player started inspecting things.");
}


void CActor::OnActionInspect(EntityId playerId)
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


void CActor::OnActionInspectEnd(EntityId playerId)
{
	CryLogAlways("Player stopped inspecting things.");
}


void CActor::OnActionInteractionStart(EntityId playerId)
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
			auto emoteAction = new CActorAnimationActionEmote(g_emoteMannequinParams.tagIDs.Awe);
			m_pActionController->Queue(*emoteAction);

			if (auto pInteractor = pInteractionEntity->GetComponent<CEntityInteractionComponent>())
			{
				// There's an interactor component, so this is an interactive entity.
				// #TODO: We should really only process an 'interact' verb - not simply the first entry.
				auto verbs = pInteractor->GetVerbs();
				if (verbs.size() > 0)
				{
					auto verb = verbs [0];

					// #HACK: Another test - just calling the interaction directly instead.
					//auto pInteraction = pInteractor->GetInteraction(verb)._Get();
					//pInteraction->OnInteractionStart();

					// HACK: Doesn't belong here, test to see if we can queue an interaction action.
					//auto action = new CActorAnimationActionInteraction();
					//m_pActionController->Queue(*action);
				}
			}
		}
	}
}


void CActor::OnActionInteraction(EntityId playerId)
{
}


void CActor::OnActionInteractionEnd(EntityId playerId)
{
	CryLogAlways("Player stopped interacting with things.");
}


void CActor::OnActionJogToggle(EntityId playerId)
{
	m_isJogging = !m_isJogging;
	CryLogAlways("Player toggled walking / jogging");
}


void CActor::OnActionSprintStart(EntityId playerId)
{
	m_isSprinting = true;
	CryLogAlways("Player started sprinting");
}


void CActor::OnActionSprintStop(EntityId playerId)
{
	m_isSprinting = false;
	CryLogAlways("Player stopped sprinting");
}


// TODO: So wrong in every way. Need some more state checking before deciding which stance to move to when exiting
// crouching, though this is a pretty decent default. Needs checks to ensure you can't crouch while swimming / falling /
// etc - maybe put this into the class itself? 

void CActor::OnActionCrouchToggle(EntityId playerId)
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


void CActor::OnActionCrawlToggle(EntityId playerId)
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


void CActor::OnActionKneelToggle(EntityId playerId)
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


void CActor::OnActionSitToggle(EntityId playerId)
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


float CActor::GetMovementBaseSpeed(uint32 movementStateFlags) const
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
	switch (movementStateFlags)
	{
		case EMovementStateFlags::Forward:
			dirScale = 1.0f;
			break;

		case (EMovementStateFlags::Forward | EMovementStateFlags::Right):
			dirScale = 0.9f;
			break;

		case (EMovementStateFlags::Forward | EMovementStateFlags::Left):
			dirScale = 0.9f;
			break;

		case EMovementStateFlags::Right:
			dirScale = 0.85f;
			break;

		case EMovementStateFlags::Left:
			dirScale = 0.85f;
			break;

		case EMovementStateFlags::Backward:
			dirScale = 0.71f;
			break;

		case (EMovementStateFlags::Backward | EMovementStateFlags::Right):
			dirScale = 0.71f;
			break;

		case (EMovementStateFlags::Backward | EMovementStateFlags::Left):
			dirScale = 0.71f;
			break;

		default:
			dirScale = 1.0f;
			break;
	}

	return baseSpeed * dirScale;
}
