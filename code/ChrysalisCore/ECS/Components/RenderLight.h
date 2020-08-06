#pragma once

#include <ECS/Components/Components.h>
#include <DefaultComponents/Lights/ILightComponent.h>
#include <DefaultComponents/Lights/ProjectorLightComponent.h>


namespace Cry
{
namespace DefaultComponents
{
/** We need to add a Serialize function for each type, since they lack one. */
bool Serialize(Serialization::IArchive& ar, ILightComponent::SOptions& desc, const char* szName, const char* szLabel);
bool Serialize(Serialization::IArchive& ar, ILightComponent::SColor& desc, const char* szName, const char* szLabel);
bool Serialize(Serialization::IArchive& ar, ILightComponent::SOptics& desc, const char* szName, const char* szLabel);
bool Serialize(Serialization::IArchive& ar, ILightComponent::SShadows& desc, const char* szName, const char* szLabel);
bool Serialize(Serialization::IArchive& ar, ILightComponent::SAnimations& desc, const char* szName, const char* szLabel);
bool Serialize(Serialization::IArchive& ar, ILightComponent::SShape& desc, const char* szName, const char* szLabel);
bool Serialize(Serialization::IArchive& ar, CProjectorLightComponent::SProjectorOptions& desc, const char* szName, const char* szLabel);
bool Serialize(Serialization::IArchive& ar, CProjectorLightComponent::SFlare& desc, const char* szName, const char* szLabel);
}
}


namespace Chrysalis::ECS
{
struct RenderLight
{
	RenderLight() = default;
	virtual ~RenderLight() = default;


	void Serialize(Serialization::IArchive& ar)
	{
		ar(optics, "optics", "optics");
		ar(options, "options", "options");
		ar(color, "color", "color");
		ar(shadows, "shadows", "shadows");
		ar(animations, "animations", "animations");
		ar(shape, "shape", "shape");
		ar(projectorOptions, "projectorOptions", "projectorOptions");
		ar(flare, "flare", "flare");

		ar(radius, "radius", "Specifies how far from the source the light affects the surrounding area.");
		ar(fovAngle, "fovAngle", "m_angle");
		ar(effectSlotMaterial, "effectSlotMaterial", "A material for the effects slot.");
	}


	/** Using the default components CryTek provided. This will help with compatibility down the line. */
	Cry::DefaultComponents::ILightComponent::SOptions options;
	Cry::DefaultComponents::ILightComponent::SColor color;
	Cry::DefaultComponents::ILightComponent::SOptics optics;
	Cry::DefaultComponents::ILightComponent::SShadows shadows;
	Cry::DefaultComponents::ILightComponent::SAnimations animations;
	Cry::DefaultComponents::ILightComponent::SShape shape;
	Cry::DefaultComponents::CProjectorLightComponent::SProjectorOptions projectorOptions;
	Cry::DefaultComponents::CProjectorLightComponent::SFlare flare;


	/** Specifies how far from the source the light affects the surrounding area. */
	Schematyc::Range<0, std::numeric_limits<int>::max()> radius = 10.f;

	/** Limit the field of view and / or frustum angle.  */
	CryTransform::CClampedAngle<0, 180> fovAngle = 45.0_degrees;

	/** A material for the effects slot. */
	Schematyc::MaterialFileName effectSlotMaterial;
};
}