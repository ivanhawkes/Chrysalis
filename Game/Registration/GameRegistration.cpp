#include <StdAfx.h>

#include <Game/Registration/GameRegistration.h>

#include <CryGame/IGameFramework.h>
#include <IItemSystem.h>
#include <IVehicleSystem.h>
#include <IGameRulesSystem.h>
#include <CryGame/IGameVolumes.h>

#include <Game/Game.h>
#include <Game/Rules/GameRules.h>
#include <Flowgraph/FlowGameEntityNode.h>
#include <Actor/Player/Player.h>
#include <Actor/Character/Character.h>
#include <Actor/Mount/Mount.h>
#include <Actor/Pet/Pet.h>
#include <Environment/Water/WaterRipplesGenerator.h>
#include <Item/Flashlight/Flashlight.h>
#include <EntityInteraction/EntityInteraction.h>
#include <EntityInteraction/EntityAwareness.h>
#include <EntityInteraction/EntityLocking.h>
#include <Openable/Containers/ContainerExtension.h>
#include <Openable/Doors/DoorExtension.h>
#include <Openable/Keys/KeyExtension.h>
#include <Openable/Locks/LockExtension.h>
#include <Camera/CameraManager.h>
#include <Camera/ActionRPGCamera.h>
#include <Camera/FirstPersonCamera.h>
#include <Actor/Player/PlayerInput/PlayerInput.h>


// TODO: Move call to register cvars and commands to here.
#include <ConsoleCommands/ConsoleCommands.h>
#include <ConsoleVariables/ConsoleVariables.h>


// Static.
std::map<string, CGameEntityNodeFactory*> CGameRegistration::s_flowNodeFactories;


// ***
// *** Templates and helpers needed for registration.
// ***

void CGameRegistration::CreateScriptTables(SEntityScriptProperties& out, uint32 flags)
{
	out.pEntityTable = gEnv->pScriptSystem->CreateTable();
	out.pEntityTable->AddRef();

	out.pEditorTable = gEnv->pScriptSystem->CreateTable();
	out.pEditorTable->AddRef();

	out.pPropertiesTable = gEnv->pScriptSystem->CreateTable();
	out.pPropertiesTable->AddRef();

	if (flags & eGORF_InstanceProperties)
	{
		out.pInstancePropertiesTable = gEnv->pScriptSystem->CreateTable();
		out.pInstancePropertiesTable->AddRef();
	}
}


template<class T>
struct CObjectCreator : public IGameObjectExtensionCreatorBase
{
	IGameObjectExtensionPtr Create()
	{
		return ComponentCreate_DeleteWithRelease<T>();
	}

	void GetGameObjectExtensionRMIData(void** ppRMI, size_t* nCount)
	{
		T::GetGameObjectExtensionRMIData(ppRMI, nCount);
	}
};


template<class T>
void CGameRegistration::RegisterGameObject(const string& name, const string& script, uint32 flags)
{
	bool registerClass = ((flags & eGORF_NoEntityClass) == 0);
	IEntityClassRegistry::SEntityClassDesc clsDesc;
	clsDesc.sName = name.c_str();
	clsDesc.sScriptFile = script.c_str();

	static CObjectCreator<T> _creator;

	gEnv->pGame->GetIGameFramework()->GetIGameObjectSystem()->RegisterExtension(name.c_str(), &_creator, registerClass ? &clsDesc : nullptr);
	T::SetExtensionId(gEnv->pGame->GetIGameFramework()->GetIGameObjectSystem()->GetID(name.c_str()));

	if ((flags & eGORF_HiddenInEditor) != 0)
	{
		IEntityClass* pEntityClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(name.c_str());
		pEntityClass->SetFlags(pEntityClass->GetFlags() | ECLF_INVISIBLE);
	}
}


