#pragma once

namespace Chrysalis
{
class CActorControllerComponent;
class CCharacterMovementAction;


class CActorStateGround
{
public:
	CActorStateGround();

	void OnEnter(CActorControllerComponent& actorControllerComponent);
	void OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent, float frameTime, const bool isHeavyWeapon, const bool isLocalPlayer);
	void OnUpdate(CActorControllerComponent& actorControllerComponent, float frameTime);
	void OnExit(CActorControllerComponent& actorControllerComponent);

private:
	bool m_inertiaIsZero;

	//void ProcessAlignToTarget (const CAutoAimManager& autoAimManager, CActorControllerComponent& actorControllerComponent, const IActor* pTarget);
	bool CheckForVaultTrigger(CActorControllerComponent & Character, float frameTime);
};
}