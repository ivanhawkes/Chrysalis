#pragma once

#include <Actor/IActorEventListener.h>


namespace Chrysalis
{
class CCharacterComponent;

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

	void OnEnter(CCharacterComponent& Character);
	bool OnPrePhysicsUpdate(CCharacterComponent& Character, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime);
	void OnExit(CCharacterComponent& Character, const bool isHeavyWeapon);
	void OnFullSerialize(TSerialize ser, CCharacterComponent& Character);
	void OnJump(CCharacterComponent& Character, const bool isHeavyWeapon, const float fVerticalSpeedModifier);
	void OnFall(CCharacterComponent& Character);

	void SetJumpState(CCharacterComponent &Character, EJumpState jumpState);
	EJumpState GetJumpState() const;
	ILINE const float GetExpectedJumpEndHeight() const { return m_expectedJumpEndHeight; }
	ILINE const float GetStartFallingHeight() const { return m_startFallingHeight; }
	bool GetSprintJump() const { return m_bSprintJump; }

private:
	void StartJump(CCharacterComponent& Character, const bool isHeavyWeapon, const float fVerticalSpeedModifier);

	ILINE bool IsJumping() const { return m_jumpState != JState_None; }
	ILINE void OnRevive() { m_jumpState = JState_None; }
	void OnMPWeaponSelect(CCharacterComponent& Character) { SetJumpState(Character, JState_Jump); }

	void UpdateJumping(CCharacterComponent& Character, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime);
	void UpdateFalling(CCharacterComponent& Character, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime);

	bool UpdateCommon(CCharacterComponent& Character, const bool isHeavyWeapon, const Vec3 &move, float frameTime, Vec3* pDesiredVel);
	bool UpdateCommon(CCharacterComponent& Character, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime, Vec3* pDesiredVel);

	void GetDesiredVelocity(const Vec3 & move, const CCharacterComponent &Character, Vec3* pDesiredVel) const;
	void FinalizeVelocity(CCharacterComponent& Character, const Vec3& newVelocity);

	void Land(CCharacterComponent &Character, const bool isHeavyWeapon, float frameTime);
	void Landed(CCharacterComponent& Character, const bool isHeavyWeapon, float fallSpeed);
	const Vec3 CalculateInAirJumpExtraVelocity(const CCharacterComponent& Character, const Vec3& desiredVelocity) const;

	void OnSpecialMove(CCharacterComponent &Character, IActorEventListener::ESpecialMove specialMove);

	EJumpState m_jumpState;
	mutable float m_jumpLock;
	float m_startFallingHeight;
	float m_expectedJumpEndHeight;
	bool m_firstPrePhysicsUpdate;
	bool m_bSprintJump;
	class CPlayerJump *m_jumpAction;
};
}