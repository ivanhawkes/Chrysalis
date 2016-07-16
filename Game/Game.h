/**
\file	Source\Game\Game.h

Declares the game class.
*/
#pragma once

#include <CryGame/IGame.h>
#include <CryCore/Platform/IPlatformOS.h>
#include <CryGame/IGameFramework.h>
#include <CryPhysics/RayCastQueue.h>
#include "ScriptBinds/ScriptBinds.h"

#define __GAME_H__

const string ProjectName = "Chrysalis";

struct IGameRules;
struct IPlayerProfileManager;
struct IActionMap;
struct IActor;
class CGamePhysicsSettings;
class CConsoleCommands;
class CConsoleVariables;
class CGameActionMaps;
class CLedgeManager;
class CObjectIdMasterFactory;
class CGameCache;


// Define paths to important folders here.
static const string GamePathConfig = "config/";
static const string GamePathUi = "ui/";
static const string GamePathLibs = "libs/";
static const string GamePathLuaScripts = "Scripts/";
static const string GamePathLuaEntities = "Scripts/Entities/";


/**
An implementation of the IGame interface. Used to decide game logic and controls how a
CRYENGINE game works. Every CRYENGINE game needs a game implementation.

\sa	IGame
\sa	ISystemEventListener
\sa	IPlatformOS::IPlatformListener
\sa	IGameFrameworkListener
*/
class CGame : public IGame, public ISystemEventListener, public IPlatformOS::IPlatformListener, public IGameFrameworkListener
{
public:

	// ***
	// *** IGame
	// ***


	/**
	Automatically called by the IGameStartup interface. Initializes this instance with the
	specified IGameFramework interface. Used to sign-in the default user, set the current game type,
	load ActionMaps, And register custom GameObjects.

	\param [in,out]	pFramework	The currently active IGameFrameWork interface.

	\return	True If initialization was successful. False otherwise.
	*/
	virtual bool Init(IGameFramework *pFramework);


	virtual void InitEditor(IGameToEditorInterface* pGameToEditor);

	/**
	Automatically called by various systems to get the memory statistics for this instance. Add your own usage via
	'*this'. Be sure to add any memory you have allocated on the heap.

	\param [in,out]	s	The ICrySizer interface used to "size-up" this instance.
	*/
	virtual void GetMemoryStatistics(ICrySizer * s);


	/**
	Automatically called by the IGameStartup interface when it is time to shutdown this instance.
	This method should not be called directly and should de-allocate it's self (delete this;) and all
	resources it is using.

	If you want to shut down this instance and close the application, you should call the
	ISystem::Quit() method.
	*/
	virtual void Shutdown();


	/**
	Automatically called by the IGameStartup interface every frame to update this instance.

	\param	haveFocus  	Specifies if the game window currently has focus or not.
	\param	updateFlags	The update flags that are active during this update (usually 0).

	\return	Whether the game should quit or not. 0 To quit, non-zero to continue.
	*/
	virtual int Update(bool haveFocus, unsigned int updateFlags);


	/**
	Automatically called by the IEditorGame interface when the editor is entering or exiting game-
	mode.

	\param	bStart	Specifies whether the editor is entering game-mode or not. True if entering,
	false if exiting.
	*/
	virtual void EditorResetGame(bool bStart);


	/**
	Stores The player's (client actor) entityId.

	\param	playerId	The EntityId of the currently active player (client actor).
	*/
	virtual void PlayerIdSet(EntityId playerId);


	/**
	Gets this instance's long name (usually the game name and description).

	\return	This instance's long name.
	*/
	virtual const char *GetLongName();


	/**
	Gets this instance's name (usually the game name).

	\return	This instance's name.
	*/
	virtual const char *GetName();


	/**
	Loads all the ActionMaps that are used by this instance.

	\param	filename	The filename of the ActionMap file you want to load the ActionMaps from (an
	XML file).
	*/
	virtual void LoadActionMaps(const char* filename = "libs/config/defaultprofile.xml");


	/**
	Called automatically when the Player's id has been reset by the game channel.
	*/
	virtual void OnClearPlayerIds();


	/**
	Creates a save game name. Used when saving this instance to generate an appropriate save
	game name.

	\return	The newly created save game name.
	*/
	virtual IGame::TSaveGameName CreateSaveGameName();


