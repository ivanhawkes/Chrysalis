#include "StdAfx.h"

#include "DRSInteractionEntity.h"
#include <Components/Interaction/DRSInteractionComponent.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>


namespace Chrysalis
{
void CDRSInteractionEntity::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CDRSInteractionEntity::ReflectType(Schematyc::CTypeDesc<CDRSInteractionEntity>& desc)
{
	desc.SetGUID(CDRSInteractionEntity::IID());
	desc.SetEditorCategory("Interaction");
	desc.SetLabel("DRSInteractionEntity");
	desc.SetDescription("Dynamic Reponse System interaction entity.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::None });
}


void CDRSInteractionEntity::Initialize()
{
	GetEntity()->GetOrCreateComponent<CDRSInteractionComponent>();
	GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();

	// Reset the entity.
	OnResetState();
}


void CDRSInteractionEntity::OnResetState()
{
}
}