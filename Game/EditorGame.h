#pragma once

#include <CrySandbox/IEditorGame.h>
#include <CryGame/IGameRef.h>

struct IGameStartup;


/**
An Implementation Of The IEditorGame Interface. Used To Allow The CRYENGINE Sandbox Editor To Interact With A
CRYENGINE Game (GameDLL). Every CRYENGINE Game NEEDS To Have An Implementation Of The IEditorGame Interface If It
Wants To Be Able To Be Playable In The CRYENGINE Sandbox Editor.

\sa	IEditorGame
*/
class CEditorGame : public IEditorGame
{
public:

	/**
	This Instance's Default Constructor.
	*/
	CEditorGame();


	/**
	This Instance's 1 Param Constructor. Used To Construct This Instance From The Specified Parameter.

	\param	pBinDir	The bin dir.
	*/
	CEditorGame(const char* pBinDir);


	/**
	This Instance's Default Destructor.
	*/
	virtual ~CEditorGame();


	// ***
	// *** IEditorGame
	// ***


	/**
	Automatically Called By The Sandbox Editor To Initialize This Instance. Used To Create And Initialize An
	IGameStartup Instance, Initialize The IGameFramework Along With The Sandbox Editor, And Start A Game Context.

	\param [in,out]	pSystem				The ISystem Interface Used By The Sandbox Editor.
	\param [in,out]	pEditorInterface	The Interface To Use To Communicate With The Sandbox Editor.

	\return	True If Initialization Was Successful. False Otherwise.
	*/
	virtual bool Init(ISystem *pSystem, IGameToEditorInterface *pEditorInterface) override;


	/**
	Automatically Called By The Sandbox Editor To Update This Instance. Used To Update The IGameStartup Instance
	(Which In-Turn Updates The Game).

	\param	haveFocus  	Specifies Whether The Sandbox Editor Window Currently Has Input Focus Or Not.
	\param	updateFlags	The Update Flags That Are Active During This Frame (Usually 0).

	\return	0 If This Instance Should Quit. Non-Zero Otherwise.
	*/
	virtual int Update(bool haveFocus, unsigned int updateFlags) override;


	/**
	Shuts Down This Instance And The IGameStartup Instance (Which In Turn Shuts Down The Game).
	*/
	virtual void Shutdown() override;


	/**
	Automatically Called By The Sandbox Editor When We Enter Or Exit "Game Mode".

	\param	bGameMode	Specifies Whether We Are Entering Or Exiting Editor "Game Mode".

	\return	False If Editor "Game Mode" Could Not Be Changed. True Otherwise.
	*/
	virtual bool SetGameMode(bool bGameMode) override;


	/**
	Gets The Local Player.

	\return	The Local Player.
	*/
	virtual IEntity* GetPlayer() override;


	/**
	Automatically Called By The Sandbox Editor When It Wants To Set The Player's Position And Rotation At The Same
	Time (Usually When The Player Starts Game Mode.) It's also called when flying through the map in the editor.

	\param	pos	   	The Camera Position.
	\param	viewDir	The Camera Rotation.
	*/
	virtual void SetPlayerPosAng(Vec3 pos, Vec3 viewDir) override;


	/**
	Hides Or Shows The Local Player.

	\param	bHide	Specifies Whether To Hide Or Show The Local Player. True To Hide, False To Show.
	*/
	virtual void HidePlayer(bool bHide) override;


	/**
	Automatically Called By The Sandbox Editor? Right Before A Level Starts To Load.
	*/
	virtual void OnBeforeLevelLoad() override;


	/**
	Automatically Called By The Sandbox Editor? Right After A Level Has Been Loaded.

	\param	levelName  	The Name Of The Level That Has Been Loaded.
	\param	levelFolder	The Path That The Specified Level Was Loaded From.
	*/
	virtual void OnAfterLevelLoad(const char *levelName, const char *levelFolder) override;


	/**
	Executes the after level initialise action.
	
	\param	levelName  	Name of the level.
	\param	levelFolder	Pathname of the level folder.
	*/
	virtual void OnAfterLevelInit(const char *levelName, const char *levelFolder) override;


	/**
	Automatically Called By The Sandbox Editor? Right After A Level Has Been UnLoaded.
	*/
	virtual void OnCloseLevel() override;


	/**
	Called when the level is being saved.
	*/
	virtual void OnSaveLevel() override;


	/**
	Automatically Called By The Sandbox Editor When The Game Is Being Exported For "Pure" Game Mode. Builds A List
	Of All The Entities That Are To Be Serialized And Exported To "Pure" Game Mode For The Specified Level.

	\param	output	The XmlNodeRef To Write The List To.

	\return	True If Building The Serialization List Was Successful. False Otherwise.
	*/
	virtual bool BuildEntitySerializationList(XmlNodeRef output) override;


	/**
	Automatically Called By The Sandbox Editor? To Allow The Game To Add Game-Specific Mini-Map Data To The Exported
	Mini-Map File.

	\param	output	The XmlNodRef To Write The Mini-Map Data To.

	\return	True If Mini-Map Data Was Successfully Added. False Otherwise.
	*/
	virtual bool GetAdditionalMinimapData(XmlNodeRef output) override;


