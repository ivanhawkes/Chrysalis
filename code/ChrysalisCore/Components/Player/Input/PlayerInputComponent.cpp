#include <StdAfx.h>

#include "PlayerInputComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include <CryMath/Cry_Math.h>
#include <Components/Actor/ActorComponent.h>
#include "Components/Player/PlayerComponent.h"
#include "Components/Player/Camera/CameraManagerComponent.h"
#include "Components/Player/Camera/ICameraComponent.h"
#include <Console/CVars.h>


namespace Chrysalis
{
#define IF_ACTOR_DO(run_action) if (auto pActorComponent = CPlayerComponent::GetLocalActor()) { pActorComponent->run_action(); }


static void RegisterPlayerInputComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerInputComponent));
		// Functions
		{
		}
	}
}


void CPlayerInputComponent::ReflectType(Schematyc::CTypeDesc<CPlayerInputComponent>& desc)
{
	desc.SetGUID(CPlayerInputComponent::IID());
	desc.SetEditorCategory("Hidden");
	desc.SetLabel("Player Input");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});
}


void CPlayerInputComponent::Initialize()
{
	// Query for the player that owns this extension.
	m_pPlayer = m_pEntity->GetComponent<CPlayerComponent>();

	// Grab the camera manager.
	m_pCameraManager = m_pEntity->GetComponent<CCameraManagerComponent>();

	// #HACK: #TODO: Is this right? We only want to register action maps on the local client.
	if (GetEntityId() == gEnv->pGameFramework->GetClientActorId())
		RegisterActionMaps();
}


void CPlayerInputComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::Update:
			Update();
			break;
	}
}


