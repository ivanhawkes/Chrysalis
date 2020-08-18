#pragma once

#include <Animation/Animation.h>
#include <ICryMannequinDefs.h>


namespace Chrysalis
{
/** All valid actor stances. */

enum class EActorStance
{
	standing,
	crouching,
	crawling,
	prone,
	falling,
	landing,
	swimming,
	flying,
	spellcasting,
	sittingChair,
	sittingFloor,
	kneeling,
	sleeping
};


/**
The actor's posture extends the stance, giving more precise control over animating their state of mind. In
general, these are applied to standing, but some may also have application to sitting or other stances.
**/
enum class EActorPosture
{
	// Alertness.
	unaware,
	distracted,
	suspicious,
	alerted,

	// Daze / sap effect on them.
	dazed,

	// Everyday postures.
	neutral,
	passive,
	aggressive,
	interested,
	bored,
	excited,
	depressed,
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


/** Listen for animation events that are triggered while running a mannequin action. */
struct IAnimationEventListener
{
	virtual ~IAnimationEventListener() = default;

	virtual void OnActionAnimationEnter() = 0;
	virtual void OnActionAnimationFail(EActionFailure actionFailure) = 0;
	virtual void OnActionAnimationExit() = 0;
	virtual void OnActionAnimationEvent(ICharacterInstance* pCharacter, const AnimEventInstance& event) = 0;
};


/** Small lightweight interface to animation systems. This allows us to pass just enough control to
	other sections of code without giving them the whole class. */

struct IActorAnimationControl
{
	virtual ~IActorAnimationControl() = default;

	/** Is there a valid action controller we can request? */
	virtual bool HasActionController() const = 0;

	/** Get the action controller. */
	virtual IActionController* GetActionController() const = 0;

	/** Get the animation context. */
	virtual SAnimationContext& GetContext() const = 0;

	/** Get the animation context, create one if there is no context. */
	virtual IProceduralContext* FindOrCreateProceduralContext(CryClassID classId) const = 0;
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
	f( ScopeAimPose ) \
	f( ScopeAiming ) \
	f( ScopeLookPose ) \
	f( ScopeLooking ) \

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
	f( FullBody1P ) \
	f( FullBody3P ) \
	f( AimPose ) \
	f( LookPose ) \

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