	/**
	Gets The IFlowSystem Interface.

	\return	The IFlowSystem Interface.
	*/
	virtual IFlowSystem * GetIFlowSystem() override;


	/**
	Gets The IGameTokenSystem Interface.

	\return	The IGameTokenSystem Interface.
	*/
	virtual IGameTokenSystem* GetIGameTokenSystem() override;


	/**
	Gets The IEquipmentSystemInterface Interface.

	\return	The IEquipmentSystemInterface Interface.
	*/
	virtual IEquipmentSystemInterface* GetIEquipmentSystemInterface() override;


	/**
	Automatically Called By The Sandbox Editor When It Wants To Determine If MultiPlayer Game Rules Are Supported Or
	Not.

	\return	True If MultiPlayer Game Rules Are Supported. False Otherwise.
	*/
	virtual bool SupportsMultiplayerGameRules() override;


	/**
	Automatically Called By The Sandbox Editor When It Requests To Switch The Currently Active
	Game Mode.
	*/
	virtual void ToggleMultiplayerGameRules() override;


	/**
	Registers The Specified ITelemetryRepository.

	\param [in,out]	pRepository	The ITelemetryRepository To Register.
	*/
	virtual void RegisterTelemetryTimelineRenderers(Telemetry::ITelemetryRepository* pRepository) override;


	/**
	Update (and render) all sorts of generic editor 'helpers' that could be used, for example, to render certain
	metrics, boundaries, invalid links, etc.
	
	\param	drawMode	The draw mode.
	*/
	virtual void UpdateHelpers(const HelpersDrawMode::EType drawMode) override {};


	/**
	Automatically Called By The Sandbox Editor? After The Renderer Has Rendered A Frame.

	\param	displayHelpers	Specifies Whether To Create And Render Helpers Or Not.
	*/
	virtual void OnDisplayRenderUpdated(bool displayHelpers) override;


	/**
	Executes the entity selection changed action.
	
	\param	entityId  	Identifier for the entity.
	\param	isSelected	true if this object is selected.
	*/
	virtual void OnEntitySelectionChanged(EntityId entityId, bool isSelected) override {}


	/**
	Executes the reload scripts action.
	
	\param	scriptsType	Type of the scripts.
	*/
	virtual void OnReloadScripts(EReloadScriptsType scriptsType) override {}

	
	// ***
	// *** ~IEditorGame
	// ***


private:

	/**
	Automatically Called By The Sandbox Editor. Sometimes The Sandbox Editor Needs To Reset The Client (Like When It
	Reloads All Scripts For Example). When This Occurs This Method Is Called. Used To Recreate The Needed Game Rules.

	The Editor Resets The Client Because It Received The "net_reseteditorclient" Console Command.

	\param [in,out]	pConsoleCommandArgs	The Console Command Arguments That Were Passed To The "net_reseteditorclient"
	Console Command.
	*/
	static void ResetClient(IConsoleCmdArgs* pConsoleCommandArgs);


	/**
	Automatically Called By The IConsoleSystem When The "net_gamemode" Console Variable Changes. Used To
	Enable/Disable Editor "Game Mode".

	\param [in,out]	pCVar	The Console Variable That Was Changed ("net_gamemode").
	*/
	static void OnChangeEditorGameMode(ICVar* pCVar);


	/**
	Enables Or Disabled A Networked Game Context. Whenever A "Game Session" Should Start,  This Means We Should
	Enable A Game Context.

	\param	bEnable	Specifies Whether To Enable Or Disable A Networked Game Context.

	\return	True If Changing The Game Context Was Successful. False Otherwise.
	*/
	bool EnableNetContext(bool bEnable);


	/**
	Enables Or Disables The Local Player (Spawns Or Removes The Player).

	\param	bEnable	Specifies Whether To Enable Or Disable The Local Player.
	*/
	void EnablePlayer(bool bEnable);


	/**
	Used To Start A CRYENGINE Game. Takes Care Of Initializing All Of CRYENGINE's Systems, Handling Window Events,
	Running The Application Loop, And Creating, Initializing, And Updating A CRYENGINE Game.
	*/
	IGameStartup* m_pGameStartup;


	/**
	The IGame Interface Reference. Simply References The IGame Interface.
	*/
	IGameRef m_GameRef;


	/**
	Specifies Whether A Networked Game Context Is Currently Active Or Not.
	*/
	bool m_bNetContext;


	/**
	Specifies Whether We Are Currently In Editor "Game Mode" Or Not.
	*/
	bool m_bGameMode;


	/**
	Specifies Whether The Local Player Is Currently Active Or Not (Whether The Player Was Spawned And Still Active).
	*/
	bool m_bPlayer;


	/**
	Specifies Whether We Are Using MultiPlayer Game Rules Or Not.
	*/
	bool m_bMultiPlayerGameRules;

	// TODO: Add implmentation of IEquipmentSystemInterface. (member)
};

