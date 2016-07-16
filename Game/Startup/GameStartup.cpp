#include <StdAfx.h>

#include <CryCore/Platform/platform_impl.inl>
#include <CryGame/IGameFramework.h>
#include <CryRenderer/IRenderer.h>
//#include <string>
#include <CryInput/IHardwareMouse.h>
#include <CryCore/Platform/CryLibrary.h>
#include "ConsoleVariables/ConsoleVariables.h"
#include "GameStartup.h"
#include "Game/Game.h"
//#include <IPluginManager_impl.h>


// A platform independant way to specify the CryAction library filename.
#define GAME_FRAMEWORK_FILENAME CryLibraryDefName("CryAction")


// Export this function using "C" linkage (to avoid c++ name mangling).
extern "C"
{
	/**
	Automatically called by the CRYENGINE Launcher when it is time to create an instance of the IGameStartup
	interface.

	This function is very important and must exist in order for the CRYENGINE Launcher to create an IGameStartup
	instance. This function should not be called manually.

	\return    The Newly Created IGameStartup Instance.
	*/
	GAME_API IGameStartup* CreateGameStartup()
	{
		static char buff [sizeof(CGameStartup)];
		return new (buff) CGameStartup();
	}


	GAME_API IGame* CreateGame(IGameFramework* pGameFramework)
	{
		ModuleInitISystem(pGameFramework->GetISystem(), ProjectName);

		static char pGameBuffer [sizeof(CGame)];
		return new ((void*) pGameBuffer) CGame();
	}
}


#if !defined(_LIB)

static HMODULE s_frameworkDLL;


static void CleanupFrameworkDLL()
{
	CRY_ASSERT(s_frameworkDLL);
	CryFreeLibrary(s_frameworkDLL);
	s_frameworkDLL = 0;
}


HMODULE GetFrameworkDLL(const char* binariesDir)
{
	MEMSTAT_CONTEXT_FMT(EMemStatContextTypes::MSC_Other, 0, "Load %s", GAME_FRAMEWORK_FILENAME);
	if (!s_frameworkDLL)
	{
		if (binariesDir && binariesDir [0])
		{
			string dllName = PathUtil::Make(binariesDir, GAME_FRAMEWORK_FILENAME);
			s_frameworkDLL = CryLoadLibrary(dllName.c_str());
		}
		else
		{
			s_frameworkDLL = CryLoadLibrary(GAME_FRAMEWORK_FILENAME);
		}
		atexit(CleanupFrameworkDLL);
	}

	return s_frameworkDLL;
}

#endif


//bool InitWindow()
//{
	//WNDCLASS wc;
	//memset(&wc, 0, sizeof(WNDCLASS));

	//// Game window should catch double click events and own it's own DC.
	//wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

	//// Specify the game's window procedure handler.
	//wc.lpfnWndProc = (WNDPROC) CGameStartup::WndProcHndl;
	//wc.cbClsExtra = 0;
	//wc.cbWndExtra = 0;
	//wc.hInstance = GetModuleHandle(0);
	//wc.hIcon = LoadIcon(GetModuleHandle(nullptr), IDI_WINLOGO);
	//wc.hCursor = LoadCursor(GetModuleHandle(nullptr), IDC_ARROW);
	//wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	//wc.lpszMenuName = 0;

	//// Specifies the game window's class name. Must be "CryENGINE". As CRYENGINE internally creates the game window using this class, so it must exist.
	//wc.lpszClassName = "CryENGINE";

	//// Register the game window class.
	//if (!RegisterClass(&wc))
	//{
	//	// Window registration failed.
	//	return false;
	//}

	// Window registration successful.
//	return true;
//}


CGameStartup::CGameStartup() :
	m_pGameFramework(nullptr),
	m_pGame(nullptr),
	m_GameRef(nullptr),
	m_bQuit(false),
	m_pCryActionDll(nullptr)
{
}


CGameStartup::~CGameStartup()
{
	// Shuts down the currently running game.
	if (m_pGame)
		m_pGame->Shutdown();
	m_pGame = nullptr;
	m_GameRef = nullptr;

	// No more listening for system events.
	GetISystem()->GetISystemEventDispatcher()->RemoveListener(this);

	// Shuts down the currently active IGameFramework (which in-turn shuts down other various CRYENGINE systems).
	if (m_pGameFramework)
	{
		m_pGameFramework->Shutdown();
		m_pGameFramework = nullptr;
	}

	// UnRegisters the game window class.
	// TODO: Double check this is needed now.
	//UnregisterClass("CryENGINE", GetModuleHandle(0));

	// Unload the CryAction library (DLL)
	if (m_pCryActionDll)
	{
		CryFreeLibrary(m_pCryActionDll);
		m_pCryActionDll = nullptr;
	}
}


