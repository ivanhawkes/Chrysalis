#include <StdAfx.h>

#include "Character.h"
#include <CryMath/Cry_Math.h>
#include <CryCore/Assert/CryAssert.h>
#include <IViewSystem.h>
#include <ICryMannequin.h>
#include <IAnimatedCharacter.h>
#include <IGameObject.h>
#include <IItemSystem.h>
#include <EntitySensing/IEntityLocking.h>
#include <EntitySensing/IEntityAwareness.h>
#include <Game/Game.h>
#include <Actor/ActorClassTypes.h>
#include <Actor/Player/Player.h>
#include <Actor/Character/Movement/CharacterMovementController.h>
#include <StateMachine/StateMachine.h>
#include <Actor/Character/Movement/StateMachine/CharacterStateEvents.h>
#include <Actor/Character/Movement/StateMachine/CharacterStateUtil.h>
#include <Actor/ActorPhysics.h>
#include <Entity/EntityScriptCalls.h>


SActorStance CCharacter::m_defaultStance;


// Definition of the state machine that controls character movement.
DEFINE_STATE_MACHINE(CCharacter, Movement);


CCharacter::CCharacter()
{
	m_characterRotation = new CCharacterRotation(*this);
}


CCharacter::~CCharacter()
{
	// Very important that this gets called. Removes the character from the system.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->RemoveActor(GetEntityId());

	// Release any extensions we grabbed.
	if (m_pAnimatedCharacter)
	{
		GetGameObject()->ReleaseExtension("AnimatedCharacter");
		GetGameObject()->DeactivateExtension("AnimatedCharacter");
	}
	if (m_pInteractor)
		GetGameObject()->ReleaseExtension("EntityLocking");

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

		GetGameObject()->ReleaseExtension("Inventory");
	}

	// Release the movement state machine.
	StateMachineReleaseMovement();

	// Release the controller first, then the animation context. Errors will occur if done the other way.
	SAFE_DELETE(m_pMovementController);
	SAFE_RELEASE(m_pActionController);
	SAFE_DELETE(m_pAnimationContext);

	// We're over worrying about rotation.
	SAFE_DELETE(m_characterRotation);
}


// ***
// *** IGameObjectExtension
// ***


void CCharacter::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);

	// Remember to add large memory objects in as needed.
	pSizer->AddObject(m_pInventory);
	pSizer->AddObject(m_pAnimatedCharacter);
	pSizer->AddObject(m_pInteractor);
	//pSizer->AddObject(m_pMovementController); // TODO: should include size of this object, but thanks to time pressure don't have time to figure out the correct way.
}


bool CCharacter::Init(IGameObject * pGameObject)
{
	// Stores the specified IGameObject in this instance.
	SetGameObject(pGameObject);

	// Initialise the movement state machine.
	StateMachineInitMovement();

	// Create a movement controller and set it as the active controller for this game object.
	// Attempt to acquire an animated character component.
	m_pMovementController = new CCharacterMovementController(this);
	if (m_pMovementController)
		GetGameObject()->SetMovementController(m_pMovementController);

	// Stores whether this instance is the client actor.
	m_bClient = (((CGame*) gEnv->pGame)->GetClientActorID() == GetEntityId());

	IEntity *pEntity = GetEntity();
	IEntityClass *pEntityClass = pEntity->GetClass();

	// Registers this instance to the actor system.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->AddActor(GetEntityId(), this);

	// Add this instance to the network.
	if (!pGameObject->BindToNetwork())
		return false;// Failed to add this instance to the network.

	// Default is for a character to be controlled by AI.
	//	m_isAIControlled = true;
	m_isAIControlled = false;

	// Select which HSM to use for our character's movement. This relies on it's AI status being
	// correctly set first.
	SelectMovementHierarchy();

	// Jump start the movement state machine.
	//if (m_bPlayIntro && IsClient ())
	//	StateMachineHandleEventMovement (CHARACTER_EVENT_INTRO_START);
	//else
	//	StateMachineHandleEventMovement (CHARACTER_EVENT_INTRO_FINISHED);
	// TODO: check client / play intro - maybe not the client check though.
	//StateMachineHandleEventMovement (CHARACTER_EVENT_INTRO_START);

	return true;
}


