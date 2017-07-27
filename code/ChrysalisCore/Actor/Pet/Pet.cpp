#include <StdAfx.h>

#include "Pet.h"

// Definition of the state machine that controls character movement.
//DEFINE_STATE_MACHINE(CPet, Movement);


CPet::CPet()
{
}


CPet::~CPet()
{
}


// ***
// *** IGameObjectExtension
// ***


void CPet::GetMemoryUsage(ICrySizer *pSizer) const
{
	CActor::GetMemoryUsage(pSizer);
	pSizer->Add(*this);
}


bool CPet::Init(IGameObject * pGameObject)
{
	CActor::Init(pGameObject);

	return true;
}


void CPet::PostInit(IGameObject * pGameObject)
{
	CActor::PostInit(pGameObject);

	// Register for game object events.
	RegisterEvents();

	// Get it into a known state.
	OnResetState();
}


bool CPet::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	CActor::ReloadExtension(pGameObject, params);

	// Register for game object events again.
	RegisterEvents();

	return true;
}


void CPet::PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	CActor::PostReloadExtension(pGameObject, params);
}


void CPet::Release()
{
	// Destroy this instance.
	delete this;
}


void CPet::FullSerialize(TSerialize ser)
{
	CActor::FullSerialize(ser);
}


bool CPet::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags)
{
	return true;
}


ISerializableInfoPtr CPet::GetSpawnInfo()
{
	return nullptr;
}


void CPet::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	CActor::Update(ctx, updateSlot);
}


void CPet::HandleEvent(const SGameObjectEvent& event)
{
	CActor::HandleEvent(event);
}


void CPet::ProcessEvent(SEntityEvent& event)
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
			OnResetState();
			break;

		default:
			break;
	}
}


// FIX: 5.4
IComponent::ComponentEventPriority CPet::GetEventPriority(const int eventID) const
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
// *** CPet
// ***


bool CPet::Physicalize()
{
	return CActor::Physicalize();
}


void CPet::PrePhysicsUpdate()
{
	// TODO: HACK: Copy and update the function from CCharacter when time allows. This is strictly for test purposes
	// for now. 
}


void CPet::RegisterEvents()
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


void CPet::OnScriptEvent(SEntityEvent& event)
{
	//CActor::OnScriptEvent(event);
}


void CPet::OnEditorPropertyChanged()
{
	//CActor::OnEditorPropertyChanged(event);
}


// ***
// *** Life-cycle
// ***


void CPet::OnResetState()
{
	CActor::OnResetState();
}


void CPet::Kill()
{
	CActor::Kill();
}


void CPet::Revive(EReasonForRevive reasonForRevive)
{
	CActor::Revive(reasonForRevive);
}


// ***
// *** Hierarchical State Machine Support
// ***


void CPet::SelectMovementHierarchy()
{
//	StateMachineHandleEventMovement(ACTOR_EVENT_ENTRY);
}


void CPet::MovementHSMRelease()
{
//	StateMachineReleaseMovement();
}


void CPet::MovementHSMInit()
{
//	StateMachineInitMovement();
}


void CPet::MovementHSMSerialize(TSerialize ser)
{
//	StateMachineSerializeMovement(SStateEventSerialize(ser));
}


void CPet::MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot)
{
	//StateMachineUpdateMovement(ctx.fFrameTime, false);

	// Pass the update into the movement state machine.
	// TODO: make this happen.
	//StateMachineHandleEventMovement(SStateEventUpdate(ctx.fFrameTime));
}


void CPet::MovementHSMReset()
{
	//StateMachineResetMovement();
}
