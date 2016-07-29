#pragma once

#include <StateMachine/StateMachine.h>
#include <Actor/Character/Character.h>
#include <Actor/Character/Movement/CharacterMovementController.h>


class CActor;
class CMelee;


enum ECharacterStates
{
	CHARACTER_STATE_ENTRY = STATE_FIRST,
	CHARACTER_STATE_MOVEMENT,
	CHARACTER_STATE_MOVEMENT_AI,
	CHARACTER_STATE_ANIMATION,
	CHARACTER_STATE_LINKED,
};


enum ECharacterStateEvent
{
	///< Pre-physics handling.
	CHARACTER_EVENT_PREPHYSICSUPDATE = STATE_EVENT_CUSTOM,

	///< Update cycle handing.
	CHARACTER_EVENT_UPDATE,

	///< Character is now being played by the local player.
	CHARACTER_EVENT_BECOME_LOCAL_PLAYER,

	///< Ragdoll.
	CHARACTER_EVENT_RAGDOLL_PHYSICALIZED,

	///< Entry point for player driven characters.
	CHARACTER_EVENT_ENTRY_CHARACTER,

	///< Entry point for AI driven characters.
	CHARACTER_EVENT_ENTRY_AI,

	///< Character is dead.
	CHARACTER_EVENT_DEAD,

	///< Character is on the ground.
	CHARACTER_EVENT_GROUND,

	///< Character is falling.
	CHARACTER_EVENT_FALL,

	///< Character is jumping.
	CHARACTER_EVENT_JUMP,

	///< TODO: investigate this. Seems to help determine if you're on the ground or falling.
	CHARACTER_EVENT_GROUND_COLLIDER_CHANGED,

	// Ledges.
	CHARACTER_EVENT_LEDGE,
	CHARACTER_EVENT_LEDGE_ANIM_FINISHED,

	// Ladders.
	CHARACTER_EVENT_LADDER,
	CHARACTER_EVENT_LADDER_LEAVE,
	CHARACTER_EVENT_LADDER_POSITION,

	// Sliding.
	CHARACTER_EVENT_SLIDE,
	CHARACTER_EVENT_SLIDE_KICK,
	CHARACTER_EVENT_SLIDE_EXIT,
	CHARACTER_EVENT_SLIDE_EXIT_LAZY,
	CHARACTER_EVENT_SLIDE_EXIT_FORCE,

	// Vehicles / characters?
	CHARACTER_EVENT_ATTACH,
	CHARACTER_EVENT_DETACH,

	// Spectators.
	CHARACTER_EVENT_SPECTATE,
	CHARACTER_EVENT_RESET_SPECTATOR_SCREEN,

	// Button mashing.
	CHARACTER_EVENT_BUTTONMASHING_SEQUENCE,
	CHARACTER_EVENT_BUTTONMASHING_SEQUENCE_END,

	///< Seems to be something to do with jumps, slides, sprints and button mashing.
	CHARACTER_EVENT_INPUT,

	///< Allow a character to fly.
	CHARACTER_EVENT_FLY,

	///< The character has switched their weapon.
	CHARACTER_EVENT_WEAPONCHANGED,

	///< The character has switched their stance.
	CHARACTER_EVENT_STANCE_CHANGED,

	// I believe these are used for viewing or skipping some form of player intro video. Unsure at present.
	CHARACTER_EVENT_INTRO_START,
	CHARACTER_EVENT_INTRO_FINISHED,

	// Used with co-operative animations, for instance stealth kills.
	CHARACTER_EVENT_STEALTHKILL,
	CHARACTER_EVENT_COOP_ANIMATION_FINISHED,

	// Uncertain at this time.
	CHARACTER_EVENT_INTERACTIVE_ACTION,

	///< Play or stop a cut-scene for this character.
	CHARACTER_EVENT_CUTSCENE,
};


#define eCharacterStateFlags(f) \
	f(ECharacterStateFlags_PrePhysicsUpdateAfterEnter) \
	f(ECharacterStateFlags_DoUpdate) \
	f(ECharacterStateFlags_IsUpdating) \
	f(ECharacterStateFlags_ExitingSlide) \
	f(ECharacterStateFlags_NetSlide) \
	f(ECharacterStateFlags_NetExitingSlide) \
	f(ECharacterStateFlags_CurrentItemIsHeavy) \
	f(ECharacterStateFlags_Ledge) \
	f(ECharacterStateFlags_Jump) \
	f(ECharacterStateFlags_Sprinting) \
	f(ECharacterStateFlags_SprintPressed) \
	f(ECharacterStateFlags_Sliding) \
	f(ECharacterStateFlags_Ground) \
	f(ECharacterStateFlags_Swimming) \
	f(ECharacterStateFlags_Spectator) \
	f(ECharacterStateFlags_InAir) \
	f(ECharacterStateFlags_InteractiveAction) \
	f(ECharacterStateFlags_PhysicsSaysFlying) \
	f(ECharacterStateFlags_OnLadder)