void CCharacter::PostInit(IGameObject * pGameObject)
{
	// Register for game object events.
	RegisterEvents();

	// Register for pre-physics update.
	RegisterEvent(ENTITY_EVENT_PREPHYSICSUPDATE, IComponent::EComponentFlags_Enable);

	// We will require pre-physics updates.
	pGameObject->EnablePrePhysicsUpdate(ePPU_Always);

	// Animated characters need to register for the post step immediate event.
	pGameObject->EnablePhysicsEvent(true, eEPE_OnPostStepImmediate);

	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);

	// Attempt to acquire an animated character component.
	m_pAnimatedCharacter = static_cast<IAnimatedCharacter*> (GetGameObject()->AcquireExtension("AnimatedCharacter"));
	if (m_pAnimatedCharacter)
	{
		// TODO: animated character stuff.
	}

	// Attempt to acquire an inventory component.
	m_pInventory = static_cast<IInventory*>(GetGameObject()->AcquireExtension("Inventory"));
	if (m_pInventory)
	{
		// TODO: set up the inventory component we acquired.
	}

	// Get it into a known state.
	Reset();

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

	// Load character
	IEntity* const pEntity = GetEntity();
	if (IScriptTable* pScriptTable = pEntity->GetScriptTable())
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			const char* modelName = NULL;
			if (propertiesTable->GetValue("objModel", modelName) && modelName)
			{
				pEntity->LoadCharacter(0, modelName);
			}
		}
	}

	// Mannequin Initialization
	IMannequin& mannequin = gEnv->pGame->GetIGameFramework()->GetMannequinInterface();
	IAnimationDatabaseManager& animationDatabaseManager = mannequin.GetAnimationDatabaseManager();

	// Loading the controller definition that we previously created.
	// This is owned by the animation database manager
	//const SControllerDef* const pControllerDef = animationDatabaseManager.LoadControllerDef("Animations/Mannequin/ADB/sampleControllerDefs.xml");
	//const SControllerDef* const pControllerDef = animationDatabaseManager.LoadControllerDef("Animations/Mannequin/ADB/PlayerControllerDefs.xml");
	const SControllerDef* const pControllerDef = animationDatabaseManager.LoadControllerDef("Animations/Mannequin/ADB/sdk_tutorial3controllerdefs.xml");
	if (pControllerDef == NULL)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load controller definition for MannequinSample.");
		return;
	}

	// Creation of the animation context.
	CRY_ASSERT(m_pAnimationContext == NULL);
	m_pAnimationContext = new SAnimationContext(*pControllerDef);

	// Creation of the action controller.
	CRY_ASSERT(m_pActionController == NULL);
	m_pActionController = mannequin.CreateActionController(pEntity, *m_pAnimationContext);

	// Scope Context Setup. In our controller definition we have a scope context that we called MainCharacter. The Scope
	// Context Setup will associate this entity, the character instance we loaded at the beginning, and the animation
	// database where we saved our fragments to this scope context.
	const TagID scopeContextId = m_pAnimationContext->controllerDef.m_scopeContexts.Find("MainCharacter");
	//const TagID scopeContextId = m_pAnimationContext->controllerDef.m_scopeContexts.Find("Char3P");
	if (scopeContextId == TAG_ID_INVALID)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to find MainCharacter scope context id for MannequinSample in controller definition.");
		return;
	}

	ICharacterInstance* const pCharacterInstance = pEntity->GetCharacter(0);
	CRY_ASSERT(pCharacterInstance != NULL);

	// Loading a database
	//const IAnimationDatabase* const pAnimationDatabase = animationDatabaseManager.Load("Animations/Mannequin/ADB/sampleDatabase.adb");
	//const IAnimationDatabase* const pAnimationDatabase = animationDatabaseManager.Load("Animations/Mannequin/ADB/sampleDatabase.adb");
	const IAnimationDatabase* const pAnimationDatabase = animationDatabaseManager.Load("Animations/Mannequin/ADB/sdk_tutorial3database.adb");
	//const IAnimationDatabase* const pAnimationDatabase = animationDatabaseManager.Load("Animations/Mannequin/ADB/Human.adb");
	//const IAnimationDatabase* const pAnimationDatabase = animationDatabaseManager.Load("animations/Mannequin/ADB/playerControllerDefs.xml");
	if (pAnimationDatabase == NULL)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load animation database for MannequinSample.");
		return;
	}

	// Setting Scope contexts can happen at any time, and what entity or character instance we have bound to a particular scope context
	// can change during the lifetime of an action controller.
	m_pActionController->SetScopeContext(scopeContextId, *pEntity, pCharacterInstance, pAnimationDatabase);

	// Start the idle fragment.
	//const FragmentID idleFragmentId = m_pAnimationContext->controllerDef.m_fragmentIDs.Find("Idle");
	const FragmentID idleFragmentId = m_pAnimationContext->controllerDef.m_fragmentIDs.Find("Move");
	//const FragmentID idleFragmentId = m_pAnimationContext->controllerDef.m_fragmentIDs.Find ("MotionMovement");

	// Try and set a crouch state for testing.
	//m_pAnimationContext->state.Set (tagId, true);

	IActionPtr pAction = new TAction< SAnimationContext >(0, idleFragmentId);
	m_pActionController->Queue(*pAction);
	//Tag "Relaxed+Walk"

	// If we are the client actor than notify the game of us being spawned.
	if (m_bClient)
		((CGame*) gEnv->pGame)->OnClientActorSpawned(this);

	// TODO: test code for now - want to get physics working on characters. This was being done in code
	// when they became the local player, but that won't work for us.
	// You will get an assert like the following:
	// Some other code changed the inertia on this living entity, every inertia change for living entities should go through the animated character params!
	// Ignore that for now, it's because of the way we're hacking the movement in presently.

	// Physicalize this instance.
	// NOTE: Handling in OnReset now.
	//if (!Physicalize())
	//	gEnv->pLog->LogWarning("CCharacter::PostInit(): Failed to physicalize the entity!");

	// Tells this instance to trigger areas.
	pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS);

	// Invalidates this instance's transformation matrix in order to force an update of the area manager and other location sensitive systems.
	pEntity->InvalidateTM(ENTITY_XFORM_POS);

	// Since we are the client always update this instance's character.
	if (ICharacterInstance * pCharacter = pEntity->GetCharacter(0))
		pCharacter->SetFlags(pCharacter->GetFlags() | CS_FLAG_UPDATE_ALWAYS);

	// Reset the entity.
	OnReset();
}


