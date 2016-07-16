#include <StdAfx.h>

#include "PlayerInput.h"
#include <IActionMapManager.h>
#include <CryMath/Cry_Math.h>
#include <Game/Game.h>
#include <ActionMaps/GameActionMaps.h>
#include <Actor/Player/Player.h>
#include <Actor/Character/Character.h>
#include <Camera/ICamera.h>
#include <Camera/ICameraManager.h>
#include <ConsoleVariables/ConsoleVariables.h>


// ***
// *** IGameObjectExtension
// ***


void CPlayerInput::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}


bool CPlayerInput::Init(IGameObject * pGameObject)
{
	// Critical this is called.
	SetGameObject(pGameObject);

	return true;
}


void CPlayerInput::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// We will require pre-physics updates.
	pGameObject->EnablePrePhysicsUpdate(ePPU_Always);

	// Register for pre-physics update.
	RegisterEvent(ENTITY_EVENT_PREPHYSICSUPDATE, IComponent::EComponentFlags_Enable);

	RegisterActionMaps();
}


bool CPlayerInput::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	// Restores this instance's game object in case it has changed.
	ResetGameObject();

	return true;
}


void CPlayerInput::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	// We can just add up all the acculmated requests to find out how much pitch / yaw is being requested.
	// It's also a good time to filter out any small movement requests to stabilise the camera / etc.
	m_lastPitchDelta = m_mousePitchDelta + m_xiPitchDelta;
	if (abs(m_lastPitchDelta) < m_pitchFilter)
		m_lastPitchDelta = 0.0f;
	m_lastYawDelta = m_mouseYawDelta + m_xiYawDelta;
	if (abs(m_lastYawDelta) < m_yawFilter)
		m_lastYawDelta = 0.0f;

	// Circle of life!
	m_mousePitchDelta = m_xiPitchDelta = m_mouseYawDelta = m_xiYawDelta = 0.0f;

	// Handle zoom level changes, result is stored for query by cameras on Update.
	m_lastZoomDelta = m_zoomDelta;
	m_zoomDelta = 0.0f;
}


void CPlayerInput::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_PREPHYSICSUPDATE:
			PrePhysicsUpdate();
			break;
	}
}


void CPlayerInput::PostUpdate(float frameTime)
{
}


// ***
// ***  IPlayerInput
// ***


void CPlayerInput::ResetMovementState()
{
	m_movementStateFlags = EMovementStateFlags::None;
	bSprint = false;
	bRun = false;
}


void CPlayerInput::ResetActionState()
{
	m_actions = 0;
	m_lastActions = m_actions;
}


Vec3 CPlayerInput::GetMovement(const Quat& baseRotation)
{
	bool allowMovement = true;
	Quat quatRelativeDirection;
	Vec3 vecMovement = Vec3(0.0f, 0.0f, 0.0f);

	// Take the mask and turn it into a vector to indicate the direction we need to pan independent of the
	// present camera direction.
	switch (m_movementStateFlags)
	{
		case EMovementStateFlags::Forward:
			quatRelativeDirection = Quat::CreateIdentity();
			break;

		case (EMovementStateFlags::Forward | EMovementStateFlags::Right) :
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(45.0f));
			break;

		case EMovementStateFlags::Right:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(90.0f));
			break;

		case (EMovementStateFlags::Backward | EMovementStateFlags::Right) :
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(135.0f));
			break;

		case EMovementStateFlags::Backward:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(180.0f));
			break;

		case (EMovementStateFlags::Backward | EMovementStateFlags::Left) :
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(225.0f));
			break;

		case EMovementStateFlags::Left:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(270.0f));
			break;

		case (EMovementStateFlags::Forward | EMovementStateFlags::Left) :
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(315.0f));
			break;

		default:
			quatRelativeDirection = Quat::CreateIdentity();
			allowMovement = false;
			break;
	}

	// Create a vector based on key direction. This is computed in local space for the base rotation.
	if (allowMovement)
		vecMovement = Vec3(baseRotation.GetFwdX(), baseRotation.GetFwdY(), 0.0f).GetNormalized() * quatRelativeDirection;

	return vecMovement;
}