AUTOENUM_BUILDFLAGS_WITHZERO(eCharacterStateFlags, ECharacterStateFlags_None);


#define LedgeTransitionList(func)	\
	func(eOLT_MidAir)				\
	func(eOLT_Falling)				\
	func(eOLT_VaultOver)			\
	func(eOLT_VaultOverIntoFall)	\
	func(eOLT_VaultOnto)			\
	func(eOLT_HighVaultOver)		\
	func(eOLT_HighVaultOverIntoFall)\
	func(eOLT_HighVaultOnto)		\
	func(eOLT_QuickLedgeGrab)		\


struct SLedgeTransitionData
{
	SLedgeTransitionData(const uint16 ledgeId)
		: m_ledgeTransition(eOLT_None)
		, m_nearestGrabbableLedgeId(ledgeId)
		, m_comingFromOnGround(false)
		, m_comingFromSprint(false)
	{}

	AUTOENUM_BUILDENUMWITHTYPE_WITHINVALID_WITHNUM(EOnLedgeTransition, LedgeTransitionList, eOLT_None, eOLT_MaxTransitions);

	EOnLedgeTransition m_ledgeTransition;
	uint16	m_nearestGrabbableLedgeId;
	bool		m_comingFromOnGround;
	bool		m_comingFromSprint;
};


struct SStateEventLedge : public SStateEvent
{
	explicit SStateEventLedge(const SLedgeTransitionData& ledgeData)
		:
		SStateEvent(CHARACTER_EVENT_LEDGE)
	{
		AddData(ledgeData.m_ledgeTransition);
		AddData(ledgeData.m_nearestGrabbableLedgeId);
		AddData(ledgeData.m_comingFromOnGround);
		AddData(ledgeData.m_comingFromSprint);
	}

	SLedgeTransitionData::EOnLedgeTransition GetLedgeTransition() const { return static_cast<SLedgeTransitionData::EOnLedgeTransition> (GetData(0).GetInt()); }
	uint16 GetLedgeId() const { return (uint16) GetData(1).GetInt(); }
	bool GetComingFromOnGround() const { return GetData(2).GetBool(); }
	bool GetComingFromSprint() const { return GetData(3).GetBool(); }
};


struct SStateEventSlideKick : public SStateEvent
{
	SStateEventSlideKick(CMelee* pMelee)
		:
		SStateEvent(CHARACTER_EVENT_SLIDE_KICK)
	{
		AddData(pMelee);
	}

	CMelee* GetMelee() const { return (CMelee*) GetData(1).GetPtr(); }
};


struct SStateEventLadder : public SStateEvent
{
	SStateEventLadder(IEntity* pLadder)
		:
		SStateEvent(CHARACTER_EVENT_LADDER)
	{
		AddData(pLadder);
	}

	IEntity* GetLadder() const { return (IEntity*) (GetData(0).GetPtr()); }
};


struct SStateEventLeaveLadder : public SStateEvent
{
	SStateEventLeaveLadder(ELadderLeaveLocation leaveLocation)
		:
		SStateEvent(CHARACTER_EVENT_LADDER_LEAVE)
	{
		AddData((int) leaveLocation);
	}

	ELadderLeaveLocation GetLeaveLocation() const { return (ELadderLeaveLocation) (GetData(0).GetInt()); }
};


struct SStateEventLadderPosition : public SStateEvent
{
	SStateEventLadderPosition(float heightFrac)
		:
		SStateEvent(CHARACTER_EVENT_LADDER_POSITION)
	{
		AddData(heightFrac);
	}

	float GetHeightFrac() const { return GetData(0).GetFloat(); }
};


struct SCharacterPrePhysicsData
{
	const float m_frameTime;
	const SActorMovementRequest m_movement;

	SCharacterPrePhysicsData(float frameTime, const SActorMovementRequest& movementRequest)
		: m_frameTime(frameTime), m_movement(movementRequest)
	{}

private:
	// DO NOT IMPLEMENT!!!
	SCharacterPrePhysicsData();
};


struct SStateEventCharacterMovementPrePhysics : public SStateEvent
{
	SStateEventCharacterMovementPrePhysics(const SCharacterPrePhysicsData* pPrePhysicsData)
		:
		SStateEvent(CHARACTER_EVENT_PREPHYSICSUPDATE)
	{
		AddData(pPrePhysicsData);
	}

	const SCharacterPrePhysicsData& GetPrePhysicsData() const { return *static_cast<const SCharacterPrePhysicsData*> (GetData(0).GetPtr()); }

private:
	// DO NOT IMPLEMENT!!!
	SStateEventCharacterMovementPrePhysics();
};


