#pragma once

class CCharacter;
struct SActorMovementRequest;


class CCharacterStateFly
{
public:
	CCharacterStateFly();

	void OnEnter(CCharacter& Character);
	bool OnPrePhysicsUpdate(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime);
	void OnExit(CCharacter& Character);

private:

	uint8 m_flyMode;
	float m_flyModeDisplayTime;
	void ProcessFlyMode(CCharacter& Character, const SActorMovementRequest& movementRequest);

};
