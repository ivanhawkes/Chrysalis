#pragma once

#include <Animation/Animation.h>


namespace Chrysalis
{
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
	f( ScopeSlave ) \
    //f( Crouch ) \
	//f( Swim ) \
	//f( Underwater ) \
	//f( Jump ) \
	//f( Move ) \
	//f( Sprint ) \
	//f( OutOfAmmo ) \
	//f( Aiming ) \
	//f( Forward ) \
	//f( Backward ) \
	//f( Left ) \
	//f( Right ) \
	//f( NW ) \
	//f( MP ) \
	//f( SP ) \
	//f( FP ) \
	//f( LocalClient ) \
	//f( Throwing ) \

#define MAN_ACTOR_TAG_GROUPS( f ) \
	//f( Stance ) \
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

extern SActorMannequinParams g_actorMannequinParams;
}