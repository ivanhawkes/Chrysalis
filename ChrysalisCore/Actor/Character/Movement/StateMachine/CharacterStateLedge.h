#pragma once

#include <CryNetwork/ISerialize.h>
#include <Actor/Character/Movement/StateMachine/CharacterStateEvents.h>
#include <Entities/Environment/Ledge/LedgeManager.h>


class CCharacter;
struct SStateEventLedge;
struct SCharacterMoveRequest;
struct IItemParamsNode;


enum ELedgeType
{
	eLT_Any,
	eLT_Wide,
	eLT_Thin,
};


class CCharacterStateLedge
{
public:
	static void SetParamsFromXml(const IItemParamsNode* pParams)
	{
		//ScopedSwitchToGlobalHeap useGlobalHeap;
		GetLedgeGrabbingParams().SetParamsFromXml(pParams);
	}

public:
	CCharacterStateLedge();
	~CCharacterStateLedge();

	ILINE bool IsOnLedge() const { return (m_onLedge); }

	void OnEnter(CCharacter& Character, const SStateEventLedge& ledgeEvent);
	void OnPrePhysicsUpdate(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime);
	void OnAnimFinished(CCharacter &Character);
	void OnExit(CCharacter& Character);

	void Serialize(TSerialize serializer);
	void PostSerialize(CCharacter& Character);

	static bool TryLedgeGrab(CCharacter& Character, const float expectedJumpEndHeight, const float startJumpHeight, const bool bSprintJump, SLedgeTransitionData* pLedgeState, const bool ignoreCharacterMovementWhenFindingLedge);

	static bool CanGrabOntoLedge(const CCharacter& Character);
	static void UpdateNearestGrabbableLedge(const CCharacter& Character, SLedgeTransitionData* pLedgeState, const bool ignoreCharacterMovement);
	static SLedgeTransitionData::EOnLedgeTransition GetBestTransitionToLedge(const CCharacter& Character, const Vec3& refPosition, const LedgeId& ledgeId, SLedgeTransitionData* pLedgeState);

	// Debug drawing of nearest ledge
#ifndef _RELEASE
	static void DebugDraw(const SLedgeTransitionData* pLedgeState);
#endif

private:

	float m_ledgeSpeedMultiplier;
	float m_lastTimeOnLedge;
	Vec3 m_exitVelocity;
	Vec3 m_ledgePreviousPosition;
	Vec3 m_ledgePreviousPositionDiff;
	Quat m_ledgePreviousRotation;
	LedgeId m_ledgeId;
	bool m_onLedge;
	bool m_enterFromGround;
	bool m_enterFromSprint;
	uint8 m_postSerializeLedgeTransition;

	// Parameters to use while blending towards/from a ledge (also used to synch code with animation).
	struct SLedgeBlending
	{
		SLedgeBlending()
			:
			m_qtTargetLocation(ZERO),
			m_forwardDir(ZERO)
		{}

		QuatT m_qtTargetLocation;
		Vec3  m_forwardDir;
	} m_ledgeBlending;


	// Data structs used to collect parameters used for blending in ledge climbing.
	struct SLedgeBlendingParams
	{
		SLedgeBlendingParams() : m_vPositionOffset(ZERO), m_fMoveDuration(0.0f), m_vExitVelocity(0.f, 1.f, 1.f), m_ledgeType(eLT_Any), m_bIsVault(false), m_bIsHighVault(false), m_bKeepOrientation(false), m_bEndFalling(false) {}
		void SetParamsFromXml(const IItemParamsNode* pParams);

		float m_fMoveDuration;
		float m_fCorrectionDuration;
		Vec3 m_vPositionOffset;
		Vec3 m_vExitVelocity;
		ELedgeType m_ledgeType;
		bool m_bIsVault;
		bool m_bIsHighVault;
		bool m_bKeepOrientation;
		bool m_bEndFalling;
	};


	struct SLedgeNearbyParams
	{
		SLedgeNearbyParams() : m_vSearchDir(ZERO), m_fMaxDistance(1.2f), m_fMaxAngleDeviationFromSearchDirInDegrees(45.0f), m_fMaxExtendedAngleDeviationFromSearchDirInDegrees(50.f) {}
		void SetParamsFromXml(const IItemParamsNode* pParams);

		Vec3 m_vSearchDir;
		float m_fMaxDistance;
		float m_fMaxAngleDeviationFromSearchDirInDegrees;
		float m_fMaxExtendedAngleDeviationFromSearchDirInDegrees;
	};


	struct SLedgeGrabbingParams
	{
		SLedgeGrabbingParams() : m_fNormalSpeedUp(1.0f), m_fMobilitySpeedUp(1.5f), m_fMobilitySpeedUpMaximum(1.5f) {}
		void SetParamsFromXml(const IItemParamsNode* pParams);

		float m_fNormalSpeedUp;
		float m_fMobilitySpeedUp;
		float m_fMobilitySpeedUpMaximum;
		SLedgeNearbyParams m_ledgeNearbyParams;
		SLedgeBlendingParams m_grabTransitionsParams [SLedgeTransitionData::eOLT_MaxTransitions];
	};


	static bool CanReachPlatform(const CCharacter& Character, const Vec3& ledgePosition, const Vec3& refPosition);
	static bool IsCharacterMovingTowardsLedge(const CCharacter& Character, const LedgeId& ledgeId);
	static bool IsLedgePointingTowardCharacter(const CCharacter& Character, const LedgeId&	 ledgeId);
	QuatT CalculateLedgeOffsetLocation(const Matrix34& worldMat, const Vec3& vPositionOffset, const bool keepOrientation) const;
	void StartLedgeBlending(CCharacter& Character, const SLedgeBlendingParams &positionParams);
	float GetLedgeGrabSpeed(const CCharacter& Character) const;

private:
	static SLedgeGrabbingParams s_ledgeGrabbingParams;
	static SLedgeGrabbingParams& GetLedgeGrabbingParams();
};
