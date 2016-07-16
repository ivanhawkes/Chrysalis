/**
\file	Source\ScriptBinds.h

Declares the script binds class.
*/
#pragma once

#include <CryScriptSystem/IScriptSystem.h>
#include <CryEntitySystem/IEntity.h>


/**
The game script bind. Used to expose all game functionality To Lua. All methods and variables that you want to
expose to Lua that pertain to the game should be added here. Every function and variable that this class has will be
accessible to the scripts as long as they are registered with the script system.

\sa					 	CScriptableBase
\sa	T:CScriptableBase
*/
class CScriptBindGame : public CScriptableBase
{
public:

	// Used to differentiate which type of resource we are trying to cache.
	enum EGameCacheResourceType
	{
		eGCRT_Texture = 0,
		eGCRT_TextureDeferredCubemap = 1,
		eGCRT_StaticObject = 2,
		eGCRT_Material = 3,
	};


	/**
	This instance's default constructor. Used to create the "Game" Lua table and to register all this instance's
	methods to the script system. They will then be accessible from the "Game" Lua table.
	*/
	CScriptBindGame();


	/**
	This Instance's Default Destructor.
	*/
	~CScriptBindGame();


	/**
	Sets The Game's Spawn Point. A Spawn Point Should Be Set In Order To Correctly Position The Player.

	\param [in,out]	pFunctionHandler	The Lua Information That Describes How This ScriptBind Function Was Called And
	Also Describes Information About The Script Calling This Function.
	\param	SpawnPoint					The EntityId Of The Spawn Point. Lua Does Not Support Integers So We Must Use
	This Helper Struct.

	\return	The Amount Of Variables That Are Being Returned To Lua.
	*/
	int SetSpawnPoint(IFunctionHandler* pFunctionHandler, ScriptHandle SpawnPoint);


	/**
	Sets The Game's Spawn Point To NULL. Should Be Called When A Spawn Point Is Removed.

	\param [in,out]	pFunctionHandler	The Lua Information That Describes How This ScriptBind Function Was Called And
	Also Describes Information About The Script Calling This Function.

	\return	An int.
	*/
	int SetSpawnPointToNull(IFunctionHandler* pFunctionHandler);


	/**
	Cache a resource for Lua.
	
	\param [in,out]	pH	   	If non-null, the p h.
	\param	whoIsRequesting	The who is requesting.
	\param	resourceName   	Name of the resource.
	\param	resourceType   	Type of the resource.
	\param	resourceFlags  	The resource flags.
	
	\return	An int.
	*/
	int CacheResource(IFunctionHandler *pH, const char* whoIsRequesting, const char* resourceName, int resourceType, int resourceFlags);


	/**
	Sends an event to game object. Used to send simple event notifications to objects e.g.

	Game.SendEventToGameObject (self.id, "propertyChanged");

	
	\param [in,out]	pH   	If non-null, the p h.
	\param	entityId	 	Identifier for the entity.
	\param [in,out]	event	If non-null, the event.
	
	\return	An int.
	*/
	int SendEventToGameObject(IFunctionHandler* pH, ScriptHandle entityId, char* event);

private:
	/** Registers global variables for Lua to make use of. */
	void RegisterGlobals();

	/** Registers the methods. */
	void RegisterMethods();
};