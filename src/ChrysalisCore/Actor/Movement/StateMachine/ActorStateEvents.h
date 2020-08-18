#pragma once

#include <IActionMapManager.h>
#include <StateMachine/StateMachine.h>


namespace Chrysalis
{
enum EActorStateEvent
{
	///< Pre-physics handling.
	ACTOR_EVENT_PREPHYSICSUPDATE = STATE_EVENT_CUSTOM,

	///< Update cycle handing.
	ACTOR_EVENT_UPDATE,

	///< Actor is now being played by the local player.
	ACTOR_EVENT_BECOME_LOCAL_PLAYER,

	///< Ragdoll.
	ACTOR_EVENT_RAGDOLL_PHYSICALIZED,

	///< Entry point for player driven actors.
	ACTOR_EVENT_ENTRY,

	///< Entry point for AI driven actors.
	ACTOR_EVENT_ENTRY_AI,

	///< Actor is dead.
	ACTOR_EVENT_DEAD,

	///< Actor is on the ground.
	ACTOR_EVENT_GROUND,

	///< Actor is falling.
	ACTOR_EVENT_FALL,

	///< Actor is jumping.
	ACTOR_EVENT_JUMP,

	///< #TODO: investigate this. Seems to help determine if you're on the ground or falling.
	ACTOR_EVENT_GROUND_COLLIDER_CHANGED,

	///< Seems to be something to do with jumps, slides, sprints and button mashing.
	ACTOR_EVENT_INPUT,

	///< Allow an actor to fly.
	ACTOR_EVENT_FLY,

	///< The actor has switched their weapon.
	ACTOR_EVENT_WEAPONCHANGED,

	///< The actor has switched their stance.
	ACTOR_EVENT_STANCE_CHANGED,

	///< Play or stop a cut-scene for this actor.
	ACTOR_EVENT_CUTSCENE,

	// It seems to be used to go into an animation controlled state.
	ACTOR_EVENT_INTERACTIVE_ACTION,

	// Required to use ladders.
	// #TODO: Would it be possible for AI / Pets / Mounts to use ladders?
	ACTOR_EVENT_LADDER,
	ACTOR_EVENT_LADDER_LEAVE,
	ACTOR_EVENT_LADDER_POSITION,

	// Vehicles / characters?
	ACTOR_EVENT_ATTACH,
	ACTOR_EVENT_DETACH,

	// I believe these are used for viewing or skipping some form of player intro video. Unsure at present.
	ACTOR_EVENT_INTRO_START,
	ACTOR_EVENT_INTRO_FINISHED,

	// Button mashing.
	//ACTOR_EVENT_BUTTONMASHING_SEQUENCE,
	//ACTOR_EVENT_BUTTONMASHING_SEQUENCE_END,

	// Used with co-operative animations, for instance stealth kills.
	//ACTOR_EVENT_STEALTHKILL,
	//ACTOR_EVENT_COOP_ANIMATION_FINISHED,

	///< Don't use, except to chain more events to the end of this list.
	ACTOR_EVENT_LAST,
};


#define eActorStateFlags(f) \
	f(eActorStateFlags_PrePhysicsUpdateAfterEnter) \
	f(eActorStateFlags_DoUpdate) \
	f(eActorStateFlags_IsUpdating) \
	f(eActorStateFlags_Ground) \
	f(eActorStateFlags_Jump) \
	f(eActorStateFlags_Sprinting) \
	f(eActorStateFlags_SprintPressed) \
	f(eActorStateFlags_Swimming) \
	f(eActorStateFlags_InAir) \
	f(eActorStateFlags_PhysicsSaysFlying) \
	f(eActorStateFlags_CurrentItemIsHeavy) \
	f(eActorStateFlags_InteractiveAction) \
	f(eActorStateFlags_OnLadder)

AUTOENUM_BUILDFLAGS_WITHZERO(eActorStateFlags, eActorStateFlags_None);


enum EActorStates
{
	ACTOR_STATE_ENTRY = STATE_FIRST,
	ACTOR_STATE_MOVEMENT,
	ACTOR_STATE_MOVEMENT_AI,
	ACTOR_STATE_ANIMATION,
	ACTOR_STATE_LINKED,
};



struct SActorPrePhysicsData
{
	const float m_frameTime;

	SActorPrePhysicsData(float frameTime)
		: m_frameTime(frameTime)
	{}

private:
	// DO NOT IMPLEMENT!!!
	SActorPrePhysicsData();
};


struct SStateEventActorMovementPrePhysics : public SStateEvent
{
	SStateEventActorMovementPrePhysics(const SActorPrePhysicsData* pPrePhysicsData)
		:
		SStateEvent(ACTOR_EVENT_PREPHYSICSUPDATE)
	{
		AddData(pPrePhysicsData);
	}

	const SActorPrePhysicsData& GetPrePhysicsData() const { return *static_cast<const SActorPrePhysicsData*> (GetData(0).GetPtr()); }

private:
	// DO NOT IMPLEMENT!!!
	SStateEventActorMovementPrePhysics();
};


struct SStateEventUpdate : public SStateEvent
{
	SStateEventUpdate(const float frameTime)
		:
		SStateEvent(ACTOR_EVENT_UPDATE)
	{
		AddData(frameTime);
	}

	const float GetFrameTime() const { return GetData(0).GetFloat(); }

private:
	// DO NOT IMPLEMENT!!!
	SStateEventUpdate();
};


struct SStateEventFly : public SStateEvent
{
	SStateEventFly(const uint8 flyMode)
		:
		SStateEvent(ACTOR_EVENT_FLY)
	{
		AddData(flyMode);
	}

