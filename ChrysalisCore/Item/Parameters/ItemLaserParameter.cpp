#include <StdAfx.h>

#include "ItemLaserParameter.h"
#include <GameXmlParamReader.h>
#include <Item/Parameters/ItemParameter.h>


CItemLaserParameter::CItemLaserParameter()
{
}


CItemLaserParameter::~CItemLaserParameter()
{
}


void CItemLaserParameter::OnResetState()
{
	*this = CItemLaserParameter();
}


bool CItemLaserParameter::Read(const XmlNodeRef& node)
{
	CGameXmlParamReader reader(node);

	return true;
}
