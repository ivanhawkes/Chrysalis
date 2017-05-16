#include "StdAfx.h"

#include "ItemFlashlightParameter.h"
#include <GameXmlParamReader.h>
#include "Game/Game.h"
#include "Game/Cache/GameCache.h"
#include "Item/Parameters/ItemParameter.h"


CItemFlashlightParameter::CItemFlashlightParameter()
{
}


CItemFlashlightParameter::~CItemFlashlightParameter()
{
}


void CItemFlashlightParameter::Reset()
{
	*this = CItemFlashlightParameter();
}


bool CItemFlashlightParameter::Read(const XmlNodeRef& node)
{
	CGameXmlParamReader reader(node);

	// Do we need to inherit from another entry?
	string inherit = reader.ReadParamValue("inherit");
	if (inherit.length() > 0)
	{
		// See if that entry exists in the cache already. Files should be laid out in a way that insures parents
		// are loaded before anything that inherits from them.
		auto pParent = g_pGame->GetGameCache().GetItemParameter(inherit);
		if (pParent)
		{
			*this = *pParent->GetItemFlashlightParameter();
		}
	}

	// Read the parameters.
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

	// Adjust values to internal range.
	color /= 255.0f;
	fogVolumeColor /= 255.0f;

	return true;
}

