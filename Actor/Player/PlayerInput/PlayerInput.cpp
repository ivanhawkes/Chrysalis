#include <StdAfx.h>

#include "PlayerInput.h"
#include <IActionMapManager.h>
#include <CryMath/Cry_Math.h>
#include <Game/Game.h>
#include <Actor/Player/Player.h>
#include <Actor/Character/Character.h>
#include <Camera/ICamera.h>
#include <ConsoleVariables/ConsoleVariables.h>


// ***
// *** ISimpleExtension
// ***


void CPlayerInput::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);

	// Query for the player that owns this extension.
	m_pPlayer = static_cast<CPlayer *>(pGameObject->QueryExtension("Player"));

	// Detect when we become the local player.
	const int requiredEvents [] = { eGFE_BecomeLocalPlayer };
	pGameObject->UnRegisterExtForEvents(this, NULL, 0);
	pGameObject->RegisterExtForEvents(this, requiredEvents, sizeof(requiredEvents) / sizeof(int));
}


void CPlayerInput::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// NOTE: By accumulating the deltas in the post update, we get consistent values back during pre-physics
// and update, which is where most the work is done. There is zero chance of errors due to drift in values
// from one update / pre-physics to another.
// The downside is we have a definite lag of one frame for all input - and for VR this might be killer.
// Ideally we would have a component system able to prioritise objects before others - and this would be
// given a high priority.
// TODO: Re-write when the above is available.

void CPlayerInput::PostUpdate(float frameTime)
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


void CPlayerInput::HandleEvent(const SGameObjectEvent &event)
{
	// We only register action maps if we are the local player.
	if (event.event == eGFE_BecomeLocalPlayer)
	{
		RegisterActionMaps();
	}
}


// ***
// ***  IPlayerInput
// ***


void CPlayerInput::ResetMovementState()
{
	m_movementStateFlags = EMovementStateFlags::None;
	shouldSprint = false;
	shouldRun = false;
}


void CPlayerInput::ResetActionState()
{
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
	return Ang3(0.0f, 0.0f, 0.0f);
}


float CPlayerInput::GetHeadPitchDelta()
{
	return 0.0f;
}


