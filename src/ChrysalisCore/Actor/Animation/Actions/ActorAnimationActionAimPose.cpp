#include "StdAfx.h"

#include "ActorAnimationActionAimPose.h"
#include <CryAnimation/ICryAnimation.h>
#include <CryAnimation/IAnimationPoseModifier.h>


namespace Chrysalis
{
#define AIMPOSE_FRAGMENTS( x ) \
	x( AimPose )

#define AIMPOSE_TAGS( x )

#define AIMPOSE_TAGGROUPS( x )

#define AIMPOSE_SCOPES( x )

#define AIMPOSE_CONTEXTS( x )

#define AIMPOSE_CHANGEFRAGMENT_FRAGMENT_TAGS( x )

#define AIMPOSE_FRAGMENT_TAGS( x )

MANNEQUIN_USER_PARAMS(SMannequinAimPoseParams, AIMPOSE_FRAGMENTS, AIMPOSE_TAGS, AIMPOSE_TAGGROUPS, AIMPOSE_SCOPES, AIMPOSE_CONTEXTS, AIMPOSE_FRAGMENT_TAGS);


FragmentID CActorAnimationActionAimPose::FindFragmentId(const SAnimationContext& context)
{
	const SMannequinAimPoseParams* pUserParams = GetMannequinUserParams<SMannequinAimPoseParams>(context);
	CRY_ASSERT(pUserParams != nullptr);

	return pUserParams->fragmentIDs.AimPose;
}


CActorAnimationActionAimPose::CActorAnimationActionAimPose() :
	CAnimationAction(EActorActionPriority::eAAP_Lowest, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY, IAction::NoAutoBlendOut | IAction::Interruptable)
{
}


void CActorAnimationActionAimPose::OnInitialise()
{
	CAnimationAction::OnInitialise();

	const FragmentID fragmentId = FindFragmentId(*m_context);
	CRY_ASSERT(fragmentId != FRAGMENT_ID_INVALID);
	SetFragment(fragmentId);
}


void CActorAnimationActionAimPose::Install()
{
	CAnimationAction::Install();

	InitialiseAimPoseBlender();
}


void CActorAnimationActionAimPose::InitialiseAimPoseBlender()
{
	IScope& rootScope = GetRootScope();
	ICharacterInstance* pCharacterInstance = rootScope.GetCharInst();
	CRY_ASSERT(pCharacterInstance);
	if (!pCharacterInstance)
	{
		return;
	}

	ISkeletonPose* pSkeletonPose = pCharacterInstance->GetISkeletonPose();
	CRY_ASSERT(pSkeletonPose);

	IAnimationPoseBlenderDir* pPoseBlenderAim = pSkeletonPose->GetIPoseBlenderAim();
	CRY_ASSERT(pPoseBlenderAim);
	if (!pPoseBlenderAim)
	{
		return;
	}

	const uint32 aimPoseAnimationLayer = rootScope.GetBaseLayer();
	pPoseBlenderAim->SetLayer(aimPoseAnimationLayer);
}


IAction::EStatus CActorAnimationActionAimPose::Update(float timePassed)
{
	CAnimationAction::Update(timePassed);

	// Update the fragments and tags if they are different.
	const IScope& rootScope = GetRootScope();
	if (rootScope.IsDifferent(m_fragmentID, m_fragTags))
		SetFragment(m_fragmentID, m_fragTags);

	return m_eStatus;
}


bool CActorAnimationActionAimPose::IsSupported(const SAnimationContext& context)
{
	const FragmentID fragmentId = FindFragmentId(context);
	const bool isSupported = (fragmentId != FRAGMENT_ID_INVALID);

	return isSupported;
}
}