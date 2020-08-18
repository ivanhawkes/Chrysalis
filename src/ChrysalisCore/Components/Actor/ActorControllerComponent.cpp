#include <StdAfx.h>

#include "ActorControllerComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include "Components/Player/PlayerComponent.h"


namespace Chrysalis
{
// Definition of the state machine that controls actor movement.
DEFINE_STATE_MACHINE(CActorControllerComponent, Movement);

static void RegisterActorControllerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CActorControllerComponent));
		// Functions
		{
		}
	}
}


void CActorControllerComponent::ReflectType(Schematyc::CTypeDesc<CActorControllerComponent>& desc)
{
	desc.SetGUID(CActorControllerComponent::IID());
	desc.SetEditorCategory("Actors");
	desc.SetLabel("Actor Controller");
	desc.SetDescription("Actor controller.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});

	// Mark the actor component as a hard requirement.
	desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, CActorComponent::IID());

	// Mark the advanced animation component as a hard requirement.
	//desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, "{3CD5DDC5-EE15-437F-A997-79C2391537FE}"_cry_guid);
}


void CActorControllerComponent::Initialize()
{
	// Mesh and animation.
	m_pActorAnimationComponent = m_pEntity->GetOrCreateComponent<CActorAnimationComponent>();

	// Character movement controller.
	m_pCharacterControllerComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();

	// We need to know which actor component we are paired with. The actor controller class is pretty worthless without this.
	m_pActorComponent = m_pEntity->GetOrCreateComponent<CActorComponent>();
	CRY_ASSERT_MESSAGE(m_pActorComponent, "The actor controller component must be paired with an actor component.");

	// Initialise the movement state machine.
	MovementHSMInit();

	// Get it into a known state.
	OnResetState();
}


void CActorControllerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		// Physicalize on level start for Launcher
		case EEntityEvent::LevelStarted:

			// Editor specific, physicalize on reset, property change or transform change
		case EEntityEvent::Reset:
		case EEntityEvent::EditorPropertyChanged:
		case EEntityEvent::TransformChangeFinishedInEditor:
			OnResetState();
			break;

		case EEntityEvent::Update:
		{
			SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];
			Update(pCtx);
		}
		break;

		case EEntityEvent::PrePhysicsUpdate:
			PrePhysicsUpdate();
			break;
	}
}


void CActorControllerComponent::Update(SEntityUpdateContext* pCtx)
{
	const float frameTime = pCtx->fFrameTime;

	UpdateMovementRequest(frameTime);
	UpdateLookDirectionRequest(frameTime);
	UpdateAnimation(frameTime);

	// Update the movement state machine.
	MovementHSMUpdate(*pCtx);
}


void CActorControllerComponent::PrePhysicsUpdate()
{
	// TODO: HACK: BROKEN: This stuff was commented out in the character pre-physics. Some of it might belong here now.

	//	// The routine will need to be rewritten to work with actors only, or we need a new one that does the actor, that
	//	// is called by a character version of this.
	//	CActorStateUtility::UpdatePhysicsState(*this, m_actorPhysics, frameTime);

	//	//#ifdef STATE_DEBUG
	//	//		if (g_pGameCVars->pl_watchPlayerState >= (bIsClient ? 1 : 2))
	//	//		{
	//	//			// NOTE: outputting this info here is 'was happened last frame' not 'what was decided this frame' as it occurs before the prePhysicsEvent is dispatched
	//	//			// also IsOnGround and IsInAir can possibly both be false e.g. - if you're swimming
	//	//			// May be able to remove this log now the new HSM debugging is in if it offers the same/improved functionality
	//	//			CryWatch("%s stance=%s flyMode=%d %s %s%s%s%s", GetEntity()->GetEntityTextDescription(), GetStanceName(GetStance()), m_actorState.flyMode, IsOnGround() ? "ON-GROUND" : "IN-AIR", IsViewFirstPerson() ? "FIRST-PERSON" : "THIRD-PERSON", IsDead() ? "DEAD" : "ALIVE", m_actorState.isScoped ? " SCOPED" : "", m_actorState.isInBlendRagdoll ? " FALLNPLAY" : "");
	//	//		}
	//	//#endif

	//	// Push the pre-physics event down to our state machine.
	//	const SActorPrePhysicsData prePhysicsData(frameTime);
	//	const SStateEventActorMovementPrePhysics prePhysicsEvent(&prePhysicsData);
	//	StateMachineHandleEventMovement(STATE_DEBUG_APPEND_EVENT(prePhysicsEvent));

	//	// Bring the animation state of the character into line with it's requested state.
	//	UpdateAnimationState();
}


