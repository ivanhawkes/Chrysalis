#include "StdAfx.h"

#include "ProceduralContextLook.h"
#include <Components/Actor/ActorComponent.h>


namespace Chrysalis
{
CRYREGISTER_CLASS(CProceduralContextLook);


void CProceduralContextLook::Initialise(IEntity& entity, IActionController& actionController)
{
	IProceduralContext::Initialise(entity, actionController);

	InitialisePoseBlenderLook();
	InitialiseGameLookTarget();
}


void CProceduralContextLook::InitialisePoseBlenderLook()
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
		if (m_pPoseBlenderLook = pSkeletonPose->GetIPoseBlenderLook())
		{
			// TODO: These used to be variables that could take values from a Lua script. We should add a way to customise
			// their values now Lua is deprecated.
			const float polarCoordinatesSmoothTimeSeconds = 0.1f;
			const float polarCoordinatesMaxYawDegreesPerSecond = 360.f;
			const float polarCoordinatesMaxPitchDegreesPerSecond = 360.f;
			const float fadeInSeconds = 0.25f;
			const float fadeOutSeconds = 0.25f;
			const float fadeOutMinDistance = 0.f;

			m_pPoseBlenderLook->SetPolarCoordinatesSmoothTimeSeconds(polarCoordinatesSmoothTimeSeconds);
			m_pPoseBlenderLook->SetPolarCoordinatesMaxRadiansPerSecond(Vec2(DEG2RAD(polarCoordinatesMaxYawDegreesPerSecond), DEG2RAD(polarCoordinatesMaxPitchDegreesPerSecond)));
			m_pPoseBlenderLook->SetFadeInSpeed(fadeInSeconds);
			m_pPoseBlenderLook->SetFadeOutSpeed(fadeOutSeconds);
			m_pPoseBlenderLook->SetFadeOutMinDistance(fadeOutMinDistance);
			m_pPoseBlenderLook->SetState(false);
		}
	}
}


void CProceduralContextLook::InitialiseGameLookTarget()
{
	CRY_ASSERT(m_entity);

	if (m_pPoseBlenderLook)
	{
		// Set a default target value which is 10 meters in front of the entity.
		m_pPoseBlenderLook->SetTarget(m_entity->GetWorldPos() + m_entity->GetForwardDir() * 10.0f);
	}
}


void CProceduralContextLook::Update(float timePassedSeconds)
{
	if (m_pPoseBlenderLook)
	{
		const bool canLook = (m_isLookingGame && m_isLookingProcClip);
		m_pPoseBlenderLook->SetState(canLook);
		if (canLook)
		{
			m_pPoseBlenderLook->SetTarget(m_gameLookTarget);

#ifdef DEBUG
			//CryWatch("Looking at %0.2f, %0.2f, %0.2f", m_gameLookTarget.x, m_gameLookTarget.y, m_gameLookTarget.z);
#endif
		}
	}
}


void CProceduralContextLook::SetBlendInTime(const float blendInTime)
{
	if (m_pPoseBlenderLook)
		m_pPoseBlenderLook->SetFadeInSpeed(blendInTime);
}


void CProceduralContextLook::SetBlendOutTime(const float blendOutTime)
{
	if (m_pPoseBlenderLook)
		m_pPoseBlenderLook->SetFadeOutSpeed(blendOutTime);
}


void CProceduralContextLook::SetFovRadians(const float fovRadians)
{
	if (m_pPoseBlenderLook)
		m_pPoseBlenderLook->SetFadeoutAngle(fovRadians);
}
}