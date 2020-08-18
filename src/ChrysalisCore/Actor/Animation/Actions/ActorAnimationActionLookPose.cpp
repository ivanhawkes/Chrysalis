#include "StdAfx.h"

#include "ActorAnimationActionLookPose.h"
#include <CryAnimation/ICryAnimation.h>
#include <CryAnimation/IAnimationPoseModifier.h>


namespace Chrysalis
{
#define LOOKPOSE_FRAGMENTS( x ) \
	x( LookPose )

#define LOOKPOSE_TAGS( x )

#define LOOKPOSE_TAGGROUPS( x )

#define LOOKPOSE_SCOPES( x )

#define LOOKPOSE_CONTEXTS( x )

#define LOOKPOSE_CHANGEFRAGMENT_FRAGMENT_TAGS( x )

#define LOOKPOSE_FRAGMENT_TAGS( x )

MANNEQUIN_USER_PARAMS(SMannequinLookPoseParams, LOOKPOSE_FRAGMENTS, LOOKPOSE_TAGS, LOOKPOSE_TAGGROUPS, LOOKPOSE_SCOPES, LOOKPOSE_CONTEXTS, LOOKPOSE_FRAGMENT_TAGS);


FragmentID FindFragmentId(const SAnimationContext& context)
{
	const SMannequinLookPoseParams* pUserParams = GetMannequinUserParams<SMannequinLookPoseParams>(context);
	CRY_ASSERT(pUserParams != nullptr);

	return pUserParams->fragmentIDs.LookPose;
}


CActorAnimationActionLookPose::CActorAnimationActionLookPose() :
	CAnimationAction(EActorActionPriority::eAAP_Lowest, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY, IAction::NoAutoBlendOut | IAction::Interruptable)
{
}


void CActorAnimationActionLookPose::OnInitialise()
{
	CAnimationAction::OnInitialise();

	const FragmentID fragmentId = FindFragmentId(*m_context);
	CRY_ASSERT(fragmentId != FRAGMENT_ID_INVALID);
	SetFragment(fragmentId);
}


void CActorAnimationActionLookPose::Install()
{
	CAnimationAction::Install();

	IScope& rootScope = GetRootScope();
	ICharacterInstance* pCharacterInstance = rootScope.GetCharInst();
	CRY_ASSERT(pCharacterInstance);
	if (!pCharacterInstance)
	{
		return;
	}

	ISkeletonPose* pSkeletonPose = pCharacterInstance->GetISkeletonPose();
	CRY_ASSERT(pSkeletonPose);

	IAnimationPoseBlenderDir* pPoseBlenderLook = pSkeletonPose->GetIPoseBlenderLook();
	CRY_ASSERT(pPoseBlenderLook);
	if (!pPoseBlenderLook)
	{
		return;
	}

	const uint32 lookPoseAnimationLayer = rootScope.GetBaseLayer();
	pPoseBlenderLook->SetLayer(lookPoseAnimationLayer);
}


IAction::EStatus CActorAnimationActionLookPose::Update(float timePassed)
{
	CAnimationAction::Update(timePassed);

	// Update the fragments and tags if they are different.
	const IScope& rootScope = GetRootScope();
	if (rootScope.IsDifferent(m_fragmentID, m_fragTags))
		SetFragment(m_fragmentID, m_fragTags);

#ifdef DEBUG
	CryWatch("CActorAnimationActionLookPose Update");
#endif

	return m_eStatus;
}


bool CActorAnimationActionLookPose::IsSupported(const SAnimationContext& context)
{
	const FragmentID fragmentId = FindFragmentId(context);
	const bool isSupported = (fragmentId != FRAGMENT_ID_INVALID);

	return isSupported;
}
}