void CActorControllerComponent::OnResetState()
{
	OnRevive();
}



// ***
// *** Hierarchical State Machine Support
// ***


void CActorControllerComponent::SelectMovementHierarchy()
{
	// Force the state machine in the proper hierarchy
	//if (IsAIControlled ())
	//{
	//	CRY_ASSERT (!IsPlayer ());

	//	StateMachineHandleEventMovement (ACTOR_EVENT_ENTRY_AI);
	//}
	//else
	//{
	//	StateMachineHandleEventMovement (ACTOR_EVENT_ENTRY_PLAYER);
	//}

	// #HACK: set it to always be player for now.
	// #TODO: NEED THIS!!!
	StateMachineHandleEventMovement(ACTOR_EVENT_ENTRY);
}


void CActorControllerComponent::MovementHSMRelease()
{
	StateMachineReleaseMovement();
}


void CActorControllerComponent::OnRevive()
{
	// Reset the various state members.
	m_lookTarget.zero();
	m_aimTarget.zero();
	m_useLookTarget = false;
	m_useLookIK = true;
	m_useAimIK = true;

	// Reset the HSM for character movement.
	MovementHSMReset();

	// Select which HSM to use for our character's movement. This relies on it's AI status being
	// correctly set first.
	SelectMovementHierarchy();
}


void CActorControllerComponent::MovementHSMInit()
{
	StateMachineInitMovement();
}


void CActorControllerComponent::MovementHSMSerialize(TSerialize ser)
{
	StateMachineSerializeMovement(SStateEventSerialize(ser));
}


void CActorControllerComponent::MovementHSMUpdate(SEntityUpdateContext& ctx)
{
	//#ifdef STATE_DEBUG
	//	const bool shouldDebug = (s_StateMachineDebugEntityID == GetEntityId ());
	//#else
	//	const bool shouldDebug = false;
	//#endif
	const bool shouldDebug = false;
	//const bool shouldDebug = true;

	StateMachineUpdateMovement(ctx.fFrameTime, shouldDebug);

	// Pass the update into the movement state machine.
	StateMachineHandleEventMovement(SStateEventUpdate(ctx.fFrameTime));
}


void CActorControllerComponent::MovementHSMReset()
{
	StateMachineResetMovement();
}


const bool CActorControllerComponent::IsPlayer() const
{
	return m_pActorComponent->IsPlayer();
}


const bool CActorControllerComponent::IsClient() const
{
	return m_pActorComponent->IsClient();
}


const Vec3 CActorControllerComponent::GetLocalEyePos() const
{
	return m_pActorComponent->GetLocalEyePos();
}


const float CActorControllerComponent::GetLowerBodyRotation(TInputFlags movementDirectionFlags) const
{
	float relativeDirection;

	// Take the mask and turn it into an angular rotation delta which represents which
	// direction the body is turned when moving in a given direction.
	switch (movementDirectionFlags)
	{
		case (TInputFlags)EInputFlag::Forward:
			relativeDirection = 0.0f;
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Right):
			relativeDirection = -45.0f;
			break;

		case (TInputFlags)EInputFlag::Right:
			//relativeDirection = -90.0f;
			relativeDirection = -45.0f;
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Right):
			relativeDirection = 45.0f;
			//relativeDirection = 0.0f;
			break;

		case (TInputFlags)EInputFlag::Backward:
			//relativeDirection = 180.0f;
			relativeDirection = 0.0f;
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Left):
			relativeDirection = -45.0f;
			//relativeDirection = 0.0f;
			break;

		case (TInputFlags)EInputFlag::Left:
			//relativeDirection = 90.0f;
			relativeDirection = 45.0f;
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Left):
			relativeDirection = 45.0f;
			break;

		default:
			relativeDirection = 0.0f;
			break;
	}

	return relativeDirection;
}