void CPlayerInputComponent::RegisterActionMaps()
{
	CryLogAlways("Registering the action maps.");

	// Get the input component, wraps access to action mapping so we can easily get callbacks when inputs are triggered.
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

	// Escape.
	m_pInputComponent->RegisterAction("player", "special_esc", [this](int activationMode, float value) { OnActionEscape(activationMode, value); });
	m_pInputComponent->BindAction("player", "special_esc", eAID_KeyboardMouse, EKeyId::eKI_Escape);

	// Interaction.
	m_pInputComponent->RegisterAction("player", "player_interaction", [this](int activationMode, float value) { OnActionInteraction(activationMode, value); });
	m_pInputComponent->BindAction("player", "player_interaction", eAID_KeyboardMouse, EKeyId::eKI_Mouse1);

	// Move left.
	m_pInputComponent->RegisterAction("player", "move_left", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Left, activationMode); });
	m_pInputComponent->BindAction("player", "move_left", eAID_KeyboardMouse, EKeyId::eKI_A);
	m_pInputComponent->BindAction("player", "move_left", eAID_XboxPad, EKeyId::eKI_XI_DPadLeft);

	// Move right.
	m_pInputComponent->RegisterAction("player", "move_right", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Right, activationMode); });
	m_pInputComponent->BindAction("player", "move_right", eAID_KeyboardMouse, EKeyId::eKI_D);
	m_pInputComponent->BindAction("player", "move_right", eAID_XboxPad, EKeyId::eKI_XI_DPadRight);

	// Move forward.
	m_pInputComponent->RegisterAction("player", "move_forward", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Forward, activationMode); });
	m_pInputComponent->BindAction("player", "move_forward", eAID_KeyboardMouse, EKeyId::eKI_W);
	m_pInputComponent->BindAction("player", "move_forward", eAID_XboxPad, EKeyId::eKI_XI_DPadUp);

	// Move backward.
	m_pInputComponent->RegisterAction("player", "move_backward", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Backward, activationMode); });
	m_pInputComponent->BindAction("player", "move_backward", eAID_KeyboardMouse, EKeyId::eKI_S);
	m_pInputComponent->BindAction("player", "move_backward", eAID_XboxPad, EKeyId::eKI_XI_DPadDown);

	// Examine.
	m_pInputComponent->RegisterAction("player", "special_examine", [this](int activationMode, float value) { OnActionExamine(activationMode, value); });
	m_pInputComponent->BindAction("player", "special_examine", eAID_KeyboardMouse, EKeyId::eKI_Z);

	// Mouse yaw and pitch handlers.
	m_pInputComponent->RegisterAction("player", "mouse_rotateyaw", [this](int activationMode, float value) { OnActionRotateYaw(activationMode, value); });
	m_pInputComponent->BindAction("player", "mouse_rotateyaw", eAID_KeyboardMouse, EKeyId::eKI_MouseX);
	m_pInputComponent->RegisterAction("player", "mouse_rotatepitch", [this](int activationMode, float value) { OnActionRotatePitch(activationMode, value); });
	m_pInputComponent->BindAction("player", "mouse_rotatepitch", eAID_KeyboardMouse, EKeyId::eKI_MouseY);

	// XBox controller yaw and pitch handlers.
	m_pInputComponent->RegisterAction("player", "xi_rotateyaw", [this](int activationMode, float value) { OnActionXIRotateYaw(activationMode, value); });
	m_pInputComponent->BindAction("player", "xi_rotateyaw", eAID_XboxPad, EKeyId::eKI_XI_ThumbRX);
	m_pInputComponent->RegisterAction("player", "xi_rotatepitch", [this](int activationMode, float value) { OnActionXIRotatePitch(activationMode, value); });
	m_pInputComponent->BindAction("player", "xi_rotatepitch", eAID_XboxPad, EKeyId::eKI_XI_ThumbRY);

	// Jump.
	m_pInputComponent->RegisterAction("player", "move_jump", [this](int activationMode, float value) { OnActionJump(activationMode, value); });
	m_pInputComponent->BindAction("player", "move_jump", eAID_KeyboardMouse, EKeyId::eKI_Space);

	// Walk, jog, sprint.
	m_pInputComponent->RegisterAction("player", "move_walkjog", [this](int activationMode, float value) { OnActionWalkJog(activationMode, value); });
	m_pInputComponent->BindAction("player", "move_walkjog", eAID_KeyboardMouse, EKeyId::eKI_N);
	m_pInputComponent->RegisterAction("player", "move_sprint", [this](int activationMode, float value) { OnActionSprintToggle(activationMode, value); });
	m_pInputComponent->BindAction("player", "move_sprint", eAID_KeyboardMouse, EKeyId::eKI_LShift);

	// Stances under player control.
	m_pInputComponent->RegisterAction("player", "stance_crouch", [this](int activationMode, float value) { OnActionCrouchToggle(activationMode, value); });
	m_pInputComponent->BindAction("player", "stance_crouch", eAID_KeyboardMouse, EKeyId::eKI_C);
	m_pInputComponent->RegisterAction("player", "stance_crawl", [this](int activationMode, float value) { OnActionCrawlToggle(activationMode, value); });
	m_pInputComponent->BindAction("player", "stance_crawl", eAID_KeyboardMouse, EKeyId::eKI_H);
	m_pInputComponent->RegisterAction("player", "stance_kneel", [this](int activationMode, float value) { OnActionKneelToggle(activationMode, value); });
	m_pInputComponent->BindAction("player", "stance_kneel", eAID_KeyboardMouse, EKeyId::eKI_V);
	m_pInputComponent->RegisterAction("player", "stance_sit", [this](int activationMode, float value) { OnActionSitToggle(activationMode, value); });
	m_pInputComponent->BindAction("player", "stance_sit", eAID_KeyboardMouse, EKeyId::eKI_B);

	// Interact with an object.
	m_pInputComponent->RegisterAction("player", "item_use", [this](int activationMode, float value) { OnActionItemUse(activationMode, value); });
	m_pInputComponent->BindAction("player", "item_use", eAID_KeyboardMouse, EKeyId::eKI_F);
	m_pInputComponent->BindAction("player", "item_use", eAID_XboxPad, EKeyId::eKI_XI_A);
	//m_pInputComponent->RegisterAction("player", "item_pickup", [this](int activationMode, float value) { OnActionItemPickup(activationMode, value); });
	//m_pInputComponent->BindAction("player", "item_pickup", eAID_KeyboardMouse, EKeyId::eKI_G);
	//m_pInputComponent->BindAction("player", "item_pickup", eAID_XboxPad, EKeyId::eKI_XI_Y);
	m_pInputComponent->RegisterAction("player", "item_drop", [this](int activationMode, float value) { OnActionItemDrop(activationMode, value); });
	m_pInputComponent->BindAction("player", "item_drop", eAID_KeyboardMouse, EKeyId::eKI_M);
	m_pInputComponent->BindAction("player", "item_drop", eAID_XboxPad, EKeyId::eKI_XI_B);
	m_pInputComponent->RegisterAction("player", "item_toss", [this](int activationMode, float value) { OnActionItemToss(activationMode, value); });
	m_pInputComponent->BindAction("player", "item_toss", eAID_KeyboardMouse, EKeyId::eKI_X);
	m_pInputComponent->BindAction("player", "item_toss", eAID_XboxPad, EKeyId::eKI_XI_X);

	// Action bars.
	m_pInputComponent->RegisterAction("player", "actionbar_01", [this](int activationMode, float value) { OnActionBarUse(activationMode, 1); });
	m_pInputComponent->BindAction("player", "actionbar_01", eAID_KeyboardMouse, EKeyId::eKI_1);
	m_pInputComponent->RegisterAction("player", "actionbar_02", [this](int activationMode, float value) { OnActionBarUse(activationMode, 2); });
	m_pInputComponent->BindAction("player", "actionbar_02", eAID_KeyboardMouse, EKeyId::eKI_2);
	m_pInputComponent->RegisterAction("player", "actionbar_03", [this](int activationMode, float value) { OnActionBarUse(activationMode, 3); });
	m_pInputComponent->BindAction("player", "actionbar_03", eAID_KeyboardMouse, EKeyId::eKI_3);
	m_pInputComponent->RegisterAction("player", "actionbar_04", [this](int activationMode, float value) { OnActionBarUse(activationMode, 4); });
	m_pInputComponent->BindAction("player", "actionbar_04", eAID_KeyboardMouse, EKeyId::eKI_4);
	m_pInputComponent->RegisterAction("player", "actionbar_05", [this](int activationMode, float value) { OnActionBarUse(activationMode, 5); });
	m_pInputComponent->BindAction("player", "actionbar_05", eAID_KeyboardMouse, EKeyId::eKI_5);
	m_pInputComponent->RegisterAction("player", "actionbar_06", [this](int activationMode, float value) { OnActionBarUse(activationMode, 6); });
	m_pInputComponent->BindAction("player", "actionbar_06", eAID_KeyboardMouse, EKeyId::eKI_6);
	m_pInputComponent->RegisterAction("player", "actionbar_07", [this](int activationMode, float value) { OnActionBarUse(activationMode, 7); });
	m_pInputComponent->BindAction("player", "actionbar_07", eAID_KeyboardMouse, EKeyId::eKI_7);
	m_pInputComponent->RegisterAction("player", "actionbar_08", [this](int activationMode, float value) { OnActionBarUse(activationMode, 8); });
	m_pInputComponent->BindAction("player", "actionbar_08", eAID_KeyboardMouse, EKeyId::eKI_8);
	m_pInputComponent->RegisterAction("player", "actionbar_09", [this](int activationMode, float value) { OnActionBarUse(activationMode, 9); });
	m_pInputComponent->BindAction("player", "actionbar_09", eAID_KeyboardMouse, EKeyId::eKI_9);
	m_pInputComponent->RegisterAction("player", "actionbar_10", [this](int activationMode, float value) { OnActionBarUse(activationMode, 10); });
	m_pInputComponent->BindAction("player", "actionbar_10", eAID_KeyboardMouse, EKeyId::eKI_0);
	m_pInputComponent->RegisterAction("player", "actionbar_11", [this](int activationMode, float value) { OnActionBarUse(activationMode, 11); });
	m_pInputComponent->BindAction("player", "actionbar_11", eAID_KeyboardMouse, EKeyId::eKI_Minus);
	m_pInputComponent->RegisterAction("player", "actionbar_12", [this](int activationMode, float value) { OnActionBarUse(activationMode, 12); });
	m_pInputComponent->BindAction("player", "actionbar_12", eAID_KeyboardMouse, EKeyId::eKI_Equals);

	// Function keys.
	m_pInputComponent->RegisterAction("player", "functionkey_01", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 1); });
	m_pInputComponent->BindAction("player", "functionkey_01", eAID_KeyboardMouse, EKeyId::eKI_F1);
	m_pInputComponent->RegisterAction("player", "functionkey_02", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 2); });
	m_pInputComponent->BindAction("player", "functionkey_02", eAID_KeyboardMouse, EKeyId::eKI_F2);
	m_pInputComponent->RegisterAction("player", "functionkey_03", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 3); });
	m_pInputComponent->BindAction("player", "functionkey_03", eAID_KeyboardMouse, EKeyId::eKI_F3);
	m_pInputComponent->RegisterAction("player", "functionkey_04", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 4); });
	m_pInputComponent->BindAction("player", "functionkey_04", eAID_KeyboardMouse, EKeyId::eKI_F4);
	m_pInputComponent->RegisterAction("player", "functionkey_05", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 5); });
	m_pInputComponent->BindAction("player", "functionkey_05", eAID_KeyboardMouse, EKeyId::eKI_F5);
	m_pInputComponent->RegisterAction("player", "functionkey_06", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 6); });
	m_pInputComponent->BindAction("player", "functionkey_06", eAID_KeyboardMouse, EKeyId::eKI_F6);
	m_pInputComponent->RegisterAction("player", "functionkey_07", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 7); });
	m_pInputComponent->BindAction("player", "functionkey_07", eAID_KeyboardMouse, EKeyId::eKI_F7);
	m_pInputComponent->RegisterAction("player", "functionkey_08", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 8); });
	m_pInputComponent->BindAction("player", "functionkey_08", eAID_KeyboardMouse, EKeyId::eKI_F8);
	m_pInputComponent->RegisterAction("player", "functionkey_09", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 9); });
	m_pInputComponent->BindAction("player", "functionkey_09", eAID_KeyboardMouse, EKeyId::eKI_F9);
	m_pInputComponent->RegisterAction("player", "functionkey_10", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 10); });
	m_pInputComponent->BindAction("player", "functionkey_10", eAID_KeyboardMouse, EKeyId::eKI_F10);
	m_pInputComponent->RegisterAction("player", "functionkey_11", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 11); });
	m_pInputComponent->BindAction("player", "functionkey_11", eAID_KeyboardMouse, EKeyId::eKI_F11);
	m_pInputComponent->RegisterAction("player", "functionkey_12", [this](int activationMode, float value) { OnFunctionBarUse(activationMode, 12); });
	m_pInputComponent->BindAction("player", "functionkey_12", eAID_KeyboardMouse, EKeyId::eKI_F12);

	// Numpad.
	m_pInputComponent->RegisterAction("player", "np_0", [this](int activationMode, float value) { OnNumpad(activationMode, 0); });
	m_pInputComponent->BindAction("player", "np_0", eAID_KeyboardMouse, EKeyId::eKI_NP_0);
	m_pInputComponent->RegisterAction("player", "np_1", [this](int activationMode, float value) { OnNumpad(activationMode, 1); });
	m_pInputComponent->BindAction("player", "np_1", eAID_KeyboardMouse, EKeyId::eKI_NP_1);
	m_pInputComponent->RegisterAction("player", "np_2", [this](int activationMode, float value) { OnNumpad(activationMode, 2); });
	m_pInputComponent->BindAction("player", "np_2", eAID_KeyboardMouse, EKeyId::eKI_NP_2);
	m_pInputComponent->RegisterAction("player", "np_3", [this](int activationMode, float value) { OnNumpad(activationMode, 3); });
	m_pInputComponent->BindAction("player", "np_3", eAID_KeyboardMouse, EKeyId::eKI_NP_3);
	m_pInputComponent->RegisterAction("player", "np_4", [this](int activationMode, float value) { OnNumpad(activationMode, 4); });
	m_pInputComponent->BindAction("player", "np_4", eAID_KeyboardMouse, EKeyId::eKI_NP_4);
	m_pInputComponent->RegisterAction("player", "np_5", [this](int activationMode, float value) { OnNumpad(activationMode, 5);; });
	m_pInputComponent->BindAction("player", "np_5", eAID_KeyboardMouse, EKeyId::eKI_NP_5);
	m_pInputComponent->RegisterAction("player", "np_6", [this](int activationMode, float value) { OnNumpad(activationMode, 6); });
	m_pInputComponent->BindAction("player", "np_6", eAID_KeyboardMouse, EKeyId::eKI_NP_6);
	m_pInputComponent->RegisterAction("player", "np_7", [this](int activationMode, float value) { OnNumpad(activationMode, 7); });
	m_pInputComponent->BindAction("player", "np_7", eAID_KeyboardMouse, EKeyId::eKI_NP_7);
	m_pInputComponent->RegisterAction("player", "np_8", [this](int activationMode, float value) { OnNumpad(activationMode, 8); });
	m_pInputComponent->BindAction("player", "np_8", eAID_KeyboardMouse, EKeyId::eKI_NP_8);
	m_pInputComponent->RegisterAction("player", "np_9", [this](int activationMode, float value) { OnNumpad(activationMode, 9); });
	m_pInputComponent->BindAction("player", "np_9", eAID_KeyboardMouse, EKeyId::eKI_NP_9);

	// Inspection.
	m_pInputComponent->RegisterAction("player", "inspect_start", [this](int activationMode, float value) { OnActionInspectStart(activationMode, value); });
	m_pInputComponent->BindAction("player", "inspect_start", eAID_KeyboardMouse, EKeyId::eKI_J);
	m_pInputComponent->RegisterAction("player", "inspect", [this](int activationMode, float value) { OnActionInspect(activationMode, value); });
	m_pInputComponent->BindAction("player", "inspect", eAID_KeyboardMouse, EKeyId::eKI_K);
	m_pInputComponent->RegisterAction("player", "inspect_end", [this](int activationMode, float value) { OnActionInspectEnd(activationMode, value); });
	m_pInputComponent->BindAction("player", "inspect_end", eAID_KeyboardMouse, EKeyId::eKI_L);
}


