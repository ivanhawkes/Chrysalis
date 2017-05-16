#include <StdAfx.h>
#include "ItemFlashlightParameter.h"
#include <GameXmlParamReader.h>


void CItemFlashlightParameter::OnResetState()
{
    // Reset code goes here.
    *this = CItemFlashlightParameter();
}


bool CItemFlashlightParameter::Read(const XmlNodeRef& node)
{
    // Read code goes here.
    CGameXmlParamReader reader(node);

    // Read the parameters.
    prototype = reader.ReadParamValue("prototype");
    reader.ReadParamValue("batteryRemaining", batteryRemaining);

    return true;
}


void CItemFlashlightParameter::MemberSerialize(TSerialize ser)
{
    ser.BeginGroup("ItemFlashlightParameter");

    // Serialize the parameters.
    ser.Value("prototype", prototype);
    ser.Value("batteryRemaining", batteryRemaining);

    ser.EndGroup();
}
