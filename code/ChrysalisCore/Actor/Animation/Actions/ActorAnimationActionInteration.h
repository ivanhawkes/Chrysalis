#pragma once

#include "ICryMannequin.h"
#include "Actor/Animation/ActorAnimation.h"
#include "Utility/Listener.h"


namespace Chrysalis
{
/** Listen for animation events that are triggered while running this action. */
struct IAnimationEventListener
{
	virtual ~IAnimationEventListener() = default;

	virtual void OnActionAnimationEnter() = 0;
	virtual void OnActionAnimationFail(EActionFailure actionFailure) = 0;
	virtual void OnActionAnimationExit() = 0;
	virtual void OnActionAnimationEvent(ICharacterInstance* pCharacter, const AnimEventInstance& event) = 0;
};


class CActorAnimationActionInteraction : public CAnimationAction
{
public:
	DEFINE_ACTION("Interaction");

	CActorAnimationActionInteraction(const std::vector<string>& tags);
	virtual ~CActorAnimationActionInteraction() = default;

	// IAction
	void OnInitialise() override;
	void Enter() override;
	void Fail(EActionFailure actionFailure) override;
	void Exit() override;
	IAction::EStatus UpdatePending(float timePassed) override;
	IAction::EStatus Update(float timePassed) override;
	void Install() override;
	virtual void OnAnimationEvent(ICharacterInstance* pCharacter, const AnimEventInstance& event) override;
	// ~IAction

	void AddEventListener(IAnimationEventListener* pListener)
	{
		stl::push_back_unique(m_listenersList, pListener);
	}

	void RemoveEventListener(IAnimationEventListener* pListener)
	{
		m_listenersList.remove(pListener);
	}

private:
	const struct SMannequinInteractionParams* m_interactionParams;
	std::vector<string> m_tags;

	/** Listeners for animation events. */
	std::list<IAnimationEventListener*> m_listenersList;
};
}