#include "StdAfx.h"

#include "ActorAnimationActionLooking.h"


namespace Chrysalis
{
#define LOOKING_FRAGMENTS( x ) \
	x( Looking )

#define LOOKING_TAGS( x )

#define LOOKING_TAGGROUPS( x )

#define LOOKING_SCOPES( x )

#define LOOKING_CONTEXTS( x )

#define LOOKING_CHANGEFRAGMENT_FRAGMENT_TAGS( x )

#define LOOKING_FRAGMENT_TAGS( x )

MANNEQUIN_USER_PARAMS(SMannequinLookingParams, LOOKING_FRAGMENTS, LOOKING_TAGS, LOOKING_TAGGROUPS, LOOKING_SCOPES, LOOKING_CONTEXTS, LOOKING_FRAGMENT_TAGS);


FragmentID CActorAnimationActionLooking::FindFragmentId(const SAnimationContext& context)
{
	const SMannequinLookingParams* pUserParams = GetMannequinUserParams<SMannequinLookingParams>(context);
	CRY_ASSERT(pUserParams != nullptr);

	return pUserParams->fragmentIDs.Looking;
}


CActorAnimationActionLooking::CActorAnimationActionLooking()
	: CAnimationAction(EActorActionPriority::eAAP_Lowest, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY, IAction::NoAutoBlendOut | IAction::Interruptable)
{
}


void CActorAnimationActionLooking::OnInitialise()
{
	CAnimationAction::OnInitialise();

	const FragmentID fragmentId = FindFragmentId(*m_context);
	CRY_ASSERT(fragmentId != FRAGMENT_ID_INVALID);
	SetFragment(fragmentId);
}


IAction::EStatus CActorAnimationActionLooking::Update(float timePassed)
{
	CAnimationAction::Update(timePassed);

	// Update the fragments and tags if they are different.
	const IScope& rootScope = GetRootScope();
	if (rootScope.IsDifferent(m_fragmentID, m_fragTags))
		SetFragment(m_fragmentID, m_fragTags);

#ifdef DEBUG
	CryWatch("CActorAnimationActionLooking Update");
#endif

	return m_eStatus;
}


bool CActorAnimationActionLooking::IsSupported(const SAnimationContext& context)
{
	// HACK: TODO: These tests need to come back in when they stop crashing the game.

	//const FragmentID fragmentId = FindFragmentId(context);
	//const bool isSupported = (fragmentId != FRAGMENT_ID_INVALID);

	//return isSupported;
	
	return true;
}
}