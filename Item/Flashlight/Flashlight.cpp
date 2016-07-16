#include <StdAfx.h>

#include "Flashlight.h"
#include <CryAction.h>
#include <GameXmlParamReader.h>
#include <CryString/CryFixedString.h>
#include <ISharedParamsManager.h>
#include <Game/Game.h>
#include <Entity/EntityScriptCalls.h>
#include <CryAnimation/IAttachment.h>


// ***
// *** IGameObjectExtension
// ***


void CFlashlight::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}


bool CFlashlight::Init(IGameObject * pGameObject)
{
	// Stores the specified IGameObject in this instance.
	SetGameObject(pGameObject);

	// TODO: don't want to load every init for every object - make this smarter.
	LoadFromXML();

	// TODO: more testing - doesn't belong here and should use the game cache for the load.
	m_fpGeomSlotId = GetEntity()->LoadGeometry(eIGS_FirstPerson, "Objects/chrysalis/items/flashlight/maglite_02d.cgf");
	//m_fpGeomSlotId = GetEntity()->LoadCharacter(eIGS_FirstPerson, "Objects/chrysalis/items/flashlight/maglite_02d.cdf");
	auto pCharacter = GetEntity()->GetCharacter(m_fpGeomSlotId);
	auto pStaticObj = GetEntity()->GetStatObj(m_fpGeomSlotId);
	SEntitySlotInfo slotInfo;
	GetEntity()->GetSlotInfo(m_fpGeomSlotId, slotInfo);
	int slotCount = GetEntity()->GetSlotCount();

	// Need to init effects controllers if we want to use one.
	GetEffectsController().Init(GetEntityId());

	// Initialization successful.
	return true;
}


void CFlashlight::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Allow this instance to be post-updated every frame.
	pGameObject->EnablePostUpdates(this);

	// Register for any extra events we want to handle.
	RegisterEvents();

	// Reset the entity.
	OnReset();
}


void CFlashlight::InitClient(int channelId)
{}


void CFlashlight::PostInitClient(int channelId)
{}


bool CFlashlight::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	ResetGameObject();

	// Register for any extra events we want to handle.
	RegisterEvents();

	return true;
}


void CFlashlight::PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{}


bool CFlashlight::GetEntityPoolSignature(TSerialize signature)
{
	return true;
}


void CFlashlight::Release()
{
	// Destroy this instance.
	delete this;
}


void CFlashlight::FullSerialize(TSerialize ser)
{}


bool CFlashlight::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags)
{
	return true;
}


ISerializableInfoPtr CFlashlight::GetSpawnInfo()
{
	return nullptr;
}


void CFlashlight::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	auto pEntity = GetEntity();
	if (!pEntity)
		return;
}


void CFlashlight::HandleEvent(const SGameObjectEvent& event)
{
	//switch (event.event)
	//{
	//	default:
	//		break;
	//}
}


void CFlashlight::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_SCRIPT_EVENT:
			OnScriptEvent(event);
			break;

		case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
			OnEditorPropertyChanged();
			break;

		case ENTITY_EVENT_RESET:
			OnReset();
			break;
	}
}


void CFlashlight::SetChannelId(uint16 id)
{}


void CFlashlight::SetAuthority(bool auth)
{}


void CFlashlight::PostUpdate(float frameTime)
{
	auto pEntity = GetEntity();
	if (!pEntity)
		return;
}


void CFlashlight::PostRemoteSpawn()
{}


// ***
// *** CFlashlight
// ***


CFlashlight::CFlashlight()
{}


CFlashlight::~CFlashlight()
{}


void CFlashlight::RegisterEvents()
{
	// Lists the game object events we want to be notified about.
	const int EventsToRegister [] =
	{
		eGFE_ScriptEvent			// Scripts events.
	};

	// Register for the specified game object events.
	GetGameObject()->UnRegisterExtForEvents(this, nullptr, 0);
	GetGameObject()->RegisterExtForEvents(this, EventsToRegister, sizeof(EventsToRegister) / sizeof(int));
}


void CFlashlight::OnScriptEvent(SEntityEvent& event)
{
	// NOTE: We're not really doing anything in this event handler at present, but I want to hold onto the sample code
	// below as a reminder of how to cast the events parameters to useful types. 

	//const char* eventName = reinterpret_cast<const char*> (event.nParam [0]);
	//IEntityClass::EventValueType eventValueType = static_cast<IEntityClass::EventValueType> (event.nParam [1]);
	//const void* pEventValue = reinterpret_cast<const void*> (event.nParam [2]);
}