void CCharacter::InitClient(int channelId)
{}


void CCharacter::PostInitClient(int channelId)
{}


bool CCharacter::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// Restores this instance's game object in case it has changed.
	ResetGameObject();

	// Add this instance to the network.
	if (!GetGameObject()->BindToNetwork())
		return false;

	// Removes this instance's previous actor and adds it's new one.
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->RemoveActor(params.prevId);
	gEnv->pGame->GetIGameFramework()->GetIActorSystem()->AddActor(GetEntityId(), this);

	// Register for game object events again.
	RegisterEvents();

	return true;
}


void CCharacter::PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// Ensure the movement controller is set and reset any current movements on it.
	pGameObject->SetMovementController(m_pMovementController);
	m_pMovementController->Reset();
}


bool CCharacter::GetEntityPoolSignature(TSerialize signature)
{
	return true;
}


void CCharacter::Release()
{
	// Destroy this instance.
	delete this;
}


void CCharacter::FullSerialize(TSerialize ser)
{
	// Serialise the movement state machine.
	StateMachineSerializeMovement(SStateEventSerialize(ser));
}


bool CCharacter::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags)
{
	return true;
}


ISerializableInfoPtr CCharacter::GetSpawnInfo()
{
	return nullptr;
}


void CCharacter::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	// Process movement and rotation.
	ProcessMovement(ctx.fFrameTime);
	ProcessRotation(ctx.fFrameTime);

	if (m_pActionController)
		m_pActionController->Update(ctx.fFrameTime);

	// HACK: Process this in player instead of here which will affect ALL characters.
	// Alternatively, and perhaps safer due to uncertain entity sorting, place a check around the character to ensure it's the
	// player's current attach target.
	if (m_pMovementController)
		m_pMovementController->Compute();

	//#ifdef STATE_DEBUG
	//	const bool shouldDebug = (s_StateMachineDebugEntityID == GetEntityId ());
	//#else
	//	const bool shouldDebug = false;
	//#endif
	const bool shouldDebug = false;
	//const bool shouldDebug = true;

	// Update the movement state machine.
	StateMachineUpdateMovement(ctx.fFrameTime, shouldDebug);

	// Pass the update into the movement state machine.
	// TODO: make this happen.
	StateMachineHandleEventMovement(SStateEventUpdate(ctx.fFrameTime));
}


void CCharacter::HandleEvent(const SGameObjectEvent& event)
{
	bool bHandled = false;

	switch (event.event)
	{
		// Called automatically by various systems when this instance should become the client actor.
		// TODO: Not getting called yet, have to figure out how to trigger this.
		case eGFE_BecomeLocalPlayer:
		{
			// Tells this instance to trigger areas and that it's the local player.
			auto pEntity = GetEntity();
			pEntity->AddFlags(ENTITY_FLAG_TRIGGER_AREAS | ENTITY_FLAG_LOCAL_PLAYER);

			//// Invalidates this instance's transformation matrix in order to force an update of the area manager and other location sensitive systems.
			// NOTE: We may need to do something like this to make player related things trigger when jumping from one
			// character to another.
			//pEntity->InvalidateTM(ENTITY_XFORM_POS);

			//// Since we are the client always update this instance's character.
			//if (ICharacterInstance * pCharacter = pEntity->GetCharacter(0))
			//	pCharacter->SetFlags(pCharacter->GetFlags() | CS_FLAG_UPDATE_ALWAYS);

			gEnv->pLog->LogAlways("CCharacter::HandleEvent(): Entity \"%s\" became the local character!", pEntity->GetName());
		}
		break;

		default:
			// TODO: add HitDeathReactions event handling.
			break;
	}
}


void CCharacter::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		// Called automatically at the start of every level.
		case ENTITY_EVENT_START_LEVEL:
			break;

		case ENTITY_EVENT_PREPHYSICSUPDATE:
			PrePhysicsUpdate();
			break;

		case ENTITY_EVENT_SCRIPT_EVENT:
			OnScriptEvent(event);
			break;

		case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
			OnEditorPropertyChanged();
			break;

		case ENTITY_EVENT_RESET:
			OnReset();
			break;

		default:
			break;
	}
}


