#include "StdAfx.h"

#include "ProceduralContextAim.h"
#include <Components/Actor/ActorComponent.h>


namespace Chrysalis
{
CRYREGISTER_CLASS(CProceduralContextAim);


void CProceduralContextAim::Initialise(IEntity& entity, IActionController& actionController)
{
	IProceduralContext::Initialise(entity, actionController);

	InitialisePoseBlenderAim();
	InitialiseGameAimTarget();
}


void CProceduralContextAim::InitialisePoseBlenderAim()
{
	CRY_ASSERT(m_entity);

	// HACK: Attempt to locate an actor component on the entity. If it exists we can get the character instance. This
	// will fail for things that aren't actors, but it's a workaround for the old code which was hard coded to slot 0 -
	// and was failing because of that. 
	ICharacterInstance* pCharacterInstance = nullptr;
	auto pActorComponent = m_entity->GetComponent<CActorComponent>();
	if (pActorComponent)
		pCharacterInstance = pActorComponent->GetCharacter();
	else
		return;

	if (ISkeletonPose* pSkeletonPose = pCharacterInstance->GetISkeletonPose())
	{
		if (m_pPoseBlenderAim = pSkeletonPose->GetIPoseBlenderAim())
		{
			// TODO: These used to be variables that could take values from a Lua script. We should add a way to customise
			// their values now Lua is deprecated.
			const float polarCoordinatesMaxYawDegreesPerSecond = 360.f;
			const float polarCoordinatesMaxPitchDegreesPerSecond = 360.f;
			const float fadeInSeconds = 0.25f;
			const float fadeOutSeconds = 0.25f;
			const float fadeOutMinDistance = 0.f;

			m_defaultPolarCoordinatesMaxSmoothRateRadiansPerSecond = Vec2(DEG2RAD(polarCoordinatesMaxYawDegreesPerSecond), DEG2RAD(polarCoordinatesMaxPitchDegreesPerSecond));
			m_defaultPolarCoordinatesSmoothTimeSeconds = 0.1f;

			m_pPoseBlenderAim->SetPolarCoordinatesSmoothTimeSeconds(m_defaultPolarCoordinatesSmoothTimeSeconds);
			m_pPoseBlenderAim->SetPolarCoordinatesMaxRadiansPerSecond(m_defaultPolarCoordinatesMaxSmoothRateRadiansPerSecond);
			m_pPoseBlenderAim->SetFadeInSpeed(fadeInSeconds);
			m_pPoseBlenderAim->SetFadeOutSpeed(fadeOutSeconds);
			m_pPoseBlenderAim->SetFadeOutMinDistance(fadeOutMinDistance);
			m_pPoseBlenderAim->SetState(false);
		}
	}
}


void CProceduralContextAim::InitialiseGameAimTarget()
{
	CRY_ASSERT(m_entity);

	if (m_pPoseBlenderAim)
	{
		// Set a default target value which is 10 meters in front of the entity.
		m_pPoseBlenderAim->SetTarget(m_entity->GetWorldPos() + m_entity->GetForwardDir() * 10.0f);
	}
}


void CProceduralContextAim::Update(float timePassedSeconds)
{
	if (m_pPoseBlenderAim)
	{
		const bool canAim = (m_isAimingGame && m_isAimingProcClip);
		m_pPoseBlenderAim->SetState(canAim);
		UpdatePolarCoordinatesSmoothingParameters();

		if (canAim)
			m_pPoseBlenderAim->SetTarget(m_aimTarget);
	}
}


void CProceduralContextAim::UpdatePolarCoordinatesSmoothingParameters()
{
	CRY_ASSERT(m_pPoseBlenderAim);

	Vec2 polarCoordinatesMaxSmoothRateRadiansPerSecond = m_defaultPolarCoordinatesMaxSmoothRateRadiansPerSecond;
	float polarCoordinatesSmoothTimeSeconds = m_defaultPolarCoordinatesSmoothTimeSeconds;

	const size_t requestCount = m_polarCoordinatesSmoothingParametersRequestList.GetCount();
	if (requestCount > 0)
	{
		const SPolarCoordinatesSmoothingParametersRequest& request = m_polarCoordinatesSmoothingParametersRequestList.GetRequest(requestCount - 1);

		polarCoordinatesMaxSmoothRateRadiansPerSecond = request.maxSmoothRateRadiansPerSecond;
		polarCoordinatesSmoothTimeSeconds = request.smoothTimeSeconds;
	}

	m_pPoseBlenderAim->SetPolarCoordinatesMaxRadiansPerSecond(polarCoordinatesMaxSmoothRateRadiansPerSecond);
	m_pPoseBlenderAim->SetPolarCoordinatesSmoothTimeSeconds(polarCoordinatesSmoothTimeSeconds);
}


void CProceduralContextAim::SetBlendInTime(const float blendInTime)
{
	if (m_pPoseBlenderAim)
		m_pPoseBlenderAim->SetFadeInSpeed(blendInTime);
}


void CProceduralContextAim::SetBlendOutTime(const float blendOutTime)
{
	if (m_pPoseBlenderAim)
		m_pPoseBlenderAim->SetFadeOutSpeed(blendOutTime);
}


uint32 CProceduralContextAim::RequestPolarCoordinatesSmoothingParameters(const Vec2& maxSmoothRateRadiansPerSecond, const float smoothTimeSeconds)
{
	SPolarCoordinatesSmoothingParametersRequest request;
	request.maxSmoothRateRadiansPerSecond = maxSmoothRateRadiansPerSecond;
	request.smoothTimeSeconds = smoothTimeSeconds;

	return m_polarCoordinatesSmoothingParametersRequestList.AddRequest(request);
}


void CProceduralContextAim::CancelPolarCoordinatesSmoothingParameters(const uint32 requestId)
{
	m_polarCoordinatesSmoothingParametersRequestList.RemoveRequest(requestId);
}
}