Ang3 CPlayerInput::GetRotationDelta()
{
	return Ang3(m_lastPitchDelta, 0.0f, m_lastYawDelta);
}


float CPlayerInput::GetPitchDelta()
{
	return m_lastPitchDelta;
}


float CPlayerInput::GetYawDelta()
{
	return m_lastYawDelta;
}


Vec3 CPlayerInput::GetHeadMovement(const Quat& baseRotation)
{
	Vec3 vecMovement = Vec3(0.0f, 0.0f, 0.0f);
	return vecMovement;
}


Ang3 CPlayerInput::GetHeadRotationDelta()
{
//	return Ang3(m_lastPitchDelta, 0.0f, m_lastYawDelta);
	return Ang3(0.0f, 0.0f, 0.0f);
}


float CPlayerInput::GetHeadPitchDelta()
{
//	return m_lastPitchDelta;
	return 0.0f;
}


float CPlayerInput::GetHeadYawDelta()
{
//	return m_lastYawDelta;
	return 0.0f;
}


// ***
// *** IActionListener
// ***

void CPlayerInput::OnAction(const ActionId& action, int activationMode, float value)
{
	// We want to dispatch the action as if it were for the character that this player is currently attached onto.
	m_actionHandler.Dispatch(this, CPlayer::GetLocalPlayer()->GetAttachedEntityId(), action, activationMode, value);
}


void CPlayerInput::AfterAction()
{}


// ***
// *** CPlayerInput
// ***

void CPlayerInput::RegisterActionMaps()
{
	IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();

	if (pActionMapManager)
	{
		// Register our action map listener.
		pActionMapManager->AddExtraActionListener(this);

		// Only add the handlers once.
		if (m_actionHandler.GetNumHandlers() == 0)
		{
			// Basic movement.
			m_actionHandler.AddHandler(g_pGame->ActionMaps().move_left, &CPlayerInput::OnActionMoveLeft);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().move_right, &CPlayerInput::OnActionMoveRight);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().move_forward, &CPlayerInput::OnActionMoveForward);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().move_backward, &CPlayerInput::OnActionMoveBackward);

			// Mouse yaw and pitch handlers.
			m_actionHandler.AddHandler(g_pGame->ActionMaps().mouse_rotateyaw, &CPlayerInput::OnActionRotateYaw);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().mouse_rotatepitch, &CPlayerInput::OnActionRotatePitch);

			// Zoom handlers.
			m_actionHandler.AddHandler(g_pGame->ActionMaps().tpv_zoom_in, &CPlayerInput::OnActionZoomIn);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().tpv_zoom_out, &CPlayerInput::OnActionZoomOut);

			// XBox controller yaw and pitch handlers.
			m_actionHandler.AddHandler(g_pGame->ActionMaps().xi_rotateyaw, &CPlayerInput::OnActionXIRotateYaw);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().xi_rotatepitch, &CPlayerInput::OnActionXIRotatePitch);

			// Jump.
			m_actionHandler.AddHandler(g_pGame->ActionMaps().move_jump, &CPlayerInput::OnActionJump);

			// Walk, run / jog, sprint.
			m_actionHandler.AddHandler(g_pGame->ActionMaps().move_walkrun, &CPlayerInput::OnActionWalkRun);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().move_sprint, &CPlayerInput::OnActionSprint);

			// Stances under player control.
			m_actionHandler.AddHandler(g_pGame->ActionMaps().stance_crouch, &CPlayerInput::OnActionCrouch);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().stance_kneel, &CPlayerInput::OnActionKneel);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().stance_sit, &CPlayerInput::OnActionSit);

			// Interact with an object.
			m_actionHandler.AddHandler(g_pGame->ActionMaps().item_use, &CPlayerInput::OnActionItemUse);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().item_pickup, &CPlayerInput::OnActionItemPickup);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().item_drop, &CPlayerInput::OnActionItemDrop);
			m_actionHandler.AddHandler(g_pGame->ActionMaps().item_throw, &CPlayerInput::OnActionItemThrow);
		}
	}
}


void CPlayerInput::UnregisterActionMaps()
{
	gEnv->pGame->GetIGameFramework()->GetIActionMapManager()->RemoveExtraActionListener(this);
}


void CPlayerInput::PrePhysicsUpdate()
{
}


