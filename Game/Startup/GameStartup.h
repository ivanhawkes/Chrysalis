#pragma once

#include <CryGame/IGameStartup.h>
#include <CrySystem/IWindowMessageHandler.h>


/**
An implementation of the IGameStartup interface. Used to start a CRYENGINE game. This class takes care of
initializing ALL of CRYENGINE's systems, handling window events, running the application loop, and creating,
initializing, and updating a CRYENGINE game. This class is instantiated by the CRYENGINE Launcher automatically upon
loading this module by calling this module's exported function "CreateGameStartup()". Every CRYENGINE game needs to
have an implementation of the IGameStartup interface.

\sa	IGameStartup
*/
class CGameStartup : public IGameStartup, public IWindowMessageHandler, public ISystemEventListener
{
public:
	CGameStartup();
	~CGameStartup();


	// ***
	// *** IGameStartup
	// ***

	/**
	Initializes this instance. Used to register the window class that CRYENGINE will use, create and initialize an
	IGameFramwork instance which in turn initializes All of the other CRYENGINE systems, and creates and initializes a
	CRYENGINE game instance.

	\param	startupParams	The system initialization parameters used to decide what and how CRYENGINE systems get
	initialized. Used this to customize the CRYENGINE startup / initialization process.

	\return	An IGameRef Instance That References The Newly Create Game Instance.
	*/
	virtual IGameRef Init(SSystemInitParams &startupParams);


	/**
	Shuts down this instance (delete this;), The currently active game, and shuts down the application.

	This method should not be called directly. If your intention is to quit the game and exit the application, you should
	call the ISystem::Quit() method instead.
	*/
	virtual void Shutdown();


	/**
	Updates this instance every frame. Used to update the currently active game.

	\param	haveFocus  	Specifies whether the game window currently has input focus or not.
	\param	updateFlags	The update flags that are active this frame (usually 0).

	\return	Returns 0 If the application should quit. Non-zero if the application should continue.
	*/
	virtual int Update(bool haveFocus, unsigned int updateFlags);


	/**
	Gets the restart level and thus triggers a restart.

	\param [in,out]	levelName	The level to load after the restart.

	\return	False To quit. True otherwise.
	*/
	virtual bool GetRestartLevel(char** levelName);


	/**
	Gets the currently needed game patch if any.

	\return	The absolute path + filename + extension of the update file that needs to be installed.
	*/
	virtual const char* GetPatch() const;


	/**
	Gets the game (GameDLL) that is to be loaded after the restart is finished.

	\param [in,out]	pModName	The name of the GameDLL to load.
	\param	nameLenMax			The amount of characters in the name of the GameDLL.

	\return	True to actually load the GameDLL,  false to ignore the request.
	*/
	virtual bool GetRestartMod(char* pModName, int nameLenMax);


	/**
	Starts the application loop. Also processes the window procedure of the game window and updates the game.

	\param	autoStartLevelName	The name of the level to jump-in to,  NULL to run normally.

	\return	0 To indicate that the application exited normally, non-zero otherwise.
	*/
	virtual int Run(const char * autoStartLevelName);


	// ***
	// *** ISystemEventListener
	// ***

	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;


	// ***
	// *** IWindowMessageHandler
	// ***

#if CRY_PLATFORM_WINDOWS
	bool HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pResult);
#endif


private:

	/**
	The IGameFramework interface. Used to manage everything in a CRYENGINE game and access all
	other systems in the SDK. It is the primary interface used in a CRYENGINE game.
	**/
	IGameFramework* m_pGameFramework;


	/** The IGame interface. Used to decide game logic and controls how a CRYENGINE game works. */
	IGame* m_pGame;


	/** The IGame interface reference. Simply references the IGame interface. */
	IGameRef m_GameRef;


	/** Indicates whether the application is in the process of quitting. Used internally. */
	bool m_bQuit;


	/** The handle to the currently loaded CryAction DLL. */
	HMODULE m_pCryActionDll;


	/**
	Used to track when we switch in and out of full screen mode.
	
	\param [in,out]	pVar If non-null, the variable.
	**/
	static void FullScreenCVarChanged(ICVar *pVar);
	

	/** Track whether we are in full-screen mode or not. */
	bool m_fullScreenCVarSetup = false;


	///**
	//Resets this instance.
	//
	//\return An IGameRef.
	//**/
	//virtual IGameRef Reset();
};