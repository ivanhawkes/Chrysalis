#pragma once

#include <Animation/Animation.h>
#include <ICryMannequinDefs.h>


namespace Chrysalis
{
/** All valid actor stances. */

enum EActorStance
{
	eAS_Standing,
	eAS_Crouching,
	eAS_Crawling,
	eAS_Prone,
	eAS_Falling,
	eAS_Landing,
	eAS_Swimming,
	eAS_Flying,
	eAS_Spellcasting,
	eAS_SittingChair,
	eAS_SittingFloor,
	eAS_Kneeling,
	eAS_Sleeping
};


/**
The actor's posture extends the stance, giving more precise control over animating their state of mind. In
general, these are applied to eAS_Standing, but some may also have application to sitting or other stances.
**/
enum EActorPosture
{
	// Alertness.
	eAP_Unaware,
	eAP_Distracted,
	eAP_Suspicious,
	eAP_Alerted,

	// Daze / sap effect on them.
	eAP_Dazed,

	// Everyday postures.
	eAP_Neutral,
	eAP_Passive,
	eAP_Aggressive,
	eAP_Interested,
	eAP_Bored,
	eAP_Excited,
	eAP_Depressed,
};


enum EActorActionPriority
{
	eAAP_Lowest,
	eAAP_Movement,
	eAAP_MovementAction,
	eAAP_Action,
	eAAP_ActionUrgent,
	eAAP_Interaction,
	eAAP_HitReaction,
	eAAP_Death,
	eAAP_PostDeath
};


#define MAN_ACTOR_FRAGMENTS( f ) \
	f( Idle ) \
	f( Move ) \
	f( Interaction ) \
	f( LadderGetOn ) \
	f( LadderGetOff ) \
	f( LadderClimb ) \
	//f( MotionIdle ) \
	//f( MotionTurn ) \
	//f( MotionMovement ) \
	//f( MotionJump ) \
	//f( MotionInAir ) \
	//f( MotionMounted ) \
	//f( FPNone ) \
	//f( FPSway ) \
	//f( FPIdle ) \
	//f( FPMovement ) \
	//f( FPLand ) \
	//f( AimPose ) \
	//f( IdlePose ) \
	//f( Idle ) \
	//f( IdleBreak ) \
	//f( Interact ) \
	//f( Melee ) \
	//f( MeleeMultipart ) \
	//f( MeleeWeapon ) \
	//f( MeleeInteract ) \
	//f( HitDeath ) \
	//f( AnimationControlled ) \
	//f( WeaponPose ) \

#define MAN_ACTOR_CONTEXTS( f ) \
	f( Weapon ) \
	f( AttachmentTop ) \
	f( AttachmentBottom ) \
	f( SlaveCharacter ) \
	f( SlaveObject ) \
    //f( Char1P ) \
	//f( Char3P ) \

#define MAN_ACTOR_TAGS( f ) \
	f( Standing )  /* Stance. */ \
	f( Crouching ) \
	f( Crawling ) \
	f( Prone ) \
	f( Falling ) \
	f( Landing ) \
	f( Swimming ) \
	f( Flying ) \
	f( Spellcasting ) \
	f( SittingChair ) \
	f( SittingFloor ) \
	f( Kneeling ) \
	f( Sleeping ) \
	f( Unaware )  /* Posture. */ \
	f( Distracted ) \
	f( Suspicious ) \
	f( Alerted ) \
	f( Dazed ) \
	f( Neutral ) \
	f( Passive ) \
	f( Aggressive ) \
	f( Interested ) \
	f( Bored ) \
	f( Excited ) \
	f( Depressed ) \
	f( ScopeSlave ) /* Misc. */ \

#define MAN_ACTOR_TAG_GROUPS( f ) \
    f( Stance ) \
    f( Posture ) \
	//f( WeaponType ) \
	//f( Item ) \
	//f( Zoom ) \
	//f( Firemode ) \
	//f( MoveDir ) \
	//f( MoveSpeed ) \
	//f( MeleeType ) \
	//f( PlayMode ) \
	//f( MPEnviro )

#define MAN_ACTOR_SCOPES( f ) \
	//f( FullBody1P ) \
	//f( FullBody3P ) \
	//f( Motion1P ) \
	//f( AimPose ) \
	//f( Torso1P ) \
	//f( Torso3P ) \
	//f( SlaveChar ) \
	//f( SlaveObject ) \
	//f(Weapon) \
	//f(AttachmentBottom)

#define MAN_MELEE_FRAGMENT_TAGS( f ) \
	//f( Into ) \
	//f( Hit ) \
	//f( Miss )

#define MAN_MELEEPOLE_FRAGMENT_TAGS( f ) \
	//f( SecondSwing )

#define MAN_ACTOR_FRAGMENT_TAGS( f ) \
	f( melee, MAN_MELEEPOLE_FRAGMENT_TAGS, MANNEQUIN_USER_PARAMS__EMPTY_LIST ) \
	f( melee_multipart, MAN_MELEE_FRAGMENT_TAGS, MANNEQUIN_USER_PARAMS__EMPTY_LIST ) \

MANNEQUIN_USER_PARAMS(SActorMannequinParams, MAN_ACTOR_FRAGMENTS, MAN_ACTOR_TAGS, MAN_ACTOR_TAG_GROUPS, MAN_ACTOR_SCOPES, MAN_ACTOR_CONTEXTS, MAN_ACTOR_FRAGMENT_TAGS);
}