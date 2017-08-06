#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>


namespace Chrysalis
{
class CActorAnimationActionInteraction : public CAnimationAction
{
public:
	DEFINE_ACTION("Interaction");

	CActorAnimationActionInteraction();
	virtual ~CActorAnimationActionInteraction() {};

	// IAction
	void OnInitialise() override;
	void Enter() override;
	void Fail(EActionFailure actionFailure) override;
	void Exit() override;
	IAction::EStatus UpdatePending(float timePassed) override;
	IAction::EStatus Update(float timePassed) override;
	void Install() override;
	// ~IAction

private:
	const struct SMannequinInteractionParams* m_interactionParams;
};
}