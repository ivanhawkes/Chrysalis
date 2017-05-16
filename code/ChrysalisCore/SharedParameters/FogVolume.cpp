#include <StdAfx.h>
#include "FogVolume.h"
#include <GameXmlParamReader.h>
#include <CrySerialization/Decorators/Resources.h>
#include <CrySerialization/Enum.h>
#include <CrySerialization/Color.h>
#include <CrySerialization/STL.h>
#include <CrySerialization/Math.h>

DEFINE_SHARED_PARAMS_TYPE_INFO(SFogVolume)


void SFogVolume::Reset()
{
    // Reset code goes here.
    *this = SFogVolume();
}


bool SFogVolume::Read(const XmlNodeRef& node)
{
    // Read code goes here.
    CGameXmlParamReader reader(node);

    // Read the parameters.
    reader.ReadParamValue("color", color);
    reader.ReadParamValue("globalDensity", globalDensity);
    reader.ReadParamValue("densityOffset", densityOffset);

	// Adjust color values to an internal range.
	//color /= 255.0f;
	
	return true;
}


void SFogVolume::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.openBlock("FogVolumeParameter", "FogVolumeParameter"))
	{
		// Serialize the parameters.
		archive(color, "color", "Color");
		archive.doc("The color (RGB).");
		archive(globalDensity, "globalDensity", "Global Density");
		archive.doc("Global density.");
		archive(densityOffset, "densityOffset", "Density Offset");
		archive.doc("Density offset.");

		archive.closeBlock();
	}

	//if (archive.isInput())
	//{
	//	OnResetState();
	//}
}