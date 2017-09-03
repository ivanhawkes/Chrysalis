#pragma once

struct SActorMovementRequest;
struct pe_status_living;
struct SCharacterMoveRequest;


namespace Chrysalis
{
class IActorComponent;
struct SStateEvent;
struct SActorPhysics;
struct IItem;


class CActorStateUtility
{
public:

	/**
	Initializes the move request.
	
	\param [in,out]	acRequest The animated character movement request.
	**/
	static void InitializeMoveRequest(SCharacterMoveRequest& acRequest);


	/**
	Process the requested rotation for the actorComponent. This will take the two movement request types and finish
	processing their rotation component. The entity is rotated at this point. The animated character component request is
	fed the inverse of that rotation, but it isn't applied until the FinalizeMovementRequest function.
	
	\param [in,out]	actorComponentComponent The actorComponent.
	\param 		   	movementRequest		    The actorComponent movement request.
	\param [in,out]	acRequest			    The animated character movement request.
	**/
	static void ProcessRotation(IActorComponent& actorComponentComponent, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest);


	/**
	The movement request data is dispatched to the animated character system before initialising the system to start
	over for the next frame.
	
	\param [in,out]	actorComponentComponent The actorComponent.
	\param 		   	movementRequest		    The movement request.
	\param [in,out]	acRequest			    The AC request.
	**/
	static void FinalizeMovementRequest(IActorComponent& actorComponentComponent, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest);


	/**
	Calculates the ground or jump movement.
	
	\param 		   	actorComponent	   The actor component.
	\param 		   	movementRequest    The movement request.
	\param 		   	bBigWeaponRestrict The big weapon restrict.
	\param [in,out]	move			   The move.
	**/
	static void CalculateGroundOrJumpMovement(const IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, const bool bBigWeaponRestrict, Vec3 &move);

	// Try and determine if a jump should be allowed.
	// #TODO: improve this to handle jump tests first.
	static bool IsJumpAllowed(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest);

	// Does the physics system report this actorComponent as being on the ground?
	static bool IsOnGround(IActorComponent& actorComponent);

	// The actorComponent is set to allow flying.
	static void PhySetFly(IActorComponent& actorComponent);

	// The actorComponent is set to no longer allow flying, and a new gravity value is set for them.
	static void PhySetNoFly(IActorComponent& actorComponent, const Vec3& gravity);

	// Try and determine if a actorComponent should be allowed to sprint.
	static bool IsSprintingAllowed(const IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, IItem* pCurrentActorItem);

	// Apply falling damage based on falling distance.
	static void ApplyFallDamage(IActorComponent& actorComponent, const float startFallingHeight, const float startJumpHeight);

	// #TODO: This seems a little trivial to break out in utils.
	static void ChangeStance(IActorComponent& actorComponent, const SStateEvent& event);

	// #TODO: only used in actorComponent state ground machine.
	static void RestorePhysics(IActorComponent& actorComponent);

	// #TODO: only used in character.cpp and function unknown at present.
	static void UpdatePhysicsState(IActorComponent& actorComponent, SActorPhysics& actorPhysics, float frameTime);

	// #TODO: Move this to the ladder state machine.
	static void CancelCrouchAndProneInputs(IActorComponent& actorComponent);

private:
	// DO NOT IMPLEMENT
	CActorStateUtility();

	CActorStateUtility(const CActorStateUtility&);


	/**
	Queries the physics engine for the actorComponent's physics state.
	
	\param 		   	actorComponent The actorComponent.
	\param [in,out]	physicsStatus  If non-null, the physics state for this living entity.
	
	\return true if it succeeds, false if it fails.
	**/
	static bool GetPhysicsLivingStatus(const IActorComponent& actorComponent, pe_status_living* physicsStatus);


	/**
	Player movement is subjected to restrictions, based on the current environmental factors, such as terrain, game
	modes, heavy weapons, items being carried.
	
	\param 		   	actorComponent    The actorComponent.
	\param [in,out]	movement		  The movement.
	\param 		   	bigWeaponRestrict The big weapon restrict.
	\param 		   	crouching		  The crouching.
	**/
	static void AdjustMovementForEnvironment(const IActorComponent& actorComponent, Vec3& movement, const bool bigWeaponRestrict, const bool crouching);

	// Try and interpolate remote network actorComponent's positions.
	static void UpdateRemoteInterpolation(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& frameRequest);

	// Works out if armour should affect fall damage and by what factor.
	static bool DoesArmorAbsorptFallDamage(IActorComponent& actorComponent, const float downwardsImpactSpeed, float& absorptedDamageFraction);

	// A method for working out if the actorComponent is moving forward. It's considered unrealiable for multiplayer modes
	// so we need to look at re-writing it.
	static bool IsMovingForward(const IActorComponent& actorComponent, const SActorMovementRequest& movementRequest);
};
}