#pragma once

#include <ICryMannequin.h>
#include <CryAnimation/ICryAnimation.h>
#include <CryAnimation/IAnimationPoseModifier.h>
#include <CryExtension/CryCreateClassInstance.h>
#include <CryExtension/ClassWeaver.h>
#include <CryExtension/ICryFactoryRegistryImpl.h>
#include <CryExtension/RegFactoryNode.h>
#include "ProceduralContextHelpers.h"


namespace Chrysalis
{
class CProceduralContextAim
	: public IProceduralContext
{
public:
	PROCEDURAL_CONTEXT(CProceduralContextAim, "ProceduralContextAim", "{3ED38A0B-DECB-40E6-BF2E-B2D6380EDDEF}"_cry_guid);

	CProceduralContextAim();
	virtual ~CProceduralContextAim() {}

	// IProceduralContext
	virtual void Initialise(IEntity& entity, IActionController& actionController) override;
	virtual void Update(float timePassedSeconds) override;
	// ~IProceduralContext

	void UpdateGameAimingRequest(const bool aimRequest);
	void UpdateProcClipAimingRequest(const bool aimRequest);

	void UpdateGameAimTarget(const Vec3& aimTarget);

	void SetBlendInTime(const float blendInTime);
	void SetBlendOutTime(const float blendOutTime);

	uint32 RequestPolarCoordinatesSmoothingParameters(const Vec2& maxSmoothRateRadiansPerSecond, const float smoothTimeSeconds);
	void CancelPolarCoordinatesSmoothingParameters(const uint32 requestId);

private:
	void InitialisePoseBlenderAim();
	void InitialiseGameAimTarget();

	void UpdatePolarCoordinatesSmoothingParameters();

private:
	IAnimationPoseBlenderDir* m_pPoseBlenderAim;

	bool m_gameRequestsAiming;
	bool m_procClipRequestsAiming;
	Vec3 m_gameAimTarget;

	Vec2 m_defaultPolarCoordinatesMaxSmoothRateRadiansPerSecond;
	float m_defaultPolarCoordinatesSmoothTimeSeconds;
	struct SPolarCoordinatesSmoothingParametersRequest
	{
		uint32 id;
		Vec2 maxSmoothRateRadiansPerSecond;
		float smoothTimeSeconds;
	};

	typedef ProceduralContextHelpers::CRequestList< SPolarCoordinatesSmoothingParametersRequest > TPolarCoordinatesSmoothingParametersRequestList;
	TPolarCoordinatesSmoothingParametersRequestList m_polarCoordinatesSmoothingParametersRequestList;
};
}