#include <StdAfx.h>

#include "PlayerInputComponent.h"
#include <CryMath/Cry_Math.h>
#include <Player/Player.h>
#include <Actor/Character/Character.h>
#include <Player/Camera/CameraManagerComponent.h>
#include <Player/Camera/ICameraComponent.h>


CRYREGISTER_CLASS(CPlayerInputComponent)


class CPlayerInputRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		CChrysalisCorePlugin::RegisterEntityWithDefaultComponent<CPlayerInputComponent>("PlayerInput");
		//RegisterEntityWithDefaultComponent<CPlayerInputComponent>("PlayerInput", "Input", "character.bmp");
	}
};

CPlayerInputRegistrator g_PlayerInputRegistrator;


// ***
// *** ISimpleExtension
// ***


void CPlayerInputComponent::Initialize()
{
}


void CPlayerInputComponent::PostInit(IGameObject * pGameObject)
{
	//pGameObject->EnableUpdateSlot(this, CPlayer::EPlayerUpdateSlot::ePlayerUpdateSlot_Main);

	// Required for 5.3 to call update.
	GetEntity()->Activate(true);

	auto pEntity = GetEntity();

	// Query for the player that owns this extension.
	m_pPlayer = pEntity->GetComponent<CPlayer>();

	// Grab the camera manager.
	m_pCameraManager = pEntity->GetComponent<CCameraManagerComponent>();

	// #HACK: #TODO: Is this right? We only want to register action maps on the local client.
	if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
		RegisterActionMaps();
}


void CPlayerInputComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			// #TODO: Switch over to this with 5.4.

			// #HACK: Where did the post update event go?
			// Until then, just call it from here, and hope it's good enough for now.
			PostUpdate(gEnv->pTimer->GetFrameTime());
			break;
	}
}


void CPlayerInputComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// NOTE: By accumulating the deltas in the post update, we get consistent values back during pre-physics
// and update, which is where most the work is done. There is zero chance of errors due to drift in values
// from one update / pre-physics to another.
// The downside is we have a definite lag of one frame for all input - and for VR this might be killer.
// Ideally we would have a component system able to prioritise objects before others - and this would be
// given a high priority.
// #TODO: Re-write when the above is available.

void CPlayerInputComponent::PostUpdate(float frameTime)
{
	// We can just add up all the acculmated requests to find out how much pitch / yaw is being requested.
	// It's also a good time to filter out any small movement requests to stabilise the camera / etc.
	m_lastPitchDelta = m_mousePitchDelta + m_xiPitchDelta;
	if (abs(m_lastPitchDelta) < m_pitchFilter)
		m_lastPitchDelta = 0.0f;
	m_lastYawDelta = m_mouseYawDelta + m_xiYawDelta;
	if (abs(m_lastYawDelta) < m_yawFilter)
		m_lastYawDelta = 0.0f;

	// Track the last values for mouse and xbox inputs. They're filtered individually for low level noise.
	m_lastMousePitchDelta = abs(m_mousePitchDelta) >= m_pitchFilter ? m_mousePitchDelta : 0.0f;
	m_lastMouseYawDelta = abs(m_mouseYawDelta) >= m_yawFilter ? m_mouseYawDelta : 0.0f;
	m_lastXiPitchDelta = abs(m_xiPitchDelta) >= m_pitchFilter ? m_xiPitchDelta : 0.0f;
	m_lastXiYawDelta = abs(m_xiYawDelta) >= m_yawFilter ? m_xiYawDelta : 0.0f;

	// Circle of life!
	m_mousePitchDelta = m_mouseYawDelta = 0.0f;

	// Handle zoom level changes, result is stored for query by cameras on Update.
	m_lastZoomDelta = m_zoomDelta;
	m_zoomDelta = 0.0f;
}


// ***
// ***  IPlayerInputComponent
// ***


void CPlayerInputComponent::ResetMovementState()
{
	m_movementStateFlags = EMovementStateFlags::None;
}


void CPlayerInputComponent::ResetActionState()
{
}


