#pragma once

struct pe_status_living;


namespace Chrysalis
{
class CActorControllerComponent;
struct SStateEvent;
struct SActorPhysics;
struct IItem;


class CActorStateUtility
{
public:

	/**
	Calculates the ground or jump movement.
	
	\param 		   	actorControllerComponent The actor component.
	\param 		   	bBigWeaponRestrict		 The big weapon restrict.
	\param [in,out]	move					 The move.
	**/
	static void CalculateGroundOrJumpMovement(const CActorControllerComponent& actorControllerComponent, const bool bBigWeaponRestrict, Vec3 &move);

	// Try and determine if a jump should be allowed.
	// #TODO: improve this to handle jump tests first.
	static bool IsJumpAllowed(CActorControllerComponent& actorControllerComponent);

	// Does the physics system report this actorControllerComponent as being on the ground?
	static bool IsOnGround(CActorControllerComponent& actorControllerComponent);

	// The actorControllerComponent is set to allow flying.
	static void PhySetFly(CActorControllerComponent& actorControllerComponent);

	// The actorControllerComponent is set to no longer allow flying, and a new gravity value is set for them.
	static void PhySetNoFly(CActorControllerComponent& actorControllerComponent, const Vec3& gravity);

	// Try and determine if a actorControllerComponent should be allowed to sprint.
	static bool IsSprintingAllowed(const CActorControllerComponent& actorControllerComponent, IItem* pCurrentActorItem);

	// Apply falling damage based on falling distance.
	static void ApplyFallDamage(CActorControllerComponent& actorControllerComponent, const float startFallingHeight, const float startJumpHeight);

	// #TODO: This seems a little trivial to break out in utils.
	static void ChangeStance(CActorControllerComponent& actorControllerComponent, const SStateEvent& event);

	// #TODO: only used in actorControllerComponent state ground machine.
	static void RestorePhysics(CActorControllerComponent& actorControllerComponent);

	// #TODO: only used in character.cpp and function unknown at present.
	static void UpdatePhysicsState(CActorControllerComponent& actorControllerComponent, SActorPhysics& actorPhysics, float frameTime);

	// #TODO: Move this to the ladder state machine.
	static void CancelCrouchAndProneInputs(CActorControllerComponent& actorControllerComponent);

private:
	// DO NOT IMPLEMENT
	CActorStateUtility();

	CActorStateUtility(const CActorStateUtility&);


	/**
	Queries the physics engine for the actorControllerComponent's physics state.
	
	\param 		   	actorControllerComponent The actorControllerComponent.
	\param [in,out]	physicsStatus  If non-null, the physics state for this living entity.
	
	\return true if it succeeds, false if it fails.
	**/
	static bool GetPhysicsLivingStatus(const CActorControllerComponent& actorControllerComponent, pe_status_living* physicsStatus);


	/**
	Player movement is subjected to restrictions, based on the current environmental factors, such as terrain, game
	modes, heavy weapons, items being carried.
	
	\param 		   	actorControllerComponent    The actorControllerComponent.
	\param [in,out]	movement		  The movement.
	\param 		   	bigWeaponRestrict The big weapon restrict.
	\param 		   	crouching		  The crouching.
	**/
	static void AdjustMovementForEnvironment(const CActorControllerComponent& actorControllerComponent, Vec3& movement, const bool bigWeaponRestrict, const bool crouching);

	// Works out if armour should affect fall damage and by what factor.
	static bool DoesArmorAbsorptFallDamage(CActorControllerComponent& actorControllerComponent, const float downwardsImpactSpeed, float& absorptedDamageFraction);

	// A method for working out if the actorControllerComponent is moving forward. It's considered unrealiable for multiplayer modes
	// so we need to look at re-writing it.
	static bool IsMovingForward(const CActorControllerComponent& actorControllerComponent);
};
}