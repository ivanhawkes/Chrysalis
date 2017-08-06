#pragma once

namespace Chrysalis
{
class CCharacterComponent;
class CCharacterMovementAction;
struct SActorMovementRequest;


class CCharacterStateGround
{
public:
	CCharacterStateGround();

	void OnEnter(CCharacterComponent& Character);
	void OnPrePhysicsUpdate(CCharacterComponent& Character, const SActorMovementRequest& movementRequest, float frameTime, const bool isHeavyWeapon, const bool isLocalPlayer);
	void OnUpdate(CCharacterComponent& Character, float frameTime);
	void OnExit(CCharacterComponent& Character);

private:
	bool m_inertiaIsZero;

	//void ProcessAlignToTarget (const CAutoAimManager& autoAimManager, CCharacterComponent& Character, const IActor* pTarget);
	bool CheckForVaultTrigger(CCharacterComponent & Character, float frameTime);
};
}