Vec3 CPlayerInputComponent::GetMovement(const Quat& baseRotation)
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

		case (EMovementStateFlags::Forward | EMovementStateFlags::Right):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(45.0f));
			break;

		case EMovementStateFlags::Right:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(90.0f));
			break;

		case (EMovementStateFlags::Backward | EMovementStateFlags::Right):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(135.0f));
			break;

		case EMovementStateFlags::Backward:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(180.0f));
			break;

		case (EMovementStateFlags::Backward | EMovementStateFlags::Left):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(225.0f));
			break;

		case EMovementStateFlags::Left:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(270.0f));
			break;

		case (EMovementStateFlags::Forward | EMovementStateFlags::Left):
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


// ***
// *** IActionListener
// ***

void CPlayerInputComponent::OnAction(const ActionId& action, int activationMode, float value)
{
	// We want to dispatch the action as if it were for the character that this player is currently attached onto.
	if (m_allowActions)
		m_actionHandler.Dispatch(this, CPlayer::GetLocalPlayer()->GetAttachedEntityId(), action, activationMode, value);
}


// ***
// *** CPlayerInputComponent
// ***

CPlayerInputComponent::~CPlayerInputComponent()
{
	// Clean up our action map usage.
	m_allowActions = false;
	IActionMapManager* pActionMapManager = gEnv->pGameFramework->GetIActionMapManager();
	pActionMapManager->EnableActionMap("player", false);
	pActionMapManager->RemoveExtraActionListener(this, "player");
	pActionMapManager->EnableActionMap("camera", false);
	pActionMapManager->RemoveExtraActionListener(this, "camera");
	pActionMapManager->EnableActionMap("inspection", false);
	pActionMapManager->RemoveExtraActionListener(this, "inspection");
	GetGameObject()->ReleaseActions(this);
}


void CPlayerInputComponent::RegisterActionMaps()
{
	// Populate the action handler callbacks so that we get action map events.
	m_allowActions = true;
	InitializeActionHandler();

	// Load and init the default action map profile.
	IActionMapManager* pActionMapManager = gEnv->pGameFramework->GetIActionMapManager();
	pActionMapManager->SetDefaultActionEntity(GetEntityId());
	pActionMapManager->InitActionMaps("libs/config/defaultprofile.xml");
	pActionMapManager->Enable(true);

	// Try and enable the "player" actions, which control our player.
	//pActionMapManager->AddExtraActionListener(this, "player"); // NOTE: Only seem to need one for all the action maps to work.
	pActionMapManager->EnableActionMap("player", true);
	pActionMapManager->EnableActionMap("camera", true);
	pActionMapManager->EnableActionMap("inspection", true);

	// Only one extension should capture the actions, so we are handling all action maps here.
	GetGameObject()->CaptureActions(this);
}