// ***
// *** Action handlers.
// ***

bool CPlayerInput::OnActionMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		m_movementStateFlags &= ~EMovementStateFlags::Left;
	}
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
	{
		m_movementStateFlags |= EMovementStateFlags::Left;
	}

	return false;
}


bool CPlayerInput::OnActionMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		m_movementStateFlags &= ~EMovementStateFlags::Right;
	}
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
	{
		m_movementStateFlags |= EMovementStateFlags::Right;
	}

	return false;
}


bool CPlayerInput::OnActionMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		m_movementStateFlags &= ~EMovementStateFlags::Forward;
	}
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
	{
		m_movementStateFlags |= EMovementStateFlags::Forward;
	}

	return false;
}


bool CPlayerInput::OnActionMoveBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		m_movementStateFlags &= ~EMovementStateFlags::Backward;
	}
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
	{
		m_movementStateFlags |= EMovementStateFlags::Backward;
	}

	return false;
}


bool CPlayerInput::OnActionRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	// Sensible scaling value for rotations in radians.
	float mouseSensitivity = 0.00032f * MAX(0.01f, g_pGameCVars->cl_mouseSensitivity * m_mousePitchYawSensitivity);

	// Add the yaw delta.
	m_mouseYawDelta -= value * mouseSensitivity;

	return false;
}


bool CPlayerInput::OnActionRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	// Have they inverted their mouse control?
	if (g_pGameCVars->cl_invertPitch)
		value *= -1.0f;

	// Sensible scaling value for rotations in radians.
	float mouseSensitivity = 0.00032f * MAX(0.01f, g_pGameCVars->cl_mouseSensitivity * m_mousePitchYawSensitivity);

	// Add the delta, taking into account mouse inversion.  Clamp the result.
	float invertYAxis = m_mouseInvertPitch ? -1.0f : 1.0f;
	m_mousePitchDelta += value * mouseSensitivity * invertYAxis;

	return false;
}


// XBox controller rotation is handled differently. Movements on the thumb stick set a value for
// rotation that should be applied every frame update.
bool CPlayerInput::OnActionXIRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	float radians = DEG2RAD(value);

	if (abs(radians) < m_xiYawFilter)
		m_xiYawDelta = 0.0f;
	else
		m_xiYawDelta = radians;

	return false;
}


// Xbox controller pitch is handled differently. Movements on the thumb stick set a value for
// pitch that should be applied every frame update.
bool CPlayerInput::OnActionXIRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	float radians = DEG2RAD(value);

	if (abs(radians) < m_xiPitchFilter)
		m_xiPitchDelta = 0.0f;
	else
		m_xiPitchDelta = radians;

	return false;
}


bool CPlayerInput::OnActionZoomIn(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_zoomDelta -= 1.0f;
	
	return false;
}


bool CPlayerInput::OnActionZoomOut(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_zoomDelta += 1.0f;

	return false;
}


bool CPlayerInput::OnActionJump(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		CryLogAlways("Player jumped");
	}

	return false;
}


bool CPlayerInput::OnActionCrouch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		CryLogAlways("Player crouched");
	}

	return false;
}


bool CPlayerInput::OnActionKneel(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		CryLogAlways("Player knelt");
	}

	return false;
}


bool CPlayerInput::OnActionSit(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		CryLogAlways("Player sat");
	}

	return false;
}


bool CPlayerInput::OnActionWalkRun(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		bRun = !bRun;
		CryLogAlways("Player toggled walking / running");
	}

	return false;
}


bool CPlayerInput::OnActionSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		bSprint = false;
		CryLogAlways("Player stopped sprinting");
	}
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
	{
		bSprint = true;
		CryLogAlways("Player sprinting");
	}

	return false;
}


bool CPlayerInput::OnActionItemUse(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionItemUse(entityId);
		}
	}

	return false;
}


bool CPlayerInput::OnActionItemPickup(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionItemPickup(entityId);
		}
	}

	return false;
}


bool CPlayerInput::OnActionItemDrop(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionItemDrop(entityId);
		}
	}

	return false;
}


bool CPlayerInput::OnActionItemThrow(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionItemThrow(entityId);
		}
	}

	return false;
}