IComponent::ComponentEventPriority CCharacter::GetEventPriority(const int eventID) const
{
	switch (eventID)
	{
		case ENTITY_EVENT_PREPHYSICSUPDATE:
//			return(ENTITY_PROXY_LAST - ENTITY_PROXY_USER + EEntityEventPriority_Actor + (m_isClient ? EEntityEventPriority_Client : 0));
			return ENTITY_PROXY_LAST - ENTITY_PROXY_USER + EEntityEventPriority_Actor + EEntityEventPriority_Client; // HACK: only used for when we are the client, fix later.
	}

	return IGameObjectExtension::GetEventPriority(eventID);
}


void CCharacter::UpdateAnimationState(const SActorMovementRequest& movementRequest)
{
/*	// TODO: get this working.

	// Update variable scope contexts.
	CWeapon *pWeapon = GetWeapon(GetCurrentItemId());
	ICharacterInstance *pICharInst = pWeapon ? pWeapon->GetEntity()->GetCharacter(0) : NULL;
	IActionController *pActionController = GetAnimatedCharacter()->GetActionController();
	IMannequin &mannequinSys = gEnv->pGame->GetIGameFramework()->GetMannequinInterface();

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

		if (!animContext.state.GetDef().IsGroupSet(animContext.state.GetMask(), PlayerMannequin.tagGroupIDs.item))
		{
			animContext.state.Set(PlayerMannequin.tagIDs.nw, true);
			animContext.state.SetGroup(PlayerMannequin.tagGroupIDs.zoom, TAG_ID_INVALID);
			animContext.state.SetGroup(PlayerMannequin.tagGroupIDs.firemode, TAG_ID_INVALID);
		}

		animContext.state.Set(PlayerMannequin.tagIDs.outOfAmmo, isOutOfAmmo);
		const bool aimEnabled = !IsSprinting() || (pWeapon && pWeapon->IsReloading());
		animContext.state.Set(PlayerMannequin.tagIDs.aiming, (movement.aimIK || m_isPlayer) && aimEnabled);

		const Vec3 referenceVel = GetActorPhysics().velocity;
		float speedXY = referenceVel.GetLength2D();

		//		CryWatch("Vel (%f, %f, %f)", m_stats.velocity.x, m_stats.velocity.y, m_stats.velocity.z);
		//		CryWatch("LastRequestedVel (%f, %f, %f)", m_lastRequestedVelocity.x, m_lastRequestedVelocity.y, m_lastRequestedVelocity.z);
		//		CryWatch("DesVel (%f, %f, %f)", desiredVelocity.x, desiredVelocity.y, desiredVelocity.z);

		TagID movementTag = IsSprinting() ? PlayerMannequin.tagIDs.sprint : TAG_ID_INVALID;
		TagID moveDir = TAG_ID_INVALID;
		if (speedXY > 0.5f)
		{
			Vec3 velXY;
			velXY.Set(referenceVel.x / speedXY, referenceVel.y / speedXY, 0.0f);

			const float signedAngle = Ang3::CreateRadZ(velXY, GetEntity()->GetForwardDir());
			const float unsignedAngle = fabs_tpl(signedAngle);
			if (unsignedAngle < gf_PI*0.25f)
			{
				moveDir = PlayerMannequin.tagIDs.forward;
			}
			else if (unsignedAngle > gf_PI*0.75f)
			{
				moveDir = PlayerMannequin.tagIDs.backward;
			}
			else if (signedAngle > 0.0f)
			{
				moveDir = PlayerMannequin.tagIDs.right;
			}
			else
			{
				moveDir = PlayerMannequin.tagIDs.left;
			}
		}

		animContext.state.SetGroup(PlayerMannequin.tagGroupIDs.moveDir, moveDir);
		animContext.state.SetGroup(PlayerMannequin.tagGroupIDs.moveSpeed, movementTag);
	}*/
}