void CPlayerInputComponent::InitializeActionHandler()
{
	// Basic movement.
	m_actionHandler.AddHandler(ActionId("move_left"), &CPlayerInputComponent::OnActionMoveLeft);
	m_actionHandler.AddHandler(ActionId("move_right"), &CPlayerInputComponent::OnActionMoveRight);
	m_actionHandler.AddHandler(ActionId("move_forward"), &CPlayerInputComponent::OnActionMoveForward);
	m_actionHandler.AddHandler(ActionId("move_backward"), &CPlayerInputComponent::OnActionMoveBackward);

	// Interaction.
	m_actionHandler.AddHandler(ActionId("player_interaction"), &CPlayerInputComponent::OnActionInteraction);

	// Mouse yaw and pitch handlers.
	m_actionHandler.AddHandler(ActionId("mouse_rotateyaw"), &CPlayerInputComponent::OnActionRotateYaw);
	m_actionHandler.AddHandler(ActionId("mouse_rotatepitch"), &CPlayerInputComponent::OnActionRotatePitch);

	// Zoom handlers.
	m_actionHandler.AddHandler(ActionId("tpv_zoom_in"), &CPlayerInputComponent::OnActionZoomIn);
	m_actionHandler.AddHandler(ActionId("tpv_zoom_out"), &CPlayerInputComponent::OnActionZoomOut);

	// XBox controller yaw and pitch handlers.
	m_actionHandler.AddHandler(ActionId("xi_rotateyaw"), &CPlayerInputComponent::OnActionXIRotateYaw);
	m_actionHandler.AddHandler(ActionId("xi_rotatepitch"), &CPlayerInputComponent::OnActionXIRotatePitch);

	// Jump.
	m_actionHandler.AddHandler(ActionId("move_jump"), &CPlayerInputComponent::OnActionJump);

	// Walk, jog, sprint.
	m_actionHandler.AddHandler(ActionId("move_walkjog"), &CPlayerInputComponent::OnActionWalkJog);
	m_actionHandler.AddHandler(ActionId("move_sprint"), &CPlayerInputComponent::OnActionSprint);

	// Stances under player control.
	m_actionHandler.AddHandler(ActionId("stance_crouch"), &CPlayerInputComponent::OnActionCrouch);
	m_actionHandler.AddHandler(ActionId("stance_kneel"), &CPlayerInputComponent::OnActionKneel);
	m_actionHandler.AddHandler(ActionId("stance_sit"), &CPlayerInputComponent::OnActionSit);

	// Interact with an object.
	m_actionHandler.AddHandler(ActionId("item_use"), &CPlayerInputComponent::OnActionItemUse);
	m_actionHandler.AddHandler(ActionId("item_pickup"), &CPlayerInputComponent::OnActionItemPickup);
	m_actionHandler.AddHandler(ActionId("item_drop"), &CPlayerInputComponent::OnActionItemDrop);
	m_actionHandler.AddHandler(ActionId("item_toss"), &CPlayerInputComponent::OnActionItemToss);

	// Action bars.
	m_actionHandler.AddHandler(ActionId("actionbar_01"), &CPlayerInputComponent::OnActionBar01);
	m_actionHandler.AddHandler(ActionId("actionbar_02"), &CPlayerInputComponent::OnActionBar02);
	m_actionHandler.AddHandler(ActionId("actionbar_03"), &CPlayerInputComponent::OnActionBar03);
	m_actionHandler.AddHandler(ActionId("actionbar_04"), &CPlayerInputComponent::OnActionBar04);
	m_actionHandler.AddHandler(ActionId("actionbar_05"), &CPlayerInputComponent::OnActionBar05);
	m_actionHandler.AddHandler(ActionId("actionbar_06"), &CPlayerInputComponent::OnActionBar06);
	m_actionHandler.AddHandler(ActionId("actionbar_07"), &CPlayerInputComponent::OnActionBar07);
	m_actionHandler.AddHandler(ActionId("actionbar_08"), &CPlayerInputComponent::OnActionBar08);
	m_actionHandler.AddHandler(ActionId("actionbar_09"), &CPlayerInputComponent::OnActionBar09);
	m_actionHandler.AddHandler(ActionId("actionbar_10"), &CPlayerInputComponent::OnActionBar10);
	m_actionHandler.AddHandler(ActionId("actionbar_11"), &CPlayerInputComponent::OnActionBar11);
	m_actionHandler.AddHandler(ActionId("actionbar_12"), &CPlayerInputComponent::OnActionBar12);

	// Camera movements.
	m_actionHandler.AddHandler(ActionId("camera_shift_up"), &CPlayerInputComponent::OnActionCameraShiftUp);
	m_actionHandler.AddHandler(ActionId("camera_shift_down"), &CPlayerInputComponent::OnActionCameraShiftDown);
	m_actionHandler.AddHandler(ActionId("camera_shift_left"), &CPlayerInputComponent::OnActionCameraShiftLeft);
	m_actionHandler.AddHandler(ActionId("camera_shift_right"), &CPlayerInputComponent::OnActionCameraShiftRight);
	m_actionHandler.AddHandler(ActionId("camera_shift_forward"), &CPlayerInputComponent::OnActionCameraShiftForward);
	m_actionHandler.AddHandler(ActionId("camera_shift_backward"), &CPlayerInputComponent::OnActionCameraShiftBackward);

	// Inspection.
	m_actionHandler.AddHandler(ActionId("inspect_start"), &CPlayerInputComponent::OnActionInspectStart);
	m_actionHandler.AddHandler(ActionId("inspect"), &CPlayerInputComponent::OnActionInspect);
	m_actionHandler.AddHandler(ActionId("inspect_end"), &CPlayerInputComponent::OnActionInspectEnd);
}


