#include "StdAfx.h"

#include "ICryMannequin.h"
#include "Animation/ProceduralContext/ProceduralContextLook.h"


namespace Chrysalis
{

class CProceduralClipLooking : public TProceduralContextualClip<CProceduralContextLook, SNoProceduralParams>
{
public:
	virtual void OnEnter(float blendTime, float duration, const SNoProceduralParams& params)
	{
		m_context->SetBlendInTime(blendTime);
		m_context->SetIsLookingProcClip(true);
	}

	virtual void OnExit(float blendTime)
	{
		m_context->SetBlendOutTime(blendTime);
		m_context->SetIsLookingProcClip(false);
	}

	virtual void Update(float timePassed)
	{
		m_context->SetIsLookingProcClip(true);

#ifdef DEBUG
		//CryWatch("CProceduralClipLooking Update");
#endif
	}
};

REGISTER_PROCEDURAL_CLIP(CProceduralClipLooking, "Looking");
}
