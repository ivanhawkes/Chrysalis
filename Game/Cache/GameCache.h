/**
\file	Game\Cache\GameCache.h

Helper for caching information from entities in the pool. Refactored from various caches inside GameSDK.

NOTE: Where possible I have moved from using _smart_ptr to std::shared_ptr. If this causes issues, just switch them back again. I'm simply
trying to reduce the surface area and dependancies of the code.
*/
#pragma once

#include <Utility/CryHash.h>


struct CItemParameter;


// TODO: Add more caches, and ability to query for items in the caches.
// TODO: Implement material loading for geometry cache. See ItemResourceCache for how.
// TODO: Add animation cache.
// TODO: Flush caches.

class CGameCache
{
	// ***
	// *** CGameCache
	// ***

public:
	/** Default constructor. */
	CGameCache();


	/** Destructor. */
	virtual ~CGameCache();
	

	/** Initialises this object. */
	void Init();


	/** Resets this object. */
	void Reset();


	/**
	Automatically called by various systems to get the memory statistics for this instance. Add your own usage via
	'*this'. Be sure to add any memory you have allocated on the heap.

	\param [in,out]	s	If non-null, the ICrySizer * to process.
	*/
	void GetMemoryUsage(ICrySizer *s) const;


	// ***
	// *** Character Geometry Cache
	// ***

public:
	enum ECharacterFileModelCacheType
	{
		eCFMCache_Default = 0,
		eCFMCache_Client,
		eCFMCache_Shadow,
		eCFMCache_COUNT,
	};

	typedef std::shared_ptr<ICharacterInstance> TCharacterInstancePtr;

	// Character file model cache helpers
	bool AddCachedCharacterFileModel(const ECharacterFileModelCacheType type, const char* szFileName);
	bool IsCharacterFileModelCached(const char* szFileName, uint32& outputFileNameHash) const;

private:
	// Character file model cache.
	typedef std::map<uint32, TCharacterInstancePtr> TEditorCharacterFileModelCache;
	TEditorCharacterFileModelCache   m_editorCharacterFileModelCache[eCFMCache_COUNT];


	// ***
	// *** Static Geometry Cache
	// ***

public:
	typedef _smart_ptr<IStatObj> TStaticObjectSmartPtr;

	void CacheGeometry(const char* geometryObjectFileName);

private:
	typedef std::map<CryHash, TStaticObjectSmartPtr> TGameStaticObjectCacheMap;
	TGameStaticObjectCacheMap m_statiObjectCache;


	// ***
	// *** Texture Cache
	// ***

public:
	struct STextureKey
	{
		STextureKey()
			: nameHash(0)
			, textureFlags(0)
		{}

		STextureKey(const CryHash& _nameHash, const int _textureFlags)
			: nameHash(_nameHash)
			, textureFlags(_textureFlags)
		{}

		struct compare
		{
			bool operator()(const STextureKey& k1, const STextureKey& k2) const
			{
				return (k1.nameHash < k2.nameHash) || ((k1.nameHash == k2.nameHash) && (k1.textureFlags < k2.textureFlags));
			}
		};

		CryHash nameHash;
		int textureFlags;
	};

	typedef _smart_ptr<ITexture> TTextureSmartPtr;
	void CacheTexture(const char* textureFileName, const int textureFlags);

private:
	typedef	std::map<STextureKey, TTextureSmartPtr, STextureKey::compare> TGameTextureCacheMap;
	TGameTextureCacheMap m_textureCache;


	// ***
	// *** Material Cache
	// ***

public:
	typedef std::shared_ptr<IMaterial> TMaterialSmartPtr;

	void CacheMaterial(const char* materialFileName);
	TMaterialSmartPtr GetMaterial(const char* materialFileName) const;

private:
	typedef std::map<CryHash, TMaterialSmartPtr> TGameMaterialCacheMap;
	TGameMaterialCacheMap m_materialCache;


	// ***
	// *** Particle Cache
	// ***

public:
	typedef std::shared_ptr<IParticleEffect> TParticleEffectSmartPtr;

	void CacheParticleEffect(const char* particleEffectFileName);
	TParticleEffectSmartPtr GetParticleEffect(const char* particleEffectFileName) const;

private:
	typedef std::map<CryHash, TParticleEffectSmartPtr> TGameParticleEffectCacheMap;
	TGameParticleEffectCacheMap m_particleEffectCache;
};
