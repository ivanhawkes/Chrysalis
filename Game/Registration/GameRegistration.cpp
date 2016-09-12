#include <StdAfx.h>

#include <Game/Registration/GameRegistration.h>

#include <CryGame/IGameFramework.h>
#include <IItemSystem.h>
#include <IVehicleSystem.h>
#include <IGameRulesSystem.h>
#include <CryGame/IGameVolumes.h>

#include <Game/Game.h>
#include <Game/GameRules.h>
#include <FlowNodes/Helpers/FlowGameEntityNode.h>
#include <Player/Player.h>
#include <Actor/Character/Character.h>
#include <Actor/Mount/Mount.h>
#include <Actor/Pet/Pet.h>
#include <Entities/Environment/Water/WaterRipplesGenerator.h>
#include <Item/Flashlight/FlashlightComponent.h>
#include <Player/Input/PlayerInputComponent.h>


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
	RegisterGameObject<CPlayerInputComponent>("PlayerInput", "", eGORF_HiddenInEditor);

	// Registers the character, pet and mount actors.
	RegisterGameObject<CCharacter>("Character", "scripts/Entities/AI/Character.Lua", eGORF_None);
	//RegisterGameObject<CMount>("Mount", "Scripts/Entities/AI/Mount.Lua", eGORF_None);
	//RegisterGameObject<CPet>("Pet", "Scripts/Entities/AI/Pet.Lua", eGORF_None);

	// ***
	// *** Items
	// ***

	//REGISTER_FACTORY(pFramework, "Item", CItem, false);
	//REGISTER_FACTORY(pFramework, "Accessory", CAccessory, false);

	//REGISTER_FACTORY(pFramework, "Flashlight", CFlashlightComponent, false);
	//RegisterGameObject<CFlashlightComponent>("Flashlight", "Scripts/Entities/Items/Flashlight.lua", eGORF_None);
	RegisterFactory<CFlashlightComponent>(pFramework, "Flashlight", false);

	// ***
	// *** Weapons
	// ***

	//REGISTER_FACTORY(pFramework, "Weapon", CWeapon, false);


	// ***
	// *** Vehicle objects
	// ***

	IVehicleSystem* pVehicleSystem = pFramework->GetIVehicleSystem();
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
