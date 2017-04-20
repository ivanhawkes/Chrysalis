#include <StdAfx.h>

#include "Character.h"
#include <Actor/Movement/ActorMovementController.h>
#include <Actor/Character/Movement/StateMachine/CharacterStateEvents.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include <CrySerialization/Decorators/Resources.h>


//CRYREGISTER_CLASS(CCharacter)

// Definition of the state machine that controls character movement.
DEFINE_STATE_MACHINE(CCharacter, Movement);


class CCharacterRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		CCharacter::Register();
	}
};

CCharacterRegistrator g_CharacterRegistrator;


void CCharacter::Register()
{
	// Register the entity class so that instances can be created later on either via Launcher or Editor.
	CChrysalisCorePlugin::RegisterEntityWithDefaultComponent<CCharacter>("Character");
	//RegisterEntityWithDefaultComponent<CCharacter>("Character", "Actors", "character.bmp");
}


void CCharacter::Initialize()
{
	CActor::Initialize();

	//// #TODO: Move the PostInit into here once we are free of GameObjects.
	//PostInit(gEnv->pGameFramework->GetGameObject(GetEntityId()));
}


void CCharacter::ProcessEvent(SEntityEvent& event)
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
			// #TODO: HACK: We don't have an update with ctx, updateslot - figure out how to do it the new way then switch
			// this on.
			//Update();
			break;

		case ENTITY_EVENT_PREPHYSICSUPDATE:
			PrePhysicsUpdate();
			break;
	}
}


void CCharacter::SerializeProperties(Serialization::IArchive& archive)
{
	archive(Serialization::ModelFilename(m_geometry), "Geometry", "Geometry");
	archive(m_mass, "Mass", "Mass");
	archive(Serialization::CharacterAnimationPicker(m_controllerDefinition), "ControllerDefinition", "Controller Definition");
	archive(Serialization::CharacterAnimationPicker(m_scopeContext), "ScopeContext", "Scope Context");
	archive(Serialization::CharacterAnimationPicker(m_animationDatabase), "AnimationDatabase", "Animation Database");

	if (archive.isInput())
	{
		OnResetState();
	}
}


// ***
// *** IGameObjectExtension
// ***


void CCharacter::PostInit(IGameObject * pGameObject)
{
	CActor::PostInit(pGameObject);

	// Register for game object events.
	RegisterEvents();

	// Get it into a known state.
	OnResetState();
}


void CCharacter::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	CActor::Update(ctx, updateSlot);
}


void CCharacter::HandleEvent(const SGameObjectEvent& event)
{
	CActor::HandleEvent(event);
}


IEntityComponent::ComponentEventPriority CCharacter::GetEventPriority(const int eventID) const
{
	switch (eventID)
	{
		case ENTITY_EVENT_PREPHYSICSUPDATE:
			//			return(ENTITY_PROXY_LAST - ENTITY_PROXY_USER + EEntityEventPriority_Actor + (m_isClient ? EEntityEventPriority_Client : 0));
			return ENTITY_PROXY_LAST - ENTITY_PROXY_USER + EEntityEventPriority_Actor + EEntityEventPriority_Client; // #HACK: only used for when we are the client, fix later.
	}

	return IGameObjectExtension::GetEventPriority(eventID);
}


// ***
// *** CCharacter
// ***


bool CCharacter::Physicalize()
{
	return CActor::Physicalize();
}


void CCharacter::PrePhysicsUpdate()
{
	// #TODO: HACK: This section of code needs a mix of both IActor / CActor code and CCharacter code, which
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

		// #TODO: process aim?


		// #TODO: there used to be code for the player to process turning here. I think it might be better handled more abstractly
		// along with other factors like movement. Sample is provided below.
		// SPlayerRotationParams::EAimType aimType = GetCurrentAimType();
		//m_pPlayerRotation->Process(pCurrentItem, movementRequest,
		//	m_playerRotationParams.m_verticalAims [IsThirdPerson() ? 1 : 0] [aimType],
		//	frameTime);


		// #TODO: Not really required just yet.
		//m_characterRotation->Process(movementRequest, frameTime);



		// #TODO: call to save data to recording system?



		// The routine will need to be rewritten to work with actors only, or we need a new one that does the actor, that
		// is called by a character version of this.
		CCharacterStateUtil::UpdatePhysicsState(*this, m_actorPhysics, frameTime);




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
		const SActorPrePhysicsData prePhysicsData(frameTime, movementRequest);
		const SStateEventActorMovementPrePhysics prePhysicsEvent(&prePhysicsData);
		StateMachineHandleEventMovement(STATE_DEBUG_APPEND_EVENT(prePhysicsEvent));

		//CryWatch("%s : velocity = %f, %f, %f\r\n", __func__, movementRequest.desiredVelocity.x, movementRequest.desiredVelocity.y, movementRequest.desiredVelocity.z);


		// #TODO: does stance change really belong here?
		//if (movementRequest.stance != STANCE_NULL)
		//{
		//	SetStance(movementRequest.stance);
		//}

		// Bring the animation state of the character into line with it's requested state.
		UpdateAnimationState(movementRequest);
	}
}


void CCharacter::RegisterEvents()
{
	// Lists the game object events we want to be notified about.
	const int EventsToRegister [] =
	{
		eGFE_OnCollision,			// Collision events.
		eGFE_OnPostStep,			// Not sure if it's needed for character animation here...but it is required for us to trap that event in this code.
	};

	// Register for the specified game object events.
	GetGameObject()->UnRegisterExtForEvents(this, nullptr, 0);
	GetGameObject()->RegisterExtForEvents(this, EventsToRegister, sizeof(EventsToRegister) / sizeof(int));
}


// ***
// *** Life-cycle
// ***


void CCharacter::OnResetState()
{
	CActor::OnResetState();
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

	//	StateMachineHandleEventMovement (ACTOR_EVENT_ENTRY_AI);
	//}
	//else
	//{
	//	StateMachineHandleEventMovement (ACTOR_EVENT_ENTRY_PLAYER);
	//}

	// #HACK: set it to always be player for now.
	// #TODO: NEED THIS!!!
	StateMachineHandleEventMovement(ACTOR_EVENT_ENTRY);
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
	// #TODO: make this happen.
	StateMachineHandleEventMovement(SStateEventUpdate(ctx.fFrameTime));
}


void CCharacter::MovementHSMReset()
{
	StateMachineResetMovement();
}