	/**
	Gets the currently active IGameFramework interface.

	\return	The currently active IGameFramework interface.
	*/
	virtual IGameFramework *GetIGameFramework();


	/**
	Gets the "official" name for the specified level name (used mainly for displaying the level
	name in the UI).

	\param	levelName	The name of the level to get the mapped name of.

	\return	The "official" name for the specified level name.
	*/
	virtual const char* GetMappedLevelName(const char *levelName) const;


	/**
	Performs the initial save game.

	\return	True if save was successful. False otherwise.
	*/
	virtual const bool DoInitialSavegame() const;


	/**
	Adds A Game Specific Warning.

	\param	stringId		 	The warning identifier.
	\param	paramMessage	 	The warning message.
	\param [in,out]	pListener	 (optional) the IGameWarningsListener you want to notify about the
	Game Warning.

	\return	The warning id if successful. 0 otherwise.
	*/
	virtual uint32 AddGameWarning(const char* stringId, const char* paramMessage, IGameWarningsListener* pListener = NULL);


	/**
	Automatically called by the I3DEngine interface to allow this instance to add polygons and
	meshes to the scene.

	\param	passInfo	Information describing the current render pass.
	*/
	virtual void OnRenderScene(const SRenderingPassInfo &passInfo);


	/**
	Renders all active game warnings.
	*/
	virtual void RenderGameWarnings();


	/**
	Removes the specified game warning.

	\param	stringId	The warning identifier for the warning you want to remove.
	*/
	virtual void RemoveGameWarning(const char* stringId);


	/**
	Automatically called by the IGameFramework interface when a level has ended.

	\param	stringId	The name of the level that has ended.

	\return	True if you handle this event and call the IGameFramework::ScheduleEndLevelNow() method
	directly, false if the IGameFramework should handle it.
	*/
	virtual bool GameEndLevel(const char* stringId);


	/**
	Creates a GameStateRecorder instance and passes it's ownership to the caller
	(CryAction / GamePlayRecorder).

	\param [in,out]	pL	The IGameplayListener to register to the created GameStateRecorder.

	\return	The newly created IGameStateRecorder if creation was successful. nullptr otherwise.
	*/
	virtual IGameStateRecorder* CreateGameStateRecorder(IGameplayListener* pL);


	/**
	Automatically called by various systems to perform full serialization of this instance.

	\param	ser	The serializer to serialize to.
	*/
	virtual void FullSerialize(TSerialize ser);


	/**
	Automatically called by various systems to perform post serialization of this instance which
	occurs after full serialization. Mainly used to fix potential full serialization issues.
	*/
	virtual void PostSerialize() {};


	/**
	Automatically called by the editor? During level data export. Used to allow this instance to
	store game-specific data inside the level PAK file.

	\param	levelName  	Name of the level being exported.
	\param	missionName	Name of the currently active mission.

	\return	An IGame::ExportFilesInfo struct describing the data you exported.
	*/
	virtual IGame::ExportFilesInfo ExportLevelData(const char* levelName, const char* missionName) const;


	/**
	Automatically called by the editor? during level data loading. Used to allow this instance to
	load the game-specific data inside the level PAK file that this instance previously exported to
	it.

	\param	levelName  	Name of the level being loaded.
	\param	missionName	Name of the currently active mission.
	*/
	virtual void LoadExportedLevelData(const char* levelName, const char* missionName);


	/**
	Automatically called by the IFlowSystem when it is time to register custom game-specific
	flownodes.
	*/
	virtual void RegisterGameFlowNodes();


	/**
	Gets the IGamePhysicsSettings interface.

	\return	the IGamePhysicsSettings interface.
	*/
	virtual IGamePhysicsSettings* GetIGamePhysicsSettings();