	const int GetFlyMode() const
	{
		return GetData(0).GetInt();
	}

private:
	// DO NOT IMPLEMENT!
	SStateEventFly();
};


struct SStateEventInteractiveAction : public SStateEvent
{
	SStateEventInteractiveAction(const EntityId objectId, bool bUpdateVisibility, int interactionIndex = 0, float actionSpeed = 1.0f)
		:
		SStateEvent(ACTOR_EVENT_INTERACTIVE_ACTION)
	{
		AddData(static_cast<int> (objectId));
		AddData(bUpdateVisibility);
		AddData(actionSpeed);
		AddData(static_cast<int> (interactionIndex));
	}


	SStateEventInteractiveAction(const char* interactionName, bool bUpdateVisibility, float actionSpeed = 1.0f)
		:
		SStateEvent(ACTOR_EVENT_INTERACTIVE_ACTION)
	{
		AddData(interactionName);
		AddData(bUpdateVisibility);
		AddData(actionSpeed);
	}


	int GetObjectEntityID() const
	{
		if (GetData(0).m_type == SStateEventData::eSEDT_int)
		{
			return GetData(0).GetInt();
		}
		return 0;
	}


	const char* GetObjectInteractionName() const
	{
		if (GetData(0).m_type == SStateEventData::eSEDT_voidptr)
		{
			return static_cast<const char*> (GetData(0).GetPtr());
		}
		return nullptr;
	}


	int GetInteractionIndex() const
	{
		if (GetData(3).m_type == SStateEventData::eSEDT_int)
		{
			return GetData(3).GetInt();
		}
		return 0;
	}


	bool GetShouldUpdateVisibility() const
	{
		return GetData(1).GetBool();
	}


	float GetActionSpeed() const
	{
		return GetData(2).GetFloat();
	}
};


struct SInputEventData
{
	enum class EInputEvent
	{
		jump,
		slide,
		sprint,
		buttonMashingSequence,
	};

	SInputEventData(EInputEvent inputEvent)
		: m_inputEvent(inputEvent), m_entityId(0), m_actionId(""), m_activationMode(0), m_value(0.0f)
	{}

	SInputEventData(EInputEvent inputEvent, EntityId entityId, const ActionId& actionId, int activationMode, float value)
		: m_inputEvent(inputEvent), m_entityId(entityId), m_actionId(actionId), m_activationMode(activationMode), m_value(value)
	{}

	EInputEvent m_inputEvent;
	const EntityId m_entityId;
	const ActionId m_actionId;
	const int m_activationMode;
	const float m_value;
};


struct SStateEventActorInput : public SStateEvent
{
	SStateEventActorInput(const SInputEventData* pInputEvent)
		: SStateEvent(ACTOR_EVENT_INPUT)
	{
		AddData(pInputEvent);
	}

	const SInputEventData& GetInputEventData() const { return *static_cast<const SInputEventData*> (GetData(0).GetPtr()); }
};


struct SStateEventStanceChanged : public SStateEvent
{
	SStateEventStanceChanged(const int targetStance)
		: SStateEvent(ACTOR_EVENT_STANCE_CHANGED)
	{
		AddData(targetStance);
	}

	ILINE int GetStance() const { return GetData(0).GetInt(); }
};


struct SStateEventJump : public SStateEvent
{
	enum EData
	{
		eD_PrePhysicsData = 0,
		eD_VerticalSpeedMofidier
	};

	SStateEventJump(const SActorPrePhysicsData& data, const float verticalSpeedModifier = 0.0f)
		:
		SStateEvent(ACTOR_EVENT_JUMP)
	{
		AddData(&data);
		AddData(verticalSpeedModifier);
	}

	ILINE const SActorPrePhysicsData& GetPrePhysicsEventData() const { return *static_cast<const SActorPrePhysicsData*> (GetData(eD_PrePhysicsData).GetPtr()); }
	ILINE float GetVerticalSpeedModifier() const { return (GetData(eD_VerticalSpeedMofidier).GetFloat()); }
};


struct SStateEventCutScene : public SStateEvent
{
	explicit SStateEventCutScene(const bool enable)
		:
		SStateEvent(ACTOR_EVENT_CUTSCENE)
	{
		AddData(enable);
	}

	ILINE bool IsEnabling() const { return (GetData(0).GetBool()); }
};


struct SStateEventGroundColliderChanged : public SStateEvent
{
	explicit SStateEventGroundColliderChanged(const bool bChanged)
		: SStateEvent(ACTOR_EVENT_GROUND_COLLIDER_CHANGED)
	{
		AddData(bChanged);
	}

	ILINE const bool OnGround() const { return GetData(0).GetBool(); }
};


enum ELadderLeaveLocation
{
	eLLL_First = 0,
	eLLL_Top = eLLL_First,
	eLLL_Bottom,
	eLLL_Drop,
	eLLL_Count,
};


struct SStateEventLadder : public SStateEvent
{
	SStateEventLadder(IEntity* pLadder)
		:
		SStateEvent(ACTOR_EVENT_LADDER)
	{
		AddData(pLadder);
	}

	IEntity* GetLadder() const { return (IEntity*)(GetData(0).GetPtr()); }
};


struct SStateEventLeaveLadder : public SStateEvent
{
	SStateEventLeaveLadder(ELadderLeaveLocation leaveLocation)
		:
		SStateEvent(ACTOR_EVENT_LADDER_LEAVE)
	{
		AddData((int)leaveLocation);
	}

	ELadderLeaveLocation GetLeaveLocation() const { return (ELadderLeaveLocation)(GetData(0).GetInt()); }
};


struct SStateEventLadderPosition : public SStateEvent
{
	SStateEventLadderPosition(float heightFrac)
		:
		SStateEvent(ACTOR_EVENT_LADDER_POSITION)
	{
		AddData(heightFrac);
	}

	float GetHeightFrac() const { return GetData(0).GetFloat(); }
};
}