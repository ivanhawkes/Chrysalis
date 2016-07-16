#pragma once

class CCharacter;
class CCharacterMovementAction;
struct SActorMovementRequest;


class CCharacterStateGround
{
public:
	CCharacterStateGround();

	void OnEnter(CCharacter& Character);
	void OnPrePhysicsUpdate(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime, const bool isHeavyWeapon, const bool isCharacter);
	void OnUpdate(CCharacter& Character, float frameTime);
	void OnExit(CCharacter& Character);

private:
	bool m_inertiaIsZero;

	//void ProcessAlignToTarget (const CAutoAimManager& autoAimManager, CCharacter& Character, const IActor* pTarget);
	bool CheckForVaultTrigger(CCharacter & Character, float frameTime);
};
