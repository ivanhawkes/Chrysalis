#include <StdAfx.h>
#include "FogVolume.h"
#include <GameXmlParamReader.h>

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


void SFogVolume::MemberSerialize(TSerialize ser)
{
    ser.BeginGroup("FogVolume");

    // Serialize the parameters.
    ser.Value("color", color);
    ser.Value("globalDensity", globalDensity);
    ser.Value("densityOffset", densityOffset);

    ser.EndGroup();
}



