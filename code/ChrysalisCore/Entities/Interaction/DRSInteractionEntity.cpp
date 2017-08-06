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
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


void CDRSInteractionEntity::Initialize()
{
	GetEntity()->CreateComponent<CDRSInteractionComponent>();
	GetEntity()->CreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();

	// Reset the entity.
	OnResetState();
}


void CDRSInteractionEntity::OnResetState()
{
}
}