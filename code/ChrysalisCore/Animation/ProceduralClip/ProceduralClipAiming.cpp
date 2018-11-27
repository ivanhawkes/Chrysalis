#include "StdAfx.h"

#include "ICryMannequin.h"
#include "Animation/ProceduralContext/ProceduralContextAim.h"


namespace Chrysalis
{
class CProceduralClipAiming
	: public TProceduralContextualClip<CProceduralContextAim, SNoProceduralParams>
{
public:
	virtual void OnEnter(float blendTime, float duration, const SNoProceduralParams& params)
	{
		m_context->SetBlendInTime(blendTime);
		m_context->SetIsAimingProcClip(true);
	}


	virtual void OnExit(float blendTime)
	{
		m_context->SetBlendOutTime(blendTime);
		m_context->SetIsAimingProcClip(false);
	}


	virtual void Update(float timePassed)
	{
		m_context->SetIsAimingProcClip(true);
	}
};

REGISTER_PROCEDURAL_CLIP(CProceduralClipAiming, "Aiming");
}
