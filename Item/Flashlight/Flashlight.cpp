#include <StdAfx.h>

#include "Flashlight.h"
#include <CryAction.h>
#include <GameXmlParamReader.h>
#include <CryString/CryFixedString.h>
#include <ISharedParamsManager.h>
#include <Game/Game.h>
#include <Entity/EntityScriptCalls.h>
#include <CryAnimation/IAttachment.h>
#include <EntityInteraction/EntityInteraction.h>


// ***
// *** IGameObjectExtension
// ***


void CFlashlight::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
	pSizer->AddObject(m_interactor);
}


bool CFlashlight::Init(IGameObject * pGameObject)
{
	// Stores the specified IGameObject in this instance.
	SetGameObject(pGameObject);

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

	// We want to supply interaction verbs.
	m_interactor = static_cast<IEntityInteraction*> (GetGameObject()->AcquireExtension("EntityInteraction"));
	if (m_interactor)
	{
		auto switchOnInteractPtr = std::make_shared<CInteractionSwitchOn>(this);
		m_interactor->AddInteraction(switchOnInteractPtr);

		auto switchOffInteractPtr = std::make_shared<CInteractionSwitchOff>(this);
		m_interactor->AddInteraction(switchOffInteractPtr);

		auto inspectInteractPtr = std::make_shared<CInteractionInspect>(this);
		m_interactor->AddInteraction(inspectInteractPtr);

		auto interactPtr = std::make_shared<CInteractionInteract> (this);
		m_interactor->AddInteraction(interactPtr);

		auto pickupInteractPtr = std::make_shared<CInteractionPickup>(this);
		m_interactor->AddInteraction(pickupInteractPtr);

		auto dropInteractPtr = std::make_shared<CInteractionDrop>(this);
		m_interactor->AddInteraction(dropInteractPtr);
	}

	// TODO: don't want to load every init for every object - make this smarter.
	LoadFromXML();

	// TODO: more testing - doesn't belong here and should use the game cache for the load.
	//m_fpGeomSlotId = GetEntity()->LoadGeometry(eIGS_FirstPerson, "objects/items/flashlight/maglite_02d.cgf");
	//m_fpGeomSlotId = GetEntity()->LoadCharacter(eIGS_FirstPerson, "objects/items/flashlight/maglite_02d.cdf");
	m_fpGeomSlotId = GetEntity()->LoadCharacter(eIGS_FirstPerson, "objects/FlashLight/FlashLight_Setup_v1/FlashLight.cdf");
	auto pCharacter = GetEntity()->GetCharacter(m_fpGeomSlotId);
	auto pStaticObj = GetEntity()->GetStatObj(m_fpGeomSlotId);
	SEntitySlotInfo slotInfo;
	GetEntity()->GetSlotInfo(m_fpGeomSlotId, slotInfo);
	int slotCount = GetEntity()->GetSlotCount();

	// Need to init effects controllers if we want to use one.
	GetEffectsController().Init(GetEntityId());

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
{
	if (m_interactor)
		GetGameObject()->ReleaseExtension("EntityInteraction");
}


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

	//const char* eventName = static_cast<const char*> (event.nParam [0]);
	//IEntityClass::EventValueType eventValueType = static_cast<IEntityClass::EventValueType> (event.nParam [1]);
	//const void* pEventValue = static_cast<const void*> (event.nParam [2]);
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
		
		// HACK: Looking at moving away from editor properties like this...
		//Switch(m_isSwitchedOn);
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
		// If we don't appear to have a light effect, then it's time to make one and attach it.
		if (m_lightId == EntityEffects::EFFECTID_INVALID)
			m_lightId = AttachLight(eIGS_FirstPerson, "light_term", Vec3(ZERO), Vec3(0.0f, 1.0f, 0.0f), eIGS_Last, m_dynamicLightParameterShared);
			//m_lightId = AttachLight(eIGS_ThirdPerson, "light_term", Vec3(ZERO), Vec3(-1.0f, 0.0f, 0.0f), eIGS_Last, m_dynamicLightParameterShared);
	}
	else
	{
		// Detach the light effect when the flashlight is switched off.
		if (m_lightId)
			DetachEffect(m_lightId);
		m_lightId = EntityEffects::EFFECTID_INVALID;
	}

	// TODO: We should update the script flag to indicate the new value.
}


bool CFlashlight::IsSwitchedOn()
{
	return m_isSwitchedOn;
}