void CCharacter::PrePhysicsUpdate()
{
	//
	// TODO: I have to re-write this section now I'm moving the movement controller. Surely it can be written better than this.
	// 


	const float frameTime = gEnv->pTimer->GetFrameTime();

	if (m_pMovementController)
	{
		SActorMovementRequest movementRequest;

		// Update the movement controller.
		m_pMovementController->UpdateActorMovementRequest(movementRequest, frameTime);

		// They requested a stance change.
		//if (movementRequest.stance != STANCE_NULL)
		//{
		//	SetStance (movementRequest.stance);
		//}

		// Request the movement from the movement controller.
		// NOTE: this one here only cleared the look target previously...searching for other calls to movementrequest.
		//CMovementRequest movementRequest;
		//m_pMovementController->RequestMovement (movementRequest);

		// TODO: process aim?

		
		// TODO: there used to be code for the player to process turning here. I think it might be better handled more abstractly
		// along with other factors like movement. Sample is provided below.
		// SPlayerRotationParams::EAimType aimType = GetCurrentAimType();
		//m_pPlayerRotation->Process(pCurrentItem, movementRequest,
		//	m_playerRotationParams.m_verticalAims [IsThirdPerson() ? 1 : 0] [aimType],
		//	frameTime);

		m_characterRotation->Process(movementRequest, frameTime);



		// TODO: call to save data to recording system?

		CCharacterStateUtil::UpdateCharacterPhysicsStats(*this, m_actorPhysics, frameTime);

		//#ifdef STATE_DEBUG
		//		if (g_pGameCVars->pl_watchPlayerState >= (bIsClient ? 1 : 2))
		//		{
		//			// NOTE: outputting this info here is 'was happened last frame' not 'what was decided this frame' as it occurs before the prePhysicsEvent is dispatched
		//			// also IsOnGround and IsInAir can possibly both be false e.g. - if you're swimming
		//			// May be able to remove this log now the new HSM debugging is in if it offers the same/improved functionality
		//			CryWatch("%s stance=%s flyMode=%d %s %s%s%s%s", GetEntity()->GetEntityTextDescription(), GetStanceName(GetStance()), m_stats.flyMode, IsOnGround() ? "ON-GROUND" : "IN-AIR", IsThirdPerson() ? "THIRD-PERSON" : "FIRST-PERSON", IsDead() ? "DEAD" : "ALIVE", m_stats.isScoped ? " SCOPED" : "", m_stats.isInBlendRagdoll ? " FALLNPLAY" : "");
		//		}
		//#endif

		// Push the pre-physics event down to our state machine.
		const SCharacterPrePhysicsData prePhysicsData(frameTime, movementRequest);
		const SStateEventCharacterMovementPrePhysics prePhysicsEvent(&prePhysicsData);
		StateMachineHandleEventMovement(STATE_DEBUG_APPEND_EVENT(prePhysicsEvent));

		CryWatch("%s : velocity = %f, %f, %f\r\n", __func__,
			movementRequest.desiredVelocity.x, movementRequest.desiredVelocity.y, movementRequest.desiredVelocity.z);

		// TODO: does stance change really belong here?
		//if (movementRequest.stance != STANCE_NULL)
		//{
		//	SetStance(movementRequest.stance);
		//}

		// Bring the animation state of the character into line with it's requested state.
		// TODO: Make sure this happens - it'c commented out for now!
		UpdateAnimationState(movementRequest);
	}
}


void CCharacter::SetChannelId(uint16 id)
{}


void CCharacter::SetAuthority(bool auth)
{}


void CCharacter::PostUpdate(float frameTime)
{
}


void CCharacter::PostRemoteSpawn()
{}


// *** 
// *** IActor
// *** 


void CCharacter::SetHealth(float health)
{}


float CCharacter::GetHealth() const
{
	return 100;
}


int	CCharacter::GetHealthAsRoundedPercentage() const
{
	return 100;
}


void CCharacter::SetMaxHealth(float maxHealth)
{}


float CCharacter::GetMaxHealth() const
{
	return 100;
}


int CCharacter::GetArmor() const
{
	return 100;
}


int	CCharacter::GetMaxArmor() const
{
	return 100;
}


bool CCharacter::IsFallen() const
{
	return false;
}


bool CCharacter::IsDead() const
{
	return false;
}


int	CCharacter::IsGod()
{
	return true;
}


void CCharacter::Fall(Vec3 hitPos)
{}


bool CCharacter::AllowLandingBob()
{
	return true;
}


void CCharacter::PlayAction(const char *action, const char *extension, bool looping)
{}


IAnimationGraphState* CCharacter::GetAnimationGraphState()
{
	return nullptr;
}


void CCharacter::ResetAnimationState()
{}


void CCharacter::CreateScriptEvent(const char *event, float value, const char *str)
{}


bool CCharacter::BecomeAggressiveToAgent(EntityId entityID)
{
	return true;
}


void CCharacter::SetFacialAlertnessLevel(int alertness)
{}


void CCharacter::RequestFacialExpression(const char* pExpressionName, f32* sequenceLength)
{}


void CCharacter::PrecacheFacialExpression(const char* pExpressionName)
{}


EntityId CCharacter::GetGrabbedEntityId() const
{
	return -1;
}


void CCharacter::HideAllAttachments(bool isHiding)
{}


void CCharacter::SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority)
{}


void CCharacter::SetViewInVehicle(Quat viewRotation)
{}


void CCharacter::SetViewRotation(const Quat &rotation)
{}


Quat CCharacter::GetViewRotation() const
{
	return GetEntity()->GetWorldRotation();
}


bool CCharacter::IsFriendlyEntity(EntityId entityId, bool bUsingAIIgnoreCharacter) const
{
	return true;
}


