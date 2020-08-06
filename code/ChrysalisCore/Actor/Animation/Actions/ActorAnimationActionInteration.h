#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>


namespace Chrysalis
{
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
		stl::push_back_unique(m_listeners, pListener);
	}

	void RemoveEventListener(IAnimationEventListener* pListener)
	{
		m_listeners.remove(pListener);
	}

	static bool IsSupported(const SAnimationContext& context);

private:
	static FragmentID FindFragmentId(const SAnimationContext& context);

	const struct SMannequinInteractionParams* m_interactionParams {nullptr};

	// Tags for controlling the animation.
	std::vector<string> m_tags;

	/** Listeners for animation events. */
	std::list<IAnimationEventListener*> m_listeners;
};
}