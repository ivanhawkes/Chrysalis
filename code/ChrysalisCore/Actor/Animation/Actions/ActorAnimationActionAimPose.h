#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>


class CActorAnimationActionAimPose : public CAnimationAction
{
public:
	DEFINE_ACTION("AimPose");

	CActorAnimationActionAimPose();

	// IAction
	virtual void OnInitialise() override;
	virtual EStatus Update(float timePassed) override;
	virtual void Install() override;
	// ~IAction

	static bool IsSupported(const SAnimationContext& context);

private:
	void InitialiseAimPoseBlender();
	static FragmentID FindFragmentId(const SAnimationContext& context);
};