void CFlashlight::Physicalize(bool enable, bool rigid)
{
	// TODO: We are not making good use of pe_player_dimensions yet.

	// We are going to be physicalizing this instance (the player), so setup the physics dimensions to use for physics calculations.
	// See http://docs.cryengine.com/display/SDKDOC4/Physics+System+Programming
	pe_player_dimensions PDim;

	// We want a capsule, not a cylinder.
	PDim.bUseCapsule = 0;

	// Specifies the z-coordinate of a point in the entity frame that is considered to be at the feet level (usually 0).
	PDim.heightPivot = 0.0f;

	// Added recently, testing if they work.
	PDim.heightCollider = 0.0f;
	PDim.heightEye = 0.0f;
	PDim.type = PE_RIGID;
	PDim.sizeCollider = Vec3(0.20f, 0.20f, 0.20f);

	// We are going to be physicalizing this instance, so setup the physics simulation parameters to use for physics calculations.
	pe_player_dynamics PDyn;

	// Setting bActive to false puts the living entity to a special 'inactive' state where it does not check collisions
	// with the environment and only moves with the requested velocity (other entities can still collide with it,
	// though; note that this applies only to the entities of the same or higher simulation classes). 
	PDyn.bActive = true;

	// TODO: See why rigid is recommended for this.
	PDyn.type = PE_RIGID;

	// A flag that indicates if the entity is allowed to attempt to move in all directions (gravity might still pull it
	// down though). If not set, the requested velocity will always be projected on the ground if the entity is not
	// flying. 
	PDyn.bSwimming = false;

	// We should collide with all entity types and areas/triggers.
	PDyn.collTypes = ent_all | ent_areas | ent_triggers;

	// We should use earth's normal gravity on this instance (the player).
	PDyn.gravity = Vec3(0.0f, 0.0f, -9.81f);

	// We should have almost all control while in the air.
	PDyn.kAirControl = 0.9f;

	// We should have relatively low air resistance.
	PDyn.kAirResistance = 0.2f;

	// We should have a standard amount of inertia.
	PDyn.kInertia = 8.0f;

	// We should have a standard amount of acceleration.
	PDyn.kInertiaAccel = 11.0f;

	// We should weigh x kilograms.
	PDyn.mass = 1.5f;

	// We should not be able to climb up hills that are steeper then 50 degrees.
	PDyn.maxClimbAngle = 50;

	// We should not be able to jump down-hill while on hills that are steeper then 50 degrees.
	PDyn.maxJumpAngle = 50;

	// We should start falling down-hill if we are on hills steeper then 50 degrees.
	PDyn.minFallAngle = 50;

	// We should start sliding down-hill if we are on hills steeper then 45 degrees.
	PDyn.minSlideAngle = DEG2RAD(45);

	// Player cannot stand on ground moving faster than this.
	PDyn.maxVelGround = 16.0f;

	// Sets the strength of camera reaction to landings.
	PDyn.nodSpeed = 60.0f;

	// Makes the entity allocate a much longer movement history array which might be required for synchronization (if
	// not set, this array will be allocated the first time network-related actions are requested, such as performing a
	// step back).
	PDyn.bNetwork = true;

	// Setup the physics parameters this instance (the player) should use.
	SEntityPhysicalizeParams PhysParams;

	// Animated character will need the poststep.
	PhysParams.nFlagsOR = pef_log_poststep;

	// We are not using density, we are using mass.
	PhysParams.density = -1;

	// We should weigh x kilograms. (doesn't matter because this value will come from the pe_player_dynamics).
	PhysParams.mass = PDyn.mass;

	// All entity slots in this instance should be physicalized using these settings.
	PhysParams.nSlot = -1;

	// Sets the player dimensions.
	PhysParams.pPlayerDimensions = &PDim;

	// Sets the player dynamics.
	PhysParams.pPlayerDynamics = &PDyn;

	// We are going to use "living" physics type, hence the use of pe_player_dimensions and pe_player_dynamics.
	PhysParams.type = PE_LIVING;

	// We should use standard human joint springiness.
	PhysParams.fStiffnessScale = 73.0f;

	// Actually physicalize this instance (the player) with the specified parameters.
	GetEntity()->Physicalize(PhysParams);


	// //TODO: DO WE HAVE TO USE LIVING PHYSICS IF IT'S A CHARACTER DEF FILE?
	// //RIGID VERSION BELOW.

	//// Setup the physics parameters this instance should use.
	//SEntityPhysicalizeParams PhysParams;

	//// We are not using density, we are using mass.
	//PhysParams.density = -1;
	//PhysParams.mass = 1.5f;

	//// All entity slots in this instance should be physicalized using these settings.
	//PhysParams.nSlot = -1;

	//// A flashlight should be rigid.
	//PhysParams.type = PE_RIGID;

	//// Actually physicalize this instance with the specified parameters.
	//GetEntity()->Physicalize(PhysParams);
}