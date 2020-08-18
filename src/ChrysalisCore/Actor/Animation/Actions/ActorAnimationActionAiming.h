#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>


namespace Chrysalis
{
class CActorAnimationActionAiming : public CAnimationAction
{
public:
	DEFINE_ACTION("Aiming");

	CActorAnimationActionAiming();

	// IAction
	virtual void OnInitialise() override;
	virtual EStatus Update(float timePassed) override;
	// ~IAction

	static bool IsSupported(const SAnimationContext& context);

private:
	static FragmentID FindFragmentId(const SAnimationContext& context);
};
}