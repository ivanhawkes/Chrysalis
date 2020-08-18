#include "StdAfx.h"

#include "ActorAnimationActionAiming.h"


namespace Chrysalis
{
#define AIMING_FRAGMENTS( x ) \
	x( Aiming )

#define AIMING_TAGS( x )

#define AIMING_TAGGROUPS( x )

#define AIMING_SCOPES( x )

#define AIMING_CONTEXTS( x )

#define AIMING_CHANGEFRAGMENT_FRAGMENT_TAGS( x )

#define AIMING_FRAGMENT_TAGS( x )

MANNEQUIN_USER_PARAMS(SMannequinAimingParams, AIMING_FRAGMENTS, AIMING_TAGS, AIMING_TAGGROUPS, AIMING_SCOPES, AIMING_CONTEXTS, AIMING_FRAGMENT_TAGS);


FragmentID CActorAnimationActionAiming::FindFragmentId(const SAnimationContext& context)
{
	const SMannequinAimingParams* pUserParams = GetMannequinUserParams<SMannequinAimingParams>(context);
	CRY_ASSERT(pUserParams != nullptr);

	return pUserParams->fragmentIDs.Aiming;
}


CActorAnimationActionAiming::CActorAnimationActionAiming() :
	CAnimationAction(EActorActionPriority::eAAP_Lowest, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY, IAction::NoAutoBlendOut | IAction::Interruptable)
{
}


void CActorAnimationActionAiming::OnInitialise()
{
	CAnimationAction::OnInitialise();

	const FragmentID fragmentId = FindFragmentId(*m_context);
	CRY_ASSERT(fragmentId != FRAGMENT_ID_INVALID);
	SetFragment(fragmentId);
}


IAction::EStatus CActorAnimationActionAiming::Update(float timePassed)
{
	CAnimationAction::Update(timePassed);

	// Update the fragments and tags if they are different.
	const IScope& rootScope = GetRootScope();
	if (rootScope.IsDifferent(m_fragmentID, m_fragTags))
		SetFragment(m_fragmentID, m_fragTags);

	return m_eStatus;
}


bool CActorAnimationActionAiming::IsSupported(const SAnimationContext& context)
{
	const FragmentID fragmentId = FindFragmentId(context);
	const bool isSupported = (fragmentId != FRAGMENT_ID_INVALID);

	return isSupported;
}
}