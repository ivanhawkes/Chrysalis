#include <StdAfx.h>

#include "ItemFlashlightParameterShared.h"
#include <GameXmlParamReader.h>


namespace Chrysalis
{
DEFINE_SHARED_PARAMS_TYPE_INFO(SItemFlashlightParameterShared)


void SItemFlashlightParameterShared::OnResetState()
{
	// Reset code goes here.
	*this = SItemFlashlightParameterShared();
}


bool SItemFlashlightParameterShared::Read(const XmlNodeRef& node)
{
	// Read code goes here.
	CGameXmlParamReader reader(node);

	// Do we need to inherit from another entry?
	//string inherit = reader.ReadParamValue("inherit");
	//if (inherit.length() > 0)
	//{
	//	// See if that entry exists in the cache already. Files should be laid out in a way that insures parents
	//	// are loaded before anything that inherits from them.
	//	auto pParent = gEnv->pGameFramework->GetGameCache().GetItemParameter(inherit);
	//	if (pParent)
	//	{
	//		*this = *pParent->GetItemFlashlightParameter();
	//	}
	//}

	// Read the parameters.
	prototype = reader.ReadParamValue("prototype");
	reader.ReadParamValue("style", style);
	reader.ReadParamValue("color", color);
	reader.ReadParamValue("diffuseMultiplier", diffuseMultiplier);
	reader.ReadParamValue("specularMultiplier", specularMultiplier);
	reader.ReadParamValue("hdrDynamic", hdrDynamic);
	reader.ReadParamValue("distance", distance);
	reader.ReadParamValue("fov", fov);
	reader.ReadParamValue("animSpeed", animSpeed);
	reader.ReadParamValue("fogVolumeColor", fogVolumeColor);
	reader.ReadParamValue("fogVolumeRadius", fogVolumeRadius);
	reader.ReadParamValue("fogVolumeSize", fogVolumeSize);
	reader.ReadParamValue("fogVolumeDensity", fogVolumeDensity);
	lightCookie = reader.ReadParamValue("lightCookie");

	// Adjust color values to an internal range.
	color /= 255.0f;
	fogVolumeColor /= 255.0f;

	return true;
}


void SItemFlashlightParameterShared::MemberSerialize(TSerialize ser)
{
	ser.BeginGroup("ItemFlashlightParameterShared");

	// Serialize the parameters.
	ser.Value("prototype", prototype);
	ser.Value("style", style);
	ser.Value("specularMultiplier", specularMultiplier);
	ser.Value("diffuseMultiplier", diffuseMultiplier);
	ser.Value("hdrDynamic", hdrDynamic);
	ser.Value("distance", distance);
	ser.Value("fov", fov);
	ser.Value("animSpeed", animSpeed);
	ser.Value("fogVolumeRadius", fogVolumeRadius);
	ser.Value("fogVolumeSize", fogVolumeSize);
	ser.Value("color", color);
	ser.Value("fogVolumeDensity", fogVolumeDensity);
	ser.Value("lightCookie", lightCookie);
	ser.Value("fogVolumeColor", fogVolumeColor);

	ser.EndGroup();
}
}