// ***
// *** Action handlers.
// ***

bool CPlayerInputComponent::OnActionMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		m_movementStateFlags &= ~EMovementStateFlags::Left;
	}
	else if (activationMode & (eAAM_OnPress | eAAM_OnHold))
	{
		m_movementStateFlags |= EMovementStateFlags::Left;
	}

	return false;
}


bool CPlayerInputComponent::OnActionMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		m_movementStateFlags &= ~EMovementStateFlags::Right;
	}
	else if (activationMode & (eAAM_OnPress | eAAM_OnHold))
	{
		m_movementStateFlags |= EMovementStateFlags::Right;
	}

	return false;
}


bool CPlayerInputComponent::OnActionMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		m_movementStateFlags &= ~EMovementStateFlags::Forward;
	}
	else if (activationMode & (eAAM_OnPress | eAAM_OnHold))
	{
		m_movementStateFlags |= EMovementStateFlags::Forward;
	}

	return false;
}


bool CPlayerInputComponent::OnActionMoveBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		m_movementStateFlags &= ~EMovementStateFlags::Backward;
	}
	else if (activationMode & (eAAM_OnPress | eAAM_OnHold))
	{
		m_movementStateFlags |= EMovementStateFlags::Backward;
	}

	return false;
}


bool CPlayerInputComponent::OnActionRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	float cl_mouseSensitivity = CChrysalisCorePlugin::Get() ? CChrysalisCorePlugin::Get()->GetChrysalisCore()->GetCVars().m_cl_mouseSensitivity : 1.0f;

	// Sensible scaling value for rotations in radians.
	float mouseSensitivity = 0.00032f * max(0.01f, cl_mouseSensitivity * m_mousePitchYawSensitivity);

	// Add the yaw delta.
	m_mouseYawDelta -= value * mouseSensitivity;

	return false;
}


bool CPlayerInputComponent::OnActionRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	float cl_mouseSensitivity = CChrysalisCorePlugin::Get() ? CChrysalisCorePlugin::Get()->GetChrysalisCore()->GetCVars().m_cl_mouseSensitivity : 1.0f;

	// Have they inverted their mouse control?
	if ((CChrysalisCorePlugin::Get()) && (CChrysalisCorePlugin::Get()->GetChrysalisCore()->GetCVars().m_cl_invertPitch))
		value *= -1.0f;

	// Sensible scaling value for rotations in radians.
	float mouseSensitivity = 0.00032f * max(0.01f, cl_mouseSensitivity * m_mousePitchYawSensitivity);

	// Add the delta, taking into account mouse inversion.  Clamp the result.
	float invertYAxis = m_mouseInvertPitch ? -1.0f : 1.0f;
	m_mousePitchDelta += value * mouseSensitivity * invertYAxis;

	return false;
}


// XBox controller rotation is handled differently. Movements on the thumb stick set a value for
// rotation that should be applied every frame update.
bool CPlayerInputComponent::OnActionXIRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
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
bool CPlayerInputComponent::OnActionXIRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	float radians = DEG2RAD(value);

	if (abs(radians) < m_xiPitchFilter)
		m_xiPitchDelta = 0.0f;
	else
		m_xiPitchDelta = radians;

	return false;
}


bool CPlayerInputComponent::OnActionZoomIn(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_zoomDelta -= 1.0f;

	return false;
}


bool CPlayerInputComponent::OnActionZoomOut(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_zoomDelta += 1.0f;

	return false;
}


bool CPlayerInputComponent::OnActionJump(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		CryLogAlways("Player jumped");
	}

	return false;
}


bool CPlayerInputComponent::OnActionCrouch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		CryLogAlways("Player crouched");
	}

	return false;
}


bool CPlayerInputComponent::OnActionKneel(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		CryLogAlways("Player knelt");
	}

	return false;
}


bool CPlayerInputComponent::OnActionSit(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		CryLogAlways("Player sat");
	}

	return false;
}


bool CPlayerInputComponent::OnActionWalkJog(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionJogToggle(entityId);
		}
	}

	return false;
}


