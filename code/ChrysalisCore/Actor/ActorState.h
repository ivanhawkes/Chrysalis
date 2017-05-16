/**
\file	Actor\ActorState.h

Provides a place to store the present state of an actor.
*/
#pragma once

#include <CryGame/CoherentValue.h>
#include <CryEntitySystem/IEntity.h>
#include <ICryMannequin.h>
#include <CryAISystem/IAgent.h>
#include <Utility/AutoEnum.h>
#include <IAnimationGraph.h>


typedef TBitfield TActorStatusFlags;

// #TODO: can this just be a regular enum?

#define ActorStatusFlagList(func)				\
	func(kActorStatus_onLadder)                 \
	func(kActorStatus_stealthKilling)           \
	func(kActorStatus_stealthKilled)            \
	func(kActorStatus_attemptingStealthKill)    \
	func(kActorStatus_pickAndThrow)             \
	func(kActorStatus_vaultLedge)               \
	func(kActorStatus_swimming)                 \
	func(kActorStatus_linkedToVehicle)          \

AUTOENUM_BUILDFLAGS_WITHZERO(ActorStatusFlagList, kActorStatus_none);


/** Present state for an actor entity. */
struct SActorState
{
	struct SItemExchangeStats
	{
		EntityId switchingToItemID;
		bool keepHistory;

		// Used only when the character is using an action controller.
		// #TODO: find out what this means, in fact, what this whole struct is used for.
		bool switchThisFrame;

		SItemExchangeStats() : switchingToItemID(0), keepHistory(false), switchThisFrame(false) {}
	};


	SActorState()
	{
		memset(this, 0, sizeof(*this));
	}

	/** Duration the character has been in the air. */
	float durationInAir;

	/** Duration the character has been on the ground. */
	float durationOnGround;

	/**
	Enables status flags.

	\param	reason	The reason.
	*/
	void EnableStatusFlags(TActorStatusFlags reason) { m_actorStatusFlags |= reason; }


	/**
	Disables status flags.

	\param	reason	The reason.
	*/
	void DisableStatusFlags(TActorStatusFlags reason) { m_actorStatusFlags &= ~reason; }


	/**
	Query to see if any of the status flags are set.

	\param	reasons	The reasons.

	\return	true if any status flags, false if not.
	*/
	bool HasAnyStatusFlags(TActorStatusFlags reasons = ~0) const { return (m_actorStatusFlags & reasons) != kActorStatus_none; }


	/**
	Query to see if all the status flags are set.

	\param	reasons	The reasons.

	\return	true if all status flags, false if not.
	*/
	bool HasAllStatusFlags(TActorStatusFlags reasons) const { return (m_actorStatusFlags & reasons) == reasons; }


	/**
	Gets status flags.

	\return	The status flags.
	*/
	TActorStatusFlags GetStatusFlags() const { return m_actorStatusFlags; }

private:
	// #TODO: can this be a CCryFlags<uint32> or similar instead?
	TActorStatusFlags m_actorStatusFlags;

	/*
	public:
	float speedFlat;

	// #TODO: might be deprecated - find out what it does, if anything.
	int movementDir;

	float inFiring;
	float maxAirSpeed; //The maximum speed the actor has reached over the duration of their current jump/fall. Set to 0 when on the ground.

	bool isGrabbed;
	bool isRagDoll;
	bool isInBlendRagdoll; // To signal when the actor is in the FallNPlay Animation Graph state
	bool wasHit;		// to signal if the player is in air due to an impulse

	bool bStealthKilling;
	bool bStealthKilled;
	bool bAttemptingStealthKill;

	EntityId spectacularKillPartner;

	bool isHidden;

	EntityId mountedWeaponID;

	SItemExchangeStats exchangeItemStats;

	float feetHeight [2];*/
};


// #TODO: Not sure these declarations are really suitable for this header. Look into a better organisation.
// #TODO: Isn't this better named as SLinkStates?

struct IVehicle;

/** Provides a method of tracking if an actor is linked to another entity, and which entity. */
struct SLinkStats
{
	enum EFlags
	{
		LINKED_FREELOOK = (1 << 0),
		LINKED_VEHICLE = (1 << 1),
	};

	/** Which entity are we linked to? */
	EntityId linkID;

	/** The flags. */
	uint32 flags;

	SLinkStats() : linkID(0), flags(0)
	{
	}

	SLinkStats(EntityId eID, uint32 flgs = 0) : linkID(eID), flags(flgs)
	{
	}

	ILINE void UnLink()
	{
		linkID = INVALID_ENTITYID;
		flags = 0;
	}

	ILINE bool CanMove()
	{
		return (!linkID);
	}

	ILINE bool CanDoIK()
	{
		return (!linkID);
	}

	ILINE bool CanMoveCharacter()
	{
		return (!linkID);
	}

	ILINE IEntity* GetLinked()
	{
		if (!linkID)
			return NULL;
		else
		{
			IEntity* pEnt = gEnv->pEntitySystem->GetEntity(linkID);

			// If the entity doesnt exist anymore forget about it.
			if (!pEnt)
				UnLink();

			return pEnt;
		}
	}

	IVehicle* GetLinkedVehicle();

	void Serialize(TSerialize ser)
	{
		assert(ser.GetSerializationTarget() != eST_Network);
		ser.BeginGroup("LinkStats");

		// When reading, reset the structure first.
		if (ser.IsReading())
			*this = SLinkStats();

		ser.Value("linkID", linkID);
		ser.Value("flags", flags);

		ser.EndGroup();
	}
};


// #TODO: This definitely doesn't belong here, I just need it defined for now.

struct SAimAccelerationParams
{
	// #TODO: Not sure what sensible default values for these are, so it's zero for now.
	float angle_min { 0.0f };
	float angle_max { 0.0f };
};


/**
This struct is used on a per frame basis to store state information on how we want our actor to move during this
frame update.

Typically the movement controller will fill this struct using the input system requests which are valid during this frame.
Currently this is done during the PrePhysicsUpdate for the actor, which must be prioritised to happen *before* the
PrePhysicsUpdate call on the CAnimatedCharacter - if this is in use.
*/
struct SActorMovementRequest
{
	/** The desired velocity for this frame (meters / second) (in local space). */
	Vec3 desiredVelocity { ZERO };

	/** Angle indicating how much you want to turn in this frame. */
	Ang3 deltaAngles { ZERO };

	/** The look target. */
	Vec3 lookTarget { ZERO };

	/** Use IK to look at the look target? */
	bool useLookIK { false };

	/** The aim target. */
	Vec3 aimTarget { ZERO };

	/** Use IK to aim at the aim target? */
	bool useAimIK { false };

	/** true to request the actor jumps. */
	bool shouldJump { false };

	/** Sprint (speed?) */
	float sprint { 0.0f };

	/** The actor's present stance. */
	EStance stance { STANCE_NULL };

	/** Clear these mannequin tags. */
	TagState mannequinTagsClear { TAG_STATE_EMPTY };

	/** Set these mannequin tags. */
	TagState mannequinTagsSet { TAG_STATE_EMPTY };

	/** true to allow, false to deny strafe. */
	bool allowStrafe { false };

	/** The desired amount of lean. */
	float desiredLean { 0.0f };

	/** The desired amount of peek over. */
	float desiredPeekOver { 0.0f };

	/** Prediction for character states. Not sure what it does yet. */
	SPredictedCharacterStates prediction;

	/** Make a request for cover. */
	SAICoverRequest coverRequest;
};
