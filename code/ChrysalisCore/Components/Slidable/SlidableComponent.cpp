#include <StdAfx.h>

#include "SlidableComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include "CrySerialization/Enum.h"


namespace Chrysalis
{
SERIALIZATION_ENUM_BEGIN(SlideState, "Slide State")
SERIALIZATION_ENUM(SlideState::initialPosition, "initialPosition", "initialPosition")
SERIALIZATION_ENUM(SlideState::inMotion, "inMotion", "inMotion")
SERIALIZATION_ENUM(SlideState::finalPosition, "finalPosition", "finalPosition")
SERIALIZATION_ENUM_END()


static void RegisterSlidableComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CSlidableComponent));
		
		// Signals.
		componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CSlidableComponent::SStateInitialSignal));
		componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CSlidableComponent::SStateInMotionSignal));
		componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CSlidableComponent::SStateFinalSignal));

		// Functions
		{
		}
	}
}


static void ReflectType(Schematyc::CTypeDesc<CSlidableComponent::SStateInitialSignal>& desc)
{
	desc.SetGUID("{BE365365-3DDA-43C7-AE43-28E9BCA0AA47}"_cry_guid);
	desc.SetLabel("State Initial");
}


static void ReflectType(Schematyc::CTypeDesc<CSlidableComponent::SStateInMotionSignal>& desc)
{
	desc.SetGUID("{3878F386-4A29-4A90-8437-030CBE59B82B}"_cry_guid);
	desc.SetLabel("State In Motion");
}


static void ReflectType(Schematyc::CTypeDesc<CSlidableComponent::SStateFinalSignal>& desc)
{
	desc.SetGUID("{EACDD32D-AD66-4380-BAC3-C65E9BAE2BB4}"_cry_guid);
	desc.SetLabel("State Final");
}


void CSlidableComponent::ReflectType(Schematyc::CTypeDesc<CSlidableComponent>& desc)
{
	desc.SetGUID(CSlidableComponent::IID());
	desc.SetEditorCategory("Interaction");
	desc.SetLabel("Slideable");
	desc.SetDescription("Allows sliding an entity back and forth along a distance range.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });

	// Slider properties.
	desc.AddMember(&CSlidableComponent::m_SliderProperties, 'sldr', "SliderProperties", "Slider Properties", "Slider Properties", CSlidableComponent::SSliderProperties());
}


void CSlidableComponent::Update(SEntityUpdateContext* pCtx)
{
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSlidableComponent)
}