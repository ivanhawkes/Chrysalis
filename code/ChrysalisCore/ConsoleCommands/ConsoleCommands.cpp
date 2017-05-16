#include <StdAfx.h>

#include "ConsoleCommands.h"
#include <CrySystem/ISystem.h>
#include <Player/Player.h>
#include <Actor/Animation/Actions/ActorAnimationActionEmote.h>
#include <Actor/Character/Character.h>
#include <ObjectID/ObjectId.h>
#include <ObjectID/ObjectIdMasterFactory.h>


void CConsoleCommands::Register(void)
{
	REGISTER_COMMAND("attach", CConsoleCommands::OnAttach, VF_NULL, "Attaches the player to a specified character.\n"
		"Usage: attach [entity name]");
	REGISTER_COMMAND("createobjectid", CConsoleCommands::OnCreateObjectId, VF_NULL, "Requests a new unique ObjectId for [class] of objects.\n"
		"Usage: createobjectid [class]");
	REGISTER_COMMAND("emote", CConsoleCommands::OnEmote, VF_NULL, "Makes a request for the character under player command to perform an emote.\n"
		"Usage: emote [emotion]");
}


void CConsoleCommands::Unregister(void)
{
	gEnv->pConsole->RemoveCommand("attach");
	gEnv->pConsole->RemoveCommand("createobjectid");
	gEnv->pConsole->RemoveCommand("emote");
}


void CConsoleCommands::OnAttach(IConsoleCmdArgs* pConsoleCommandArgs)
{
	if (pConsoleCommandArgs->GetArgCount() == 2)
	{
		auto clientActorId = gEnv->pGameFramework->GetClientActorId();
		if (clientActorId)
		{
			// HACK: The client actor should always be a CPlayer, so we can up-cast to that.
			// TODO: This will no longer be true after switching to 5.4.
			auto pPlayer = static_cast<CPlayer*>(gEnv->pGameFramework->GetIActorSystem()->GetActor(clientActorId));

			// See if they gave us a valid entity name. Attach if they did.
			// #TODO: put in a check to ensure only characters will ever be cast.
			auto pCharacterEntity = gEnv->pEntitySystem->FindEntityByName(pConsoleCommandArgs->GetArg(1));
			if (pCharacterEntity)
			{
				pPlayer->AttachToCharacter(pCharacterEntity->GetId());
			}
		}
	}
	else
	{
		CryLogAlways("Please supply the name of the entity you wish to attach onto.");
	}
}


void CConsoleCommands::OnCreateObjectId(IConsoleCmdArgs* pConsoleCommandArgs)
{
	if (pConsoleCommandArgs->GetArgCount() == 2)
	{
		auto pClass = pConsoleCommandArgs->GetArg(1);
		ObjectId objectId = CObjectIdFactory::InvalidId;

		if (strcmp (pClass, "account") == 0)
		{
			objectId = CChrysalisCorePlugin::Get()->GetChrysalisCore()->GetObjectId()->GetAccount()->CreateObjectId();
		}
		else if (strcmp(pClass, "character") == 0)
		{ 
			objectId = CChrysalisCorePlugin::Get()->GetChrysalisCore()->GetObjectId()->GetCharacter()->CreateObjectId();
		}
		else if (strcmp(pClass, "item") == 0)
		{
			objectId = CChrysalisCorePlugin::Get()->GetChrysalisCore()->GetObjectId()->GetItem()->CreateObjectId();
		}
		else if (strcmp(pClass, "faction") == 0)
		{
			objectId = CChrysalisCorePlugin::Get()->GetChrysalisCore()->GetObjectId()->GetFaction()->CreateObjectId();
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


void CConsoleCommands::OnEmote(IConsoleCmdArgs* pConsoleCommandArgs)
{
	if (pConsoleCommandArgs->GetArgCount() == 2)
	{
		if (auto pCharacter = CPlayer::GetLocalCharacter())
		{
			if (auto pActionController = pCharacter->GetActionController())
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

