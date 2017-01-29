#include <StdAfx.h>

#include "ItemAccessoryParameter.h"
#include <GameXmlParamReader.h>
#include <Item/Parameters/ItemParameter.h>


CItemAccessoryParameter::CItemAccessoryParameter()
{
}


CItemAccessoryParameter::~CItemAccessoryParameter()
{
}


void CItemAccessoryParameter::OnResetState()
{
	*this = CItemAccessoryParameter();
}


bool CItemAccessoryParameter::Read(const XmlNodeRef& node)
{
	CGameXmlParamReader reader(node);

	return true;
}
