#include "StdAfx.h"

#include "ItemParameter.h"
#include <GameXmlParamReader.h>
#include "Item/Parameters/ItemAccessoryParameter.h"
#include "Item/Parameters/ItemBaseParameter.h"
//#include "Item/Parameters/ItemFlashlightParameter.h"
#include "Item/Parameters/ItemGeometryParameter.h"
#include "Item/Parameters/ItemLaserParameter.h"


CItemParameter::CItemParameter()
{
}


CItemParameter::~CItemParameter()
{
}


void CItemParameter::Reset()
{
	// TODO: Should this call reset on all the dependants instead?
	
	SAFE_DELETE(m_pItemAccessoryParameter);
	SAFE_DELETE(m_pItemBaseParameter);
	//SAFE_DELETE(m_pItemFlashlightParameter);
	SAFE_DELETE(m_pItemGeometryParameter);
	SAFE_DELETE(m_pItemLaserParameter);

	//m_pItemAccessoryParameter = new CItemAccessoryParameter();
	//m_pItemBaseParameter = new CItemBaseParameter();
	//m_pItemFlashlightParameter = new CItemFlashlightParameter();
	//m_pItemGeometryParameter = new CItemGeometryParameter();
	//m_pItemLaserParameter = new CItemLaserParameter();
}


bool CItemParameter::Read(const XmlNodeRef& node)
{
	return true;
}

