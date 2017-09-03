#include <StdAfx.h>

#include "Character.h"
#include <Actor/Movement/ActorMovementController.h>
#include <Actor/Movement/StateMachine/ActorStateEvents.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
#include <Actor/Character/CharacterAttributesComponent.h>


namespace Chrysalis
{
void CCharacterComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CCharacterComponent::ReflectType(Schematyc::CTypeDesc<CCharacterComponent>& desc)
{
	desc.SetGUID(CCharacterComponent::IID());
	desc.SetEditorCategory("Actors");
	desc.SetLabel("Character");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


void CCharacterComponent::Initialize()
{
	const auto pEntity = GetEntity();

	// All characters are actors.
	m_pActorComponent = pEntity->GetOrCreateComponent<CActorComponent>();

	// Manage attributes.
	m_pCharacterAttributesComponent = pEntity->GetOrCreateComponent<CCharacterAttributesComponent>();

	// Get it into a known state.
	OnResetState();
}


void CCharacterComponent::ProcessEvent(SEntityEvent& event)
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
			break;
	}
}


// ***
// *** CCharacterComponent
// ***


void CCharacterComponent::PrePhysicsUpdate()
{
	//// #TODO: HACK: This section of code needs a mix of both IActor / CActorComponent code and CCharacterComponent code, which makes it a
	//// bit messy to do with simple inheritance. It needs to be properly refactored at some point, preferably before
	//// going on to work on the same function in CCharacterComponent and CPetComponent.
	//// 
	//// At some point it will need to call into CActorComponent::PrePhysicsUpdate() and some other similar granular routines.
	//// 
	//// If it wasn't for the dependancy on the state machine code, we could factor this into another component, since it
	//// doesn't really seem to belong here. 

	//const float frameTime = gEnv->pTimer->GetFrameTime();

	//if (m_pMovementController)
	//{
	//	SActorMovementRequest movementRequest;

	//	// Update the movement controller.
	//	m_pMovementController->UpdateActorMovementRequest(movementRequest, frameTime);

	//	// They requested a stance change.
	//	// TODO: Maybe we ignore the one in the movement request and handle it locally here, along with the state machine for it?
	//	//if (movementRequest.stance != STANCE_NULL)
	//	//{
	//	//	SetStance (movementRequest.stance);
	//	//}

	//	// Request the movement from the movement controller.
	//	// NOTE: this one here only cleared the look target previously...searching for other calls to movementrequest.
	//	//CMovementRequest movementRequest;
	//	//m_pMovementController->RequestMovement (movementRequest);

	//	// #TODO: process aim?


	//	// #TODO: there used to be code for the player to process turning here. I think it might be better handled more abstractly
	//	// along with other factors like movement. Sample is provided below.
	//	// SPlayerRotationParams::EAimType aimType = GetCurrentAimType();
	//	//m_pPlayerRotation->Process(pCurrentItem, movementRequest,
	//	//	m_playerRotationParams.m_verticalAims [IsThirdPerson() ? 1 : 0] [aimType],
	//	//	frameTime);


	//	// #TODO: Not really required just yet.
	//	//m_characterRotation->Process(movementRequest, frameTime);



	//	// #TODO: call to save data to recording system?



	//	// The routine will need to be rewritten to work with actors only, or we need a new one that does the actor, that
	//	// is called by a character version of this.
	//	CActorStateUtility::UpdatePhysicsState(*this, m_actorPhysics, frameTime);

	//	//#ifdef STATE_DEBUG
	//	//		if (g_pGameCVars->pl_watchPlayerState >= (bIsClient ? 1 : 2))
	//	//		{
	//	//			// NOTE: outputting this info here is 'was happened last frame' not 'what was decided this frame' as it occurs before the prePhysicsEvent is dispatched
	//	//			// also IsOnGround and IsInAir can possibly both be false e.g. - if you're swimming
	//	//			// May be able to remove this log now the new HSM debugging is in if it offers the same/improved functionality
	//	//			CryWatch("%s stance=%s flyMode=%d %s %s%s%s%s", GetEntity()->GetEntityTextDescription(), GetStanceName(GetStance()), m_actorState.flyMode, IsOnGround() ? "ON-GROUND" : "IN-AIR", IsThirdPerson() ? "THIRD-PERSON" : "FIRST-PERSON", IsDead() ? "DEAD" : "ALIVE", m_actorState.isScoped ? " SCOPED" : "", m_actorState.isInBlendRagdoll ? " FALLNPLAY" : "");
	//	//		}
	//	//#endif

	//	// Push the pre-physics event down to our state machine.
	//	const SActorPrePhysicsData prePhysicsData(frameTime, movementRequest);
	//	const SStateEventActorMovementPrePhysics prePhysicsEvent(&prePhysicsData);
	//	StateMachineHandleEventMovement(STATE_DEBUG_APPEND_EVENT(prePhysicsEvent));

	//	// Bring the animation state of the character into line with it's requested state.
	//	UpdateAnimationState(movementRequest);
	//}
}


void CCharacterComponent::OnResetState()
{
}
}