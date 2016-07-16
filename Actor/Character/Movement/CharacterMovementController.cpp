#include <StdAfx.h>

#include "CharacterMovementController.h"
#include <CryAISystem/IAgent.h>
#include <Actor/Character/Character.h>
#include <Actor/Player/Player.h>
#include <Actor/ActorState.h>
#include <Camera/ICamera.h>
#include <PlayerInput/IPlayerInput.h>
#include <IMovementController.h>
#include <Camera/ICamera.h>


// ***
// *** IMovementController
// ***


bool CCharacterMovementController::RequestMovement(CMovementRequest& request)
{
	// We have to process right through and not early out, because otherwise we won't modify the request to a
	// "close but correct" state.
	bool ok = true;

	// Because we're not allowed to "commit" values here, we make a backup, perform modifications, and then copy the
	// modifications if everything was successful. 
	CMovementRequest state = m_movementRequest;

	// Handle delta movement and rotation.
	if (request.HasDeltaMovement())
		state.AddDeltaMovement(request.GetDeltaMovement());
	if (request.HasDeltaRotation())
		state.AddDeltaRotation(request.GetDeltaRotation());

	// Pass along the jump request.
	if (request.ShouldJump())
		state.SetJump();

	// Commit the modifications.
	if (ok)
		m_movementRequest = state;

	// TODO: handle the movement request.
	
	// TODO: handle all the other possible input values.

	return ok;
}


// TODO: Find out where and when this is called.

bool CCharacterMovementController::GetStanceState(const SStanceStateQuery& query, SStanceState& state)
{
	IEntity* pEntity = m_pCharacter->GetEntity();

	const SActorStance* actorStance = m_pCharacter->GetStanceInfo(query.stance);
	if (!actorStance)
		return false;

	Quat orientation = pEntity->GetWorldRotation();
	Vec3 forward = orientation.GetColumn1();

	// Allow the query to overwrite the current position of the actor if requested.
	Vec3 pos = query.position.IsZero() ? pEntity->GetWorldPos() : query.position;

	// Look and aim.
	Vec3 aimTarget = m_aimTarget;
	Vec3 lookTarget = m_lookTarget;

	// Allow the query to overwrite the current target if requested.
	if (!query.target.IsZero())
	{
		aimTarget = query.target;
		lookTarget = query.target;
		forward = (query.target - pos).GetNormalizedSafe(forward);
	}

	state.pos = pos;
	state.m_StanceSize = actorStance->GetStanceBounds();
	state.m_ColliderSize = actorStance->GetColliderBounds();
	state.lean = query.lean;	// pass through
	state.peekOver = query.peekOver;

	if (query.defaultPose)
	{
		state.eyePosition = pos + actorStance->GetViewOffsetWithLean(query.lean, query.peekOver);
		state.upDirection.Set(0, 0, 1);
		state.eyeDirection = FORWARD_DIRECTION;
		state.aimDirection = FORWARD_DIRECTION;
		state.fireDirection = FORWARD_DIRECTION;
		state.entityDirection = FORWARD_DIRECTION;

		// TODO: Need to make sure we calc weapon position too, once weapons are in place.
		//state.weaponPosition = pos + m_pPlayer->GetWeaponOffsetWithLean(query.stance, query.lean, query.peekOver, m_pPlayer->GetEyeOffset());
	}
	else
	{
		Vec3 constrainedLookDir = lookTarget - pos;
		Vec3 constrainedAimDir = aimTarget - pos;
		constrainedLookDir.z = 0.0f;
		constrainedAimDir.z = 0.0f;

		constrainedAimDir = constrainedAimDir.GetNormalizedSafe(constrainedLookDir.GetNormalizedSafe(forward));
		constrainedLookDir = constrainedLookDir.GetNormalizedSafe(forward);

		Matrix33 lookRot;
		lookRot.SetRotationVDir(constrainedLookDir);
		state.eyePosition = pos + lookRot.TransformVector(actorStance->GetViewOffsetWithLean(query.lean, query.peekOver));

		Matrix33 aimRot;
		aimRot.SetRotationVDir(constrainedAimDir);

		// TODO: Need to make sure we calc weapon position too, once weapons are in place.
		//state.weaponPosition = pos + aimRot.TransformVector(m_pPlayer->GetWeaponOffsetWithLean(query.stance, query.lean, query.peekOver, m_pPlayer->GetEyeOffset()));

		state.upDirection = orientation.GetColumn2();

		state.eyeDirection = (lookTarget - state.eyePosition).GetNormalizedSafe(forward);
		state.aimDirection = (aimTarget - state.weaponPosition).GetNormalizedSafe((lookTarget - state.weaponPosition).GetNormalizedSafe(forward));
		state.fireDirection = state.aimDirection;
		state.entityDirection = forward;
	}

	return true;
}


