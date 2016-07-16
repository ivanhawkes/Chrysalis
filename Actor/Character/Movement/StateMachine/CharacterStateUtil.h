#pragma once

class CCharacter;
struct SActorMovementRequest;
struct pe_status_living;
struct SCharacterMoveRequest;
struct SStateEvent;
struct SActorPhysics;
struct IItem;


class CCharacterStateUtil
{
public:
	static void CalculateGroundOrJumpMovement(const CCharacter& character, const SActorMovementRequest& movementRequest, const bool bBigWeaponRestrict, Vec3 &move);
	static bool IsOnGround(CCharacter& character);
	static bool GetPhysicsLivingStat(const CCharacter& character, pe_status_living *livStat);
	static void AdjustMovementForEnvironment(const CCharacter& character, Vec3& movement, const bool bigWeaponRestrict, const bool crouching);
	static void PhySetFly(CCharacter& character);
	static void PhySetNoFly(CCharacter& character, const Vec3& gravity);
	static bool ShouldJump(CCharacter& character, const SActorMovementRequest& movementRequest);
	static void RestoreCharacterPhysics(CCharacter& character);
	static void UpdateCharacterPhysicsStats(CCharacter& character, SActorPhysics& actorPhysics, float frameTime);
	static void UpdateRemoteCharactersInterpolation(CCharacter& character, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& frameRequest);
	static bool IsMovingForward(const CCharacter& character, const SActorMovementRequest& movementRequest);
	static bool ShouldSprint(const CCharacter& character, const SActorMovementRequest& movementRequest, IItem* pCurrentCharacterItem);
	static void ApplyFallDamage(CCharacter& character, const float startFallingHeight, const float startJumpHeight);
	static bool DoesArmorAbsorptFallDamage(CCharacter& character, const float downwardsImpactSpeed, float& absorptedDamageFraction);
	static void CancelCrouchAndProneInputs(CCharacter& character);
	static void ChangeStance(CCharacter& character, const SStateEvent& event);


	static void ProcessRotation(CCharacter& character, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest);

	static void InitializeMoveRequest(SCharacterMoveRequest& frameRequest);
	static void FinalizeMovementRequest(CCharacter& character, const SActorMovementRequest& movementRequest, SCharacterMoveRequest& acRequest);

private:
	// DO NOT IMPLEMENT
	CCharacterStateUtil();

	CCharacterStateUtil(const CCharacterStateUtil&);
	CCharacterStateUtil& operator=(const CCharacterStateUtil&);
};
