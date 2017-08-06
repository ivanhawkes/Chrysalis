/**
\file	Actor\ActorAnimationEvents.h

Provides a struct, which is suitable for all actors, with Ids to be used when sound events occur for that actor.

NOTE: This only seems valid for characters at present, but there should probably be something here which defines
all the base sounds you'd expect every actor to require.
*/
#pragma once

#include <CryCore/BaseTypes.h>


namespace Chrysalis
{
struct SActorAnimationEvents
{
	SActorAnimationEvents()
		: m_initialized(false)
	{}

	void Init();

	// #TODO: Come back and edit the list of sound events to match what we need and have implemented.
	// Currently, it's just copied from the sample SDK.
	// Also, double check what this is doing, and see if there's a smarter way to handle it. It looks
	// like code is allowed to modify this table in response to various events.

	uint32 m_soundId;
	uint32 m_plugginTriggerId;
	uint32 m_footstepSignalId;
	uint32 m_foleySignalId;
	uint32 m_groundEffectId;
	uint32 m_swimmingStrokeId;
	uint32 m_footStepImpulseId;
	uint32 m_forceFeedbackId;
	uint32 m_grabObjectId;
	uint32 m_stowId;
	uint32 m_weaponLeftHandId;
	uint32 m_weaponRightHandId;
	uint32 m_deathReactionEndId;
	uint32 m_reactionOnCollision;
	uint32 m_forbidReactionsId;
	uint32 m_ragdollStartId;
	uint32 m_killId;
	uint32 m_deathBlow;
	uint32 m_startFire;
	uint32 m_stopFire;
	uint32 m_shootGrenade;
	uint32 m_meleeHitId;
	uint32 m_meleeStartDamagePhase;
	uint32 m_meleeEndDamagePhase;
	uint32 m_endReboundAnim;
	uint32 m_detachEnvironmentalWeapon;
	uint32 m_stealthMeleeDeath;

private:
	bool m_initialized;
};
}