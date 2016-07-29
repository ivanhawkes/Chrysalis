#pragma once

#include "CharacterStateLedge.h"
#include <Actor/IActorEventListener.h>


class CCharacter;

template<typename HOST>
class CStateHierarchy;


class CCharacterStateJump
{
public:

	enum EJumpState
	{
		JState_None,
		JState_Jump,
		JState_Falling,
		JState_Total
	};

	CCharacterStateJump();
	~CCharacterStateJump();

	void OnEnter(CCharacter& Character);
	bool OnPrePhysicsUpdate(CCharacter& Character, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime);
	void OnExit(CCharacter& Character, const bool isHeavyWeapon);
	void OnFullSerialize(TSerialize ser, CCharacter& Character);
	void OnJump(CCharacter& Character, const bool isHeavyWeapon, const float fVerticalSpeedModifier);
	void OnFall(CCharacter& Character);

	void SetJumpState(CCharacter &Character, EJumpState jumpState);
	EJumpState GetJumpState() const;
	ILINE const float GetExpectedJumpEndHeight() const { return m_expectedJumpEndHeight; }
	ILINE const float GetStartFallingHeight() const { return m_startFallingHeight; }
	bool GetSprintJump() const { return m_bSprintJump; }

private:
	void StartJump(CCharacter& Character, const bool isHeavyWeapon, const float fVerticalSpeedModifier);

	ILINE bool IsJumping() const { return m_jumpState != JState_None; }
	ILINE void OnRevive() { m_jumpState = JState_None; }
	void OnMPWeaponSelect(CCharacter& Character) { SetJumpState(Character, JState_Jump); }

	void UpdateJumping(CCharacter& Character, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime);
	void UpdateFalling(CCharacter& Character, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime);

	bool UpdateCommon(CCharacter& Character, const bool isHeavyWeapon, const Vec3 &move, float frameTime, Vec3* pDesiredVel);
	bool UpdateCommon(CCharacter& Character, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime, Vec3* pDesiredVel);

	void GetDesiredVelocity(const Vec3 & move, const CCharacter &Character, Vec3* pDesiredVel) const;
	void FinalizeVelocity(CCharacter& Character, const Vec3& newVelocity);

	void Land(CCharacter &Character, const bool isHeavyWeapon, float frameTime);
	void Landed(CCharacter& Character, const bool isHeavyWeapon, float fallSpeed);
	const Vec3 CalculateInAirJumpExtraVelocity(const CCharacter& Character, const Vec3& desiredVelocity) const;

	void OnSpecialMove(CCharacter &Character, IActorEventListener::ESpecialMove specialMove);

	EJumpState m_jumpState;
	mutable float m_jumpLock;
	float m_startFallingHeight;
	float m_expectedJumpEndHeight;
	bool m_firstPrePhysicsUpdate;
	bool m_bSprintJump;
	class CPlayerJump *m_jumpAction;
};
