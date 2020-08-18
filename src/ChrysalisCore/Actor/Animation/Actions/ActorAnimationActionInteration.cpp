#include <StdAfx.h>

#include "ActorAnimationActionInteration.h"
#include <Components/Actor/ActorComponent.h>


namespace Chrysalis
{
#define INTERACTION_FRAGMENTS( x ) \
	x( Interaction )

#define INTERACTION_TAGS( x ) \
	x( InteractionHigh ) /* The general height of the interaction object. */ \
	x( InteractionMiddle ) \
	x( InteractionLow ) \
	x( InteractionGrabObject ) /* Interaction method. */ \
	x( InteractionFlipSwitch ) \
	x( InteractionDeadLift ) \
	x( InteractionTurnWheel )

#define INTERACTION_TAGGROUPS( x ) \
	x( InteractionHeight ) \
	x( InteractionMethod )

#define INTERACTION_SCOPES( x )

#define INTERACTION_CONTEXTS( x )

#define INTERACTION_FRAGMENT_TAGS( x )

MANNEQUIN_USER_PARAMS(SMannequinInteractionParams, INTERACTION_FRAGMENTS, INTERACTION_TAGS, INTERACTION_TAGGROUPS, INTERACTION_SCOPES, INTERACTION_CONTEXTS, INTERACTION_FRAGMENT_TAGS);


CActorAnimationActionInteraction::CActorAnimationActionInteraction(const std::vector<string>& tags)
	: CAnimationAction(EActorActionPriority::eAAP_Interaction, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY, IAction::FragmentIsOneShot)
	, m_tags(tags)
{
}


FragmentID CActorAnimationActionInteraction::FindFragmentId(const SAnimationContext& context)
{
	const SMannequinInteractionParams* pUserParams = GetMannequinUserParams<SMannequinInteractionParams>(context);
	CRY_ASSERT(pUserParams != nullptr);

	return pUserParams->fragmentIDs.Interaction;
}


bool CActorAnimationActionInteraction::IsSupported(const SAnimationContext& context)
{
	const FragmentID fragmentId = FindFragmentId(context);
	const bool isSupported = (fragmentId != FRAGMENT_ID_INVALID);

	return isSupported;
}


void CActorAnimationActionInteraction::OnInitialise()
{
	CAnimationAction::OnInitialise();

	const FragmentID fragmentId = FindFragmentId(*m_context);
	CRY_ASSERT(fragmentId != FRAGMENT_ID_INVALID);
	SetFragment(fragmentId);
}


void CActorAnimationActionInteraction::Install()
{
	CAnimationAction::Install();
}


void CActorAnimationActionInteraction::OnAnimationEvent(ICharacterInstance* pCharacter, const AnimEventInstance& event)
{
	// Notify listeners.
	for (auto it : m_listeners)
		it->OnActionAnimationEvent(pCharacter, event);
}


void CActorAnimationActionInteraction::Enter()
{
	CAnimationAction::Enter();

	// I'm going to push some tags...
	const SControllerDef& controllerDef = m_rootScope->GetActionController().GetContext().controllerDef;
	for (auto& it : m_tags)
	{
		const auto tagId = controllerDef.m_tags.Find(it);
		if (tagId != TAG_ID_INVALID)
		{
			GetContext().state.Set(tagId, true);
		}
	}

	// Notify listeners.
	for (auto it : m_listeners)
		it->OnActionAnimationEnter();
}


void CActorAnimationActionInteraction::Fail(EActionFailure actionFailure)
{
	CAnimationAction::Fail(actionFailure);

	// Notify listeners.
	for (auto it : m_listeners)
		it->OnActionAnimationFail(actionFailure);
}


void CActorAnimationActionInteraction::Exit()
{
	CAnimationAction::Exit();

	// I'm going to pop some tags...
	const SControllerDef& controllerDef = m_rootScope->GetActionController().GetContext().controllerDef;
	for (auto& it : m_tags)
	{
		const auto tagId = controllerDef.m_tags.Find(it);
		if (tagId != TAG_ID_INVALID)
		{
			GetContext().state.Set(tagId, false);
		}
	}

	// Notify listeners.
	for (auto it : m_listeners)
		it->OnActionAnimationExit();
}


IAction::EStatus CActorAnimationActionInteraction::UpdatePending(float timePassed)
{
	CAnimationAction::UpdatePending(timePassed);

	return EStatus();
}


IAction::EStatus CActorAnimationActionInteraction::Update(float timePassed)
{
	CAnimationAction::Update(timePassed);

	// Update the fragments and tags if they are different.
	if (m_rootScope->IsDifferent(m_fragmentID, m_fragTags))
		SetFragment(m_fragmentID, m_fragTags);

	return EStatus();
}
}