IGameRef CGameStartup::Init(SSystemInitParams &startupParams)
{
	// Allows all CRYENGINE systems to access and know about our IGameStartup instance.
	startupParams.pGameStartup = this;

	// Sets the log file to use. Don't try to log before the call to m_pGameFramework->Init(startupParams) has occurred.
	startupParams.sLogFileName = ProjectName + ".log";

	// Initialize the game window class.
	//if (!InitWindow())
	//	return nullptr;

	// Gives us access to the CryAction library to create the IGameFramework (which in-turn creates other various CRYENGINE systems).
	m_pCryActionDll = CryLoadLibraryDefName("CryAction");
	if (!m_pCryActionDll)
		return nullptr;

	// Gets the CreateGameFramework() function from the CryAction library.
	IGameFramework::TEntryFunction CreateGameFramework = (IGameFramework::TEntryFunction) CryGetProcAddress(m_pCryActionDll, "CreateGameFramework");
	if (!CreateGameFramework)
	{
		CryFreeLibrary(m_pCryActionDll);
		return nullptr;
	}

	// Creates the IGameFramework.
	m_pGameFramework = CreateGameFramework();
	if (!m_pGameFramework)
	{
		CryFreeLibrary(m_pCryActionDll);
		return nullptr;
	}

	// Initializes the IGameFramework (which in-turn initializes other various CRYENGINE systems).
	if (!m_pGameFramework->Init(startupParams))
	{
		CryFreeLibrary(m_pCryActionDll);
		return nullptr;
	}

	CryLogAlways("Chrysalis: Init");

	// Used to sync global pointers between different modules (DLL's).
	ModuleInitISystem(m_pGameFramework->GetISystem(), ProjectName);

	// Sets the user data of the game window to this instance.
#ifdef WIN32
	SetWindowLongPtr(reinterpret_cast<HWND>(gEnv->pRenderer->GetHWND()), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
#endif

	// Allows all CRYENGINE systems to access and know about the ISystem instance.
	// Also signifies that it was created / initialized already and thus will prevent other systems from creating / initializing it.
	startupParams.pSystem = m_pGameFramework->GetISystem();

	//PluginManager::RememberStartupParams(startupParams);

	// Create a game instance and initialize it.
	static char pGameBuffer [sizeof(CGame)];
	m_pGame = new ((void*) pGameBuffer) CGame();
	if (!m_pGame->Init(m_pGameFramework))
	{
		CryFreeLibrary(m_pCryActionDll);
		return nullptr;
	}

	// Stores the new IGame instance into an IGame reference.
	m_GameRef = &m_pGame;

	// Finishes initialization of the IGameFramework (which in-turn finishes the initializes of other various CRYENGINE systems).
	if (!m_pGameFramework->CompleteInit())
	{
		CryFreeLibrary(m_pCryActionDll);
		return nullptr;
	}

	// Executes the command line if any.
	if (startupParams.bExecuteCommandLine)
		gEnv->pSystem->ExecuteCommandLine();

	// Register for system events.
	GetISystem()->GetISystemEventDispatcher()->RegisterListener(this);

	// Create the random seed value.
	GetISystem()->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_RANDOM_SEED, (UINT_PTR) gEnv->pTimer->GetAsyncTime().GetMicroSecondsAsInt64(), 0);

	//PluginManager::InitPluginsLast();

	// TODO: Init Steam. Check the SteamInit routine on ISystem. It looks like it needs to be implemented by the client - but not sure how as yet.

	// Return The IGameRef Instance.
	return m_GameRef;
}


void CGameStartup::Shutdown()
{
	// Deletes this instance.
	this->~CGameStartup();
}


int CGameStartup::Update(bool haveFocus, unsigned int updateFlags)
{
	//CRYPROFILE_SCOPE_PROFILE_MARKER("MT: MainLoop");

#if defined(JOBMANAGER_SUPPORT_PROFILING)
	gEnv->GetJobManager()->SetFrameStartTime(gEnv->pTimer->GetAsyncTime());
#endif

	int returnCode = 0;

	if (gEnv->pConsole)
	{
#if CRY_PLATFORM_WINDOWS
		if (gEnv && gEnv->pRenderer && gEnv->pRenderer->GetHWND())
		{
			bool focus = (::GetFocus() == gEnv->pRenderer->GetHWND());
			static bool focused = focus;
			if (focus != focused)
			{
				if (GetISystem()->GetISystemEventDispatcher())
				{
					GetISystem()->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_CHANGE_FOCUS, focus, 0);
				}
				focused = focus;
			}
		}
#endif
	}

	// Pass the update down to the actual game.
	if (m_pGame)
		returnCode = m_pGame->Update(haveFocus, updateFlags);

	if (!m_fullScreenCVarSetup && gEnv->pConsole)
	{
		ICVar *pVar = gEnv->pConsole->GetCVar("r_Fullscreen");
		if (pVar)
		{
			pVar->SetOnChangeCallback(FullScreenCVarChanged);
			m_fullScreenCVarSetup = true;
		}
	}

	return returnCode;
}


