#include <StdAfx.h>

#include "ActorAnimationActionLocomotion.h"
#include <Actor/ActorControllerComponent.h>


namespace Chrysalis
{
#define LOCOMOTION_FRAGMENTS( x ) \
	x( Idle ) \
	x( Move ) \

#define LOCOMOTION_TAGS( x ) \
	x( NoMovement )  /* LocalMoveDirection. */  \
	x( MoveBackward ) \
	x( MoveForward ) \
	x( MoveLeft ) \
	x( MoveRight ) \
	x( NoGait ) /* Gait. */ \
	x( Walk ) \
	x( Run ) \
	x( Sprint ) \

#define LOCOMOTION_TAGGROUPS( x ) \
    x( LocalMoveDirection ) \
    x( Gait ) \

#define LOCOMOTION_SCOPES( x ) \
	//x( FullBody3P )

#define LOCOMOTION_CONTEXTS( x )

#define LOCOMOTION_FRAGMENT_TAGS( x )

MANNEQUIN_USER_PARAMS(SMannequinLocomotionParams, LOCOMOTION_FRAGMENTS, LOCOMOTION_TAGS, LOCOMOTION_TAGGROUPS, LOCOMOTION_SCOPES, LOCOMOTION_CONTEXTS, LOCOMOTION_FRAGMENT_TAGS);


CActorAnimationActionLocomotion::CActorAnimationActionLocomotion()
	: CAnimationAction(EActorActionPriority::eAAP_Movement, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY, IAction::NoAutoBlendOut | IAction::Interruptable)
{
}


void CActorAnimationActionLocomotion::OnInitialise()
{
	CAnimationAction::OnInitialise();

	m_locomotionParams = GetMannequinUserParams<SMannequinLocomotionParams>(*m_context);
	CRY_ASSERT(m_locomotionParams);

	// Idle pose is a reasonable default.
	m_lastFragmentId = m_locomotionParams->fragmentIDs.Idle;
	SetFragment(m_lastFragmentId);
}


void CActorAnimationActionLocomotion::Install()
{
	CAnimationAction::Install();
}


void CActorAnimationActionLocomotion::Enter()
{
	CAnimationAction::Enter();
}


void CActorAnimationActionLocomotion::Fail(EActionFailure actionFailure)
{
	CAnimationAction::Fail(actionFailure);
}


void CActorAnimationActionLocomotion::Exit()
{
	CAnimationAction::Exit();
}


IAction::EStatus CActorAnimationActionLocomotion::UpdatePending(float timePassed)
{
	CAnimationAction::UpdatePending(timePassed);

	// TODO: We are meant to check for fragment change here a well as in Update.

	return EStatus();
}


IAction::EStatus CActorAnimationActionLocomotion::Update(float timePassed)
{
	CAnimationAction::Update(timePassed);

	// Grab the actor in the root scope.
	CActorComponent& actor = *CActorComponent::GetActor(m_rootScope->GetEntityId());

	// Set stance tags if they have changed.
	const auto newStance = actor.GetControllerComponent()->GetStance();
	if (m_lastStance != newStance)
	{
		TagID tagId = actor.GetStanceTagId(newStance);
		GetContext().state.Set(tagId, true);
	}

	// Cycle.
	m_lastStance = newStance;

	// Set posture tags if they have changed.
	const auto newPosture = actor.GetControllerComponent()->GetPosture();
	if (m_lastPosture != newPosture)
	{
		TagID tagId = actor.GetPostureTagId(newPosture);
		GetContext().state.Set(tagId, true);
	}

	// Cycle.
	m_lastPosture = newPosture;

	FragmentID newFragmentId;
	if (actor.GetVelocity().len() > FLT_EPSILON)
		newFragmentId = m_locomotionParams->fragmentIDs.Move;
	else
		newFragmentId = m_locomotionParams->fragmentIDs.Idle;

	// Set the new fragment, if needed.
	if (m_lastFragmentId != newFragmentId)
	{
		SetFragment(newFragmentId);
		m_lastFragmentId = newFragmentId;
	}

	// Update the fragments and tags if they are different.
	if (m_rootScope->IsDifferent(m_fragmentID, m_fragTags))
		SetFragment(m_fragmentID, m_fragTags);

	return EStatus();
}
}