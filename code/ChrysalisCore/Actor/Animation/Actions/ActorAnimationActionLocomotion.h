#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>
#include "Actor/ActorControllerComponent.h"


namespace Chrysalis
{
class CActorAnimationActionLocomotion : public CAnimationAction
{
public:
	DEFINE_ACTION("Locomotion");

	CActorAnimationActionLocomotion();
	virtual ~CActorAnimationActionLocomotion() = default;

	// IAction
	void OnInitialise() override;
	void Install() override;
	void Enter() override;
	void Fail(EActionFailure actionFailure) override;
	void Exit() override;
	IAction::EStatus UpdatePending(float timePassed) override;
	IAction::EStatus Update(float timePassed) override;
	// ~IAction

private:
	const struct SMannequinLocomotionParams* m_locomotionParams;

	FragmentID m_lastFragmentId { FRAGMENT_ID_INVALID };

	EActorStance m_lastStance { EActorStance::eAS_Standing };

	EActorPosture m_lastPosture { EActorPosture::eAP_Neutral };
};
}