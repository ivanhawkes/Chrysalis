#include <StdAfx.h>

#include "Mount.h"

// Definition of the state machine that controls character movement.
//DEFINE_STATE_MACHINE(CMount, Movement);


CMount::CMount()
{
}


CMount::~CMount()
{
}


// ***
// *** IGameObjectExtension
// ***


void CMount::GetMemoryUsage(ICrySizer *pSizer) const
{
	CActor::GetMemoryUsage(pSizer);
	pSizer->Add(*this);
}


bool CMount::Init(IGameObject * pGameObject)
{
	CActor::Init(pGameObject);

	return true;
}


void CMount::PostInit(IGameObject * pGameObject)
{
	CActor::PostInit(pGameObject);

	// Register for game object events.
	RegisterEvents();

	// Get it into a known state.
	Reset();
}


bool CMount::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	CActor::ReloadExtension(pGameObject, params);

	// Register for game object events again.
	RegisterEvents();

	return true;
}


void CMount::PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	CActor::PostReloadExtension(pGameObject, params);
}


void CMount::Release()
{
	// Destroy this instance.
	delete this;
}


void CMount::FullSerialize(TSerialize ser)
{
	CActor::FullSerialize(ser);
}


bool CMount::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags)
{
	return true;
}


ISerializableInfoPtr CMount::GetSpawnInfo()
{
	return nullptr;
}


void CMount::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	CActor::Update(ctx, updateSlot);
}


void CMount::HandleEvent(const SGameObjectEvent& event)
{
	CActor::HandleEvent(event);
}


void CMount::ProcessEvent(SEntityEvent& event)
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


IComponent::ComponentEventPriority CMount::GetEventPriority(const int eventID) const
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
// *** CMount
// ***


bool CMount::Physicalize()
{
	return CActor::Physicalize();
}


void CMount::PrePhysicsUpdate()
{
	// TODO: HACK: Copy and update the function from CCharacter when time allows. This is strictly for test purposes
	// for now. 
}


void CMount::RegisterEvents()
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


void CMount::OnScriptEvent(SEntityEvent& event)
{
	//CActor::OnScriptEvent(event);
}


void CMount::OnEditorPropertyChanged()
{
	//CActor::OnEditorPropertyChanged(event);
}


// ***
// *** Life-cycle
// ***


void CMount::Reset()
{
	CActor::Reset();
}


void CMount::Kill()
{
	CActor::Kill();
}


void CMount::Revive(EReasonForRevive reasonForRevive)
{
	CActor::Revive(reasonForRevive);
}


// ***
// *** Hierarchical State Machine Support
// ***


void CMount::SelectMovementHierarchy()
{
	//StateMachineHandleEventMovement(CHARACTER_EVENT_ENTRY_CHARACTER);
}


void CMount::MovementHSMRelease()
{
	//StateMachineReleaseMovement();
}


void CMount::MovementHSMInit()
{
	//StateMachineInitMovement();
}


void CMount::MovementHSMSerialize(TSerialize ser)
{
	//StateMachineSerializeMovement(SStateEventSerialize(ser));
}


void CMount::MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot)
{
	//StateMachineUpdateMovement(ctx.fFrameTime, false);

	// Pass the update into the movement state machine.
	// TODO: make this happen.
	//StateMachineHandleEventMovement(SStateEventUpdate(ctx.fFrameTime));
}


void CMount::MovementHSMReset()
{
	//StateMachineResetMovement();
}
