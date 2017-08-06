#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>


namespace Chrysalis
{
class CActorAnimationActionLooking : public CAnimationAction
{
public:
	DEFINE_ACTION("Looking");

	CActorAnimationActionLooking();

	// IAction
	virtual void OnInitialise() override;
	virtual EStatus Update(float timePassed) override;
	// ~IAction

	static bool IsSupported(const SAnimationContext& context);

private:
	static FragmentID FindFragmentId(const SAnimationContext& context);
};
}