#include <StdAfx.h>

#include "ActorAnimationActionInteration.h"
#include "IActorSystem.h"
#include <Actor/Actor.h>


#define INTERACTION_FRAGMENTS( x ) \
	x( Interaction ) \

#define INTERACTION_TAGS( x ) \

#define INTERACTION_TAGGROUPS( x ) \

#define INTERACTION_SCOPES( x ) \

#define INTERACTION_CONTEXTS( x )

#define INTERACTION_FRAGMENT_TAGS( x )

MANNEQUIN_USER_PARAMS(SMannequinInteractionParams, INTERACTION_FRAGMENTS, INTERACTION_TAGS, INTERACTION_TAGGROUPS, INTERACTION_SCOPES, INTERACTION_CONTEXTS, INTERACTION_FRAGMENT_TAGS);


CActorAnimationActionInteraction::CActorAnimationActionInteraction()
	: CAnimationAction(EActorActionPriority::eAAP_Interaction, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY, IAction::FragmentIsOneShot)
{
}


void CActorAnimationActionInteraction::OnInitialise()
{
	CAnimationAction::OnInitialise();

	m_interactionParams = GetMannequinUserParams<SMannequinInteractionParams>(*m_context);
	CRY_ASSERT(m_interactionParams);

	// Set the fragment for animation.
	SetFragment(m_interactionParams->fragmentIDs.Interaction);
}


void CActorAnimationActionInteraction::Install()
{
	CAnimationAction::Install();
}


void CActorAnimationActionInteraction::Enter()
{
	CAnimationAction::Enter();
}


void CActorAnimationActionInteraction::Fail(EActionFailure actionFailure)
{
	CAnimationAction::Fail(actionFailure);
}


void CActorAnimationActionInteraction::Exit()
{
	CAnimationAction::Exit();
}


IAction::EStatus CActorAnimationActionInteraction::UpdatePending(float timePassed)
{
	CAnimationAction::UpdatePending(timePassed);

	return EStatus();
}


IAction::EStatus CActorAnimationActionInteraction::Update(float timePassed)
{
	CAnimationAction::Update(timePassed);

	// Grab the actor in the root scope.
	const IScope& rootScope = GetRootScope();
	CActor& actor = *CActor::GetActor(rootScope.GetEntityId());

	// Update the fragments and tags if they are different.
	if (rootScope.IsDifferent(m_fragmentID, m_fragTags))
	{
		SetFragment(m_fragmentID, m_fragTags);
	}

	return EStatus();
}
