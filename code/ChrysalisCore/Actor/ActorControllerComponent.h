#pragma once

#include <StateMachine/StateMachine.h>
#include "Actor/Animation/ActorAnimation.h"
#include "Components/Player/Input/PlayerInputComponent.h"
#include "DefaultComponents/Physics/CharacterControllerComponent.h"
#include "DefaultComponents/Geometry/AdvancedAnimationComponent.h"


namespace Chrysalis
{
class CActorComponent;


struct SActorPhysics
{
	enum EActorPhysicsFlags
	{
		Flying = BIT(0),
		WasFlying = BIT(1),
		Stuck = BIT(2)
	};


	SActorPhysics()
		/*: angVelocity(ZERO)
		, velocity(ZERO)
		, velocityDelta(ZERO)
		, velocityUnconstrained(ZERO)
		, velocityUnconstrainedLast(ZERO)
		, gravity(ZERO)
		, groundNormal(0, 0, 1)
		, speed(0)
		, groundHeight(0.0f)
		, mass(80.0f)
		, lastFrameUpdate(0)
		, groundMaterialIdx(-1)
		, groundColliderId(0)*/
	{}

	void Serialize(TSerialize ser, EEntityAspects aspects) {};

	CCryFlags<uint32> flags;

	/*	Vec3 angVelocity;
	Vec3 velocity;
	Vec3 velocityDelta;
	Vec3 velocityUnconstrained;
	Vec3 velocityUnconstrainedLast;
	Vec3 gravity;
	Vec3 groundNormal;

	float speed;
	float groundHeight;
	float mass;

	int lastFrameUpdate;
	int groundMaterialIdx;
	EntityId groundColliderId;
	*/
};


/** Keeps track of the stance the actor is in. */

struct CActorStance
{
public:
	EActorStance GetStance() const { return m_stance; }
	void SetStance(EActorStance stance) { m_stance = stance; }
	EActorPosture GetPosture() const { return m_posture; }
	void SetPosture(EActorPosture posture) { m_posture = posture; }

private:

	/** The actor is currently in this stance, or moving into this stance. */
	EActorStance m_stance { eAS_Standing };

	/** The actor's posture, which should indicate their state of mind, or game conditions they presently have e.g. sapped. */
	EActorPosture m_posture { eAP_Neutral };
}; 


class CActorControllerComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;

	// Declaration of the state machine that controls actor movement.
	DECLARE_STATE_MACHINE(CActorControllerComponent, Movement);

	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_PREPHYSICSUPDATE); }
	// ~IEntityComponent

	virtual void Update(SEntityUpdateContext* pCtx);

	/** Pre physics update. */
	virtual void PrePhysicsUpdate();

	// ***
	// *** Hierarchical State Machine Support.
	// ***

	/** Init the HSM. */
	virtual void MovementHSMInit();

	/**	Select movement hierarchy for our HSM. */
	virtual void SelectMovementHierarchy();

	/** Update the HSM. */
	virtual void MovementHSMUpdate(SEntityUpdateContext& ctx);

	/** Serialize the HSM. */
	virtual void MovementHSMSerialize(TSerialize ser);

	/** Reset the HSM. */
	virtual void MovementHSMReset();

	/** Release the HSM. */
	virtual void MovementHSMRelease();

	virtual void UpdateMovementRequest(float frameTime);
	virtual void UpdateLookDirectionRequest(float frameTime);
	virtual void UpdateAnimation(float frameTime);

