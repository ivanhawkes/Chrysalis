#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>


class CActorAnimationActionLocomotion : public CAnimationAction
{
public:
	CActorAnimationActionLocomotion();
	virtual ~CActorAnimationActionLocomotion() {};


	// IAction
	void OnInitialise() override;
	void Enter() override;
	void Fail(EActionFailure actionFailure) override;
	void Exit() override;
	IAction::EStatus UpdatePending(float timePassed) override;
	IAction::EStatus Update(float timePassed) override;
	void Install() override;
	//void OnSequenceFinished(int layer, uint32 scopeID) override;
	// ~IAction

private:
	const struct SMannequinLocomotionParams* m_locomotionParams;

	FragmentID m_lastFragmentId { FRAGMENT_ID_INVALID };
};

