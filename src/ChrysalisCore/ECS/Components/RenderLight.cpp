#include <StdAfx.h>

#include "RenderLight.h"


namespace Cry
{
namespace DefaultComponents
{
SERIALIZATION_ENUM_BEGIN(ELightGIMode, "Light GI Mode")
SERIALIZATION_ENUM(ELightGIMode::Disabled, "none", "Disabled")
SERIALIZATION_ENUM(ELightGIMode::StaticLight, "staticLight", "Static")
SERIALIZATION_ENUM(ELightGIMode::DynamicLight, "dynamicLight", "Dynamic")
SERIALIZATION_ENUM(ELightGIMode::ExcludeForGI, "excludeForGI", "Hide if GI is Active")
SERIALIZATION_ENUM_END()


SERIALIZATION_ENUM_BEGIN(ELightShape, "Area Light Shape")
SERIALIZATION_ENUM(ELightShape::Point, "point", "Point")
SERIALIZATION_ENUM(ELightShape::Rectangle, "rectangle", "Rectangle")
SERIALIZATION_ENUM(ELightShape::Disk, "disk", "Disk")
SERIALIZATION_ENUM_END()


SERIALIZATION_ENUM_BEGIN(EMiniumSystemSpec, "Minimum System Specification")
SERIALIZATION_ENUM(EMiniumSystemSpec::Disabled, "disabled", "Disabled")
SERIALIZATION_ENUM(EMiniumSystemSpec::Always, "always", "Always")
SERIALIZATION_ENUM(EMiniumSystemSpec::Medium, "medium", "Medium")
SERIALIZATION_ENUM(EMiniumSystemSpec::High, "high", "High")
SERIALIZATION_ENUM(EMiniumSystemSpec::VeryHigh, "veryHigh", "Very High")
SERIALIZATION_ENUM_END()


bool Serialize(Serialization::IArchive& ar, ILightComponent::SOptics& desc, const char* szName, const char* szLabel)
{
	ar(desc.m_lensFlareName, "lensFlareName", "Diffuse color expressed as RGB e.g. 128, 255, 128.");
	ar(desc.m_attachToSun, "attachToSun", "Control the strength of the diffuse color.");
	ar(desc.m_flareEnable, "flareEnable", "Control the strength of the specular brightness.");
	ar(desc.m_flareFOV, "flareFOV", "Control the strength of the specular brightness.");

	return true;
}


bool Serialize(Serialization::IArchive& ar, ILightComponent::SOptions& desc, const char* szName, const char* szLabel)
{
	ar(desc.m_attenuationBulbSize, "attenuationBulbSize", "Controls the fall-off exponentially from the origin, a value of 1 means that the light is at full intensity within a 1 meter ball before it begins to fall-off.");
	ar(desc.m_bIgnoreVisAreas, "ignoreVisAreas", "Ignore VisAreas");
	ar(desc.m_bAffectsVolumetricFog, "affectVolumetricFog", "Affect Volumetric Fog");
	ar(desc.m_bVolumetricFogOnly, "volumetricFogOnly", "Only Affect Volumetric Fog");
	ar(desc.m_bAffectsOnlyThisArea, "onlyAffectThisArea", "Only Affect This Area");
	ar(desc.m_bLinkToSkyColor, "linkToSkyColor", "Multiply light color with current sky color (use GI sky color if available).");
	ar(desc.m_bAmbient, "ambient", "Ambient");
	ar(desc.m_fogRadialLobe, "fogRadialLobe", "Fog Radial Lobe");
	ar(desc.m_giMode, "GIMode", "Global Illumination");

	return true;
}


bool Serialize(Serialization::IArchive& ar, ILightComponent::SColor& desc, const char* szName, const char* szLabel)
{
	ar(desc.m_color, "Color", "color");
	ar(desc.m_diffuseMultiplier, "diffuseMultiplier", "Diffuse Multiplier");
	ar(desc.m_specularMultiplier, "specularMultiplier", "Specular Multiplier");
	return true;
}


bool Serialize(Serialization::IArchive& ar, ILightComponent::SShadows& desc, const char* szName, const char* szLabel)
{
	ar(desc.m_castShadowSpec, "shadowSpec", "Minimum Shadow Graphics");
	ar(desc.m_shadowBias, "shadowBias", "Shadow Bias");
	ar(desc.m_shadowSlopeBias, "shadowSlopeBias", "Shadow Slope Bias");
	ar(desc.m_shadowResolutionScale, "shadownResolution", "Shadow Resolution Scale");
	ar(desc.m_shadowUpdateMinRadius, "shadowUpdateMin", "Shadow Min Update Radius");
	ar(desc.m_shadowMinResolution, "shadowMinRes", "Shadow Min Resolution");
	ar(desc.m_shadowUpdateRatio, "shadowUpdateRatio", "Shadow Update Ratio");

	return true;
}


bool Serialize(Serialization::IArchive& ar, ILightComponent::SAnimations& desc, const char* szName, const char* szLabel)
{
	ar(desc.m_style, "style", "Style");
	ar(desc.m_speed, "speed", "Speed");

	return true;
}


bool Serialize(Serialization::IArchive& ar, ILightComponent::SShape& desc, const char* szName, const char* szLabel)
{
	ar(desc.m_areaShape, "shape", "Shape");
	ar(desc.m_twoSided, "twoSided", "Two sided light contribution");
	ar(desc.m_texturePath, "texturePath", "Texture path");
	ar(desc.m_width, "width", "Width of the area shape");
	ar(desc.m_height, "height", "Height of the area shape");

	return true;
}


bool Serialize(Serialization::IArchive& ar, CProjectorLightComponent::SProjectorOptions& desc, const char* szName, const char* szLabel)
{
	ar(desc.m_nearPlane, "nearPlane", "nearPlane");
	ar(desc.m_texturePath, "texturePath", "Texture Path");
	ar(desc.m_materialPath, "materialPath", "Material Path");

	return true;
}


bool Serialize(Serialization::IArchive& ar, CProjectorLightComponent::SFlare& desc, const char* szName, const char* szLabel)
{
	ar(desc.m_angle, "angle", "angle");
	ar(desc.m_texturePath, "texturePath", "Texture Path");

	return true;
}
}
}

