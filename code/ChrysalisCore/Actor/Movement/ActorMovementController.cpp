#include <StdAfx.h>

#include "ActorMovementController.h"
#include <CryAISystem/IAgent.h>
#include <Actor/Actor.h>
#include <Actor/ActorStance.h>
#include <Actor/ActorState.h>
#include "Components/Player/PlayerComponent.h"
#include <Components/Player/Camera/ICameraComponent.h>
#include <Components/Player/Input/IPlayerInputComponent.h>
#include <IMovementController.h>


namespace Chrysalis
{
// ***
// *** IMovementController
// ***


bool CActorMovementController::RequestMovement(CMovementRequest& request)
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

	// #TODO: handle the movement request.

	// #TODO: handle all the other possible input values.

	return ok;
}


// ***
// *** CActorMovementController
// ***


CActorMovementController::CActorMovementController(CActorComponent* pActor)
{
	m_pActor = pActor;
	OnResetState();
}


CActorMovementController::~CActorMovementController()
{}


void CActorMovementController::OnResetState()
{
	// Clear it down.
	m_movementRequest = CMovementRequest();

	// Set it all to it's default state.
	m_desiredSpeed = 0.0f;
	m_lookTarget.zero();
	m_aimTarget.zero();
	m_fireTarget.zero();
	m_bAtTarget = false;
	m_bUsingLookIK = true;
	m_bUsingAimIK = true;
	m_bAimClamped = false;
	m_animTargetSpeed = -1.0f;

	if (!GetISystem()->IsSerializingFile() == 1)
		ComputeMovementState();
}


void CActorMovementController::UpdateActorMovementRequest(SActorMovementRequest& movementRequest, float frameTime)
{
	// Desired movement.
	if (m_movementRequest.HasDeltaMovement())
	{
		// NOTE: desiredVelocity is normalized to the maximum speed for the specified stance, so DeltaMovement should be
		// between 0 and 1! 
		movementRequest.desiredVelocity += m_movementRequest.GetDeltaMovement();
	}

	// Rotation.
	if (m_movementRequest.HasDeltaRotation())
	{
		movementRequest.deltaAngles += m_movementRequest.GetDeltaRotation();
	}

	// Clearing and setting mannequin tags.
	const SMannequinTagRequest& mannequinTagRequest = m_movementRequest.GetMannequinTagRequest();
	movementRequest.mannequinTagsClear = mannequinTagRequest.clearTags;
	movementRequest.mannequinTagsSet = mannequinTagRequest.setTags;

	// Request for cover.
	movementRequest.coverRequest = m_movementRequest.GetAICoverRequest();

	movementRequest.shouldJump = m_movementRequest.ShouldJump();

	// #TODO: Are we really expected to clear the jump flag here?
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


	//movementRequest.prediction // #TODO: not handled, unsure of it's function.
}


void CActorMovementController::Compute()
{
	ComputeMovementRequest();
	ComputeMovementState();
}