/**
NOTE: By accumulating the deltas in the post update, we get consistent values back during pre-physics and update,
which is where most the work is done. There is zero chance of errors due to drift in values from one update / pre-
physics to another. The downside is we have a definite lag of one frame for all input - and for VR this might be
killer. Ideally we would have a component system able to prioritise objects before others - and this would be given a
high priority.

#TODO: Re-write when the above is available.
**/
void CPlayerInputComponent::Update()
{
	// We can just add up all the acculmated requests to find out how much pitch / yaw is being requested.
	// It's also a good time to filter out any small movement requests to stabilise the camera / etc.
	m_lastPitchDelta = m_mousePitchDelta + m_xiPitchDelta;
	if (std::abs(m_lastPitchDelta) < m_pitchFilter)
		m_lastPitchDelta = 0.0f;
	m_lastYawDelta = m_mouseYawDelta + m_xiYawDelta;
	if (std::abs(m_lastYawDelta) < m_yawFilter)
		m_lastYawDelta = 0.0f;

	// Track the last values for mouse and xbox inputs. They're filtered individually for low level noise.
	m_lastMousePitchDelta = std::abs(m_mousePitchDelta) >= m_pitchFilter ? m_mousePitchDelta : 0.0f;
	m_lastMouseYawDelta = std::abs(m_mouseYawDelta) >= m_yawFilter ? m_mouseYawDelta : 0.0f;
	m_lastXiPitchDelta = std::abs(m_xiPitchDelta) >= m_pitchFilter ? m_xiPitchDelta : 0.0f;
	m_lastXiYawDelta = std::abs(m_xiYawDelta) >= m_yawFilter ? m_xiYawDelta : 0.0f;

	// Circle of life!
	m_mousePitchDelta = m_mouseYawDelta = 0.0f;
}


