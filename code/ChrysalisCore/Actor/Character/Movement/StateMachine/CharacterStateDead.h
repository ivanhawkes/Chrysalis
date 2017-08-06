#pragma once

class CCharacterComponent;
class CCharacterStateMachine;


namespace Chrysalis
{
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

	void OnEnter(CCharacterComponent& Character);
	void OnLeave(CCharacterComponent& Character);
	void OnPrePhysicsUpdate(CCharacterComponent& Character, const SActorMovementRequest& movementRequest, float frameTime);
	void OnUpdate(CCharacterComponent& Character, const CCharacterStateDead::UpdateCtx& updateCtx);
	void Serialize(TSerialize& serializer);

protected:
	void UpdateAICorpseStatus(CCharacterComponent& Character, const CCharacterStateDead::UpdateCtx& updateCtx);

private:
	float m_swapToCorpseTimeout;
	EAICorpseUpdateStatus m_corpseUpdateStatus;
};
}