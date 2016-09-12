#include <StdAfx.h>

#include "PlayerInputComponent.h"
#include <IActionMapManager.h>
#include <CryMath/Cry_Math.h>
#include <Game/Game.h>
#include <Player/Player.h>
#include <Actor/Character/Character.h>
#include <Player/Camera/ICameraComponent.h>
#include <Game/GameFactory.h>


class CPlayerInputRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		CGameFactory::RegisterGameObject<CPlayerInputComponent>("PlayerInput");
	}
};

CPlayerInputRegistrator g_PlayerInputRegistrator;


// ***
// *** ISimpleExtension
// ***


void CPlayerInputComponent::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, CPlayer::EPlayerUpdateSlot::ePlayerUpdateSlot_Main);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);

	// Query for the player that owns this extension.
	m_pPlayer = static_cast<CPlayer *>(pGameObject->QueryExtension("Player"));

	// Detect when we become the local player.
	const int requiredEvents [] = { eGFE_BecomeLocalPlayer };
	pGameObject->UnRegisterExtForEvents(this, NULL, 0);
	pGameObject->RegisterExtForEvents(this, requiredEvents, sizeof(requiredEvents) / sizeof(int));
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
// TODO: Re-write when the above is available.

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

	// Circle of life!
	m_mousePitchDelta = m_xiPitchDelta = m_mouseYawDelta = m_xiYawDelta = 0.0f;

	// Handle zoom level changes, result is stored for query by cameras on Update.
	m_lastZoomDelta = m_zoomDelta;
	m_zoomDelta = 0.0f;
}


void CPlayerInputComponent::HandleEvent(const SGameObjectEvent &event)
{
	// We only register action maps if we are the local player.
	if (event.event == eGFE_BecomeLocalPlayer)
	{
		RegisterActionMaps();
	}
}


// ***
// ***  IPlayerInputComponent
// ***