template<class T>
void CGameRegistration::RegisterNoScriptGameObject(const string& name, const string& editorPath, uint32 flags)
{
	SEntityScriptProperties props;
	CGameFactory::CreateScriptTables(props, flags);
	gEnv->pScriptSystem->SetGlobalValue(name.c_str(), props.pEntityTable);

	CGameEntityNodeFactory* pNodeFactory = new CGameEntityNodeFactory();
	T::RegisterProperties(props, pNodeFactory);

	if (!editorPath.empty())
	{
		props.pEditorTable->SetValue("EditorPath", editorPath.c_str());
	}

	props.pEntityTable->SetValue("Editor", props.pEditorTable);
	props.pEntityTable->SetValue("Properties", props.pPropertiesTable);

	if (flags & eGORF_InstanceProperties)
	{
		props.pEntityTable->SetValue("PropertiesInstance", props.pInstancePropertiesTable);
	}

	string nodeName = "entity:";
	nodeName += name.c_str();
	pNodeFactory->Close();
	s_flowNodeFactories [nodeName] = pNodeFactory;

	const bool registerClass = ((flags & eGORF_NoEntityClass) == 0);
	IEntityClassRegistry::SEntityClassDesc clsDesc;
	clsDesc.sName = name.c_str();
	clsDesc.pScriptTable = props.pEntityTable;

	static CObjectCreator<T> _creator;

	gEnv->pGame->GetIGameFramework()->GetIGameObjectSystem()->RegisterExtension(name.c_str(), &_creator, registerClass ? &clsDesc : nullptr);
	T::SetExtensionId(gEnv->pGame->GetIGameFramework()->GetIGameObjectSystem()->GetID(name.c_str()));

	if ((flags & eGORF_HiddenInEditor) != 0)
	{
		IEntityClass* pEntityClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(name.c_str());
		pEntityClass->SetFlags(pEntityClass->GetFlags() | ECLF_INVISIBLE);
	}
}


template<class T>
void CGameRegistration::RegisterFactory(IGameFramework *pFramework, const string& className, bool isAi)
{
	pFramework->RegisterFactory(className, static_cast<T*> (nullptr), isAi, static_cast<T*> (nullptr));
}



// ***
// *** Registrations
// ***