void CActorControllerComponent::UpdateMovementRequest(float frameTime)
{
	// Default is for them to not request movement.
	m_movementRequest = ZERO;

	// Don't handle input if we are in air.
	if (!m_pCharacterControllerComponent->IsOnGround())
	{
		m_movingDuration = 0.0f;
		return;
	}

	// If there's a player controlling us, we can query them for inputs and camera and apply that to our movement.
	if (auto* pPlayer = m_pActorComponent->GetPlayer())
	{
		/**
		The request needs to take into account both the direction of the camera and the direction of
		the movement i.e. left is alway left relative to the camera. TODO: What will it take to make this
		use AddVelocity instead? The values don't seem to match with what I'd expect to input to it.
		**/
		auto* pPlayerInput = pPlayer->GetPlayerInput();
		float moveSpeed = GetMovementBaseSpeed(pPlayerInput->GetMovementDirectionFlags());
		if ((moveSpeed > FLT_EPSILON) && (pPlayerInput->IsMovementRequested()))
		{
			m_movingDuration += frameTime;
			m_movementRequest = pPlayerInput->GetMovement(pPlayer->GetCamera()->GetRotation()) * moveSpeed;
			SetVelocity(m_movementRequest);
		}
		else
		{
			// I'm forcing the velocity to zero if we're not actively controlling the character. This naive
			// approach is probably wrong, but it works for now. 
			SetVelocity(ZERO);
		}
	}
}


void CActorControllerComponent::UpdateLookDirectionRequest(float frameTime)
{
	// The angular velocity maximum (Full rotations / second).
	const float angularVelocityMax = g_PI2 * 1.5f;

	// The catchup speed (Full rotations / second).
	const float catchupSpeed = g_PI2 * 1.2f;

	// If there's a player controlling us, we can query them for inputs and camera and apply that to our rotation.
	if (auto* pPlayer = m_pActorComponent->GetPlayer())
	{
		auto* pPlayerInput = pPlayer->GetPlayerInput();

		// Only allow the character to rotate in first person, and third person if they are moving.
		if ((pPlayer->IsViewFirstPerson()) || (!pPlayer->IsViewFirstPerson() && m_movementRequest.len() > FLT_EPSILON))
		{
			Ang3 facingDir;
			if (pPlayer->IsViewFirstPerson())
				facingDir = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
			else
				facingDir = CCamera::CreateAnglesYPR(m_movementRequest.GetNormalizedFast());

			// Use their last orientation as their present direction.
			// NOTE: I tried it with GetEntity()->GetWorldTM() but that caused crazy jitter issues.
			Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));

			// We add in some extra rotation to 'catch up' to the direction they are being moved. This will perform a gradual
			// turn on the actor over several frames.
			float rotationDelta {0.0f};
			if (std::abs(facingDir.x - ypr.x) > g_PI)
				rotationDelta = ypr.x - facingDir.x;
			else
				rotationDelta = facingDir.x - ypr.x;

			// Catchup allows us to step towards the goal direction in even steps using a set angular velocity.
			float catchUp {0.0f};
			if (std::abs(rotationDelta) > FLT_EPSILON)
			{
				if (rotationDelta > 0.0f)
					catchUp = std::min(rotationDelta, catchupSpeed * frameTime);
				else
					catchUp = std::max(rotationDelta, -catchupSpeed * frameTime);
			}

			// Update angular velocity metrics.
			m_yawAngularVelocity = CLAMP(pPlayerInput->GetMouseYawDelta() + catchUp, -angularVelocityMax * frameTime, angularVelocityMax * frameTime);

			// Yaw.
			ypr.x += m_yawAngularVelocity;

			// Roll (zero it).
			ypr.z = 0;

			// Update the preferred direction we face.
			m_lookOrientation = Quat(CCamera::CreateOrientationYPR(ypr));
		}
	}
}


