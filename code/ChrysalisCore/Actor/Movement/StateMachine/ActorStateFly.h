#pragma once

namespace Chrysalis
{
class IActorComponent;
struct SActorMovementRequest;


class CActorStateFly
{
public:
	CActorStateFly();

	void OnEnter(IActorComponent& actorComponent);
	bool OnPrePhysicsUpdate(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, float frameTime);
	void OnExit(IActorComponent& actorComponent);

private:

	uint8 m_flyMode;
	float m_flyModeDisplayTime;
	void ProcessFlyMode(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest);

};
}