Vec3 CPlayerInputComponent::GetMovement(const Quat& baseRotation)
{
	bool allowMovement = true;
	Quat quatRelativeDirection;
	Vec3 vecMovement = Vec3(ZERO);

	// Take the mask and turn it into a vector to indicate the direction we need to pan independent of the present
	// camera direction. 
	switch (m_inputFlags)
	{
		case (TInputFlags)EInputFlag::Forward:
			quatRelativeDirection = Quat::CreateIdentity();
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Right):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(45.0f));
			break;

		case (TInputFlags)EInputFlag::Right:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(90.0f));
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Right):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(135.0f));
			break;

		case (TInputFlags)EInputFlag::Backward:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(180.0f));
			break;

		case ((TInputFlags)EInputFlag::Backward | (TInputFlags)EInputFlag::Left):
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(225.0f));
			break;

		case (TInputFlags)EInputFlag::Left:
			quatRelativeDirection = Quat::CreateRotationZ(DEG2RAD(270.0f));
			break;

		case ((TInputFlags)EInputFlag::Forward | (TInputFlags)EInputFlag::Left):
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


void CPlayerInputComponent::HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type)
{
	switch (type)
	{
		case EInputFlagType::Hold:
		{
			if (activationMode & eIS_Pressed)
			{
				m_inputFlags |= flags;
			}
			else if (activationMode & eIS_Released)
			{
				m_inputFlags &= ~flags;
			}
		}
		break;

		case EInputFlagType::Toggle:
		{
			if (activationMode & eIS_Released)
			{
				// Toggle the bit(s)
				m_inputFlags ^= flags;
			}
		}
		break;
	}
}