// ***
// *** CCharacterMovementController
// ***


CCharacterMovementController::CCharacterMovementController(CCharacter* pCharacter)
{
	m_pCharacter = pCharacter;
	Reset();
}


CCharacterMovementController::~CCharacterMovementController()
{}


void CCharacterMovementController::Reset()
{
	// Clear it down.
	m_movementRequest = CMovementRequest();

	// Set it all to it's default state.
	m_desiredSpeed = 0.0f;
	m_bAtTarget = false;
	m_bAimClamped = false;
	m_bUsingLookIK = true;
	m_bUsingAimIK = true;
	m_targetStance = STANCE_NULL;
	m_lookTarget.zero();
	m_aimTarget.zero();
	m_fireTarget.zero();
	m_rotationRateZAxis = 0.0f;

	if (!GetISystem()->IsSerializingFile() == 1)
		ComputeMovementState();
}


void CCharacterMovementController::UpdateActorMovementRequest(SActorMovementRequest& movementRequest, float frameTime)
{
	// Desired movement.
	if (m_movementRequest.HasDeltaMovement())
	{
		// NOTE: desiredVelocity is normalized to the maximumspeed for the specified stance, so DeltaMovement should be
		// between 0 and 1! 
		movementRequest.desiredVelocity += m_movementRequest.GetDeltaMovement();
		CryWatch("%s : velocity = %f, %f, %f\r\n", __func__, movementRequest.desiredVelocity.x, movementRequest.desiredVelocity.y, movementRequest.desiredVelocity.z);
	}

	// Rotation.
	if (m_movementRequest.HasDeltaRotation())
	{
		movementRequest.deltaAngles += m_movementRequest.GetDeltaRotation();
		CryWatch("%s : rotation = %f, %f, %f\r\n", __func__, movementRequest.deltaAngles.x, movementRequest.deltaAngles.y, movementRequest.deltaAngles.z);
	}

	// Clearing and setting mannequin tags.
	const SMannequinTagRequest& mannequinTagRequest = m_movementRequest.GetMannequinTagRequest();
	movementRequest.mannequinTagsClear = mannequinTagRequest.clearTags;
	movementRequest.mannequinTagsSet = mannequinTagRequest.setTags;

	// Request for cover.
	movementRequest.coverRequest = m_movementRequest.GetAICoverRequest();

	movementRequest.shouldJump = m_movementRequest.ShouldJump();

	// TODO: Are we really expected to clear the jump flag here?
	m_movementRequest.ClearJump();

	movementRequest.aimTarget.zero();
	movementRequest.lookTarget.zero();
	movementRequest.useLookIK = false;
	movementRequest.useAimIK = false;

	// Hard coding for now.
	movementRequest.allowStrafe = true;
	//movementRequest.allowStrafe = false;
	movementRequest.coverRequest = m_movementRequest.GetAICoverRequest();
	movementRequest.desiredPeekOver = 0.0f;
	movementRequest.desiredLean = 0.0f;
	movementRequest.sprint = 4.0f;
	movementRequest.stance = EStance::STANCE_RELAXED;
	//movementRequest.prediction // TODO: not handled, unsure of it's function.
}


void CCharacterMovementController::Compute()
{
	ComputeMovementRequest();
	ComputeMovementState();
}