void CFlashlight::OnEditorPropertyChanged()
{
	// HACK: shouldn't do it at this time, making life easier for testing for now. Remove this!
	LoadFromXML();

	SmartScriptTable pProperties;
	GetEntity()->GetScriptTable()->GetValue("Properties", pProperties);

	if (pProperties)
	{
		// Grab the values from the editor.
		// NOTE: At some point we need to be able to push these back into the editor as well.
		pProperties->GetValue("BatteryLevel", m_batteryLevel);
		pProperties->GetValue("IsSwitchedOn", m_isSwitchedOn);
		Switch(m_isSwitchedOn);
	}
}


void CFlashlight::OnReset()
{
	Physicalize(true, false);

	// Check to see if the light should be on, and switch it on if needed.
	Switch(m_isSwitchedOn);
}


void CFlashlight::GetSharedParameters(XmlNodeRef rootParams)
{
	m_itemFlashlightParameterShared = SharedParameters::GetSharedParameters<SItemFlashlightParameterShared>(
		rootParams, "item", GetEntity()->GetClass()->GetName(), "flashlight");
	m_dynamicLightParameterShared = SharedParameters::GetSharedParameters<SDynamicLight>(
		rootParams, "item", GetEntity()->GetClass()->GetName(), "dynamic_light");
}


void CFlashlight::LoadFromXML()
{
	// We use the class name to determine which XML file contains our parameters. The item system scan means it's
	// location is already known.
	auto pItemSystem = gEnv->pGame->GetIGameFramework()->GetIItemSystem();
	auto xmlFilename = pItemSystem->GetItemParamsDescriptionFile(GetEntity()->GetClass()->GetName());
	XmlNodeRef rootParams = gEnv->pSystem->LoadXmlFromFile(xmlFilename);

	if (rootParams)
	{
		CItem::GetSharedParameters(rootParams);
		GetSharedParameters(rootParams);
	}
	else
	{
		GameWarning("Item description file %s doesn't exist for item %s", xmlFilename, GetEntity()->GetClass()->GetName());
	}
}


void CFlashlight::ToggleSwitch()
{
	m_isSwitchedOn = !m_isSwitchedOn;
	Switch(m_isSwitchedOn);
}


void CFlashlight::Switch(bool isSwitchedOn)
{
	m_isSwitchedOn = isSwitchedOn;

	// TODO: There is a pre-amble of code that seems to be needed which creates a render proxy for the owner of this
	// light - where the owner *must* be derived from CEntity. Still not quite sure why it's doing this.
	auto pEntity = GetEntity();
	auto pRenderProxy = static_cast<IEntityRenderProxy*> (pEntity->GetProxy(ENTITY_PROXY_RENDER));

	// TODO: Determine slot based on FP / TP, etc.
	//int slot = ownerIsFP ? eIGS_FirstPerson : eIGS_ThirdPerson;
	//int slot = m_fpGeomSlotId;

	if (isSwitchedOn)
	{
		// TODO: THIS IS IDIOTIC - IT'S ADDING A NEW LIGHT EVERY TIME WE CHANGE A VALUE.
		// Problem is that switch is called each time a property changes and we're leaking resources.
		m_lightId = AttachLight(eIGS_ThirdPerson, "light_term", Vec3(IDENTITY), Vec3(-1.0f, 0.0f, 0.0f), eIGS_Last, m_dynamicLightParameterShared);
	}
	else
	{
		// Detach the light effect when the flashlight is switched off.
		if (m_lightId)
			DetachEffect(m_lightId);
		m_lightId = 0;
	}

	// TODO: We should update the script flag to indicate the new value.
}


bool CFlashlight::IsSwitchedOn()
{
	return m_isSwitchedOn;
}


void CFlashlight::Physicalize(bool enable, bool rigid)
{
	// Setup the physics parameters this instance should use.
	SEntityPhysicalizeParams PhysParams;

	// We are not using density, we are using mass.
	PhysParams.density = -1;
	PhysParams.mass = 1.5f;

	// All entity slots in this instance should be physicalized using these settings.
	PhysParams.nSlot = -1;

	// A flashlight should be rigid.
	PhysParams.type = PE_RIGID;

	// Actually physicalize this instance with the specified parameters.
	GetEntity()->Physicalize(PhysParams);
}