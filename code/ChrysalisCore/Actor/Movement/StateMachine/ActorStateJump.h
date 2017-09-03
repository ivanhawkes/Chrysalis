#pragma once

#include <Actor/IActorEventListener.h>


namespace Chrysalis
{
class IActorComponent;

template<typename HOST>
class CStateHierarchy;


class CActorStateJump
{
public:

	enum EJumpState
	{
		JState_None,
		JState_Jump,
		JState_Falling,
		JState_Total
	};

	CActorStateJump();
	~CActorStateJump();

	void OnEnter(IActorComponent& actorComponent);
	bool OnPrePhysicsUpdate(IActorComponent& actorComponent, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime);
	void OnExit(IActorComponent& actorComponent, const bool isHeavyWeapon);
	void OnFullSerialize(TSerialize ser, IActorComponent& actorComponent);
	void OnJump(IActorComponent& actorComponent, const bool isHeavyWeapon, const float fVerticalSpeedModifier);
	void OnFall(IActorComponent& actorComponent);

	void SetJumpState(IActorComponent& actorComponent, EJumpState jumpState);
	EJumpState GetJumpState() const;
	ILINE const float GetExpectedJumpEndHeight() const { return m_expectedJumpEndHeight; }
	ILINE const float GetStartFallingHeight() const { return m_startFallingHeight; }
	bool GetSprintJump() const { return m_bSprintJump; }

private:
	void StartJump(IActorComponent& actorComponent, const bool isHeavyWeapon, const float fVerticalSpeedModifier);

	ILINE bool IsJumping() const { return m_jumpState != JState_None; }
	ILINE void OnRevive() { m_jumpState = JState_None; }
	void OnMPWeaponSelect(IActorComponent& actorComponent) { SetJumpState(actorComponent, JState_Jump); }

	void UpdateJumping(IActorComponent& actorComponent, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime);
	void UpdateFalling(IActorComponent& actorComponent, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime);

	bool UpdateCommon(IActorComponent& actorComponent, const bool isHeavyWeapon, const Vec3 &move, float frameTime, Vec3* pDesiredVel);
	bool UpdateCommon(IActorComponent& actorComponent, const bool isHeavyWeapon, const SActorMovementRequest& movementRequest, float frameTime, Vec3* pDesiredVel);

	void GetDesiredVelocity(const Vec3 & move, const IActorComponent& actorComponent, Vec3* pDesiredVel) const;
	void FinalizeVelocity(IActorComponent& actorComponent, const Vec3& newVelocity);

	void Land(IActorComponent& actorComponent, const bool isHeavyWeapon, float frameTime);
	void Landed(IActorComponent& actorComponent, const bool isHeavyWeapon, float fallSpeed);
	const Vec3 CalculateInAirJumpExtraVelocity(const IActorComponent& actorComponent, const Vec3& desiredVelocity) const;

	void OnSpecialMove(IActorComponent& actorComponent, IActorEventListener::ESpecialMove specialMove);

	EJumpState m_jumpState;
	mutable float m_jumpLock;
	float m_startFallingHeight;
	float m_expectedJumpEndHeight;
	bool m_firstPrePhysicsUpdate;
	bool m_bSprintJump;
	class CPlayerJump *m_jumpAction;
};
}