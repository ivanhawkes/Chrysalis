#pragma once

class IActorComponent;


namespace Chrysalis
{
struct SActorMovementRequest;

class CActorStateDead
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

	CActorStateDead();
	~CActorStateDead();

	void OnEnter(IActorComponent& actorComponent);
	void OnLeave(IActorComponent& actorComponent);
	void OnPrePhysicsUpdate(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, float frameTime);
	void OnUpdate(IActorComponent& actorComponent, const CActorStateDead::UpdateCtx& updateCtx);
	void Serialize(TSerialize& serializer);

protected:
	void UpdateAICorpseStatus(IActorComponent& actorComponent, const CActorStateDead::UpdateCtx& updateCtx);

private:
	float m_swapToCorpseTimeout;
	EAICorpseUpdateStatus m_corpseUpdateStatus;
};
}