void CPlayerInputComponent::OnActionRotateYaw(int activationMode, float value)
{
	float cl_mouseSensitivity = CChrysalisCorePlugin::Get() ? g_cvars.m_cl_mouseSensitivity : 1.0f;

	// Sensible scaling value for rotations in radians.
	float mouseSensitivity = 0.00032f * max(0.01f, cl_mouseSensitivity * m_mousePitchYawSensitivity);

	// Add the yaw delta.
	m_mouseYawDelta -= value * mouseSensitivity;
}


void CPlayerInputComponent::OnActionRotatePitch(int activationMode, float value)
{
	float cl_mouseSensitivity = CChrysalisCorePlugin::Get() ? g_cvars.m_cl_mouseSensitivity : 1.0f;

	// Have they inverted their mouse control?
	if ((CChrysalisCorePlugin::Get()) && (g_cvars.m_cl_invertPitch))
		value *= -1.0f;

	// Sensible scaling value for rotations in radians.
	float mouseSensitivity = 0.00032f * max(0.01f, cl_mouseSensitivity * m_mousePitchYawSensitivity);

	// Add the delta, taking into account mouse inversion.  Clamp the result.
	float invertYAxis = m_mouseInvertPitch ? -1.0f : 1.0f;
	m_mousePitchDelta += value * mouseSensitivity * invertYAxis;
}


