#pragma once

#include <CryPhysics/RayCastQueue.h>


namespace Chrysalis
{
class CActorControllerComponent;


class CActorStateSwimWaterTestProxy
{
	enum EProxyInternalState
	{
		eProxyInternalState_OutOfWater = 0,
		eProxyInternalState_PartiallySubmerged,
		eProxyInternalState_Swimming,
	};

public:
	CActorStateSwimWaterTestProxy();
	~CActorStateSwimWaterTestProxy();

	void Reset(bool bCancelRays);

	void OnEnterWater(const CActorControllerComponent& actorControllerComponent);
	void OnExitWater(const CActorControllerComponent& actorControllerComponent);

	void PreUpdateNotSwimming(const CActorControllerComponent& actorControllerComponent, const float frameTime);
	void PreUpdateSwimming(const CActorControllerComponent& actorControllerComponent, const float frameTime);
	void Update(const CActorControllerComponent& actorControllerComponent, const float frameTime);
	void ForceUpdateBottomLevel(const CActorControllerComponent& actorControllerComponent);

	ILINE bool ShouldSwim() const { return m_shouldSwim; }
	ILINE bool IsHeadUnderWater() const { return m_headUnderwater; }
	ILINE bool IsHeadComingOutOfWater() const { return m_headComingOutOfWater; }
	ILINE float GetRelativeBottomDepth() const { return m_relativeBottomLevel; }
	ILINE float GetWaterLevel() const { return m_waterLevel; }
	ILINE float GetRelativeWaterLevel() const { return m_actorWaterLevel; }
	ILINE float GetSwimmingTimer() const { return m_swimmingTimer; }
	ILINE float GetLastRaycastResult() const { return m_lastRayCastResult; }

	ILINE void SetSubmergedFraction(const float submergedFraction) { m_submergedFraction = submergedFraction; }
	ILINE float GetSubmergedFraction() const { return m_submergedFraction; }

	ILINE float GetWaterLevelTimeUpdated() const { return m_timeWaterLevelLastUpdated; }

	ILINE static float GetRayLength() { return s_rayLength; }

private:
	void UpdateWaterLevel(const Vec3& worldReferencePos, const Vec3& CharacterWorldPos, IPhysicalEntity* piPhysEntity);
	void UpdateOutOfWater(const CActorControllerComponent& actorControllerComponent, const float frameTime);
	void UpdateInWater(const CActorControllerComponent& actorControllerComponent, const float frameTime);
	void UpdateSubmergedFraction(const float referenceHeight, const float CharacterHeight, const float waterLevel);

	static Vec3 GetLocalReferencePosition(const CActorControllerComponent& actorControllerComponent);
	bool ShouldSwim(const float referenceHeight) const;

	// Deferred raycast functions.
	void OnRayCastBottomLevelDataReceived(const QueuedRayID& rayID, const RayCastResult& result);
	ILINE bool IsWaitingForBottomLevelResults() const { return (m_bottomLevelRayID != 0); }
	void RayTestBottomLevel(const CActorControllerComponent& actorControllerComponent, const Vec3& referencePosition, float maxRelevantDepth);
	void CancelPendingRays();

	// Debug
#if !defined(_RELEASE)
	void DebugDraw(const CActorControllerComponent& actorControllerComponent, const Vec3& referencePosition);
#endif

	EProxyInternalState m_internalState;
	EProxyInternalState m_lastInternalState;
	Vec3 m_lastWaterLevelCheckPosition;
	float m_submergedFraction;
	float m_bottomLevel;
	float m_relativeBottomLevel;
	float m_waterLevel;
	float m_actorWaterLevel;
	float m_swimmingTimer;
	float m_timeWaterLevelLastUpdated;
	float m_lastRayCastResult;
	bool m_headUnderwater;
	bool m_headComingOutOfWater;
	bool m_shouldSwim;
	QueuedRayID m_bottomLevelRayID;
	static float s_rayLength;
};
}