	/**
	Automatically called by various system when a system event occurs.

	\param	event 	The system event that occurred.
	\param	wparam	An unsigned 64 bit integer value specific to the system event.
	\param	lparam	An unsigned 64 bit integer value specific to the system event.
	*/
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam);


	virtual void* GetGameInterface() { return nullptr; };


	// ***
	// *** IPlatformOS::IPlatformListener
	// ***


	/**
	Automatically called by various systems when a platform specific event has occurred.

	\param	event	The Platform-specific event that occurred. Can be cast to the appropriate
	derived SPlatformEvent struct if needed.
	*/
	virtual void OnPlatformEvent(const IPlatformOS::SPlatformEvent& event);


	// ***
	// *** IGameFrameworkListener
	// ***


	/**
	Automatically called by the IGameFramework when it is time to perform post updates.

	\param	fDeltaTime	The time (in seconds) that has elapsed since last frame.
	*/
	virtual void OnPostUpdate(float fDeltaTime) override;


	/**
	Automatically called by the IGameFramework when it is time to save game-specific
	settings / content to the specified save game.

	\param [in,out]	pSaveGame	The ISaveGame instance currently being used to save the game.
	*/
	virtual void OnSaveGame(ISaveGame* pSaveGame) override;


	/**
	Automatically called by the IGameFramework when it is time to load game-specific
	Settings / content from the specified load game.

	\param [in,out]	pLoadGame	The ILoadGame instance currently being used to load the game.
	*/
	virtual void OnLoadGame(ILoadGame* pLoadGame) override;


	/**
	Automatically called by the IGameFramework when a level has been unloaded and a new level
	should be loaded.

	\param	nextLevel	The level that should be loaded.
	*/
	virtual void OnLevelEnd(const char* nextLevel) override;


	/**
	Automatically called by various systems when a game framework event has occurred.

	\param	event	The game framework event that occurred.
	*/
	virtual void OnActionEvent(const SActionEvent& event) override;


	/**
	Automatically called by the IGameFramework when it is time to execute pre-render game logic.
	*/
	virtual void OnPreRender() override;


	/**
	Automatically called by the IGameFramework when a save file is loaded in memory but has not
	yet been processed.

	\param	pLevelName	The name of the level that is to be loaded / saved?.
	*/
	virtual void OnSavegameFileLoadedInMemory(const char* pLevelName) override;


	/**
	Automatically called by the IGameFramework when the game should forcefully load with flash
	(Scaleform).
	*/
	virtual void OnForceLoadingWithFlash() override;


	// ***	
	// *** Global ray-casting. Provides support for a ray-cast test that is queue based and global in scope.
	// ***

public:
	// Provides a type to use for all global ray-casting operations.
	typedef RayCastQueue<41> GlobalRayCaster;


	/**
	Gets access to the global ray caster.

	\return	The ray caster.
	*/
	GlobalRayCaster& GetRayCaster() { assert(m_pRayCaster); return *m_pRayCaster; }


private:
	GlobalRayCaster* m_pRayCaster = nullptr;


	// ***	
	// *** CGame
	// ***

public:

	/** This instance's default constructor. */
	CGame();


	/** This instance's default destructor. */
	~CGame();


	/**
	Automatically called by the IGameRules when a connected client enters the game (the Level).

	\param [in,out]	pClientActor	The actor that entered the game (level).
	*/
	void OnClientEnteredGame(IActor* pClientActor);


	/**
	Called automatically by the player class when the client actor has spawned in the game (the
	level).

	\param [in,out]	pClientActor	The client actor that was spawned.
	*/
	void OnClientActorSpawned(IActor* pClientActor);


	/**
	Gets the currently active gamerules. Every CRYENGINE game needs to have an active GameRules.

	\return	The currently active GameRules.
	*/
	IGameRules* GetGameRules();


	/**
	Registers all the custom GameObjects that will be available in this instance.

	Before custom GameObjects can be used they must be registered first.
	*/
	void RegisterGameObjects();


	/**
	Logs in the specified user. At least 1 user needs to be logged in, and a profile set in order
	to run this instance.

	\param	UserId	The zero-based index of the user you want to login. For single-player games,
	this is usually 0.

	\return	True if login was successful. False otherwise.
	*/
	bool LoginUser(unsigned int UserId);


	/**
	Initializes the IPlatformOS interface. Used to setup an encryption key for the currently
	running platform (PC, PS4, XBO, etc).
	*/
	void InitPlatformOS();


	/**
	Gets the client actor's EntityId (the local player's EntityId).

	\return	The client actor's EntityId.
	*/
	EntityId GetClientActorID();


	/**
	Sets the local player's spawn point. Used to set the position of the player when he / she
	enters a level.

	\param [in,out]	pSpawnPoint	The spawn point you want the player to use.
	*/
	void SetSpawnPoint(IEntity* pSpawnPoint);


	/**
	Gets the player's spawn point.

	\return	The player's spawn point.
	*/
	IEntity* GetSpawnPoint();


	/**
	Gets a pointer to the console variables.

	\return	null if it fails, else the console variables.
	*/
	ILINE CConsoleVariables* GetCVars() { return m_pConsoleVariables; }


	///**
	//Access to the game's action maps.

	//\return	A CGameActionMaps&amp;
	//*/
	CGameActionMaps& ActionMaps() const { return *m_pGameActionMaps; };


	/**
	Gets ledge manager.

	\return	null if it fails, else the ledge manager.
	*/
	CLedgeManager* GetLedgeManager();


	/**
	Gets object identifier master factory.

	\return	null if it fails, else the object identifier master factory.
	*/
	CObjectIdMasterFactory* GetObjectId() { return m_pObjectIdMasterFactory; }

	// Access to the game cache.
	CGameCache& GetGameCache();
	const CGameCache& GetGameCache() const;


	/**
	Gets access to the script binds.
	
	\return	null if it fails, else the script binds.
	*/
	CScriptBinds* GetScriptBinds() { return m_pScriptBinds; }

