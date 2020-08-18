#pragma once

// TODO: Commented it all out for now since it needs some changes made to work with characters instead of players.

/*
#include <ICryMannequin.h>
#include <CryAnimation/ICryAnimation.h>
#include <CryAnimation/IAnimationPoseModifier.h>
#include <CryExtension/CryCreateClassInstance.h>
#include <CryExtension/ClassWeaver.h>
#include <CryExtension/ICryFactoryRegistryImpl.h>
#include <CryExtension/RegFactoryNode.h>


namespace Chrysalis
{
class CProceduralContextRagdoll
	: public IProceduralContext
{
public:
	PROCEDURAL_CONTEXT(CProceduralContextRagdoll, "ProceduralContextRagdoll", "{A6ABBF16-2345-4E6E-B6DF-1BF41CBE58CE}"_cry_guid);

	CProceduralContextRagdoll();
	virtual ~CProceduralContextRagdoll() {}

	ILINE EntityId GetEntityTarget() const { return m_targetEntityId; }
	ILINE void SetEntityTarget(const EntityId entityID) { m_targetEntityId = entityID; }
	ILINE void SetAspectProfileScope(bool bScope) { m_bDispatchedAspectProfile = bScope; }
	ILINE bool GetAspectProfileScope() const { return m_bDispatchedAspectProfile; }
	ILINE bool IsInRagdoll() const { return m_bInRagdoll; }
	ILINE void SetBlendOut(float blendOutTime)
	{
		m_blendOutTime = blendOutTime;
		m_bInBlendOut = true;
	}
	void EnableRagdoll(const EntityId entityID, const bool bAlive, const float stiffness, const bool bFromProcClip = false);
	void DisableRagdoll(float blendOutTime);
	void QueueRagdoll(bool bAlive);
	void ForceRagdollFinish(IActor* piActor, bool bForceDead);

protected:
	// IProceduralContext
	virtual void Update(float timePassedSeconds) override;
	// ~IProceduralContext

	void Reset();

private:
	EntityId m_targetEntityId;
	float m_stiffness;
	float m_blendOutTime;
	float m_blendOutTimeCurrent;
	bool m_bInRagdoll;
	bool m_bInBlendOut;
	bool m_bEntityAlive;
	bool m_bDispatchedAspectProfile;
	bool m_bForceRagdollFinish;
	bool m_bFromProcClip;
};
}
*/