void CCharacterMovementController::ComputeMovementRequest()
{
	CMovementRequest request;
	auto pPlayer = CPlayer::GetLocalPlayer();
	auto pPlayerInput = pPlayer->GetPlayerInput();

	// FIXME: This isn't a great place to reset the deltas for the movement and rotation, but it's the best option
	// for quick testing. Improve this later. CPlayer did it in another function - check where and see if it's
	// a better option than here.
	m_movementRequest.RemoveDeltaRotation();
	m_movementRequest.RemoveDeltaMovement();

	// Based on directional inputs, we calculate a vector in the direction the player wants to move.
	// This is added as a delta movement.
	// TODO: speed has to be handled somewhere - not sure where is best yet.
	Quat movementDirection = pPlayer->GetCamera()->GetRotation();
	Vec3 movement = pPlayerInput->GetMovement(movementDirection);
	if (movement.GetLengthFast() > FLT_EPSILON)
		request.AddDeltaMovement(movement * 2.4f); // HACK: movement speed is hard coded here right now...fix later.

	// HACK: For now we are using the player input without checking to see if this is the client, and other
	// safety checks.
	request.AddDeltaRotation(Ang3(0.0f, 0.0f, pPlayerInput->GetYawDelta()));

	// This seems to be a really awful way of conveying the animated movement speed. See FlowNodeAIAction.cpp
	// for details - maybe also search other places and see if that description is correct.
	request.SetPseudoSpeed(1.0f);	

	// TODO: we can also set values for where the player is looking and aiming. This can come from the camera 
	// for player input (within turning limits of the head) allowing the head to track where the player
	// is looking. Aim target should also be set here. AI input routines can work out their own needs.
	// Camera should support a LookTarget () query to help out, since it knows if they are FP / TP / VR.
	//const Vec3 aimTarget = pPlayer->GetCamera()->GetAimTarget();
	//request.SetAimTarget(aimTarget);
	//request.SetLookTarget(aimTarget); // TODO: needs to be limited to amount neck can turn.

	CryWatch("%s : velocity = %f, %f, %f\r\n", __func__, movement.x, movement.y, movement.z);

	// Request the movement. 
	RequestMovement(request);
}


void CCharacterMovementController::ComputeMovementState()
{
	IEntity* pEntity = m_pCharacter->GetEntity();

	if (pEntity && m_pCharacter)
	{
		m_movementState.minSpeed = -1.0f;
		m_movementState.maxSpeed = -1.0f;
		m_movementState.normalSpeed = -1.0f;

		// Definitely not the right way to move them :D
		if (m_movementRequest.HasDeltaMovement())
		{
			Vec3 vecMove = m_movementRequest.GetDeltaMovement();

			m_movementState.pos = pEntity->GetWorldPos();
			m_movementState.desiredSpeed = 2.4f;  // HACK: get speed multiplier from stance, etc. It's worth noting that a later piece of code is scaling this back to 1.0f presently. See if we need this happening here at all.

			m_movementState.isMoving = true;
			m_movementState.movementDirection = vecMove.GetNormalizedSafe();

			m_movementState.eyePosition = pEntity->GetWorldPos() + m_pCharacter->GetLocalEyePos();
			
			// TODO: ** NOW ** add in the look target, aimtarget, firetargets and set them to be 5 metres in front of where we are looking.
			// I suspect these are what is causing him to rotate back around again.

			//m_movementState.lookTarget = 
			
			// HACK: just sticking in a rough estimate for now to get raycasts / world queries working.
			//m_movementState.eyeDirection = (m_lookTarget - m_movementState.eyePosition).GetNormalizedSafe(forward);
			m_movementState.eyeDirection = m_movementRequest.GetDeltaMovement();
		}
		else
		{
			// TODO: Check if we need to look out for falling / sliding movement when setting this value.
			m_movementState.isMoving = false;
		}

		// NOTE: try to keep code un-entangled.
		auto pPlayer = CPlayer::GetLocalPlayer();
		if (pPlayer)
		{
			// HACK: For testing purposes.
			//m_movementState.isAiming = true;
			m_movementState.isAiming = false;
			//m_movementState.aimDirection = (m_aimTarget - m_movementState.weaponPosition).GetNormalizedSafe((m_lookTarget - m_movementState.weaponPosition).GetNormalizedSafe(forward));
			//m_movementState.aimDirection = pPlayer->GetCamera()->GetRotation().GetColumn1();
		}

		// NOTE: try to keep code un-entangled.
		{
			// HACK: For testing purposes.
			//m_movementState.isFiring = false;
			m_movementState.isFiring = false;
			//m_movementState.fireTarget = m_fireTarget;
			//m_movementState.fireDirection = (m_movementState.fireTarget - m_movementState.weaponPosition).GetNormalizedSafe(FORWARD_DIRECTION);
			
			// HACK:
			m_movementState.isAlive = true;
			m_movementState.isVisible = true;
			
			// Missing values - track these down and implement.
			//m_movementState.stance = m_targetStance;
			//m_movementState.animationEyeDirection = ;
			//m_movementState.lastMovementDirection = ;
			//m_movementState.minSpeed = ;
			//m_movementState.maxSpeed = ;
			//m_movementState.normalSpeed = ;
			//m_movementState.slopeAngle = ;
			//m_movementState.atMoveTarget = ;
		}
	}
}


