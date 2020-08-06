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
	void Init();

	// #TODO: Come back and edit the list of sound events to match what we need and have implemented.
	// Currently, it's just copied from the sample SDK.
	// Also, double check what this is doing, and see if there's a smarter way to handle it. It looks
	// like code is allowed to modify this table in response to various events.

	uint32 m_soundId {0};
	uint32 m_plugginTriggerId {0};
	uint32 m_footstepSignalId {0};
	uint32 m_foleySignalId {0};
	uint32 m_groundEffectId {0};
	uint32 m_swimmingStrokeId {0};
	uint32 m_footStepImpulseId {0};
	uint32 m_forceFeedbackId {0};
	uint32 m_grabObjectId {0};
	uint32 m_stowId {0};
	uint32 m_weaponLeftHandId {0};
	uint32 m_weaponRightHandId {0};
	uint32 m_deathReactionEndId {0};
	uint32 m_reactionOnCollision {0};
	uint32 m_forbidReactionsId {0};
	uint32 m_ragdollStartId {0};
	uint32 m_killId {0};
	uint32 m_deathBlow {0};
	uint32 m_startFire {0};
	uint32 m_stopFire {0};
	uint32 m_shootGrenade {0};
	uint32 m_meleeHitId {0};
	uint32 m_meleeStartDamagePhase {0};
	uint32 m_meleeEndDamagePhase {0};
	uint32 m_endReboundAnim {0};
	uint32 m_detachEnvironmentalWeapon {0};
	uint32 m_stealthMeleeDeath {0};

private:
	bool m_initialized {false};
};
}