Vec3 CCharacter::GetLocalEyePos() const
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
		const auto eyeLeft = pAttachmentManager->GetIndexByName("eye_left");
		const auto eyeRight = pAttachmentManager->GetIndexByName("eye_right");
		Vec3 eyeLeftPosition;
		Vec3 eyeRightPosition;
		int eyeFlags = 0;

		// Is there a left eye?
		if (eyeLeft > 0)
		{
			const IAttachment* pEyeLeftAttachment = pAttachmentManager->GetInterfaceByIndex(eyeLeft);
			if (pEyeLeftAttachment)
			{
				eyeLeftPosition = pEyeLeftAttachment->GetAttModelRelative().t;
				eyeFlags |= 0x01;
			}
		}

		// Is there a right eye?
		if (eyeRight > 0)
		{
			const IAttachment* pEyeRightAttachment = pAttachmentManager->GetInterfaceByIndex(eyeRight);
			if (pEyeRightAttachment)
			{
				eyeRightPosition = pEyeRightAttachment->GetAttModelRelative().t;
				eyeFlags |= 0x02;
			}
		}

		switch (eyeFlags)
		{
			case 0:
				// Failure, didn't find any eyes.
				// This will most likely spam the log. Disable it if it's annoying.
				CryLogAlways("Character class %s does not have either left_eye, right_eye or either.", GetActorClassName());
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


void CCharacter::CameraShake(float angle, float shift, float duration, float frequency, Vec3 pos, int ID, const char* source)
{}


IItem* CCharacter::GetHolsteredItem() const
{
	if (m_pInventory)
	{
		return g_pGame->GetIGameFramework()->GetIItemSystem()->GetItem(m_pInventory->GetHolsteredItem());
	}

	return nullptr;
}


void CCharacter::HolsterItem(bool holster, bool playSelect, float selectSpeedBias, bool hideLeftHandObject)
{}


EntityId CCharacter::GetCurrentItemId(bool includeVehicle) const
{
	// TODO: Add handling of vehicles in this routine.

	// Let the inventory extension handle the hard work.
	return m_pInventory ? m_pInventory->GetCurrentItem() : INVALID_ENTITYID;
}


IItem* CCharacter::GetCurrentItem(bool includeVehicle) const
{
	if (EntityId itemId = GetCurrentItemId(includeVehicle))
		return g_pGame->GetIGameFramework()->GetIItemSystem()->GetItem(itemId);

	return nullptr;
}


bool CCharacter::DropItem(EntityId itemId, float impulseScale, bool selectNext, bool byDeath)
{
	return true;
}


IInventory* CCharacter::GetInventory() const
{
	return m_pInventory;
}


void CCharacter::NotifyCurrentItemChanged(IItem* newItem)
{}


IMovementController* CCharacter::GetMovementController() const
{
	return m_pMovementController;
}


IEntity* CCharacter::LinkToVehicle(EntityId vehicleId)
{
	return nullptr;
}


IEntity* CCharacter::GetLinkedEntity() const
{
	return nullptr;
}


uint8 CCharacter::GetSpectatorMode() const
{
	return 0;
}


bool CCharacter::IsThirdPerson() const
{
	return m_bIsThirdPerson;
}


void CCharacter::ToggleThirdPerson()
{
	m_bIsThirdPerson = !m_bIsThirdPerson;
}


bool CCharacter::IsPlayer() const
{
	//CryLogAlways("[Warning] IsPlayer () tested, possible cast problems if they cast CCharacter to a CPlayer");
	return m_pAttachedPlayer != nullptr;
}


bool CCharacter::IsClient() const
{
	return m_bClient;
}


bool CCharacter::IsMigrating() const
{
	return false;
}


void CCharacter::SetMigrating(bool isMigrating)
{}


void CCharacter::InitLocalPlayer()
{}


const char* CCharacter::GetActorClassName() const
{
	return "CCharacter";
}


ActorClass CCharacter::GetActorClass() const
{
	return EACT_CHARACTER;
}


const char* CCharacter::GetEntityClassName() const
{
	return GetEntity()->GetClass()->GetName();
}


void CCharacter::SerializeXML(XmlNodeRef& node, bool bLoading)
{}


void CCharacter::SerializeLevelToLevel(TSerialize &ser)
{}


IAnimatedCharacter* CCharacter::GetAnimatedCharacter()
{
	return m_pAnimatedCharacter;
}


const IAnimatedCharacter* CCharacter::GetAnimatedCharacter() const
{
	return nullptr;
}


void CCharacter::PlayExactPositioningAnimation(const char* sAnimationName, bool bSignal, const Vec3& vPosition, const Vec3& vDirection, float startWidth, float startArcAngle, float directionTolerance)
{}


void CCharacter::CancelExactPositioningAnimation()
{}


void CCharacter::PlayAnimation(const char* sAnimationName, bool bSignal)
{}


void CCharacter::EnableTimeDemo(bool bTimeDemo)
{}


void CCharacter::SwitchDemoModeSpectator(bool activate)
{}


IVehicle* CCharacter::GetLinkedVehicle() const
{
	return nullptr;
}


void CCharacter::OnAIProxyEnabled(bool enabled)
{}


void CCharacter::OnReturnedToPool()
{}


void CCharacter::OnPreparedFromPool()
{}


bool CCharacter::ShouldMuteWeaponSoundStimulus() const
{
	return false;
}


void CCharacter::OnReused(IEntity *pEntity, SEntitySpawnParams &params)
{}


void CCharacter::RegisterEvents()
{
	// Lists the game object events we want to be notified about.
	const int EventsToRegister [] =
	{
		eGFE_OnCollision,			// Collision events.
		eGFE_BecomeLocalPlayer,		// Become client actor events.
		eGFE_OnPostStep,			// Not sure if it's needed for character animation here...but it is required for us to trap that event in this code.
	};

	// Register for the specified game object events.
	GetGameObject()->UnRegisterExtForEvents(this, nullptr, 0);
	GetGameObject()->RegisterExtForEvents(this, EventsToRegister, sizeof(EventsToRegister) / sizeof(int));
}


bool CCharacter::Physicalize()
{
	// TODO: We are not making good use of pe_player_dimensions yet.

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

	// TODO: grab player dimensions based on the stance they are in. I presume this needs updating when their stance
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
	
	// TODO: See why rigid is recommended for this.
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
		//params.timeImpulseRecover = GetTimeImpulseRecover(); // TODO: ILH figure out if this was useful.
		m_pAnimatedCharacter->SetParams(params);
		m_pAnimatedCharacter->ResetInertiaCache();
	}

	return GetEntity()->GetPhysics() != nullptr;
}


