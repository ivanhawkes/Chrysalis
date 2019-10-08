#include <StdAfx.h>

#include "Pet.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"


namespace Chrysalis
{
static void RegisterPetComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPetComponent));
		// Functions
		{
		}
	}
}


void CPetComponent::ReflectType(Schematyc::CTypeDesc<CPetComponent>& desc)
{
	desc.SetGUID("{03DCFDAE-6F31-41D3-9FF5-26F79BB04278}"_cry_guid);
	desc.SetEditorCategory("Actors");
	desc.SetLabel("Pet");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});
}


void CPetComponent::Initialize()
{
	// Get it into a known state.
	OnResetState();
}


void CPetComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		// Physicalize on level start for Launcher
	case EEntityEvent::LevelStarted:

		// Editor specific, physicalize on reset, property change or transform change
	case EEntityEvent::Reset:
	case EEntityEvent::EditorPropertyChanged:
	case EEntityEvent::TransformChangeFinishedInEditor:
		OnResetState();
		break;

	case EEntityEvent::Update:
		break;
	}
}


void CPetComponent::OnResetState()
{
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPetComponent)
}