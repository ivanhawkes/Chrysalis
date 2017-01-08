#include <StdAfx.h>

#include "ConsoleCommands.h"
#include <CrySystem/ISystem.h>
#include <Player/Player.h>
#include <Actor/Character/Character.h>
#include <ObjectID/ObjectId.h>
#include <ObjectID/ObjectIdMasterFactory.h>
#include "Plugin/ChrysalisCorePlugin.h"
#include "Plugin/ChrysalisCore.h"


void CConsoleCommands::Register(void)
{
	REGISTER_COMMAND("attach", CConsoleCommands::OnAttach, VF_NULL, "Attaches the player to a specified character.\n"
		"Usage: attach [entity name]");
	REGISTER_COMMAND("createobjectid", CConsoleCommands::OnCreateObjectId, VF_NULL, "Requests a new unique ObjectId for [class] of objects.\n"
		"Usage: createobjectid [class]");
}


void CConsoleCommands::Unregister(void)
{
	gEnv->pConsole->RemoveCommand("attach");
	gEnv->pConsole->RemoveCommand("createobjectid");
}


void CConsoleCommands::OnAttach(IConsoleCmdArgs* pConsoleCommandArgs)
{
	if (pConsoleCommandArgs->GetArgCount() == 2)
	{
		auto clientActorId = gEnv->pGameFramework->GetClientActorId();
		if (clientActorId)
		{
			// The client actor should always be a CPlayer, so we can up-cast to that.
			auto pPlayer = static_cast<CPlayer*>(gEnv->pGameFramework->GetIActorSystem()->GetActor(clientActorId));

			// See if they gave us a valid entity name. Attach if they did.
			// TODO: put in a check to ensure only characters will ever be cast.
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
