#include <StdAfx.h>

#include "ActorAnimationEvents.h"


namespace Chrysalis
{
void SActorAnimationEvents::Init()
{
	if (!m_initialized)
	{
		m_soundId = CCrc32::Compute("sound");
		m_plugginTriggerId = CCrc32::Compute("PluginTrigger");
		m_footstepSignalId = CCrc32::Compute("footstep");
		m_foleySignalId = CCrc32::Compute("foley");
		m_groundEffectId = CCrc32::Compute("groundEffect");
		m_swimmingStrokeId = CCrc32::Compute("swimmingStroke");
		m_footStepImpulseId = CCrc32::Compute("footstep_impulse");
		m_forceFeedbackId = CCrc32::Compute("ForceFeedback");
		m_grabObjectId = CCrc32::Compute("FlagGrab");
		m_stowId = CCrc32::Compute("Stow");
		m_weaponLeftHandId = CCrc32::Compute("leftHand");
		m_weaponRightHandId = CCrc32::Compute("rightHand");
		m_deathReactionEndId = CCrc32::Compute("DeathReactionEnd");
		m_reactionOnCollision = CCrc32::Compute("ReactionOnCollision");
		m_forbidReactionsId = CCrc32::Compute("ForbidReactions");
		m_ragdollStartId = CCrc32::Compute("RagdollStart");
		m_deathBlow = CCrc32::Compute("DeathBlow");
		m_killId = CCrc32::Compute("Kill");
		m_startFire = CCrc32::Compute("StartFire");
		m_stopFire = CCrc32::Compute("StopFire");
		m_shootGrenade = CCrc32::Compute("ShootGrenade");
		m_meleeHitId = CCrc32::Compute("MeleeHit");
		m_meleeStartDamagePhase = CCrc32::Compute("MeleeStartDamagePhase");
		m_meleeEndDamagePhase = CCrc32::Compute("MeleeEndDamagePhase");
		m_detachEnvironmentalWeapon = CCrc32::Compute("DetachEnvironmentalWeapon");
		m_stealthMeleeDeath = CCrc32::Compute("StealthMeleeDeath");
		m_endReboundAnim = CCrc32::Compute("EndReboundAnim");
	}

	m_initialized = true;
}
}