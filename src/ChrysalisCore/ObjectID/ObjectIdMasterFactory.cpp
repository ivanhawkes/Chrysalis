#include <StdAfx.h>

#include "ObjectIdMasterFactory.h"
#include "ObjectId.h"


namespace Chrysalis
{
CObjectIdMasterFactory::CObjectIdMasterFactory(uint32 instanceId)
	: m_instanceId(instanceId)
{
	m_pCharacterFactory = new CObjectIdFactory(instanceId);
	m_pAccountFactory = new CObjectIdFactory(instanceId);
	m_pFactionFactory = new CObjectIdFactory(instanceId);
	m_pItemFactory = new CObjectIdFactory(instanceId);
}


CObjectIdMasterFactory::~CObjectIdMasterFactory()
{
	SAFE_DELETE(m_pCharacterFactory);
	SAFE_DELETE(m_pAccountFactory);
	SAFE_DELETE(m_pFactionFactory);
	SAFE_DELETE(m_pItemFactory);
}
}