public:
	CActorControllerComponent() {}
	virtual ~CActorControllerComponent() { MovementHSMRelease(); }

	static void ReflectType(Schematyc::CTypeDesc<CActorControllerComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{B36CEFF9-E9BE-4EBB-8AE5-FC0FB8F49357}"_cry_guid;
		return id;
	}

	/** Resets the character to an initial state. */
	virtual void OnResetState();


	/** Called when the actor has been revived from death. */
	virtual void OnRevive();


	/**
	Query if this object is controlled by AI. This doesn't imply that there is no player for this actor, simply
	that they are under AI control at present.

	\return true if AI controlled, false if not.
	**/
	bool IsAIControlled() const { return m_isAIControlled; };

	/** A cut-scene has been triggered for this character. */
	void OnBeginCutScene() {};

	/** A cut-scene has finished playing or been cancelled for this character. */
	void OnEndCutScene() {};

	/**	Is this actor being controller by the local player? */
	const bool IsPlayer() const;

	/**	Is this actor being controller by the local player? */
	const bool IsClient() const;

	// Returns the local eye position for this actor.
	const virtual Vec3 GetLocalEyePos() const;


	/**
	Determines a suitable rotation for the lower body based on the movement direction being requested. This should
	typically be combined with look / aim poses or perhaps an inverse of this to turn the upper body suitably.

	\param	movementDirectionFlags The movement direction flags. Typically this should come from your imput handler.

	\return The body rotation in degrees relative to the movement direction in the state flags.
	**/
	const virtual float GetLowerBodyRotation(TInputFlags movementDirectionFlags) const;

	// Provide a way to access the character controller functions indirectly.
	virtual void AddVelocity(const Vec3& velocity) { m_pCharacterControllerComponent->AddVelocity(velocity); }
	virtual void SetVelocity(const Vec3& velocity) { m_pCharacterControllerComponent->SetVelocity(velocity); }

	/** Provide a means for them to query the move velocity and direction. */
	const Vec3& GetVelocity() const { return m_pCharacterControllerComponent->GetVelocity(); }
	Vec3 GetMoveDirection() const { return m_pCharacterControllerComponent->GetMoveDirection(); }

	float virtual GetMovementBaseSpeed(TInputFlags movementDirectionFlags) const;

	/** Should the actor attemp a jump this frame? */
	bool GetShouldJump() { return m_shouldJump; };
	void SetShouldJump(bool shouldJump) { m_shouldJump = shouldJump; };

	EActorStance GetStance() const { return m_actorStance.GetStance(); }
	void SetStance(EActorStance stance) { m_actorStance.SetStance(stance); }
	EActorPosture GetPosture() const { return m_actorStance.GetPosture(); }
	void SetPosture(EActorPosture posture) { m_actorStance.SetPosture(posture); }

	void OnActionCrouchToggle();
	void OnActionCrawlToggle();
	void OnActionKneelToggle();
	void OnActionSitToggle();
	void OnActionJogToggle() { m_isJogging = !m_isJogging; };
	void OnActionSprintStart() { m_isSprinting = true; };
	void OnActionSprintStop() { m_isSprinting = false; };
	bool IsSprinting() const { return m_isSprinting; }
	bool IsJogging() const { return m_isJogging; }

	/** Duration the actor has been in the air. May not be in use yet. */
	float durationInAir;

	/** Duration the actor has been on the ground. May not be in use yet. */
	float durationOnGround;

	const CActorComponent* GetActor() { return m_pActorComponent; };

private:
	/** The actor component we are paired with. */
	CActorComponent* m_pActorComponent { nullptr };

	/** true if this object is controlled by an AI. */
	bool m_isAIControlled;

	/** The look target. */
	Vec3 m_lookTarget { ZERO };

	/** true to use the look at target. */
	bool m_useLookTarget { false };

	/** true to look using IK. */
	bool m_useLookIK { true };

	/** The aim target. */
	Vec3 m_aimTarget { ZERO };

	/** true to using aim IK. */
	bool m_useAimIK { true };

	/** Actor should try and jump this frame if allowed. */
	bool m_shouldJump { false };

	/** Is the actor allowed to request a jump this frame? */
	bool m_allowJump { false };

	Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent { nullptr };
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterControllerComponent { nullptr };

	TagID m_rotateTagId { TAG_ID_INVALID };

	/** A vector representing the direction and distance the player has requested this actor to move. */
	Vec3 m_movementRequest { ZERO };

	/** The direction the actor should be facing (pelvis) based on their movement inputs. */
	Quat m_lookOrientation { IDENTITY };

	/**	The yaw angular velocity - a measure of how quickly the actor is turning. **/
	float m_yawAngularVelocity { 0.0f };

	/** The continuous amount of time the actor has been receiving movement requests (seconds). */
	float m_movingDuration { 0.0f };

	/** The actor is sprinting. */
	bool m_isSprinting { false };

	/** The actor is jogging. */
	bool m_isJogging { false };

	/** The actor's present stance and posture. */
	CActorStance m_actorStance;

	/** Clear these mannequin tags. */
	TagState m_mannequinTagsClear { TAG_STATE_EMPTY };

	/** Set these mannequin tags. */
	TagState m_mannequinTagsSet { TAG_STATE_EMPTY };
};
}