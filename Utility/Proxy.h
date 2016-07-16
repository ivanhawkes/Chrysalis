/**
\file D:\CRYENGINE\Code\ChrysalisSDK\Utility\Proxy.h

Convenience / utility functions that retrieve, cast and even create proxies for an entity as requested. You should
use these when possible and add any missing ones as needed. The GameSDK code is lousy with calls for this which could
all be slightly simplified by using these calls instead.
**/
#pragma once

#include <CryEntitySystem/IEntityProxy.h>
#include <CryEntitySystem/IEntity.h>


namespace Proxy
{
	/**
	Gets the audio proxy for this entity. It can create a proxy if one doesn't already exist.

	\param [in,out]	pIEntity If non-null, the entity.
	\param	create			 true to create.

	\return null if it fails, else the audio proxy.
	**/
	ILINE IEntityAudioProxy* GetAudioProxy(IEntity* pIEntity, bool create = false)
	{
		auto pProxy = static_cast <IEntityAudioProxy*> (pIEntity->GetProxy(ENTITY_PROXY_AUDIO));

		if (!pProxy && create)
			pProxy = crycomponent_cast<IEntityAudioProxyPtr> (pIEntity->CreateProxy(ENTITY_PROXY_AUDIO)).get();

		return pProxy;
	}


	/**
	Gets the physical proxy for this entity. It can create a proxy if one doesn't already exist.

	\param [in,out]	pIEntity If non-null, the entity.
	\param	create			 true to create.

	\return null if it fails, else the physical proxy.
	**/
	ILINE IEntityPhysicalProxy* GetPhysicalProxy(IEntity* pIEntity, bool create = false)
	{
		auto pProxy = static_cast <IEntityPhysicalProxy*> (pIEntity->GetProxy(ENTITY_PROXY_PHYSICS));
		if (!pProxy && create)
			pProxy = crycomponent_cast<IEntityPhysicalProxyPtr> (pIEntity->CreateProxy(ENTITY_PROXY_PHYSICS)).get();

		return pProxy;
	}


	/**
	Gets the render proxy for this entity. It can create a proxy if one doesn't already exist.

	\param [in,out]	pIEntity If non-null, the entity.
	\param	create			 true to create.

	\return null if it fails, else the render proxy.
	**/
	ILINE IEntityRenderProxy* GetRenderProxy(IEntity* pIEntity, bool create = false)
	{
		auto pProxy = static_cast <IEntityRenderProxy*> (pIEntity->GetProxy(ENTITY_PROXY_RENDER));
		if (!pProxy && create)
			pProxy = crycomponent_cast<IEntityRenderProxyPtr> (pIEntity->CreateProxy(ENTITY_PROXY_RENDER)).get();

		return pProxy;
	}


	/**
	Gets user proxy.

	\param [in,out]	pIEntity If non-null, the entity.

	\return null if it fails, else the user proxy.
	**/
	ILINE CGameObject* GetUserProxy(IEntity* pIEntity)
	{
		return reinterpret_cast <CGameObject*> (pIEntity->GetProxy(ENTITY_PROXY_USER));
	}
}
