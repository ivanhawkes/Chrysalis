#include <StdAfx.h>

#include "Mount.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"


namespace Chrysalis
{
static void RegisterMountComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CMountComponent));
		// Functions
		{
		}
	}
}


void CMountComponent::ReflectType(Schematyc::CTypeDesc<CMountComponent>& desc)
{
	desc.SetGUID("{19A8D24A-E321-4E28-BFD7-BD35CDF85C32}"_cry_guid);
	desc.SetEditorCategory("Actors");
	desc.SetLabel("Mount");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});
}


void CMountComponent::Initialize()
{
	// Get it into a known state.
	OnResetState();
}


void CMountComponent::ProcessEvent(const SEntityEvent& event)
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


void CMountComponent::OnResetState()
{
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterMountComponent)
}