float CPlayerInput::GetHeadYawDelta()
{
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


// ***
// *** CPlayerInput
// ***

CPlayerInput::~CPlayerInput()
{
	// Clean up our action map usage.
	IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	//pActionMapManager->EnableActionMap("player", false);
	pActionMapManager->RemoveExtraActionListener(this, "player");

	// NOTE: Good sense says we should be able to capture and release the actions - but it doesn't seem to like it when
	// two extensions do it on one game object...commented out to make it work, but left in codefor explanation.
	// CCameraManager is handling it for us.
	// This technique seems to be fragile - expect it to break at some point.
	// TODO: refactor this out.
	//GetGameObject()->ReleaseActions(this);
}

void CPlayerInput::RegisterActionMaps()
{
	// Populate the action handler callbacks so that we get action map events.
	InitializeActionHandler();

	// Try and enable the "player" actions, which control our player.
	IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	//pActionMapManager->EnableActionMap("player", true);
	pActionMapManager->AddExtraActionListener(this, "player");

	// NOTE: Good sense says we should be able to capture and release the actions - but it doesn't seem to like it when
	// two extensions do it on one game object...commented out to make it work, but left in codefor explanation.
	// CCameraManager is handling it for us.
	// This technique seems to be fragile - expect it to break at some point.
	// TODO: refactor this out.
	// GetGameObject()->CaptureActions(this); 
}


void CPlayerInput::InitializeActionHandler()
{
	// Basic movement.
	m_actionHandler.AddHandler(ActionId("move_left"), &CPlayerInput::OnActionMoveLeft);
	m_actionHandler.AddHandler(ActionId("move_right"), &CPlayerInput::OnActionMoveRight);
	m_actionHandler.AddHandler(ActionId("move_forward"), &CPlayerInput::OnActionMoveForward);
	m_actionHandler.AddHandler(ActionId("move_backward"), &CPlayerInput::OnActionMoveBackward);

	// Mouse yaw and pitch handlers.
	m_actionHandler.AddHandler(ActionId("mouse_rotateyaw"), &CPlayerInput::OnActionRotateYaw);
	m_actionHandler.AddHandler(ActionId("mouse_rotatepitch"), &CPlayerInput::OnActionRotatePitch);

	// Zoom handlers.
	m_actionHandler.AddHandler(ActionId("tpv_zoom_in"), &CPlayerInput::OnActionZoomIn);
	m_actionHandler.AddHandler(ActionId("tpv_zoom_out"), &CPlayerInput::OnActionZoomOut);

	// XBox controller yaw and pitch handlers.
	m_actionHandler.AddHandler(ActionId("xi_rotateyaw"), &CPlayerInput::OnActionXIRotateYaw);
	m_actionHandler.AddHandler(ActionId("xi_rotatepitch"), &CPlayerInput::OnActionXIRotatePitch);

	// Jump.
	m_actionHandler.AddHandler(ActionId("move_jump"), &CPlayerInput::OnActionJump);

	// Walk, run / jog, sprint.
	m_actionHandler.AddHandler(ActionId("move_walkrun"), &CPlayerInput::OnActionWalkRun);
	m_actionHandler.AddHandler(ActionId("move_sprint"), &CPlayerInput::OnActionSprint);

	// Stances under player control.
	m_actionHandler.AddHandler(ActionId("stance_crouch"), &CPlayerInput::OnActionCrouch);
	m_actionHandler.AddHandler(ActionId("stance_kneel"), &CPlayerInput::OnActionKneel);
	m_actionHandler.AddHandler(ActionId("stance_sit"), &CPlayerInput::OnActionSit);

	// Interact with an object.
	m_actionHandler.AddHandler(ActionId("item_use"), &CPlayerInput::OnActionItemUse);
	m_actionHandler.AddHandler(ActionId("item_pickup"), &CPlayerInput::OnActionItemPickup);
	m_actionHandler.AddHandler(ActionId("item_drop"), &CPlayerInput::OnActionItemDrop);
	m_actionHandler.AddHandler(ActionId("item_throw"), &CPlayerInput::OnActionItemThrow);

	// Action bars.
	m_actionHandler.AddHandler(ActionId("actionbar_01"), &CPlayerInput::OnActionBar01);
	m_actionHandler.AddHandler(ActionId("actionbar_02"), &CPlayerInput::OnActionBar02);
	m_actionHandler.AddHandler(ActionId("actionbar_03"), &CPlayerInput::OnActionBar03);
	m_actionHandler.AddHandler(ActionId("actionbar_04"), &CPlayerInput::OnActionBar04);
	m_actionHandler.AddHandler(ActionId("actionbar_05"), &CPlayerInput::OnActionBar05);
	m_actionHandler.AddHandler(ActionId("actionbar_06"), &CPlayerInput::OnActionBar06);
	m_actionHandler.AddHandler(ActionId("actionbar_07"), &CPlayerInput::OnActionBar07);
	m_actionHandler.AddHandler(ActionId("actionbar_08"), &CPlayerInput::OnActionBar08);
	m_actionHandler.AddHandler(ActionId("actionbar_09"), &CPlayerInput::OnActionBar09);
	m_actionHandler.AddHandler(ActionId("actionbar_10"), &CPlayerInput::OnActionBar10);
	m_actionHandler.AddHandler(ActionId("actionbar_11"), &CPlayerInput::OnActionBar11);
	m_actionHandler.AddHandler(ActionId("actionbar_12"), &CPlayerInput::OnActionBar12);
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
		shouldRun = !shouldRun;
		CryLogAlways("Player toggled walking / running");
	}

	return false;
}


bool CPlayerInput::OnActionSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		shouldSprint = false;
		CryLogAlways("Player stopped sprinting");
	}
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
	{
		shouldSprint = true;
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


bool CPlayerInput::OnActionBar(EntityId entityId, const ActionId& actionId, int activationMode, int buttonId)
{
	if (activationMode == eAAM_OnRelease)
	{
		// Nothing happening at present.
	}
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
	{
		if (auto pActor = CPlayer::GetLocalActor())
		{
			pActor->OnActionBarUse(entityId, buttonId);
		}
	}

	return false;
}


bool CPlayerInput::OnActionBar01(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 1);
}


bool CPlayerInput::OnActionBar02(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 2);
}


bool CPlayerInput::OnActionBar03(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 3);
}


bool CPlayerInput::OnActionBar04(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 4);
}


bool CPlayerInput::OnActionBar05(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 5);
}


bool CPlayerInput::OnActionBar06(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 6);
}


bool CPlayerInput::OnActionBar07(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 7);
}


bool CPlayerInput::OnActionBar08(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 8);
}


bool CPlayerInput::OnActionBar09(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 9);
}


bool CPlayerInput::OnActionBar10(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 10);
}


bool CPlayerInput::OnActionBar11(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 11);
}


bool CPlayerInput::OnActionBar12(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 12);
}