void CCharacterMovementController::FullSerialize(TSerialize ser)
{
	// Don't perform serialisation to the network. Game save and script only.
	if (ser.GetSerializationTarget() != eST_Network)
	{
		ser.Value("DesiredSpeed", m_desiredSpeed);
		ser.Value("atTarget", m_bAtTarget);
		ser.Value("usingLookIK", m_bUsingLookIK);
		ser.Value("usingAimIK", m_bUsingAimIK);
		ser.Value("aimClamped", m_bAimClamped);
		ser.Value("lookTarget", m_lookTarget);
		ser.Value("aimTarget", m_aimTarget);
		ser.Value("animTargetSpeed", m_animTargetSpeed);
		ser.Value("animTargetSpeedCounter", m_animTargetSpeedCounter);
		ser.Value("fireTarget", m_fireTarget);
		ser.EnumValue("targetStance", m_targetStance, STANCE_NULL, STANCE_LAST);

		ser.BeginGroup("MovementState");
		ser.Value("entityDirection", m_movementState.entityDirection);
		ser.Value("aimDir", m_movementState.aimDirection);
		ser.Value("fireDir", m_movementState.fireDirection);
		ser.Value("fireTarget", m_movementState.fireTarget);
		ser.Value("weaponPos", m_movementState.weaponPosition);
		ser.Value("desiredSpeed", m_movementState.desiredSpeed);
		ser.Value("moveDir", m_movementState.movementDirection);
		ser.Value("upDir", m_movementState.upDirection);
		ser.EnumValue("stance", m_movementState.stance, STANCE_NULL, STANCE_LAST);
		ser.Value("pos", m_movementState.pos);
		ser.Value("eyePos", m_movementState.eyePosition);
		ser.Value("eyeDir", m_movementState.eyeDirection);
		ser.Value("animationEyeDirection", m_movementState.animationEyeDirection);
		ser.Value("minSpeed", m_movementState.minSpeed);
		ser.Value("normalSpeed", m_movementState.normalSpeed);
		ser.Value("maxSpeed", m_movementState.maxSpeed);
		ser.Value("stanceSize.Min", m_movementState.m_StanceSize.min);
		ser.Value("stanceSize.Max", m_movementState.m_StanceSize.max);
		ser.Value("colliderSize.Min", m_movementState.m_ColliderSize.min);
		ser.Value("colliderSize.Max", m_movementState.m_ColliderSize.max);
		ser.Value("atMoveTarget", m_movementState.atMoveTarget);
		ser.Value("isAlive", m_movementState.isAlive);
		ser.Value("isAiming", m_movementState.isAiming);
		ser.Value("isFiring", m_movementState.isFiring);
		ser.Value("isVisible", m_movementState.isVisible);
		ser.EndGroup();

		// Movement state needs to be updated if we just read out state.
		if (ser.IsReading())
			ComputeMovementState();
	}
}
