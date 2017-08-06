#pragma once

namespace Chrysalis
{
class CCharacterComponent;
struct SActorMovementRequest;


class CCharacterStateFly
{
public:
	CCharacterStateFly();

	void OnEnter(CCharacterComponent& Character);
	bool OnPrePhysicsUpdate(CCharacterComponent& Character, const SActorMovementRequest& movementRequest, float frameTime);
	void OnExit(CCharacterComponent& Character);

private:

	uint8 m_flyMode;
	float m_flyModeDisplayTime;
	void ProcessFlyMode(CCharacterComponent& Character, const SActorMovementRequest& movementRequest);

};
}