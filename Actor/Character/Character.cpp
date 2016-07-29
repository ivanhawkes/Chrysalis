#include <StdAfx.h>

#include "Character.h"
#include <Actor/Character/Movement/StateMachine/CharacterStateEvents.h>
#include <Actor/Character/Movement/StateMachine/CharacterStateUtil.h>

// Definition of the state machine that controls character movement.
DEFINE_STATE_MACHINE(CCharacter, Movement);


CCharacter::CCharacter()
{
}


CCharacter::~CCharacter()
{
}


// ***
// *** IGameObjectExtension
// ***


void CCharacter::GetMemoryUsage(ICrySizer *pSizer) const
{
	CActor::GetMemoryUsage(pSizer);
	pSizer->Add(*this);
}


bool CCharacter::Init(IGameObject * pGameObject)
{
	CActor::Init(pGameObject);

	return true;
}


void CCharacter::PostInit(IGameObject * pGameObject)
{
	CActor::PostInit(pGameObject);

	// Register for game object events.
	RegisterEvents();

	// Get it into a known state.
	Reset();
}


bool CCharacter::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	CActor::ReloadExtension(pGameObject, params);

	// Register for game object events again.
	RegisterEvents();

	return true;
}


void CCharacter::PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	CActor::PostReloadExtension(pGameObject, params);
}


void CCharacter::Release()
{
	// Destroy this instance.
	delete this;
}


void CCharacter::FullSerialize(TSerialize ser)
{
	CActor::FullSerialize(ser);
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
	CActor::Update(ctx, updateSlot);
}


void CCharacter::HandleEvent(const SGameObjectEvent& event)
{
	CActor::HandleEvent(event);
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


// *** 
// *** IActor
// *** 



// ***
// *** CActor
// ***



// ***
// *** CCharacter
// ***


bool CCharacter::Physicalize()
{
	return CActor::Physicalize();
}


void CCharacter::PrePhysicsUpdate()
{
	// TODO: HACK: This section of code needs a mix of both IActor / CActor code and CCharacter code, which
	// makes it a bit messy to do with simple inheritance. It needs to be properly refactored at some point,
	// preferably before going on to work on the same function in CMount and CPet.
	// 
	// At some point it will need to call into CActor::PrePhysicsUpdate() and some other similar
	// granular routines.
	
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


		// TODO: Not really required just yet.
		//m_characterRotation->Process(movementRequest, frameTime);



		// TODO: call to save data to recording system?



		// TODO: GET THIS BACK IN! REMOVED WHILE REFACTORING CACTOR FROM CCHARACTER.
		// HACK: THIS WILL NEED TO BE PLACED BACK IN ONCE WE WANT TO START WORKING ON GETTING THE
		// PHYSICS FOR THE ACTOR ALL WORKING.
		// The routine will need to be rewritten to work with actors only, or we need a new one that does the actor, that
		// is called by a character version of this.
		//CCharacterStateUtil::UpdateCharacterPhysicsStats(*this, m_actorPhysics, frameTime);




		//#ifdef STATE_DEBUG
		//		if (g_pGameCVars->pl_watchPlayerState >= (bIsClient ? 1 : 2))
		//		{
		//			// NOTE: outputting this info here is 'was happened last frame' not 'what was decided this frame' as it occurs before the prePhysicsEvent is dispatched
		//			// also IsOnGround and IsInAir can possibly both be false e.g. - if you're swimming
		//			// May be able to remove this log now the new HSM debugging is in if it offers the same/improved functionality
		//			CryWatch("%s stance=%s flyMode=%d %s %s%s%s%s", GetEntity()->GetEntityTextDescription(), GetStanceName(GetStance()), m_actorState.flyMode, IsOnGround() ? "ON-GROUND" : "IN-AIR", IsThirdPerson() ? "THIRD-PERSON" : "FIRST-PERSON", IsDead() ? "DEAD" : "ALIVE", m_actorState.isScoped ? " SCOPED" : "", m_actorState.isInBlendRagdoll ? " FALLNPLAY" : "");
		//		}
		//#endif



		// Push the pre-physics event down to our state machine.
		// TODO: This should become an Actor prephysics event instead, so all derived classes can share it.
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


void CCharacter::OnScriptEvent(SEntityEvent& event)
{
	//CActor::OnScriptEvent(event);
}


void CCharacter::OnEditorPropertyChanged()
{
	//CActor::OnEditorPropertyChanged(event);
}


// ***
// *** Life-cycle
// ***


void CCharacter::Reset()
{
	CActor::Reset();
}


void CCharacter::Kill()
{
	CActor::Kill();
}


void CCharacter::Revive(EReasonForRevive reasonForRevive)
{
	CActor::Revive(reasonForRevive);
}


// ***
// *** Hierarchical State Machine Support
// ***


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


void CCharacter::MovementHSMRelease()
{
	StateMachineReleaseMovement();
}


void CCharacter::MovementHSMInit()
{
	StateMachineInitMovement();
}


void CCharacter::MovementHSMSerialize(TSerialize ser)
{
	StateMachineSerializeMovement(SStateEventSerialize(ser));
}


void CCharacter::MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot)
{
	//#ifdef STATE_DEBUG
	//	const bool shouldDebug = (s_StateMachineDebugEntityID == GetEntityId ());
	//#else
	//	const bool shouldDebug = false;
	//#endif
	const bool shouldDebug = false;
	//const bool shouldDebug = true;

	StateMachineUpdateMovement(ctx.fFrameTime, shouldDebug);

	// Pass the update into the movement state machine.
	// TODO: make this happen.
	StateMachineHandleEventMovement(SStateEventUpdate(ctx.fFrameTime));
}


void CCharacter::MovementHSMReset()
{
	StateMachineResetMovement();
}
