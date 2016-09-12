#pragma once

struct SActorMovementRequest;
class CCharacter;
class CCharacterStateSwimWaterTestProxy;
struct IItemParamsNode;


class CCharacterStateSwim
{
public:
	static void SetParamsFromXml(const IItemParamsNode* pParams)
	{
		//ScopedSwitchToGlobalHeap useGlobalHeap;
		GetSwimParams().SetParamsFromXml(pParams);
	}

	CCharacterStateSwim();

	void OnEnter(CCharacter& Character);
	bool OnPrePhysicsUpdate(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime);
	void OnUpdate(CCharacter& Character, float frameTime);
	void OnExit(CCharacter& Character);

	static void UpdateSoundListener(CCharacter &Character);

	bool DetectJump(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime, float* pVerticalSpeedModifier) const;

private:
	Vec3 m_gravity;
	float m_lastWaterLevel;
	float m_lastWaterLevelTime;
	float m_verticalVelDueToSurfaceMovement; // e.g. waves.
	float m_headUnderWaterTimer;
	bool m_onSurface;
	bool m_enduranceSwimSoundPlaying;

	void PlayEnduranceSound(const EntityId ownerId);
	void StopEnduranceSound(const EntityId ownerId);

	struct CSwimmingParams
	{
		CSwimmingParams()
			: m_swimSpeedSprintSpeedMul(2.5f)
			, m_swimUpSprintSpeedMul(2.0f)
			, m_swimSprintSpeedMul(1.4f)
			, m_stateSwim_animCameraFactor(0.25f)
			, m_swimDolphinJumpDepth(0.1f)
			, m_swimDolphinJumpThresholdSpeed(3.0f)
			, m_swimDolphinJumpSpeedModification(0.0f)
		{}
		void SetParamsFromXml(const IItemParamsNode* pParams);

		float m_swimSpeedSprintSpeedMul;
		float m_swimUpSprintSpeedMul;
		float m_swimSprintSpeedMul;
		float m_stateSwim_animCameraFactor;

		float m_swimDolphinJumpDepth;
		float m_swimDolphinJumpThresholdSpeed;
		float m_swimDolphinJumpSpeedModification;
	};

	static CSwimmingParams s_swimParams;
	static CSwimmingParams& GetSwimParams() { return s_swimParams; }

	// DO NOT IMPLEMENT!
	CCharacterStateSwim(const CCharacterStateSwim&);
	CCharacterStateSwim& operator=(const CCharacterStateSwim&);
};