#ifdef IMGUI
void Chrysalis::ECS::RenderLight::ImGuiRender()
{
	ImGui::Text("Optics");
	//ar(desc.m_lensFlareName, "lensFlareName", "Diffuse color expressed as RGB e.g. 128, 255, 128.");
	ImGui::Checkbox("Attach to the Sun:", &optics.m_attachToSun);
	ImGui::Checkbox("Flare Enable:", &optics.m_flareEnable);
	ImGui::InputInt("Flare FoV:", &optics.m_flareFOV);
	ImGui::Separator();

	ImGui::Text("Options");
	//Schematyc::Range<0, 64000> m_attenuationBulbSize = SRenderLight().m_fAttenuationBulbSize;
	ImGui::Checkbox("Ignore Vis Areas:", &options.m_bIgnoreVisAreas);
	ImGui::Checkbox("Volumetric Fog Only:", &options.m_bVolumetricFogOnly);
	ImGui::Checkbox("Affects Volumetric Fog:", &options.m_bAffectsVolumetricFog);
	ImGui::Checkbox("Affects Only This Area:", &options.m_bAffectsOnlyThisArea);
	ImGui::Checkbox("Link To Sky Color:", &options.m_bLinkToSkyColor);
	ImGui::Checkbox("Ambient:", &options.m_bAmbient);
	//ar(desc.m_fogRadialLobe, "fogRadialLobe", "Fog Radial Lobe");
	//ar(desc.m_giMode, "GIMode", "Global Illumination");
	ImGui::Separator();

	ImGui::Text("Color");
	ImGui::InputFloat("Diffuse Multiplier:", &color.m_diffuseMultiplier.value);
	ImGui::InputFloat("Specular Multiplier:", &color.m_specularMultiplier.value);
	ImGui::Separator();

	ImGui::Text("Shadows");
	//ar(desc.m_castShadowSpec, "shadowSpec", "Minimum Shadow Graphics");
	ImGui::InputFloat("Shadow Bias:", &shadows.m_shadowBias);
	ImGui::InputFloat("Shadow Slope Bias:", &shadows.m_shadowSlopeBias);
	ImGui::InputFloat("Shadow Resolution Scale:", &shadows.m_shadowResolutionScale);
	ImGui::InputFloat("Shadow UpdateMin Radius:", &shadows.m_shadowUpdateMinRadius);
	ImGui::InputInt("Shadow Min Resolution:", &shadows.m_shadowMinResolution);
	ImGui::InputInt("Shadow Update Ratio:", &shadows.m_shadowUpdateRatio);
	ImGui::Separator();

	ImGui::Text("Animations");
	ImGui::InputInt("Style:", reinterpret_cast<int*>(&animations.m_style));
	ImGui::InputFloat("Speed:", &animations.m_speed);
	ImGui::Separator();

	ImGui::Text("Shape");
	//ar(desc.m_areaShape, "shape", "Shape");
	//ar(desc.m_twoSided, "twoSided", "Two sided light contribution");
	//ar(desc.m_texturePath, "texturePath", "Texture path");
	ImGui::InputFloat("Width:", &shape.m_width.value);
	ImGui::InputFloat("Height:", &shape.m_height.value);
	ImGui::Separator();

	ImGui::Text("Projector Options");
	ImGui::InputFloat("Near Plane:", &projectorOptions.m_nearPlane.value);
	//ar(desc.m_texturePath, "texturePath", "Texture Path");
	//ar(desc.m_materialPath, "materialPath", "Material Path");
	ImGui::Separator();

	ImGui::Text("Flare");
	//ar(flare, "flare", "flare");
	ImGui::Separator();

	// Members that are directly on the class.
	ImGui::Text("Misc");
	ImGui::InputFloat("radius:", &radius.value);
	//ImGui::InputFloat("FoV Angle:", &fovAngle.);
	//ImGui::Text(&effectSlotMaterial.value.c_str());
}
#endif