struct SStateEventUpdate : public SStateEvent
{
	SStateEventUpdate(const float frameTime)
		:
		SStateEvent(CHARACTER_EVENT_UPDATE)
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
		SStateEvent(CHARACTER_EVENT_FLY)
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
		SStateEvent(CHARACTER_EVENT_INTERACTIVE_ACTION)
	{
		AddData(static_cast<int> (objectId));
		AddData(bUpdateVisibility);
		AddData(actionSpeed);
		AddData(static_cast<int> (interactionIndex));
	}


	SStateEventInteractiveAction(const char* interactionName, bool bUpdateVisibility, float actionSpeed = 1.0f)
		:
		SStateEvent(CHARACTER_EVENT_INTERACTIVE_ACTION)
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
		return NULL;
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
	enum EInputEvent
	{
		EInputEvent_Jump = 0,
		EInputEvent_Slide,
		EInputEvent_Sprint,
		EInputEvent_ButtonMashingSequence,
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


struct SStateEventCharacterInput : public SStateEvent
{
	SStateEventCharacterInput(const SInputEventData* pInputEvent)
		: SStateEvent(CHARACTER_EVENT_INPUT)
	{
		AddData(pInputEvent);
	}

	const SInputEventData& GetInputEventData() const { return *static_cast<const SInputEventData*> (GetData(0).GetPtr()); }
};


struct SStateEventStanceChanged : public SStateEvent
{
	SStateEventStanceChanged(const int targetStance)
		:
		SStateEvent(CHARACTER_EVENT_STANCE_CHANGED)
	{
		AddData(targetStance);
	}

	ILINE int GetStance() const { return GetData(0).GetInt(); }
};


struct SStateEventCoopAnim : public SStateEvent
{
	explicit SStateEventCoopAnim(EntityId targetID)
		:
		SStateEvent(CHARACTER_EVENT_COOP_ANIMATION_FINISHED)
	{
		AddData(static_cast<int>(targetID));
	}

	ILINE EntityId GetTargetEntityId() const { return static_cast<EntityId> (GetData(0).GetInt()); }
};


struct SStateEventJump : public SStateEvent
{
	enum EData
	{
		eD_PrePhysicsData = 0,
		eD_VerticalSpeedMofidier
	};

	SStateEventJump(const SCharacterPrePhysicsData& data, const float verticalSpeedModifier = 0.0f)
		:
		SStateEvent(CHARACTER_EVENT_JUMP)
	{
		AddData(&data);
		AddData(verticalSpeedModifier);
	}

	ILINE const SCharacterPrePhysicsData& GetPrePhysicsEventData() const { return *static_cast<const SCharacterPrePhysicsData*> (GetData(eD_PrePhysicsData).GetPtr()); }
	ILINE float GetVerticalSpeedModifier() const { return (GetData(eD_VerticalSpeedMofidier).GetFloat()); }
};


struct SStateEventCutScene : public SStateEvent
{
	explicit SStateEventCutScene(const bool enable)
		:
		SStateEvent(CHARACTER_EVENT_CUTSCENE)
	{
		AddData(enable);
	}

	ILINE bool IsEnabling() const { return (GetData(0).GetBool()); }
};


struct SStateEventGroundColliderChanged : public SStateEvent
{
	explicit SStateEventGroundColliderChanged(const bool bChanged)
		: SStateEvent(CHARACTER_EVENT_GROUND_COLLIDER_CHANGED)
	{
		AddData(bChanged);
	}

	ILINE const bool OnGround() const { return GetData(0).GetBool(); }
};


struct SStateEventButtonMashingSequence : public SStateEvent
{
	enum Type
	{
		SystemX = 0,
	};

	struct Params
	{
		EntityId targetLocationId;
		EntityId lookAtTargetId;
		float beamingTime;

		// While the Character is struggling to escape from the boss, he will push 
		// himself back a certain distance (depending on the progression of
		// the button-mashing) (>= 0.0f) (in meters).
		float strugglingMovementDistance;

		// The speed with which the Character will move along the 'struggling
		// line' (>= 0.0f) (in meters / second).
		float strugglingMovementSpeed;
	};

	struct CallBacks
	{
		Functor1<const float&>	onSequenceStart;
		Functor1<const float&>  onSequenceUpdate;
		Functor1<const bool&>   onSequenceEnd;
	};

	explicit SStateEventButtonMashingSequence(const Type sequenceType, const Params& params, const CallBacks& callbacks)
		: SStateEvent(CHARACTER_EVENT_BUTTONMASHING_SEQUENCE)
	{
		AddData((int) sequenceType);
		AddData((const void*) &params);
		AddData((const void*) &callbacks);
	}

	ILINE const Type GetEnemyType() const { return static_cast<Type>(GetData(0).GetInt()); }
	ILINE const Params& GetParams() const { return *static_cast<const Params*>(GetData(1).GetPtr()); }
	ILINE const CallBacks& GetCallBacks() const { return *static_cast<const CallBacks*>(GetData(2).GetPtr()); }
};
