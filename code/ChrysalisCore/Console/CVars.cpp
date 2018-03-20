#include <StdAfx.h>

#include "CVars.h"
#include <CrySystem/ISystem.h>
#include "Components/Player/PlayerComponent.h"
#include <Actor/ActorComponent.h>
#include <Actor/Animation/Actions/ActorAnimationActionEmote.h>
#include <Actor/Character/CharacterComponent.h>
#include <ObjectID/ObjectId.h>
#include <ObjectID/ObjectIdMasterFactory.h>
#include <Plugin/ChrysalisCorePlugin.h>


namespace Chrysalis
{
CCVars g_cvars;

void CCVars::RegisterVariables()
{
	// ***
	// *** VARIABLES
	// ***

	// Game - misc
	REGISTER_CVAR2("game_rayCastQuota", &m_rayCastQuota, 64, VF_CHEAT, "Number of allowed deferred raycasts.");
	REGISTER_CVAR2("cl_invertPitch", &m_cl_invertPitch, false, VF_CHEAT, "Should we invert the Y axis for camera movements? This is preferred by some players, particularly those using a flight yoke.");
	REGISTER_CVAR2("cl_mouseSensitivity", &m_cl_mouseSensitivity, 1.0f, VF_CHEAT, "Overall mouse sensitivity. This should be factored into any movements involving the mouse.");
	REGISTER_CVAR2("watch_enabled", &m_watch_enabled, true, VF_CHEAT, "Is watch debug enabled?");
	REGISTER_CVAR2("watch_text_render_start_pos_x", &m_watch_text_render_start_pos_x, 35.0f, VF_CHEAT, "The watch text render start position x coordinate.");
	REGISTER_CVAR2("watch_text_render_start_pos_y", &m_watch_text_render_start_pos_y, 18.0f, VF_CHEAT, " The watch text render start position y coordinate.");
	REGISTER_CVAR2("watch_text_render_size", &m_watch_text_render_size, 1.75f, VF_CHEAT, "Size at which the watch text will render.");
	REGISTER_CVAR2("watch_text_render_lineSpacing", &m_watch_text_render_lineSpacing, 9.3f, VF_CHEAT, "Line spacing for watch text.");
	REGISTER_CVAR2("watch_text_render_fxscale", &m_watch_text_render_fxscale, 13.0f, VF_CHEAT, "The watch text render fxscale.");

	// TODO: Deprecate this.
	REGISTER_CVAR2("ladder_logVerbosity", &m_ladder_logVerbosity, 0, VF_CHEAT, "Ladder logging.");

	// Camera manager
	m_cameraManagerDebugViewOffset = REGISTER_STRING("camera_manager_debug_view_offset", "0, 0, 0", VF_CHEAT, "A translation vector which is applied after the camera is initially positioned.");
	REGISTER_CVAR2("camera_manager_default_camera", &m_cameraManagerDefaultCamera, 1, VF_CHEAT, "Default camera mode. 0 - FP, 1 - ActionRPG");

	// Action RPG Camera
	REGISTER_CVAR2("camera_actionrpg_Debug", &m_actionRPGCameraDebug, 0, VF_CHEAT, "Allow debug display.");
	REGISTER_CVAR2("camera_actionrpg_PitchMin", &m_actionRPGCameraPitchMin, -85.0f, VF_CHEAT, "Minimum pitch angle of the camera (degrees)");
	REGISTER_CVAR2("camera_actionrpg_PitchMax", &m_actionRPGCameraPitchMax, 85.0f, VF_CHEAT, "Maximum pitch angle of the camera (degrees)");
	REGISTER_CVAR2("camera_actionrpg_TargetDistance", &m_actionRPGCameraTargetDistance, 5.0f, VF_CHEAT, "Preferred distance in metres from the view camera to the target.");
	REGISTER_CVAR2("camera_actionrpg_ReversePitchTilt", &m_actionRPGCameraReversePitchTilt, 0.1f, VF_CHEAT, "A factor that applies pitch in the direction the camera is facing. The effect of this is to tilt it towards or away from the orbit centre.");
	REGISTER_CVAR2("camera_actionrpg_SlerpSpeed", &m_actionRPGCameraSlerpSpeed, 20.0f, VF_CHEAT, "The slerp speed. The speed at which we apply spherical linear interpolation to the camera rotations.");
	REGISTER_CVAR2("camera_actionrpg_ZoomMin", &m_actionRPGCameraZoomMin, 0.2f, VF_CHEAT, "The minimum value for camera zoom.");
	REGISTER_CVAR2("camera_actionrpg_ZoomMax", &m_actionRPGCameraZoomMax, 1.0f, VF_CHEAT, "The maximum value for camera zoom.");
	REGISTER_CVAR2("camera_actionrpg_ZoomStep", &m_actionRPGCameraZoomStep, 0.02f, VF_CHEAT, "Each zoom event in or out will alter the zoom factor, m_zoom, by this amount. Use lower values for more steps and higher values to zoom in / out faster with less steps.");
	REGISTER_CVAR2("camera_actionrpg_ZoomSpeed", &m_actionRPGCameraZoomSpeed, 10.0f, VF_CHEAT, "When the zoom changes we interpolate between it's last value and the goal value. This provides for smoother movement on camera zooms. Higher values will interpolate faster than lower values.");
	m_actionRPGCameraViewPositionOffset = REGISTER_STRING("camera_actionrpg_view_position_offset", "0, 0, 0", VF_CHEAT, "A translation vector which is applied after the camera is initially positioned. This provides for 'over the shoulder' views of the target actor.");
	m_actionRPGCameraAimPositionOffset = REGISTER_STRING("camera_actionrpg_aim_position_offset", "0.45, -0.5, 0.0", VF_CHEAT, "A translation vector which is applied after the camera is initially positioned. This provides for 'over the shoulder' views of the target actor.");

	// First Person Camera
	REGISTER_CVAR2("camera_firstperson_debug", &m_firstPersonCameraDebug, 0, VF_CHEAT, "Allow debug display.");
	REGISTER_CVAR2("camera_firstperson_pitch_min", &m_firstPersonCameraPitchMin, -85.0f, VF_CHEAT, "Minimum pitch angle of the camera (degrees)");
	REGISTER_CVAR2("camera_firstperson_pitch_max", &m_firstPersonCameraPitchMax, 85.0f, VF_CHEAT, "Maximum pitch angle of the camera (degrees)");

	// Examine Camera
	REGISTER_CVAR2("camera_examine_debug", &m_examineCameraDebug, 0, VF_CHEAT, "Allow debug display.");
	REGISTER_CVAR2("camera_examine_yaw_min", &m_examineCameraYawMin, -85.0f, VF_CHEAT, "Minimum yaw angle of the camera (degrees)");
	REGISTER_CVAR2("camera_examine_yaw_max", &m_examineCameraYawMax, 85.0f, VF_CHEAT, "Maximum yaw angle of the camera (degrees)");
	REGISTER_CVAR2("camera_examine_pitch_min", &m_examineCameraPitchMin, -85.0f, VF_CHEAT, "Minimum pitch angle of the camera (degrees)");
	REGISTER_CVAR2("camera_examine_pitch_max", &m_examineCameraPitchMax, 85.0f, VF_CHEAT, "Maximum pitch angle of the camera (degrees)");


	// Components - misc
	REGISTER_CVAR2("component_keyring_debug", &m_componentKeyringDebug, 0, VF_CHEAT, "Allow debug display.");
	REGISTER_CVAR2("component_equipment_debug", &m_componentEquipmentDebug, 0, VF_CHEAT, "Allow debug display.");
	REGISTER_CVAR2("component_character_attributes_debug", &m_componentCharacterAttributesDebug, 0, VF_CHEAT, "Allow debug display.");
	REGISTER_CVAR2("component_awareness_debug", &m_componentAwarenessDebug, 0, VF_CHEAT, "Allow debug display.");
	REGISTER_CVAR2("component_inventory_debug", &m_componentInventoryDebug, 0, VF_CHEAT, "Allow debug display.");

	// ***
	// *** COMMANDS
	// ***

	REGISTER_COMMAND("attach", CCVars::OnAttach, VF_NULL, "Attaches the player to a specified character.\n"
		"Usage: attach [entity name]");
	REGISTER_COMMAND("createobjectid", CCVars::OnCreateObjectId, VF_NULL, "Requests a new unique ObjectId for [class] of objects.\n"
		"Usage: createobjectid [class]");
	REGISTER_COMMAND("emote", CCVars::OnEmote, VF_NULL, "Makes a request for the character under player command to perform an emote.\n"
		"Usage: emote [emotion]");
}


void CCVars::UnregisterVariables()
{
	// ***
	// *** VARIABLES
	// ***


	// ***
	// *** COMMANDS
	// ***

	gEnv->pConsole->RemoveCommand("attach");
	gEnv->pConsole->RemoveCommand("createobjectid");
	gEnv->pConsole->RemoveCommand("emote");
}


void CCVars::OnAttach(IConsoleCmdArgs* pConsoleCommandArgs)
{
	if (pConsoleCommandArgs->GetArgCount() == 2)
	{
		auto clientActorId = gEnv->pGameFramework->GetClientActorId();
		if (clientActorId)
		{
			if (auto pPlayer = CPlayerComponent::GetLocalPlayer())
			{
				// See if they gave us a valid entity name. Attach if they did.
				// #TODO: put in a check to ensure only characters will ever be cast.
				if (auto pCharacterEntity = gEnv->pEntitySystem->FindEntityByName(pConsoleCommandArgs->GetArg(1)))
				{
					pPlayer->AttachToCharacter(pCharacterEntity->GetId());
				}
			}
		}
	}
	else
	{
		CryLogAlways("Please supply the name of the entity you wish to attach onto.");
	}
}


void CCVars::OnCreateObjectId(IConsoleCmdArgs* pConsoleCommandArgs)
{
	if (pConsoleCommandArgs->GetArgCount() == 2)
	{
		auto pClass = pConsoleCommandArgs->GetArg(1);
		ObjectId objectId = CObjectIdFactory::InvalidId;

		if (strcmp(pClass, "account") == 0)
		{
			objectId = CChrysalisCorePlugin::Get()->GetObjectId()->GetAccount()->CreateObjectId();
		}
		else if (strcmp(pClass, "character") == 0)
		{
			objectId = CChrysalisCorePlugin::Get()->GetObjectId()->GetCharacter()->CreateObjectId();
		}
		else if (strcmp(pClass, "item") == 0)
		{
			objectId = CChrysalisCorePlugin::Get()->GetObjectId()->GetItem()->CreateObjectId();
		}
		else if (strcmp(pClass, "faction") == 0)
		{
			objectId = CChrysalisCorePlugin::Get()->GetObjectId()->GetFaction()->CreateObjectId();
		}

		if (objectId != CObjectIdFactory::InvalidId)
		{
			CryLogAlways("New ObjectID: Decimal [%llu]  Hexadecimal [%llX]", objectId, objectId);
		}
		else
		{
			CryLogAlways("Unknown class. No Id created.");
		}
	}
	else
	{
		CryLogAlways("Syntax: CREATEOBJECTID <class>");
	}
}


void CCVars::OnEmote(IConsoleCmdArgs* pConsoleCommandArgs)
{
	if (pConsoleCommandArgs->GetArgCount() == 2)
	{
		if (auto pActorComponent = CPlayerComponent::GetLocalActor())
		{
			if (auto pActionController = pActorComponent->GetActionController())
			{
				auto emoteStr = string(pConsoleCommandArgs->GetArg(1)).MakeLower();
				TagID emoteTagId { TAG_ID_INVALID };

				// This is pretty brutal. It might be better to do it as a vector<pair> and enumerate through instead. Then again,
				// constructing the vector each time isn't a great idea, and doing it outside this routine needs to be sure the
				// tags are all set first. 
				if (strcmp(emoteStr, "anger") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Anger;
				else if (strcmp(emoteStr, "annoyance") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Annoyance;
				else if (strcmp(emoteStr, "anxiety") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Anxiety;
				else if (strcmp(emoteStr, "apathy") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Apathy;
				else if (strcmp(emoteStr, "awe") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Awe;
				else if (strcmp(emoteStr, "boredom") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Boredom;
				else if (strcmp(emoteStr, "confidence") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Confidence;
				else if (strcmp(emoteStr, "contempt") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Contempt;
				else if (strcmp(emoteStr, "contentment") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Contentment;
				else if (strcmp(emoteStr, "courage") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Courage;
				else if (strcmp(emoteStr, "curiosity") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Curiosity;
				else if (strcmp(emoteStr, "depression") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Depression;
				else if (strcmp(emoteStr, "despair") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Despair;
				else if (strcmp(emoteStr, "disappointment") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Disappointment;
				else if (strcmp(emoteStr, "disgust") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Disgust;
				else if (strcmp(emoteStr, "dread") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Dread;
				else if (strcmp(emoteStr, "embarrassment") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Embarrassment;
				else if (strcmp(emoteStr, "excitement") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Excitement;
				else if (strcmp(emoteStr, "fear") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Fear;
				else if (strcmp(emoteStr, "frustration") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Frustration;
				else if (strcmp(emoteStr, "gratitude") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Gratitude;
				else if (strcmp(emoteStr, "grief") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Grief;
				else if (strcmp(emoteStr, "guilt") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Guilt;
				else if (strcmp(emoteStr, "happiness") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Happiness;
				else if (strcmp(emoteStr, "hatred") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Hatred;
				else if (strcmp(emoteStr, "hope") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Hope;
				else if (strcmp(emoteStr, "horror") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Horror;
				else if (strcmp(emoteStr, "hostility") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Hostility;
				else if (strcmp(emoteStr, "hurt") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Hurt;
				else if (strcmp(emoteStr, "hysteria") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Hysteria;
				else if (strcmp(emoteStr, "indifference") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Indifference;
				else if (strcmp(emoteStr, "interest") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Interest;
				else if (strcmp(emoteStr, "joy") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Joy;
				else if (strcmp(emoteStr, "loathing") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Loathing;
				else if (strcmp(emoteStr, "loneliness") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Loneliness;
				else if (strcmp(emoteStr, "outrage") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Outrage;
				else if (strcmp(emoteStr, "panic") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Panic;
				else if (strcmp(emoteStr, "pity") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Pity;
				else if (strcmp(emoteStr, "pleasure") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Pleasure;
				else if (strcmp(emoteStr, "pride") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Pride;
				else if (strcmp(emoteStr, "rage") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Rage;
				else if (strcmp(emoteStr, "regret") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Regret;
				else if (strcmp(emoteStr, "relief") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Relief;
				else if (strcmp(emoteStr, "remorse") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Remorse;
				else if (strcmp(emoteStr, "sadness") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Sadness;
				else if (strcmp(emoteStr, "satisfaction") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Satisfaction;
				else if (strcmp(emoteStr, "shame") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Shame;
				else if (strcmp(emoteStr, "shock") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Shock;
				else if (strcmp(emoteStr, "shyness") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Shyness;
				else if (strcmp(emoteStr, "sorrow") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Sorrow;
				else if (strcmp(emoteStr, "surprise") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Surprise;
				else if (strcmp(emoteStr, "terror") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Terror;
				else if (strcmp(emoteStr, "trust") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Trust;
				else if (strcmp(emoteStr, "wonder") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Wonder;
				else if (strcmp(emoteStr, "worry") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Worry;
				else if (strcmp(emoteStr, "zeal") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Zeal;
				else if (strcmp(emoteStr, "zest") == 0) emoteTagId = g_emoteMannequinParams.tagIDs.Zest;

				// Queue an emote to play for that character.
				if (emoteTagId != TAG_ID_INVALID)
				{
					auto emoteAction = new CActorAnimationActionEmote(emoteTagId);
					pActionController->Queue(*emoteAction);
				}
				else
				{
					CryLogAlways("There is no emote by that name.");
				}
			}
		}
	}
	else
	{
		CryLogAlways("Please supply the name of the emote to play.");
	}
}
}