bool CPlayerInputComponent::OnActionSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionSprintStop(entityId);
		}
	}
	else if (activationMode & (eAAM_OnPress | eAAM_OnHold))
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionSprintStart(entityId);
		}
	}

	return false;
}


bool CPlayerInputComponent::OnActionItemUse(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	// #TODO: BUG this gets called twice for every press of the "f" key. Not sure if others are being called twice as well.
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionItemUse(entityId);
		}
	}

	return false;
}


bool CPlayerInputComponent::OnActionItemPickup(EntityId entityId, const ActionId& actionId, int activationMode, float value)
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


bool CPlayerInputComponent::OnActionItemDrop(EntityId entityId, const ActionId& actionId, int activationMode, float value)
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


bool CPlayerInputComponent::OnActionItemToss(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionItemToss(entityId);
		}
	}

	return false;
}


bool CPlayerInputComponent::OnActionBar(EntityId entityId, const ActionId& actionId, int activationMode, int buttonId)
{
	if (activationMode == eAAM_OnPress || activationMode == eAAM_OnHold)
	{
		CryLogAlways("OnActionBar");
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionBarUse(entityId, buttonId);
		}
	}

	return false;
}


bool CPlayerInputComponent::OnActionBar01(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 1);
}


bool CPlayerInputComponent::OnActionBar02(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 2);
}


bool CPlayerInputComponent::OnActionBar03(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 3);
}


bool CPlayerInputComponent::OnActionBar04(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 4);
}


bool CPlayerInputComponent::OnActionBar05(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 5);
}


bool CPlayerInputComponent::OnActionBar06(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 6);
}


bool CPlayerInputComponent::OnActionBar07(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 7);
}


bool CPlayerInputComponent::OnActionBar08(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 8);
}


bool CPlayerInputComponent::OnActionBar09(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 9);
}


bool CPlayerInputComponent::OnActionBar10(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 10);
}


bool CPlayerInputComponent::OnActionBar11(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 11);
}


bool CPlayerInputComponent::OnActionBar12(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return OnActionBar(entityId, actionId, activationMode, 12);
}


bool CPlayerInputComponent::OnActionCameraShiftUp(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return m_pCameraManager->OnActionCameraShiftUp(entityId, actionId, activationMode, value);
}


bool CPlayerInputComponent::OnActionCameraShiftDown(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return m_pCameraManager->OnActionCameraShiftDown(entityId, actionId, activationMode, value);
}


bool CPlayerInputComponent::OnActionCameraShiftLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return m_pCameraManager->OnActionCameraShiftLeft(entityId, actionId, activationMode, value);
}


bool CPlayerInputComponent::OnActionCameraShiftRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return m_pCameraManager->OnActionCameraShiftRight(entityId, actionId, activationMode, value);
}


bool CPlayerInputComponent::OnActionCameraShiftForward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return m_pCameraManager->OnActionCameraShiftForward(entityId, actionId, activationMode, value);
}


bool CPlayerInputComponent::OnActionCameraShiftBackward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return m_pCameraManager->OnActionCameraShiftBackward(entityId, actionId, activationMode, value);
}


bool CPlayerInputComponent::OnActionInspectStart(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionInspectStart(entityId);
		}
	}

	return false;
}


bool CPlayerInputComponent::OnActionInspect(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionInspect(entityId);
		}
	}

	return false;
}


bool CPlayerInputComponent::OnActionInspectEnd(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			pCharacter->OnActionInspectEnd(entityId);
		}
	}

	return false;
}


bool CPlayerInputComponent::OnActionInteraction(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	switch (activationMode)
	{
		case eAAM_OnPress:
		{
			if (auto pCharacter = CPlayer::GetLocalCharacter())
			{
				pCharacter->OnActionInteractionStart(entityId);
			}
		}
			break;

		case eAAM_OnHold:
		{
			if (auto pCharacter = CPlayer::GetLocalCharacter())
			{
				pCharacter->OnActionInteraction(entityId);
			}
		}
			break;

		case eAAM_OnRelease:
		{
			if (auto pCharacter = CPlayer::GetLocalCharacter())
			{
				pCharacter->OnActionInteractionEnd(entityId);
			}
		}
			break;
	}

	return false;
}


