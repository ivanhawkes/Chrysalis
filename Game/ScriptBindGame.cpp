#include <StdAfx.h>

#include "ScriptBindGame.h"
#include <CryEntitySystem/IEntitySystem.h>
#include <Game/Game.h>
#include "Game/Cache/GameCache.h"
#include <GameObjects/GameObject.h>


// Useful for debugging. Comment out this when it's not needed.
// TODO: Give it a debug flag to check.
void LogLuaCacheResource(const char* whoIsRequesting, const char* type, const char* resourceName, const int flags)
{
#ifdef DEBUG
	if (resourceName && resourceName [0])
	{
		CryLog("[GAME CACHE LUA] by '%s' : %s - %s Flags(%d)", whoIsRequesting, type, resourceName, flags);
	}
#endif
}


CScriptBindGame::CScriptBindGame()
{
	// Initializes this instance by storing the passed-in values and creating a table that will hold this instance's
	// registered methods and variables.
	Init(gEnv->pSystem->GetIScriptSystem(), gEnv->pSystem);

	// Creates the "game" table in Lua that will hold all this instance's registered methods and variables.
	SetGlobalName("Game");
	RegisterGlobals();
	RegisterMethods();
}


CScriptBindGame::~CScriptBindGame()
{

}


void CScriptBindGame::RegisterGlobals()
{
	// Needed for the cache.
	m_pSS->SetGlobalValue("eGameCacheResourceType_Texture", CScriptBindGame::eGCRT_Texture);
	m_pSS->SetGlobalValue("eGameCacheResourceType_TextureDeferredCubemap", CScriptBindGame::eGCRT_TextureDeferredCubemap);
	m_pSS->SetGlobalValue("eGameCacheResourceType_StaticObject", CScriptBindGame::eGCRT_StaticObject);
	m_pSS->SetGlobalValue("eGameCacheResourceType_Material", CScriptBindGame::eGCRT_Material);
	m_pSS->SetGlobalValue("eGameCacheResourceFlag_TextureNoStream", FT_DONT_STREAM);
	m_pSS->SetGlobalValue("eGameCacheResourceFlag_TextureReplicateAllSides", FT_REPLICATE_TO_ALL_SIDES);
}


void CScriptBindGame::RegisterMethods()
{
	// TODO: Doesn't the compiler provide a means of getting the class name automatically?
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBindGame::

	RegisterTemplateFunction("SetSpawnPoint", "EntityId", *this, &CScriptBindGame::SetSpawnPoint);
	RegisterFunction("SetSpawnPointToNull", functor_ret(*this, &CScriptBindGame::SetSpawnPointToNull));
	SCRIPT_REG_TEMPLFUNC(CacheResource, "whoIsRequesting, resourceName, resourceType, resourceFlags");

	// Provides a simple means of sending a text event message to an object.
	SCRIPT_REG_TEMPLFUNC(SendEventToGameObject, "entityId, event");

#undef SCRIPT_REG_CLASSNAME
}


int CScriptBindGame::SetSpawnPoint(IFunctionHandler* pFunctionHandler, ScriptHandle SpawnPoint)
{
	// Sets the game's spawn point.
	((CGame*) gEnv->pGame)->SetSpawnPoint(gEnv->pEntitySystem->GetEntity((EntityId) SpawnPoint.n));

	return pFunctionHandler->EndFunction();
}


int CScriptBindGame::SetSpawnPointToNull(IFunctionHandler* pFunctionHandler)
{
	// Sets the game's spawn point to NULL.
	((CGame*) gEnv->pGame)->SetSpawnPoint(nullptr);

	return pFunctionHandler->EndFunction();
}


int CScriptBindGame::CacheResource(IFunctionHandler *pH, const char* whoIsRequesting, const char* resourceName, int resourceType, int resourceFlags)
{
	// Only cache in pure game mode.
	//if (gEnv->IsEditor())
	//	return pH->EndFunction();

	CGameCache& gameCache = g_pGame->GetGameCache();

	switch (resourceType)
	{
		case eGCRT_Texture:
		{
			gameCache.CacheTexture(resourceName, resourceFlags);
			LogLuaCacheResource(whoIsRequesting, "Texture", resourceName, resourceFlags);
		}
			break;

		case eGCRT_TextureDeferredCubemap:
		{
			// Some magic strings ops Copy & Pasted from ScriptBind_Entity::ParseLightProperties
			// TODO: DRY principle, provide a function to handle this sort of thing.
			const char* specularCubemap = resourceName;

			if (specularCubemap && strlen(specularCubemap) > 0)
			{
				CryFixedStringT<256> sSpecularName(specularCubemap);
				int strIndex = sSpecularName.find("_diff");
				if (strIndex >= 0)
				{
					sSpecularName = sSpecularName.substr(0, strIndex) + sSpecularName.substr(strIndex + 5, sSpecularName.length());
					specularCubemap = sSpecularName.c_str();
				}

				CryFixedStringT<256> diffuseCubemap;
				diffuseCubemap.Format("%s%s%s.%s", PathUtil::AddSlash(PathUtil::GetPath(specularCubemap).c_str()).c_str(),
					PathUtil::GetFileName(specularCubemap).c_str(), "_diff", PathUtil::GetExt(specularCubemap));

				// '\\' in filename causing texture duplication
				string specularCubemapUnix = PathUtil::ToUnixPath(specularCubemap);
				string diffuseCubemapUnix = PathUtil::ToUnixPath(diffuseCubemap.c_str());

				gameCache.CacheTexture(specularCubemapUnix.c_str(), resourceFlags);
				gameCache.CacheTexture(diffuseCubemapUnix.c_str(), resourceFlags);

				LogLuaCacheResource(whoIsRequesting, "CubeMap Specular", specularCubemapUnix.c_str(), resourceFlags);
				LogLuaCacheResource(whoIsRequesting, "CubeMap Diffuse", diffuseCubemapUnix.c_str(), resourceFlags);
			}
		}
			break;

		case eGCRT_StaticObject:
		{
			gameCache.CacheGeometry(resourceName);
			LogLuaCacheResource(whoIsRequesting, "Static Object", resourceName, resourceFlags);
		}
			break;

		case eGCRT_Material:
		{
			gameCache.CacheMaterial(resourceName);
			LogLuaCacheResource(whoIsRequesting, "Material", resourceName, resourceFlags);
		}
			break;
	}

	return pH->EndFunction();
}


int CScriptBindGame::SendEventToGameObject(IFunctionHandler* pH, ScriptHandle entityId, char* event)
{
	IEntity* pEntity = gEnv->pEntitySystem->GetEntity((EntityId) entityId.n);
	if (pEntity != NULL)
	{
		CGameObject* pGameObject = static_cast<CGameObject*>(pEntity->GetProxy(ENTITY_PROXY_USER));
		if (pGameObject != NULL)
		{
			SGameObjectEvent goEvent((uint32) eGFE_ScriptEvent, eGOEF_ToExtensions, IGameObjectSystem::InvalidExtensionID, event);
			pGameObject->SendEvent(goEvent);
		}
	}
	return pH->EndFunction();
}
