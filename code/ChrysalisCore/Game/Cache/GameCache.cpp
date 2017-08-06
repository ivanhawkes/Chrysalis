#include <StdAfx.h>

#include "GameCache.h"
#include <CryEntitySystem/IEntitySystem.h>
#include <CryAnimation/ICryAnimation.h>
#include "Item/Parameters/ItemParameter.h"
#include <CryString/StringUtils.h>


namespace Chrysalis
{
CGameCache::CGameCache()
{}


CGameCache::~CGameCache()
{
}


void CGameCache::Init()
{
}


void CGameCache::Reset()
{
	m_textureCache.clear();
	m_materialCache.clear();
	m_statiObjectCache.clear();
	m_particleEffectCache.clear();
}


void CGameCache::GetMemoryUsage(ICrySizer *s) const
{
	s->Add(*this);

	s->AddContainer(m_textureCache);
	s->AddContainer(m_materialCache);
	s->AddContainer(m_statiObjectCache);
	s->AddContainer(m_particleEffectCache);
}


// ***
// *** Character Geometry Cache
// ***


bool CGameCache::AddCachedCharacterFileModel(const ECharacterFileModelCacheType type, const char* szFileName)
{
	uint32 fileNameHash = 0;
	bool bCached = IsCharacterFileModelCached(szFileName, fileNameHash);

	if (!bCached && szFileName && szFileName [0])
	{
		if (gEnv->IsEditor())
		{
			ICharacterInstance *pCachedInstance = gEnv->pCharacterManager->CreateInstance(szFileName);
			if (pCachedInstance)
			{
				m_editorCharacterFileModelCache [type].insert(TEditorCharacterFileModelCache::value_type(fileNameHash, TCharacterInstancePtr(pCachedInstance)));
				bCached = true;
			}
		}
		else
		{
			bool hasLoaded = gEnv->pCharacterManager->LoadAndLockResources(szFileName, 0);
			if (hasLoaded)
			{
			}
		}
	}

	return bCached;
}


ILINE bool CGameCache::IsCharacterFileModelCached(const char* szFileName, uint32& outputFileNameHash) const
{
	outputFileNameHash = 0;

	if (szFileName && szFileName [0])
	{
		outputFileNameHash = CCrc32::Compute(szFileName);
		if (gEnv->IsEditor())
		{
			for (uint32 i = eCFMCache_Default; i < eCFMCache_COUNT; ++i)
			{
				TEditorCharacterFileModelCache::const_iterator itCache = m_editorCharacterFileModelCache [i].find(outputFileNameHash);

				if (itCache == m_editorCharacterFileModelCache [i].end())
					continue;

				return true;
			}
		}
	}

	return false;
}


// ***
// *** Static Geometry Cache
// ***


void CGameCache::CacheGeometry(const char* geometryFileName)
{
	const bool validName = (geometryFileName && geometryFileName [0]);
	if (validName)
	{
		stack_string ext(PathUtil::GetExt(geometryFileName));

		if ((ext == "cdf") || (ext == "chr") || (ext == "cga"))
		{
			AddCachedCharacterFileModel(eCFMCache_Default, geometryFileName);
		}
		else
		{
			const CryHash hashName = CryStringUtils::HashString(geometryFileName);

			if (m_statiObjectCache.find(hashName) == m_statiObjectCache.end())
			{
				IStatObj* pStaticObject = gEnv->p3DEngine->LoadStatObj(geometryFileName);
				if (pStaticObject)
				{
					m_statiObjectCache.insert(TGameStaticObjectCacheMap::value_type(hashName, TStaticObjectSmartPtr(pStaticObject)));
				}
			}
		}
	}
}


// ***
// *** Texture Cache
// ***


void CGameCache::CacheTexture(const char* textureFileName, const int textureFlags)
{
	const bool validName = (textureFileName && textureFileName [0]);

	if (validName)
	{
		const STextureKey textureKey(CryStringUtils::HashString(textureFileName), textureFlags);

		if (m_textureCache.find(textureKey) == m_textureCache.end())
		{
			ITexture* pTexture = gEnv->pRenderer->EF_LoadTexture(textureFileName, textureFlags);
			if (pTexture)
			{
				m_textureCache.insert(TGameTextureCacheMap::value_type(textureKey, TTextureSmartPtr(pTexture)));
				pTexture->Release();
			}
		}
	}
}


// ***
// *** Material Cache
// ***


void CGameCache::CacheMaterial(const char* materialFileName)
{
	const bool validName = (materialFileName && materialFileName [0]);

	if (validName)
	{
		const CryHash hashName = CryStringUtils::HashString(materialFileName);

		if (m_materialCache.find(hashName) == m_materialCache.end())
		{
			IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(materialFileName);
			if (pMaterial)
			{
				m_materialCache.insert(TGameMaterialCacheMap::value_type(hashName, TMaterialSmartPtr(pMaterial)));
			}
		}
	}
}


CGameCache::TMaterialSmartPtr CGameCache::GetMaterial(const char* materialFileName) const
{
	const bool validName = (materialFileName && materialFileName [0]);

	if (validName)
	{
		const CryHash hashName = CryStringUtils::HashString(materialFileName);
		auto it = m_materialCache.find(hashName);
		if (it != m_materialCache.end())
			return it->second;
	}

	return nullptr;
}


// ***
// *** Particle Cache
// ***


void CGameCache::CacheParticleEffect(const char* particleEffectFileName)
{
	const bool validName = (particleEffectFileName && particleEffectFileName [0]);

	if (validName)
	{
		const CryHash hashName = CryStringUtils::HashString(particleEffectFileName);

		if (m_particleEffectCache.find(hashName) == m_particleEffectCache.end())
		{
			IParticleEffect* pParticleEffect = gEnv->p3DEngine->GetParticleManager()->FindEffect(particleEffectFileName, "CGameCache::CacheParticleEffect");
			if (pParticleEffect)
			{
				m_particleEffectCache.insert(TGameParticleEffectCacheMap::value_type(hashName, TParticleEffectSmartPtr(pParticleEffect)));
			}
		}
	}
}


CGameCache::TParticleEffectSmartPtr CGameCache::GetParticleEffect(const char* particleEffectFileName) const
{
	const bool validName = (particleEffectFileName && particleEffectFileName [0]);

	if (validName)
	{
		const CryHash hashName = CryStringUtils::HashString(particleEffectFileName);
		auto it = m_particleEffectCache.find(hashName);
		if (it != m_particleEffectCache.end())
			return it->second;
	}

	return nullptr;
}
}