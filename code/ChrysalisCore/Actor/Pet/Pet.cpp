#include <StdAfx.h>

#include "Pet.h"

namespace Chrysalis
{
// Definition of the state machine that controls character movement.
//DEFINE_STATE_MACHINE(CPetComponent, Movement);


void CPetComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CPetComponent::ReflectType(Schematyc::CTypeDesc<CPetComponent>& desc)
{
	desc.SetGUID(CPetComponent::IID());
	desc.SetEditorCategory("Actors");
	desc.SetLabel("Pet");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


void CPetComponent::Initialize()
{
	const auto pEntity = GetEntity();

	// Get it into a known state.
	OnResetState();
}


void CPetComponent::ProcessEvent(SEntityEvent& event)
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

		case ENTITY_EVENT_PREPHYSICSUPDATE:
			PrePhysicsUpdate();
			break;
	}
}


void CPetComponent::PrePhysicsUpdate()
{
	// TODO: HACK: Copy and update the function from CPetComponent when time allows. This is strictly for test purposes
	// for now. 
}


void CPetComponent::OnResetState()
{
}


//// ***
//// *** Hierarchical State Machine Support
//// ***
//
//
//void CPetComponent::SelectMovementHierarchy()
//{
//	//	StateMachineHandleEventMovement(ACTOR_EVENT_ENTRY);
//}
//
//
//void CPetComponent::MovementHSMRelease()
//{
//	//	StateMachineReleaseMovement();
//}
//
//
//void CPetComponent::MovementHSMInit()
//{
//	//	StateMachineInitMovement();
//}
//
//
//void CPetComponent::MovementHSMSerialize(TSerialize ser)
//{
//	//	StateMachineSerializeMovement(SStateEventSerialize(ser));
//}
//
//
//void CPetComponent::MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot)
//{
//	//StateMachineUpdateMovement(ctx.fFrameTime, false);
//
//	// Pass the update into the movement state machine.
//	// TODO: make this happen.
//	//StateMachineHandleEventMovement(SStateEventUpdate(ctx.fFrameTime));
//}
//
//
//void CPetComponent::MovementHSMReset()
//{
//	//StateMachineResetMovement();
//}
}