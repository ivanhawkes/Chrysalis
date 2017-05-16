#pragma once

#include <StateMachine/StateMachine.h>
#include <Actor/Movement/StateMachine/ActorStateEvents.h>


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
	///< Entry point for player driven characters.
	CHARACTER_EVENT_ENTRY = EActorStateEvent::ACTOR_EVENT_LAST,

	///< Entry point for AI driven characters.
	CHARACTER_EVENT_ENTRY_AI,

	// Required to use ladders.
	// #TODO: Would it be possible for AI / Pets / Mounts to use ladders?
	CHARACTER_EVENT_LADDER,
	CHARACTER_EVENT_LADDER_LEAVE,
	CHARACTER_EVENT_LADDER_POSITION,

	// Vehicles / characters?
	CHARACTER_EVENT_ATTACH,
	CHARACTER_EVENT_DETACH,

	// I believe these are used for viewing or skipping some form of player intro video. Unsure at present.
	CHARACTER_EVENT_INTRO_START,
	CHARACTER_EVENT_INTRO_FINISHED,

	///< Play or stop a cut-scene for this character.
	CHARACTER_EVENT_CUTSCENE,

	// Spectators.
	//CHARACTER_EVENT_SPECTATE,
	//CHARACTER_EVENT_RESET_SPECTATOR_SCREEN,

	// Button mashing.
	//CHARACTER_EVENT_BUTTONMASHING_SEQUENCE,
	//CHARACTER_EVENT_BUTTONMASHING_SEQUENCE_END,

	// Used with co-operative animations, for instance stealth kills.
	//CHARACTER_EVENT_STEALTHKILL,
	//CHARACTER_EVENT_COOP_ANIMATION_FINISHED,
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


//struct SStateEventButtonMashingSequence : public SStateEvent
//{
//	enum Type
//	{
//		SystemX = 0,
//	};
//
//	struct Params
//	{
//		EntityId targetLocationId;
//		EntityId lookAtTargetId;
//		float beamingTime;
//
//		// While the Character is struggling to escape from the boss, he will push 
//		// himself back a certain distance (depending on the progression of
//		// the button-mashing) (>= 0.0f) (in meters).
//		float strugglingMovementDistance;
//
//		// The speed with which the Character will move along the 'struggling
//		// line' (>= 0.0f) (in meters / second).
//		float strugglingMovementSpeed;
//	};
//
//	struct CallBacks
//	{
//		Functor1<const float&>	onSequenceStart;
//		Functor1<const float&>  onSequenceUpdate;
//		Functor1<const bool&>   onSequenceEnd;
//	};
//
//	explicit SStateEventButtonMashingSequence(const Type sequenceType, const Params& params, const CallBacks& callbacks)
//		: SStateEvent(CHARACTER_EVENT_BUTTONMASHING_SEQUENCE)
//	{
//		AddData((int) sequenceType);
//		AddData((const void*) &params);
//		AddData((const void*) &callbacks);
//	}
//
//	ILINE const Type GetEnemyType() const { return static_cast<Type>(GetData(0).GetInt()); }
//	ILINE const Params& GetParams() const { return *static_cast<const Params*>(GetData(1).GetPtr()); }
//	ILINE const CallBacks& GetCallBacks() const { return *static_cast<const CallBacks*>(GetData(2).GetPtr()); }
//};



//struct SStateEventSlideKick : public SStateEvent
//{
//	SStateEventSlideKick(CMelee* pMelee)
//		:
//		SStateEvent(CHARACTER_EVENT_SLIDE_KICK)
//	{
//		AddData(pMelee);
//	}
//
//	CMelee* GetMelee() const { return (CMelee*) GetData(1).GetPtr(); }
//};
//
//


//struct SStateEventCoopAnim : public SStateEvent
//{
//	explicit SStateEventCoopAnim(EntityId targetID)
//		:
//		SStateEvent(CHARACTER_EVENT_COOP_ANIMATION_FINISHED)
//	{
//		AddData(static_cast<int>(targetID));
//	}
//
//	ILINE EntityId GetTargetEntityId() const { return static_cast<EntityId> (GetData(0).GetInt()); }
//};