// ***
// *** ICharacterEventListener
// ***

void CCharacter::OnDeath(IActor* pActor, bool bIsGod)
{
	// NOTE: interactor routines are not tested or known to be working yet.
	ResetInteractor();

	// Restore interactor.
	IEntityLocking * pInteractor = GetInteractor();
	if (!GetGameObject()->GetUpdateSlotEnables(pInteractor, 0))
		GetGameObject()->EnableUpdateSlot(pInteractor, 0);
}


void CCharacter::OnEnterVehicle(IActor* pActor, const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson)
{}


void CCharacter::OnExitVehicle(IActor* pActor)
{}


void CCharacter::OnHealthChanged(IActor* pActor, float newHealth)
{}


void CCharacter::OnItemDropped(IActor* pActor, EntityId itemId)
{}


void CCharacter::OnItemPickedUp(IActor* pActor, EntityId itemId)
{}


void CCharacter::OnItemUsed(IActor* pActor, EntityId itemId)
{}


void CCharacter::OnObjectGrabbed(IActor* pActor, bool bIsGrab, EntityId objectId, bool bIsNPC, bool bIsTwoHanded)
{}


void CCharacter::OnPickedUpPickableAmmo(IActor* pActor, IEntityClass* pAmmoType, int count)
{}


void CCharacter::OnRevive(IActor* pActor, bool bIsGod)
{
	// NOTE: interactor routines are not tested or known to be working yet.
	ResetInteractor();

	// Restore interactor.
	auto pInteractor = GetInteractor();
	if (!GetGameObject()->GetUpdateSlotEnables(pInteractor, 0))
		GetGameObject()->EnableUpdateSlot(pInteractor, 0);
}


void CCharacter::OnSpectatorModeChanged(IActor* pActor, uint8 mode)
{}


void CCharacter::OnSpecialMove(IActor* pActor, ICharacterEventListener::ESpecialMove move)
{}


void CCharacter::OnSprintStaminaChanged(IActor* pActor, float newStamina)
{}


void CCharacter::OnStanceChanged(IActor* pActor, EStance stance)
{}


void CCharacter::OnToggleThirdPerson(IActor* pActor, bool bThirdPerson)
{}


// ***
// *** CCharacter
// ***


void CCharacter::ProcessMovement(float DeltaTime)
{}


void CCharacter::ProcessRotation(float deltaTime)
{}


void CCharacter::SelectMovementHierarchy()
{
	// Force the state machine in the proper hierarchy
	//if (IsAIControlled ())
	//{
	//	CRY_ASSERT (!IsPlayer ());

	//	StateMachineHandleEventMovement (CHARACTER_EVENT_ENTRY_AI);
	//}
	//else
	//{
	//	StateMachineHandleEventMovement (CHARACTER_EVENT_ENTRY_PLAYER);
	//}

	// HACK: set it to always be player for now.
	// TODO: NEED THIS!!!
	StateMachineHandleEventMovement(CHARACTER_EVENT_ENTRY_CHARACTER);
}


void CCharacter::OnScriptEvent(SEntityEvent& event)
{
	// NOTE: We're not really doing anything in this event handler at present, but I want to hold onto the sample code
	// below as a reminder of how to cast the events parameters to useful types. 

	//const char* eventName = reinterpret_cast<const char*> (event.nParam [0]);
	//IEntityClass::EventValueType eventValueType = static_cast<IEntityClass::EventValueType> (event.nParam [1]);
	//const void* pEventValue = reinterpret_cast<const void*> (event.nParam [2]);
}


void CCharacter::OnEditorPropertyChanged()
{
	// TODO: Handle when properties change, like the model.
}


void CCharacter::OnReset()
{
	// TODO: TESTING - not sure this is needed here, but it was useful for the flashlight, so who knows.
	Physicalize();
}


// ***
// *** AI / Player Control
// ***


