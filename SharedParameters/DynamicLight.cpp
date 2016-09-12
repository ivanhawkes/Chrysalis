#include <StdAfx.h>
#include "DynamicLight.h"
#include <GameXmlParamReader.h>

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


void SDynamicLight::MemberSerialize(TSerialize ser)
{
    ser.BeginGroup("DynamicLight");

    // Serialize the parameters.
    ser.Value("radius", radius);
    ser.Value("specularMultiplier", specularMultiplier);
    ser.Value("diffuseMultiplier", diffuseMultiplier);
    ser.Value("attenuationRadius", attenuationRadius);
    ser.Value("diffuseColor", diffuseColor);
    ser.Value("projectorFoV", projectorFoV);
    ser.Value("projectorNearPlane", projectorNearPlane);
    ser.Value("projectorTexture", projectorTexture);
	ser.Value("material", material);
	ser.Value("lightStyle", lightStyle);
    ser.Value("animationSpeed", animationSpeed);
    ser.Value("lightPhase", lightPhase);
    ser.Value("shadowBias", shadowBias);
    ser.Value("shadowSlopeBias", shadowSlopeBias);
    ser.Value("shadowResolutionScale", shadowResolutionScale);
    ser.Value("shadowMinimumResolutionPercent", shadowMinimumResolutionPercent);
    ser.Value("shadowUpdateMinimumRadius", shadowUpdateMinimumRadius);
    ser.Value("shadowUpdateRatio", shadowUpdateRatio);

    ser.EndGroup();
}
