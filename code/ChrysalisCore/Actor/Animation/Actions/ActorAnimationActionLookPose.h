#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>


namespace Chrysalis
{
class CActorAnimationActionLookPose : public CAnimationAction
{
public:
	DEFINE_ACTION("LookPose");

	CActorAnimationActionLookPose();

	// IAction
	virtual void OnInitialise() override;
	virtual EStatus Update(float timePassed) override;
	virtual void Install() override;
	// ~IAction

	static bool IsSupported(const SAnimationContext& context);
};
}