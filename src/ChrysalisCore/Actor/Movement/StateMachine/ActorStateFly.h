#pragma once

namespace Chrysalis
{
class CActorControllerComponent;


class CActorStateFly
{
public:
	CActorStateFly();

	void OnEnter(CActorControllerComponent& actorControllerComponent);
	bool OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent, float frameTime);
	void OnExit(CActorControllerComponent& actorControllerComponent);

private:

	uint8 m_flyMode;
	float m_flyModeDisplayTime;
	void ProcessFlyMode(CActorControllerComponent& actorControllerComponent);

};
}