private:

	/** Registers Game-Specific Console Variables. */
	void RegisterGameCVars();


	/** Un-Registers Game-Specific Console Variables. */
	void UnRegisterGameCVars();


	/** Registers Game-Specific Console Commands. */
	void RegisterGameConsoleCommands();


	/** Un-Registers Game-Specific Console Commands. */
	void UnRegisterGameConsoleCommands();


	/**
	The IGameFramework interface. Used to manage everything in a CRYENGINE game and access all other systems in the
	SDK. It is the primary interface used in a CRYENGINE game.
	*/
	IGameFramework* m_pGameFramework = nullptr;


	/** The IPlayerProfileManager interface. Used to create / delete / manage users and profiles. */
	IPlayerProfileManager* m_pPlayerProfileManager = nullptr;


	/**
	The IGameRules interface. Used to control the "win" conditions of a CRYENGINE game and provides various helper
	functions specific to a CRYENGINE game. Also acts as an interface between a CRYENGINE game and the rest of the system.
	*/
	IGameRules* m_pGameRules = nullptr;


	/**
	The default ActionMap. Used to provide input logic to a CRYENGINE game. An ActionMap provides mappings between
	"actions" and input devices, such that when a specific input device sends a signal, the mapped action happens.
	*/
	IActionMap* m_pDefaultAM = nullptr;

	/** The EntityId of the player (client actor). */
	EntityId m_PlayerEntityId = INVALID_ENTITYID;

	/** The spawn point for the local player. Used to set the position of the player when he / she enters a level. */
	IEntity* m_pSpawnPoint = nullptr;

	/** The game specific physics settings such as collision filtering. Used by the scripts as well. */
	CGamePhysicsSettings* m_pGamePhysicsSettings = nullptr;

	/** The console commands. Used to register and unregister the console commands. */
	CConsoleCommands* m_pConsoleCommands;

	/** The console variables. Used to register and unregister the console variables. */
	CConsoleVariables* m_pConsoleVariables = nullptr;

	///** The game action maps. */
	CGameActionMaps* m_pGameActionMaps = nullptr;

	/** A manager for handling the ledges in levels. */
	CLedgeManager* m_pLedgeManager = nullptr;

	/** The object identifier master factory. */
	CObjectIdMasterFactory* m_pObjectIdMasterFactory = nullptr;

	/** The script binds. */
	CScriptBinds* m_pScriptBinds = nullptr;

	/** Provides a way to request assets be cached by the game. */
	CGameCache* m_pGameCache = nullptr;
};


// ***
// *** GLOBALS: quick and dirty access to the most needed structures. Be sure they have been set before
// *** you try and dereference them or comedy will ensure.
// ***


/**
A global pointer to the game.

\return	A pointer to the running game instance.
*/
extern CGame *g_pGame;

/**
A global pointer to the game's console variables.

\return	A pointer to the game's console variables.
*/
extern CConsoleVariables* g_pGameCVars;