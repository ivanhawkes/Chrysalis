#pragma once

class CCharacter;
class CCharacterStateMachine;
struct SActorMovementRequest;


class CCharacterStateDead
{
	enum EAICorpseUpdateStatus
	{
		eCorpseStatus_WaitingForSwap = 0,
		eCorpseStatus_SwapDone
	};

public:

	struct UpdateCtx
	{
		float frameTime;
	};

	CCharacterStateDead();
	~CCharacterStateDead();

	void OnEnter(CCharacter& Character);
	void OnLeave(CCharacter& Character);
	void OnPrePhysicsUpdate(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime);
	void OnUpdate(CCharacter& Character, const CCharacterStateDead::UpdateCtx& updateCtx);
	void Serialize(TSerialize& serializer);

protected:
	void UpdateAICorpseStatus(CCharacter& Character, const CCharacterStateDead::UpdateCtx& updateCtx);

private:
	float m_swapToCorpseTimeout;
	EAICorpseUpdateStatus m_corpseUpdateStatus;
};