bool CGameStartup::GetRestartLevel(char** levelName)
{
	// Gets whether we are relaunching.
	if (GetISystem()->IsRelaunch())
	{
		// We are relaunching, so set the relaunch level (the level to load once relaunching is complete).
		*levelName = (char*) (gEnv->pGame->GetIGameFramework()->GetLevelName());
	}

	return GetISystem()->IsRelaunch();
}


const char* CGameStartup::GetPatch() const
{
	return nullptr;
}


bool CGameStartup::GetRestartMod(char* pModName, int nameLenMax)
{
	return false;
}


int CGameStartup::Run(const char * autoStartLevelName)
{
	// TODO: This message queue handler is suspect. I don't think it's handling unicode, and it might be
	// better to use the PumpWindowMessage from ISystem instead.

#if CRY_PLATFORM_WINDOWS
	for (;;)
	{
		MSG msg;

		// Must be PeekMessageW for Scaleform IME to function correctly and to ensure WM_CHAR contains
		// Unicode wide char for languages like Russian
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message != WM_QUIT)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				m_bQuit = true;
				break;
			}
		}
		else
		{
			// Update this instance while we are not processing any Windows messages.
			if (!Update(true, 0))
			{
				// Need to clean the message loop (WM_QUIT might cause problems in the case of a restart)
				// another message loop might have WM_QUIT already so we cannot rely only on this.

				// Must be PeekMessageW for Scaleform IME to function correctly and to ensure WM_CHAR contains
				// Unicode wide char for languages like Russian.
				while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				break;
			}
		}
	}

#else
	// We should use bVisibleByDefault=false then...
	if (gEnv && gEnv->pHardwareMouse)
		gEnv->pHardwareMouse->DecrementCounter();

#if !CRY_PLATFORM_DURANGO
	for (;;)
	{
		if (!Update(true, 0))
		{
			break;
		}
	}
#endif // !CRY_PLATFORM_DURANGO


#endif // CRY_PLATFORM_WINDOWS

	return 0;
}



void CGameStartup::FullScreenCVarChanged(ICVar *pVar)
{
	if (gEnv->pSystem->GetISystemEventDispatcher())
	{
		gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_TOGGLE_FULLSCREEN, pVar->GetIVal(), 0);
	}
}



//IGameRef CGameStartup::Reset()
//{
//	 TODO: Implement it like this way fro gamezero?
//
//	static char pGameBuffer [sizeof(CGame)];
//	m_pGame = new ((void*) pGameBuffer) CGame();
//
//	if (m_pGame->Init(m_pFramework))
//	{
//		return m_gameRef;
//	}
//
//	return nullptr;
//}


// ***
// *** ISystemEventListener
// ***

void CGameStartup::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		case ESYSTEM_EVENT_RANDOM_SEED:
			cry_random_seed(gEnv->bNoRandomSeed ? 0 : (uint32) wparam);
			break;

		case ESYSTEM_EVENT_CHANGE_FOCUS:
			GetISystem()->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_RANDOM_SEED, (UINT_PTR) gEnv->pTimer->GetAsyncTime().GetMicroSecondsAsInt64(), 0);
			break;

		case ESYSTEM_EVENT_LEVEL_LOAD_START:
			break;

		case ESYSTEM_EVENT_FAST_SHUTDOWN:
			m_bQuit = true;
			break;
	}
}



// ***
// *** IWindowMessageHandler
// ***

#if CRY_PLATFORM_WINDOWS

bool CGameStartup::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	switch (msg)
	{
		// Prevent ALT + key combinations from creating 'ding' sounds.
		case WM_SYSCHAR:
		{
			const bool bAlt = (lParam & (1 << 29)) != 0;
			if (bAlt && wParam == VK_F4)
			{
				// Pass though ALT+F4
				return false;
			}

			*pResult = 0;
			return true;
		}
		break;

		case WM_SIZE:
			break;

		case WM_SETFOCUS:
			if (g_pGameCVars)
				g_pGame->GetCVars()->cl_isClientFocusWindow = true;
			break;

		case WM_KILLFOCUS:
			if (g_pGameCVars)
				g_pGame->GetCVars()->cl_isClientFocusWindow = false;
			break;

		case WM_SETCURSOR:
		{
			// This is sample code to change the displayed cursor for Windows applications.
			// Note that this sample loads a texture (ie, .TIF or .DDS), not a .ICO or .CUR resource.
			IHardwareMouse* const pMouse = gEnv ? gEnv->pHardwareMouse : nullptr;
			CRY_ASSERT(pMouse && "HWMouse should be initialized before window is shown, check engine initialization order");

			const char* currentCursorPath = gEnv->pConsole->GetCVar("r_MouseCursorTexture")->GetString();
			const bool bResult = pMouse ? pMouse->SetCursor(currentCursorPath) : false;
			if (!bResult)
			{
				GameWarning("Unable to load cursor %s, does this file exist?", currentCursorPath);
			}

			*pResult = bResult ? true : false;
			return bResult;
		}
		break;
	}

	return false;
}

#endif