void CActorControllerComponent::UpdateAnimation(float frameTime)
{
	if (auto* pPlayer = m_pActorComponent->GetPlayer())
	{
		// Radians / sec
		const float angularVelocityMin = 0.174f;

		// Update tags and motion parameters used for turning
		const bool isTurning = std::abs(m_yawAngularVelocity) > angularVelocityMin;

		// Resolve the animation tags.
		// HACK: This should be done once on init or on entity changed events or similar. It fails hard if the init order
		// is switched with CActorAnimationComponent.
		if ((m_rotateTagId == TAG_ID_INVALID) && (strlen(m_pActorAnimationComponent->GetControllerDefinitionFile()) > 0))
			m_rotateTagId = m_pActorAnimationComponent->GetTagId("Rotate");

		// Set the tag, if it exists.
		if (m_rotateTagId != TAG_ID_INVALID)
			m_pActorAnimationComponent->SetTagWithId(m_rotateTagId, isTurning);

		if (isTurning)
		{
			// Expect the turning motion to take approximately one second.
			// TODO: Get to work on making this happen more like Blade and Soul.
			const float turnDuration = 1.0f;
			m_pActorAnimationComponent->SetMotionParameter(eMotionParamID_TurnAngle, m_yawAngularVelocity * turnDuration);
		}

		// Update entity rotation as the player turns. We only want to affect Z-axis rotation, zero pitch and roll.
		// TODO: is there a case where we want to avoid zeroing out pitch and roll?
		Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
		ypr.y = 0;
		ypr.z = 0;
		const Quat correctedOrientation = Quat(CCamera::CreateOrientationYPR(ypr));

		// Send updated transform to the entity, only orientation changes.
		GetEntity()->SetPosRotScale(GetEntity()->GetWorldPos(), correctedOrientation, Vec3(1, 1, 1));
	}
}


float CActorControllerComponent::GetMovementBaseSpeed(TInputFlags movementDirectionFlags) const
{
	const static float walkBaseSpeed {2.1f};
	const static float jogBaseSpeed {4.2f};
	const static float runBaseSpeed {6.3f};
	const static float crawlBaseSpeed {1.2f};
	const static float proneBaseSpeed {0.4f};
	const static float crouchBaseSpeed {1.2f};
	float baseSpeed {0.0f};
	float dirScale {1.0f};

	switch (GetStance())
	{
		case EActorStance::standing:
			// Work out a base for walking, jogging or sprinting.
			if (IsSprinting())
			{
				baseSpeed = runBaseSpeed;
			}
			else
			{
				if (IsJogging())
					baseSpeed = jogBaseSpeed;
				else
					baseSpeed = walkBaseSpeed;
			}
			break;

		case EActorStance::crawling:
			baseSpeed = crawlBaseSpeed;
			break;

		case EActorStance::prone:
			baseSpeed = proneBaseSpeed;
			break;

		case EActorStance::crouching:
			baseSpeed = crouchBaseSpeed;
			break;

		case EActorStance::swimming:
			baseSpeed = walkBaseSpeed;
			break;

		case EActorStance::flying:
			baseSpeed = jogBaseSpeed;
			break;

		case EActorStance::spellcasting:
			baseSpeed = walkBaseSpeed;
			break;

		default:
			// Don't let them control movement.
			baseSpeed = 0.0f;
			break;
	}

	// Scale it based on their movement direction.
	switch (movementDirectionFlags)
	{
		case (TInputFlags)EInputFlag::Forward:
			dirScale = 1.0f;
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Right):
			dirScale = 0.9f;
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Left):
			dirScale = 0.9f;
			break;

		case (TInputFlags)EInputFlag::Right:
			dirScale = 0.85f;
			break;

		case (TInputFlags)EInputFlag::Left:
			dirScale = 0.85f;
			break;

		case (TInputFlags)EInputFlag::Backward:
			dirScale = 0.71f;
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Right):
			dirScale = 0.71f;
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Left):
			dirScale = 0.71f;
			break;

		default:
			dirScale = 0.0f;
			break;
	}

	return baseSpeed * dirScale;
}


// TODO: So wrong in every way. Need some more state checking before deciding which stance to move to when exiting
// crouching, though this is a pretty decent default. Needs checks to ensure you can't crouch while swimming / falling /
// etc - maybe put this into the class itself? 

void CActorControllerComponent::OnActionCrouchToggle()
{
	if (GetStance() == EActorStance::crouching)
		SetStance(EActorStance::standing);
	else
		SetStance(EActorStance::crouching);
}


void CActorControllerComponent::OnActionCrawlToggle()
{
	if (GetStance() == EActorStance::crawling)
		SetStance(EActorStance::standing);
	else
		SetStance(EActorStance::crawling);
}


void CActorControllerComponent::OnActionKneelToggle()
{
	if (GetStance() == EActorStance::kneeling)
		SetStance(EActorStance::standing);
	else
		SetStance(EActorStance::kneeling);
}


void CActorControllerComponent::OnActionSitToggle()
{
	if (GetStance() == EActorStance::sittingFloor)
		SetStance(EActorStance::standing);
	else
		SetStance(EActorStance::sittingFloor);
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActorControllerComponent)
}