void CCharacter::OnPlayerAttach(CPlayer& player)
{
	// Make a note of the player for back reference.
	m_pAttachedPlayer = &player;

	// Default assumption is we now control the character.
	m_isAIControlled = false;

	// NOTE: This was handled in Player.cpp in the InitLocalPlayer() method. It should be valid here,
	// though I'm not sure if it is yet.
	auto pInteractor = GetInteractor();
	//if (!GetGameObject()->GetUpdateSlotEnables(pInteractor, 0))
	//	GetGameObject()->EnableUpdateSlot(pInteractor, 0);
}


void CCharacter::OnPlayerDetach()
{
	m_pAttachedPlayer = nullptr;

	// TODO: Detach the camera.

	// TODO: We can remove the entity awareness component if that's desireable.

	// TODO: handle transitioning this character back into the loving hands of the AI.
	m_isAIControlled = true;
}


// ***
// *** Character life-cycle
// ***


void CCharacter::Reset()
{
	// Reset the HSM for character movement.
	StateMachineResetMovement();

	// Select which HSM to use for our character's movement. This relies on it's AI status being
	// correctly set first.
	SelectMovementHierarchy();

	//SetLastTimeInLedge (0.0f);
	//SetupAimIKProperties ();
	//DisableStumbling ();
	//m_playerStateSwim_WaterTestProxy.Reset (true);
	//GetSpectacularKill ().Reset ();
}


void CCharacter::Kill()
{
	// TODO: A *LOT* of code needs to be added here to handle reviving.
}


void CCharacter::Revive(EReasonForRevive reasonForRevive)
{
	// TODO: A *LOT* of code needs to be added here to handle reviving.

	// Reset the HSM for character movement.
	StateMachineResetMovement();

	// Select which HSM to use for our character's movement. This relies on it's AI status being
	// correctly set first.
	SelectMovementHierarchy();
}


// ***
// *** Handle interactions with other entities.
// ***


IEntityLocking* CCharacter::GetInteractor()
{
	//if (IsClient()) // TODO: IsClient needs some rework for our situation.
	// TODO: assured this call can be moved to the PostInit step. Check soon and move if it can.
	{
		if (!m_pInteractor)
			m_pInteractor = static_cast<IEntityLocking*> (GetGameObject()->AcquireExtension("EntityLocking"));
		return m_pInteractor;
	}

	return nullptr;
}


void CCharacter::LockInteractor(EntityId lockId)
{
	//auto pInteractor = GetInteractor();

	// TODO: Do we want to add any pre-condition checking here?
	LockInteractor(lockId, true);
}


void CCharacter::UnlockInteractor(EntityId unlockId)
{
	auto pInteractor = GetInteractor();

	if (pInteractor && pInteractor->GetLockedEntityId() == unlockId)
		LockInteractor(unlockId, false);
}


void CCharacter::LockInteractor(EntityId lockId, bool lock)
{
	// DEBUG: Adding some debug statements to make this easier to start working with.
	CryLogAlways("LockInteractor: %s, Target Lock Id %d, lock %d", GetEntity()->GetName(), lockId, lock);

	SmartScriptTable lockTable(gEnv->pScriptSystem);
	lockTable->SetValue("lockTable", ScriptHandle(lockId));
	lockTable->SetValue("lockId", ScriptHandle(lock ? lockId : 0));
	lockTable->SetValue("lockIdx", lock ? 1 : 0);

	// Does this piece of code set the script table values on the CInteractor extension rather than our base component?
	GetGameObject()->SetExtensionParams("EntityLocking", lockTable);
}


void CCharacter::ResetInteractor()
{
	EntityId lockedId = m_pInteractor ? m_pInteractor->GetLockedEntityId() : INVALID_ENTITYID;
	if (lockedId)
		LockInteractor(lockedId, false);

	// TODO: Looks like we need to add the player plugin abilities to this class.
	//if (m_pLocalPlayerInteractionPlugin)
	//	m_pLocalPlayerInteractionPlugin->Reset();
}


void CCharacter::OnActionItemUse(EntityId playerId)
{
	CryLogAlways("Player tried to use an item");

	auto pInteractor = GetInteractor();
	if (pInteractor)
	{
		CryLogAlways("GetLockedEntityId = %d", pInteractor->GetLockedEntityId());
	}

	// TODO: This should use IEntityLocking instead, and that should be given some useful interface. It might need a rename too.
	auto pEntityAwareness = static_cast <IEntityAwareness*> (GetGameObject()->AcquireExtension("EntityAwareness"));
	if (pEntityAwareness)
	{
		if (auto pEntity = pEntityAwareness->GetEntityInFrontOf())
		{
			if (IScriptTable* pScriptTable = pEntity->GetScriptTable())
			{
				// TODO: Figure out how to pass an IEntity to the function.				
				EntityScripts::CallScriptFunction(pEntity, pScriptTable, "OnUsed", GetEntityId(), 1);
			}
		}
	}
}


void CCharacter::OnActionItemPickup(EntityId playerId)
{
	CryLogAlways("Player picked up an item");
}


void CCharacter::OnActionItemDrop(EntityId playerId)
{
	CryLogAlways("Player dropped an item");
}


void CCharacter::OnActionItemThrow(EntityId playerId)
{
	CryLogAlways("Player threw an item");
}
