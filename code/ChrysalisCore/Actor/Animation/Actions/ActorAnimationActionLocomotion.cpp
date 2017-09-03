#include <StdAfx.h>

#include "ActorAnimationActionLocomotion.h"


namespace Chrysalis
{
#define LOCOMOTION_FRAGMENTS( x ) \
	x( Idle ) \
	x( Move ) \
	//x( Motion_Idle ) \
	//x( Motion_IdleTurn ) \
	//x( Motion_IdleTurnBig ) \
	//x( Motion_Move ) \
	//x( Motion_Air )

#define LOCOMOTION_TAGS( x ) \
	x( Standing )  /* Stance. */ \
	x( Crouching ) \
	x( Crawling ) \
	x( Falling ) \
	x( Landing ) \
	x( Swimming ) \
	x( Flying ) \
	x( Spellcasting ) \
	x( SittingChair ) \
	x( SittingFloor ) \
	x( Kneeling ) \
	x( Unaware )  /* Posture. */ \
	x( Distracted ) \
	x( Suspicious ) \
	x( Alerted ) \
	x( Dazed ) \
	x( Neutral ) \
	x( Passive ) \
	x( Aggressive ) \
	x( Interested ) \
	x( Bored ) \
	x( Excited ) \
	x( Depressed ) \
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
    x( Stance ) \
    x( Posture ) \
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
	const IScope& rootScope = GetRootScope();
	CActorComponent& actor = *CActorComponent::GetActor(rootScope.GetEntityId());

	// Set stance tags if they have changed.
	const auto newStance = actor.GetStance();
	if (m_lastStance != newStance)
	{
		switch (newStance)
		{
			case EActorStance::eAS_Crawling:
				GetContext().state.Set(m_locomotionParams->tagIDs.Crawling, true);
				break;

			case EActorStance::eAS_Crouching:
				GetContext().state.Set(m_locomotionParams->tagIDs.Crouching, true);
				break;

			case EActorStance::eAS_Falling:
				GetContext().state.Set(m_locomotionParams->tagIDs.Falling, true);
				break;

			case EActorStance::eAS_Flying:
				GetContext().state.Set(m_locomotionParams->tagIDs.Flying, true);
				break;

			case EActorStance::eAS_Kneeling:
				GetContext().state.Set(m_locomotionParams->tagIDs.Kneeling, true);
				break;

			case EActorStance::eAS_Landing:
				GetContext().state.Set(m_locomotionParams->tagIDs.Landing, true);
				break;

			case EActorStance::eAS_SittingChair:
				GetContext().state.Set(m_locomotionParams->tagIDs.SittingChair, true);
				break;

			case EActorStance::eAS_SittingFloor:
				GetContext().state.Set(m_locomotionParams->tagIDs.SittingFloor, true);
				break;

			case EActorStance::eAS_Spellcasting:
				GetContext().state.Set(m_locomotionParams->tagIDs.Spellcasting, true);
				break;

			case EActorStance::eAS_Standing:
				GetContext().state.Set(m_locomotionParams->tagIDs.Standing, true);
				break;

			case EActorStance::eAS_Swimming:
				GetContext().state.Set(m_locomotionParams->tagIDs.Swimming, true);
				break;
		}
	}

	// Cycle.
	m_lastStance = newStance;

	// Set posture tags if they have changed.
	const auto newPosture = actor.GetPosture();
	if (m_lastPosture != newPosture)
	{
		switch (newPosture)
		{
			case EActorPosture::eAP_Aggressive:
				GetContext().state.Set(m_locomotionParams->tagIDs.Aggressive, true);
				break;

			case EActorPosture::eAP_Alerted:
				GetContext().state.Set(m_locomotionParams->tagIDs.Alerted, true);
				break;

			case EActorPosture::eAP_Bored:
				GetContext().state.Set(m_locomotionParams->tagIDs.Bored, true);
				break;

			case EActorPosture::eAP_Dazed:
				GetContext().state.Set(m_locomotionParams->tagIDs.Dazed, true);
				break;

			case EActorPosture::eAP_Depressed:
				GetContext().state.Set(m_locomotionParams->tagIDs.Depressed, true);
				break;

			case EActorPosture::eAP_Distracted:
				GetContext().state.Set(m_locomotionParams->tagIDs.Distracted, true);
				break;

			case EActorPosture::eAP_Excited:
				GetContext().state.Set(m_locomotionParams->tagIDs.Excited, true);
				break;

			case EActorPosture::eAP_Interested:
				GetContext().state.Set(m_locomotionParams->tagIDs.Interested, true);
				break;

			case EActorPosture::eAP_Neutral:
				GetContext().state.Set(m_locomotionParams->tagIDs.Neutral, true);
				break;

			case EActorPosture::eAP_Passive:
				GetContext().state.Set(m_locomotionParams->tagIDs.Passive, true);
				break;

			case EActorPosture::eAP_Suspicious:
				GetContext().state.Set(m_locomotionParams->tagIDs.Suspicious, true);
				break;

			case EActorPosture::eAP_Unaware:
				GetContext().state.Set(m_locomotionParams->tagIDs.Unaware, true);
				break;
		}
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
	if (rootScope.IsDifferent(m_fragmentID, m_fragTags))
	{
		SetFragment(m_fragmentID, m_fragTags);
	}

	return EStatus();
}
}