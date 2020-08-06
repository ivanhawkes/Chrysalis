#pragma once

#include <Components/Actor/ActorComponent.h>


namespace Chrysalis
{
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
		_LAST
	};

	CActorStateJump();
	~CActorStateJump();

	void OnEnter(CActorControllerComponent& actorControllerComponent);
	bool OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime);
	void OnExit(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon);
	void OnFullSerialize(TSerialize ser, CActorControllerComponent& actorControllerComponent);
	void OnJump(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, const float fVerticalSpeedModifier);
	void OnFall(CActorControllerComponent& actorControllerComponent);

	void SetJumpState(CActorControllerComponent& actorControllerComponent, EJumpState jumpState);
	EJumpState GetJumpState() const;
	ILINE const float GetExpectedJumpEndHeight() const { return m_expectedJumpEndHeight; }
	ILINE const float GetStartFallingHeight() const { return m_startFallingHeight; }
	bool GetSprintJump() const { return m_bSprintJump; }

private:
	void StartJump(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, const float fVerticalSpeedModifier);

	ILINE bool IsJumping() const { return m_jumpState != JState_None; }
	ILINE void OnRevive() { m_jumpState = JState_None; }
	void OnMPWeaponSelect(CActorControllerComponent& actorControllerComponent) { SetJumpState(actorControllerComponent, JState_Jump); }

	void UpdateJumping(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime);
	void UpdateFalling(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime);

	bool UpdateCommon(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, const Vec3 &move, float frameTime, Vec3* pDesiredVel);
	bool UpdateCommon(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime, Vec3* pDesiredVel);

	void GetDesiredVelocity(const Vec3 & move, const CActorControllerComponent& actorControllerComponent, Vec3* pDesiredVel) const;
	void FinalizeVelocity(CActorControllerComponent& actorControllerComponent, const Vec3& newVelocity);

	void Land(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float frameTime);
	void Landed(CActorControllerComponent& actorControllerComponent, const bool isHeavyWeapon, float fallSpeed);
	const Vec3 CalculateInAirJumpExtraVelocity(const CActorControllerComponent& actorControllerComponent, const Vec3& desiredVelocity) const;

	EJumpState m_jumpState;
	mutable float m_jumpLock;
	float m_startFallingHeight;
	float m_expectedJumpEndHeight;
	bool m_firstPrePhysicsUpdate;
	bool m_bSprintJump;
	class CPlayerJump *m_jumpAction;
};
}