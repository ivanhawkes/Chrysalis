#include <StdAfx.h>

#include "ActorAnimationActionLocomotion.h"
#include "IActorSystem.h"
#include <Actor/Actor.h>


#define MAN_AIMOVEMENT_FRAGMENTS( x ) \
	x( Idle ) \
	x( Move ) \
	//x( Motion_Idle ) \
	//x( Motion_IdleTurn ) \
	//x( Motion_IdleTurnBig ) \
	//x( Motion_Move ) \
	//x( Motion_Air )

#define MAN_AIMOVEMENT_TAGS( x ) \
	x( NoGait )

#define MAN_AIMOVEMENT_TAGGROUPS( x )

#define MAN_AIMOVEMENT_SCOPES( x ) \
	x( FullBody3P )

#define MAN_AIMOVEMENT_CONTEXTS( x )

#define MAN_AIMOVEMENT_FRAGMENT_TAGS( x )

MANNEQUIN_USER_PARAMS(SMannequinLocomotionParams, MAN_AIMOVEMENT_FRAGMENTS, MAN_AIMOVEMENT_TAGS, MAN_AIMOVEMENT_TAGGROUPS, MAN_AIMOVEMENT_SCOPES, MAN_AIMOVEMENT_CONTEXTS, MAN_AIMOVEMENT_FRAGMENT_TAGS);


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

	return EStatus();
}


IAction::EStatus CActorAnimationActionLocomotion::Update(float timePassed)
{
	CAnimationAction::Update(timePassed);

	// Grab the actor in the root scope.
	CActor& actor = *static_cast<CActor*>(gEnv->pGameFramework->GetIActorSystem()->GetActor(GetRootScope().GetEntityId()));

	FragmentID newFragmentId;
	if (actor.GetMovingLastFrame())
		newFragmentId = m_locomotionParams->fragmentIDs.Move;
	else
		newFragmentId = m_locomotionParams->fragmentIDs.Idle;

	// Set the new fragment, if needed.
	if (m_lastFragmentId != newFragmentId)
	{
		SetFragment(newFragmentId);
		m_lastFragmentId = newFragmentId;
	}

	return EStatus();
}


void CActorAnimationActionLocomotion::Install()
{
	CAnimationAction::Install();
}