// XBox controller rotation is handled differently. Movements on the thumb stick set a value for
// rotation that should be applied every frame update.
void CPlayerInputComponent::OnActionXIRotateYaw(int activationMode, float value)
{
	float radians = DEG2RAD(value);

	if (std::abs(radians) < m_xiYawFilter)
		m_xiYawDelta = 0.0f;
	else
		m_xiYawDelta = radians;
}


// Xbox controller pitch is handled differently. Movements on the thumb stick set a value for
// pitch that should be applied every frame update.
void CPlayerInputComponent::OnActionXIRotatePitch(int activationMode, float value)
{
	float radians = DEG2RAD(value);

	if (std::abs(radians) < m_xiPitchFilter)
		m_xiPitchDelta = 0.0f;
	else
		m_xiPitchDelta = radians;
}


void CPlayerInputComponent::OnActionJump(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		CryLogAlways("Player jumped");
}


void CPlayerInputComponent::OnActionCrouchToggle(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionCrouchToggle);
}


void CPlayerInputComponent::OnActionCrawlToggle(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionCrawlToggle);
}


void CPlayerInputComponent::OnActionKneelToggle(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionKneelToggle);
}


void CPlayerInputComponent::OnActionSitToggle(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionSitToggle);
}


void CPlayerInputComponent::OnActionWalkJog(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionJogToggle);
}


void CPlayerInputComponent::OnActionSprintToggle(int activationMode, float value)
{
	if (activationMode == eAAM_OnRelease)
	{
		IF_ACTOR_DO(OnActionSprintStop);
	}
	else if (activationMode & (eAAM_OnPress | eAAM_OnHold))
	{
		IF_ACTOR_DO(OnActionSprintStart);
	}
}


void CPlayerInputComponent::OnActionItemUse(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionItemUse);
}


void CPlayerInputComponent::OnActionItemPickup(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionItemPickup);
}


void CPlayerInputComponent::OnActionItemDrop(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionItemDrop);
}


void CPlayerInputComponent::OnActionItemToss(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionItemToss);
}


void CPlayerInputComponent::OnActionEscape(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		// Notify listeners.
		for (auto it : m_listeners)
			it->OnInputEscape(activationMode);
	}
}


void CPlayerInputComponent::OnActionInteraction(int activationMode, float value)
{
	// Notify listeners.
	for (auto it : m_listeners)
		it->OnInputInteraction(activationMode);
}


void CPlayerInputComponent::OnActionBarUse(int activationMode, int buttonId)
{
	// Notify listeners.
	for (auto it : m_listeners)
		it->OnInputActionBarUse(activationMode, buttonId);
}


void CPlayerInputComponent::OnFunctionBarUse(int activationMode, int buttonId)
{
	// Notify listeners.
	for (auto it : m_listeners)
		it->OnInputFunctionBarUse(activationMode, buttonId);
}


void CPlayerInputComponent::OnActionExamine(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		CryLogAlways("Examine");

		//// Notify listeners.
		//for (auto it : m_listenersSpecial.GetListeners())
		//	it->OnInputSpecialExamine();
	}
}


void CPlayerInputComponent::OnNumpad(int activationMode, int buttonId)
{
	if (activationMode == eAAM_OnPress || activationMode == eAAM_OnHold)
	{
		CryLogAlways("OnNumpad");
	}
}


void CPlayerInputComponent::OnActionInspectStart(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionInspectStart);
}


void CPlayerInputComponent::OnActionInspect(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionInspect);
}


void CPlayerInputComponent::OnActionInspectEnd(int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
		IF_ACTOR_DO(OnActionInspectEnd);
}


CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerInputComponent)
}