void CActorMovementController::ComputeMovementRequest()
{
	CMovementRequest request;

	// FIXME: This isn't a great place to reset the deltas for the movement and rotation, but it's the best option
	// for quick testing. Improve this later. CPlayerComponent did it in another function - check where and see if it's
	// a better option than here.
	m_movementRequest.RemoveDeltaRotation();
	m_movementRequest.RemoveDeltaMovement();

	if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
	{
		// Check to see if we're allowed to take player input.
		// HACK: I have a feeling this isn't a great way to handle player input once networked players come into the game.
		// Look at refactoring this. 
		if (pPlayer->GetAttachedEntityId() == m_pActor->GetEntityId())
		{
			auto pPlayerInput = pPlayer->GetPlayerInput();
			auto pCamera = pPlayer->GetCamera();

			// Based on directional inputs, we calculate a vector in the direction the player wants to move.
			// This is added as a delta movement.
			// #TODO: speed has to be handled somewhere - not sure where is best yet.
			const Quat movementDirection = pCamera ? pPlayer->GetCamera()->GetRotation() : IDENTITY;

			// Player input always provides a unit or zero vector for the movement request.
			const Vec3 unitMovement = pPlayerInput->GetMovement(movementDirection);

			// #HACK: movement speed should be done much cleaner than this. We shouldn't need to mess
			// around working out if they are jogging or sprinting. Move to CActorComponent.
			Vec3 movement = unitMovement * m_pActor->GetMovementBaseSpeed(pPlayerInput->GetMovementDirectionFlags());

			// Rotation is handled differently in third person views.
			if (pPlayer->IsThirdPerson())
			{
				// Handle turning differently, based on whether they are moving.
				if (unitMovement.GetLengthFast() > FLT_EPSILON)
				{
					if (pPlayer->IsCharacterMovementAllowed())
					{
						// Add the movement.
						request.AddDeltaMovement(movement);
					}

					if (pPlayer->IsCharacterRotationAllowed())
					{
						// Find out the relative direction the body should naturally be facing when moving in the given direction.
						auto bodyRotation = GetLowerBodyRotation(pPlayerInput->GetMovementDirectionFlags());

						// Because they are moving, we want to turn their character to face the direction they are moving.
						float zDelta = movementDirection.GetRotZ() - m_pActor->GetEntity()->GetWorldAngles().z + bodyRotation;
						request.AddDeltaRotation(Ang3(0.0f, 0.0f, zDelta));
					}
				}
				//else
				//{
				//  // NOTE: If you want to have the character turn in lock-step with the camera, enable this section of code.
				//	request.AddDeltaRotation(Ang3(0.0f, 0.0f, pPlayerInput->GetYawDelta()));
				//	CryWatch("%s : turn = %f\r\n", __func__, request.GetDeltaRotation().z);
				//}
			}
			else
			{
				// Add the movement.
				if (pPlayer->IsCharacterMovementAllowed())
				{
					request.AddDeltaMovement(movement);
				}

				// Rotate the actor based on player input.
				if (pPlayer->IsCharacterRotationAllowed())
				{
					request.AddDeltaRotation(Ang3(0.0f, 0.0f, pPlayerInput->GetMouseYawDelta() - pPlayerInput->GetXiYawDelta()));
				}

				// #TODO: I also need to tilt the FP view camera up and down - and have this change the actor look / aim pose.
				// camera isn't connected to a bone at present due to not having a suitable one that doesn't scan around in
				// idle mode - super bad for nausea in VR. I'd prefer to lock that view directly to the actor's eyes, so
				// ideally no more code is required right here.
			}

			// This seems to be a really awful way of conveying the animated movement speed. See FlowNodeAIAction.cpp
			// for details - maybe also search other places and see if that description is correct.
			request.SetPseudoSpeed(1.0f);

			// #TODO: we can also set values for where the player is looking and aiming. This can come from the camera 
			// for player input (within turning limits of the head) allowing the head to track where the player
			// is looking. Aim target should also be set here. AI input routines can work out their own needs.
			// Camera should support a LookTarget () query to help out, since it knows if they are FP / TP / VR.
			if (pCamera)
			{
				const Vec3 aimTarget = pPlayer->GetCamera()->GetAimTarget(m_pActor->GetEntity());
				request.SetAimTarget(aimTarget);
				request.SetLookTarget(aimTarget); // #TODO: needs to be limited to amount neck can turn.
			}
		}
	}

	// Request the movement. 
	RequestMovement(request);
}


