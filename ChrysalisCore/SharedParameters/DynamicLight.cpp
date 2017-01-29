#include <StdAfx.h>
#include "DynamicLight.h"
#include <GameXmlParamReader.h>
#include <CrySerialization/Decorators/Resources.h>
#include <CrySerialization/Enum.h>
#include <CrySerialization/Color.h>
#include <CrySerialization/STL.h>
#include <CrySerialization/Math.h>

DEFINE_SHARED_PARAMS_TYPE_INFO(SDynamicLight)


void SDynamicLight::Reset()
{
    // Reset code goes here.
    *this = SDynamicLight();
}


bool SDynamicLight::Read(const XmlNodeRef& node)
{
    // Read code goes here.
    CGameXmlParamReader reader(node);

    // Read the parameters.
    reader.ReadParamValue("radius", radius);
    reader.ReadParamValue("specularMultiplier", specularMultiplier);
    reader.ReadParamValue("diffuseMultiplier", diffuseMultiplier);
    reader.ReadParamValue("attenuationRadius", attenuationRadius);
    reader.ReadParamValue("diffuseColor", diffuseColor);
    reader.ReadParamValue("projectorFoV", projectorFoV);
    reader.ReadParamValue("projectorNearPlane", projectorNearPlane);
    projectorTexture = reader.ReadParamValue("projectorTexture");
	material = reader.ReadParamValue("material");
	reader.ReadParamValue("lightStyle", lightStyle);
    reader.ReadParamValue("animationSpeed", animationSpeed);
    reader.ReadParamValue("lightPhase", lightPhase);
    reader.ReadParamValue("shadowBias", shadowBias);
    reader.ReadParamValue("shadowSlopeBias", shadowSlopeBias);
    reader.ReadParamValue("shadowResolutionScale", shadowResolutionScale);
    reader.ReadParamValue("shadowMinimumResolutionPercent", shadowMinimumResolutionPercent);
    reader.ReadParamValue("shadowUpdateMinimumRadius", shadowUpdateMinimumRadius);
    reader.ReadParamValue("shadowUpdateRatio", shadowUpdateRatio);

	// Adjust color values to an internal range.
	diffuseColor /= 255.0f;

    return true;
}


void SDynamicLight::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.openBlock("DynamicLightParameter", "DynamicLightParameter"))
	{
		// Serialize the parameters.
		archive(radius, "radius", "Radius");
		archive.doc("Specifies how far from the source the light affects the surrounding area.");
		archive(specularMultiplier, "specularMultiplier", "Specular Multiplier");
		archive.doc("Control the strength of the specular brightness.");
		archive(diffuseMultiplier, "diffuseMultiplier", "Diffuse Multiplier");
		archive.doc("Control the strength of the diffuse color.");
		archive(attenuationRadius, "attenuationRadius", "Attenuation Radius");
		archive.doc("Specifies the radius of the area light bulb.");
		archive(diffuseColor, "diffuseColor", "Diffuse Color");
		archive.doc("Diffuse color expressed as RGB e.g. 128, 255, 128");
		archive(projectorFoV, "projectorFoV", "Projector Field of View");
		archive.doc("Specifies the Angle on which the light texture is projected.");
		archive(projectorNearPlane, "projectorNearPlane", "Projector Near Plane");
		archive.doc("Set the near plane for the projector, any surfaces closer to the light source than this value will not be projected on.");
		archive(projectorTexture, "projectorTexture", "Projector Texture");
		archive.doc("A texture to used for custom falloff.");
		archive(lightStyle, "lightStyle", "Light Style");
		archive.doc("Style variation (flickering, waxing / wanning / etc)");
		archive(animationSpeed, "animationSpeed", "Animation Speed");
		archive.doc("Rate at which the style animation will play.");
		archive(lightPhase, "lightPhase", "Light Phase");
		archive.doc("Point in the cycle (style) at which light animation begins.");
		archive(shadowBias, "shadowBias", "Shadow Bias");
		archive.doc("Moves the shadow cascade toward or away from the shadow-casting object.");
		archive(shadowSlopeBias, "shadowSlopeBias", "Shadow Slope Bias");
		archive.doc("Allows you to adjust the gradient (slope-based) bias used to compute the shadow bias.");
		archive(shadowResolutionScale, "shadowResolutionScale", "Shadow Resolution Scale");
		archive.doc("");
		archive(shadowMinimumResolutionPercent, "shadowMinimumResolutionPercent", "Shadow Minimum Resolution Percent");
		archive.doc("Percentage of the shadow pool the light should use for its shadows.");
		archive(shadowUpdateMinimumRadius, "shadowUpdateMinimumRadius", "Shadow Update Minimum Radius");
		archive.doc("Define the minimum radius from the light source to the player camera that the ShadowUpdateRatio setting will be ignored.");
		archive(shadowUpdateRatio, "shadowUpdateRatio", "Shadow Update Ratio");
		archive.doc("Define the update ratio for shadow maps cast from this light.");

		archive.closeBlock();
	}

	//if (archive.isInput())
	//{
	//	OnResetState();
	//}
}
