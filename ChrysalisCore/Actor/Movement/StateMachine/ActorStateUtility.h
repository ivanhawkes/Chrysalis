#pragma once

class CCharacter;
class CActor;
struct SActorMovementRequest;
struct pe_status_living;
struct SCharacterMoveRequest;
struct SStateEvent;
struct SActorPhysics;
struct IItem;


class CCharacterStateUtil
{
public:
	/**
	Initializes the move request.

	\param [in,out]	acRequest The animated character movement request.
	**/
	static void InitializeMoveRequest(SCharacterMoveRequest& acRequest);


	/**
	Process the requested rotation for the actor. This will take the two movement request types and finish
	processing their rotation component. The entity is rotated at this point. The animated character component request is
	fed the inverse of that rotation, but it isn't applied until the FinalizeMovementRequest function.
	
	\param [in,out]	actor	  The actor.
	\param	movementRequest   The actor movement request.
	\param [in,out]	acRequest The animated character movement request.
	**/
	static void ProcessRotation(CActor& actor, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest);


	/**
	The movement request data is dispatched to the animated character system before initialising the system to start
	over for the next frame.
	
	\param [in,out]	actor	  The actor.
	\param	movementRequest   The movement request.
	\param [in,out]	acRequest The AC request.
	**/
	static void FinalizeMovementRequest(CActor& actor, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest);


	
	
	static void CalculateGroundOrJumpMovement(const CActor& actor, const SActorMovementRequest& movementRequest, const bool bBigWeaponRestrict, Vec3 &move);

	// Try and determine if a jump should be allowed.
	// #TODO: improve this to handle jump tests first.
	static bool IsJumpAllowed(CActor& actor, const SActorMovementRequest& movementRequest);

	// Does the physics system report this actor as being on the ground?
	static bool IsOnGround(CActor& actor);

	// The actor is set to allow flying.
	static void PhySetFly(CActor& actor);

	// The actor is set to no longer allow flying, and a new gravity value is set for them.
	static void PhySetNoFly(CActor& actor, const Vec3& gravity);

	// Try and determine if a actor should be allowed to sprint.
	static bool IsSprintingAllowed(const CActor& actor, const SActorMovementRequest& movementRequest, IItem* pCurrentActorItem);

	// Apply falling damage based on falling distance.
	static void ApplyFallDamage(CActor& actor, const float startFallingHeight, const float startJumpHeight);

	// #TODO: This seems a little trivial to break out in utils.
	static void ChangeStance(CActor& actor, const SStateEvent& event);

	// #TODO: only used in actor state ground machine.
	static void RestorePhysics(CActor& actor);

	// #TODO: only used in character.cpp and function unknown at present.
	static void UpdatePhysicsState(CActor& actor, SActorPhysics& actorPhysics, float frameTime);

	// #TODO: Move this to the ladder state machine.
	static void CancelCrouchAndProneInputs(CActor& actor);

private:
	// DO NOT IMPLEMENT
	CCharacterStateUtil();

	CCharacterStateUtil(const CCharacterStateUtil&);
	CCharacterStateUtil& operator=(const CCharacterStateUtil&);


	/**
	Queries the physics engine for the actor's physics state.
	
	\param	actor				  The actor.
	\param [in,out]	physicsStatus If non-null, the physics state for this living entity.
	
	\return true if it succeeds, false if it fails.
	**/
	static bool GetPhysicsLivingStatus(const CActor& actor, pe_status_living* physicsStatus);


	/**
	Player movement is subjected to restrictions, based on the current environmental factors, such as terrain, game
	modes, heavy weapons, items being carried.
	
	\param	actor			  The actor.
	\param [in,out]	movement  The movement.
	\param	bigWeaponRestrict The big weapon restrict.
	\param	crouching		  The crouching.
	**/
	static void AdjustMovementForEnvironment(const CActor& actor, Vec3& movement, const bool bigWeaponRestrict, const bool crouching);

	// Try and interpolate remote network actor's positions.
	static void UpdateRemoteInterpolation(CActor& actor, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& frameRequest);

	// Works out if armour should affect fall damage and by what factor.
	static bool DoesArmorAbsorptFallDamage(CActor& actor, const float downwardsImpactSpeed, float& absorptedDamageFraction);

	// A method for working out if the actor is moving forward. It's considered unrealiable for multiplayer modes
	// so we need to look at re-writing it.
	static bool IsMovingForward(const CActor& actor, const SActorMovementRequest& movementRequest);
};