void CPlayerInputComponent::ResetMovementState()
{
	m_movementStateFlags = EMovementStateFlags::None;
	shouldSprint = false;
	shouldRun = false;
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


Ang3 CPlayerInputComponent::GetRotationDelta()
{
	return Ang3(m_lastPitchDelta, 0.0f, m_lastYawDelta);
}


float CPlayerInputComponent::GetPitchDelta()
{
	return m_lastPitchDelta;
}


float CPlayerInputComponent::GetYawDelta()
{
	return m_lastYawDelta;
}


Vec3 CPlayerInputComponent::GetHeadMovement(const Quat& baseRotation)
{
	Vec3 vecMovement = Vec3(0.0f, 0.0f, 0.0f);
	return vecMovement;
}


Ang3 CPlayerInputComponent::GetHeadRotationDelta()
{
	return Ang3(0.0f, 0.0f, 0.0f);
}


float CPlayerInputComponent::GetHeadPitchDelta()
{
	return 0.0f;
}


float CPlayerInputComponent::GetHeadYawDelta()
{
	return 0.0f;
}


// ***
// *** IActionListener
// ***

void CPlayerInputComponent::OnAction(const ActionId& action, int activationMode, float value)
{
	// We want to dispatch the action as if it were for the character that this player is currently attached onto.
	m_actionHandler.Dispatch(this, CPlayer::GetLocalPlayer()->GetAttachedEntityId(), action, activationMode, value);
}


// ***
// *** CPlayerInputComponent
// ***

CPlayerInputComponent::~CPlayerInputComponent()
{
	// Clean up our action map usage.
	IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	//pActionMapManager->EnableActionMap("player", false);
	pActionMapManager->RemoveExtraActionListener(this, "player");

	// NOTE: Good sense says we should be able to capture and release the actions - but it doesn't seem to like it when
	// two extensions do it on one game object...commented out to make it work, but left in codefor explanation.
	// CCameraManagerComponent is handling it for us.
	// This technique seems to be fragile - expect it to break at some point.
	// TODO: refactor this out.
	//GetGameObject()->ReleaseActions(this);
}

void CPlayerInputComponent::RegisterActionMaps()
{
	// Populate the action handler callbacks so that we get action map events.
	InitializeActionHandler();

	// Try and enable the "player" actions, which control our player.
	IActionMapManager* pActionMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	//pActionMapManager->EnableActionMap("player", true);
	pActionMapManager->AddExtraActionListener(this, "player");

	// NOTE: Good sense says we should be able to capture and release the actions - but it doesn't seem to like it when
	// two extensions do it on one game object...commented out to make it work, but left in code for explanation.
	// CCameraManagerComponent is handling it for us.
	// This technique seems to be fragile - expect it to break at some point.
	// TODO: refactor this out.
	// GetGameObject()->CaptureActions(this); 
}


void CPlayerInputComponent::InitializeActionHandler()
{
	// Basic movement.
	m_actionHandler.AddHandler(ActionId("move_left"), &CPlayerInputComponent::OnActionMoveLeft);
	m_actionHandler.AddHandler(ActionId("move_right"), &CPlayerInputComponent::OnActionMoveRight);
	m_actionHandler.AddHandler(ActionId("move_forward"), &CPlayerInputComponent::OnActionMoveForward);
	m_actionHandler.AddHandler(ActionId("move_backward"), &CPlayerInputComponent::OnActionMoveBackward);

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

	// Walk, run / jog, sprint.
	m_actionHandler.AddHandler(ActionId("move_walkrun"), &CPlayerInputComponent::OnActionWalkRun);
	m_actionHandler.AddHandler(ActionId("move_sprint"), &CPlayerInputComponent::OnActionSprint);

	// Stances under player control.
	m_actionHandler.AddHandler(ActionId("stance_crouch"), &CPlayerInputComponent::OnActionCrouch);
	m_actionHandler.AddHandler(ActionId("stance_kneel"), &CPlayerInputComponent::OnActionKneel);
	m_actionHandler.AddHandler(ActionId("stance_sit"), &CPlayerInputComponent::OnActionSit);

	// Interact with an object.
	m_actionHandler.AddHandler(ActionId("item_use"), &CPlayerInputComponent::OnActionItemUse);
	m_actionHandler.AddHandler(ActionId("item_pickup"), &CPlayerInputComponent::OnActionItemPickup);
	m_actionHandler.AddHandler(ActionId("item_drop"), &CPlayerInputComponent::OnActionItemDrop);
	m_actionHandler.AddHandler(ActionId("item_throw"), &CPlayerInputComponent::OnActionItemThrow);

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
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
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
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
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
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
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
	else if (activationMode && (eAAM_OnPress || eAAM_OnHold))
	{
		m_movementStateFlags |= EMovementStateFlags::Backward;
	}

	return false;
}


bool CPlayerInputComponent::OnActionRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	// Sensible scaling value for rotations in radians.
	float mouseSensitivity = 0.00032f * MAX(0.01f, g_pGame->GetCVars().m_cl_mouseSensitivity * m_mousePitchYawSensitivity);

	// Add the yaw delta.
	m_mouseYawDelta -= value * mouseSensitivity;

	return false;
}


bool CPlayerInputComponent::OnActionRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	// Have they inverted their mouse control?
	if (g_pGame->GetCVars().m_cl_invertPitch)
		value *= -1.0f;

	// Sensible scaling value for rotations in radians.
	float mouseSensitivity = 0.00032f * MAX(0.01f, g_pGame->GetCVars().m_cl_mouseSensitivity * m_mousePitchYawSensitivity);

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


bool CPlayerInputComponent::OnActionWalkRun(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		shouldRun = !shouldRun;
		CryLogAlways("Player toggled walking / running");
	}

	return false;
}


bool CPlayerInputComponent::OnActionSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value)
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


bool CPlayerInputComponent::OnActionItemUse(EntityId entityId, const ActionId& actionId, int activationMode, float value)
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


bool CPlayerInputComponent::OnActionItemThrow(EntityId entityId, const ActionId& actionId, int activationMode, float value)
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


bool CPlayerInputComponent::OnActionBar(EntityId entityId, const ActionId& actionId, int activationMode, int buttonId)
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