// Register the factory templates used to create classes from names. Called via CGame::Init()
void CGameRegistration::RegisterGameObjects(IGameFramework *pFramework)
{
	CRY_ASSERT(pFramework);

	// ***
	// *** Players and characters.
	// ***

	// Register player related components.
	RegisterGameObject<CPlayer>("Player", "", eGORF_HiddenInEditor);
	RegisterGameObject<CPlayerInput>("PlayerInput", "", eGORF_HiddenInEditor);
	RegisterGameObject<CCameraManager>("CameraManager", "", eGORF_HiddenInEditor);
	RegisterGameObject<CActionRPGCamera>("ActionRPGCamera", "", eGORF_HiddenInEditor);
	RegisterGameObject<CFirstPersonCamera>("FirstPersonCamera", "", eGORF_HiddenInEditor);

	// Registers the character, pet and mount actors.
	RegisterGameObject<CCharacter>("Character", "Scripts/Entities/AI/Character.Lua", eGORF_None);
	RegisterGameObject<CMount>("Mount", "Scripts/Entities/AI/Mount.Lua", eGORF_None);
	RegisterGameObject<CPet>("Pet", "Scripts/Entities/AI/Pet.Lua", eGORF_None);

	// Interactive entities.
	RegisterGameObject<CEntityInteraction>("EntityInteraction", "", eGORF_HiddenInEditor);


	// ***
	// *** Items
	// ***

	//REGISTER_FACTORY(pFramework, "Item", CItem, false);
	//REGISTER_FACTORY(pFramework, "Accessory", CAccessory, false);

	//REGISTER_FACTORY(pFramework, "Flashlight", CFlashlight, false);
	//RegisterGameObject<CFlashlight>("Flashlight", "Scripts/Entities/Items/Flashlight.lua", eGORF_None);
	RegisterFactory<CFlashlight>(pFramework, "Flashlight", false);


	// ***
	// *** Various GameObjecs
	// ***
	RegisterFactory<CEntityAwareness>(pFramework, "EntityAwareness", false);
	RegisterFactory<CEntityLocking>(pFramework, "EntityLocking", false);


	// ***
	// *** Doors / Containers / Locks / Keys
	// ***
	RegisterFactory<CContainerExtension>(pFramework, "ContainerExtension", false);
	RegisterFactory<CDoorExtension>(pFramework, "DoorExtension", false);
	RegisterFactory<CKeyExtension>(pFramework, "KeyExtension", false);
	RegisterFactory<CLockExtension>(pFramework, "LockExtension", false);


	// ***
	// *** Weapons
	// ***

	//REGISTER_FACTORY(pFramework, "Weapon", CWeapon, false);


	// ***
	// *** Vehicle objects
	// ***

	IVehicleSystem* pVehicleSystem = pFramework->GetIVehicleSystem();

	//REGISTER_VEHICLEOBJECT("Burn", CVehicleDamageBehaviorBurn);


	// ***
	// *** Vehicle movements
	// ***

	//REGISTER_FACTORY(pVehicleSystem, "DummyMovement", CVehicleMovementDummy, false);
	//REGISTER_FACTORY(pVehicleSystem, "StdBoat", CVehicleMovementStdBoat, false);


	// ***
	// *** Custom GameObjects
	// ***

	//REGISTER_GAME_OBJECT(pFramework, Rain, "Scripts/Entities/Environment/Rain.lua");


	// ***
	// *** Shape / Volume objects
	// ***

	//REGISTER_GAME_OBJECT(pFramework, MPPath, "Scripts/Entities/Multiplayer/MPPath.lua");
	//HIDE_FROM_EDITOR("MPPath");
	//REGISTER_EDITOR_VOLUME_CLASS( pFramework, "MPPath" );

	//REGISTER_GAME_OBJECT(pFramework, LedgeObject, "Scripts/Entities/ContextualNavigation/LedgeObject.lua");
	//HIDE_FROM_EDITOR("LedgeObject");
	//REGISTER_EDITOR_VOLUME_CLASS( pFramework, "LedgeObject" );

	//REGISTER_GAME_OBJECT(pFramework, LedgeObjectStatic, "Scripts/Entities/ContextualNavigation/LedgeObjectStatic.lua");
	//HIDE_FROM_EDITOR("LedgeObjectStatic");
	//REGISTER_EDITOR_VOLUME_CLASS( pFramework, "LedgeObjectStatic" );

	//REGISTER_GAME_OBJECT(pFramework, WaterPuddle, "Scripts/Entities/Environment/WaterPuddle.lua");
	//HIDE_FROM_EDITOR("WaterPuddle");
	//REGISTER_EDITOR_VOLUME_CLASS(pFramework, "WaterPuddle");


	// ***
	// *** Water
	// ***

	// TODO: Put this back in when you sort out a decent set of registration macros.
	//REGISTER_GAME_OBJECT(pFramework, WaterRipplesGenerator, CWaterRipplesGenerator, Environment, false, "Scripts/Entities/Environment/WaterRipplesGenerator.lua");


	// ***
	// *** GameRules
	// ***

	// Registers The GameRules GameObject Extension. Needed for EVERY CRYENGINE game.
	RegisterGameObject<CGameRules>("GameRules", "", eGORF_HiddenInEditor);

	// Registers The SinglePlayer GameRules With The GameRules System. Needed For EVERY CRYENGINE Game.
	//pFramework->GetIGameRulesSystem()->RegisterGameRules("SinglePlayer", "GameRules");
	//pFramework->GetIGameRulesSystem()->AddGameRulesAlias("SinglePlayer", "sp");
	pFramework->GetIGameRulesSystem()->RegisterGameRules("SoloExploration", "GameRules");
	pFramework->GetIGameRulesSystem()->AddGameRulesAlias("SoloExploration", "SE");

	// Registers The MultiPlayer GameRules With The GameRules System. Needed For EVERY CRYENGINE Game.
	pFramework->GetIGameRulesSystem()->RegisterGameRules("MultiPlayer", "GameRules");
	pFramework->GetIGameRulesSystem()->AddGameRulesAlias("MultiPlayer", "mp");
}


void CGameRegistration::RegisterEntityFlowNodes()
{
	IFlowSystem* pFlowSystem = gEnv->pGame->GetIGameFramework()->GetIFlowSystem();
	std::map<string, CGameEntityNodeFactory*>::iterator it = s_flowNodeFactories.begin(), end = s_flowNodeFactories.end();
	for (; it != end; ++it)
	{
		pFlowSystem->RegisterType(it->first.c_str(), it->second);
	}

	stl::free_container(s_flowNodeFactories);
}
