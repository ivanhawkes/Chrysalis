#pragma once

class CActorControllerComponent;


namespace Chrysalis
{
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

	void OnEnter(CActorControllerComponent& actorControllerComponent);
	void OnLeave(CActorControllerComponent& actorControllerComponent);
	void OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent, float frameTime);
	void OnUpdate(CActorControllerComponent& actorControllerComponent, const CActorStateDead::UpdateCtx& updateCtx);
	void Serialize(TSerialize& serializer);

protected:
	void UpdateAICorpseStatus(CActorControllerComponent& actorControllerComponent, const CActorStateDead::UpdateCtx& updateCtx);

private:
	float m_swapToCorpseTimeout;
	EAICorpseUpdateStatus m_corpseUpdateStatus;
};
}