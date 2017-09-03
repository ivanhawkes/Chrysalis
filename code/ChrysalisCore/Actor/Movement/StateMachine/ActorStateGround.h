#pragma once

namespace Chrysalis
{
class IActorComponent;
class CCharacterMovementAction;
struct SActorMovementRequest;


class CActorStateGround
{
public:
	CActorStateGround();

	void OnEnter(IActorComponent& actorComponent);
	void OnPrePhysicsUpdate(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, float frameTime, const bool isHeavyWeapon, const bool isLocalPlayer);
	void OnUpdate(IActorComponent& actorComponent, float frameTime);
	void OnExit(IActorComponent& actorComponent);

private:
	bool m_inertiaIsZero;

	//void ProcessAlignToTarget (const CAutoAimManager& autoAimManager, IActorComponent& actorComponent, const IActor* pTarget);
	bool CheckForVaultTrigger(IActorComponent & Character, float frameTime);
};
}