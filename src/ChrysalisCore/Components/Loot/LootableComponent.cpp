#include <StdAfx.h>

#include "LootableComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"


namespace Chrysalis
{
static void RegisterLootableComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CLootableComponent));
		// Functions
		{
		}
	}
}


void CLootableComponent::ReflectType(Schematyc::CTypeDesc<CLootableComponent>& desc)
{
	desc.SetGUID(CLootableComponent::IID());
	desc.SetEditorCategory("Items");
	desc.SetLabel("Lootable");
	desc.SetDescription("Give an entity a loot table.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});

	desc.AddMember(&CLootableComponent::m_bActive, 'actv', "Active", "Active", "Is this active?", true);
	desc.AddMember(&CLootableComponent::m_lootTableEntry, 'ltbl', "LootTableEntry", "Loot Table Entry", "Entry on the loot table.", CLootableComponent::SLootTableEntry());
}


void CLootableComponent::OnResetState()
{
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterLootableComponent)
}