void CActorMovementController::ComputeMovementState()
{
	if (m_pActor)
	{
		if (auto pEntity = m_pActor->GetEntity())
		{
			m_movementState.minSpeed = -1.0f;
			m_movementState.maxSpeed = -1.0f;
			m_movementState.normalSpeed = -1.0f;

			m_movementState.eyePosition = pEntity->GetWorldPos() + m_pActor->GetLocalEyePos();

			//m_movementState.lookTarget = 

			// Definitely not the right way to move them :D
			if (m_movementRequest.HasDeltaMovement())
			{
				Vec3 vecMove = m_movementRequest.GetDeltaMovement();
				m_movementState.movementDirection = vecMove.GetNormalizedSafe();
				m_movementState.isMoving = true;
				m_movementState.pos = pEntity->GetWorldPos();

				// Not required in this codebase.
				m_movementState.desiredSpeed = 1.0f;

				// #HACK: just sticking in a rough estimate for now to get raycasts / world queries working.
				//m_movementState.eyeDirection = (m_lookTarget - m_movementState.eyePosition).GetNormalizedSafe(forward);
				m_movementState.eyeDirection = m_movementRequest.GetDeltaMovement();
			}
			else
			{
				// #TODO: Check if we need to look out for falling / sliding movement when setting this value.
				m_movementState.isMoving = false;
			}

			// TODO: It seems very likely that stance is an unused field, and can be ignored. We're setting it to NULL here
			// just to be sure.
			m_movementState.stance = EStance::STANCE_NULL;

			// NOTE: try to keep code un-entangled.
			auto pPlayer = CPlayerComponent::GetLocalPlayer();
			if (pPlayer)
			{
				// #HACK: For testing purposes.
				m_movementState.isAiming = false;
				//m_movementState.isAiming = true;
				//m_movementState.aimDirection = (m_aimTarget - m_movementState.weaponPosition).GetNormalizedSafe((m_lookTarget - m_movementState.weaponPosition).GetNormalizedSafe(forward));
				//m_movementState.aimDirection = pPlayer->GetCamera()->GetRotation().GetColumn1();
			}

			// NOTE: try to keep code un-entangled.
			{
				// #HACK: For testing purposes.
				m_movementState.isFiring = false;
				//m_movementState.fireTarget = m_fireTarget;
				//m_movementState.fireDirection = (m_movementState.fireTarget - m_movementState.weaponPosition).GetNormalizedSafe(FORWARD_DIRECTION);

				// #HACK:
				m_movementState.isAlive = true;
				m_movementState.isVisible = true;

				// Missing values - track these down and implement.
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
}


void CActorMovementController::FullSerialize(TSerialize ser)
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
		ser.Value("fireTarget", m_fireTarget);

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


const float CActorMovementController::GetLowerBodyRotation(TInputFlags movementDirectionFlags) const
{
	float relativeDirection;

	// Take the mask and turn it into an angular rotation delta which represents which
	// direction the body is turned when moving in a given direction.
	switch (movementDirectionFlags)
	{
		case (TInputFlags) EInputFlag::Forward:
			relativeDirection = 0.0f;
			break;

		case ((TInputFlags) EInputFlag::Forward | (TInputFlags) EInputFlag::Right):
			relativeDirection = -45.0f;
			break;

		case (TInputFlags) EInputFlag::Right:
			//relativeDirection = -90.0f;
			relativeDirection = -45.0f;
			break;

		case ((TInputFlags) EInputFlag::Backward | (TInputFlags) EInputFlag::Right):
			//relativeDirection = 45.0f;
			relativeDirection = 0.0f;
			break;

		case (TInputFlags) EInputFlag::Backward:
			relativeDirection = 0.0f;
			break;

		case ((TInputFlags) EInputFlag::Backward | (TInputFlags) EInputFlag::Left):
			//relativeDirection = -45.0f;
			relativeDirection = 0.0f;
			break;

		case (TInputFlags) EInputFlag::Left:
			//relativeDirection = 90.0f;
			relativeDirection = 45.0f;
			break;

		case ((TInputFlags) EInputFlag::Forward | (TInputFlags) EInputFlag::Left):
			relativeDirection = 45.0f;
			break;

		default:
			relativeDirection = 0.0f;
			break;
	}

	return relativeDirection;
}
}