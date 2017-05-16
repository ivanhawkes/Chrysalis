#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntityClass.h>

class CChrysalisCore;


class CChrysalisCorePlugin : public ICryPlugin, public ISystemEventListener
{
public:
	CRYINTERFACE_BEGIN()
		CRYINTERFACE_ADD(ICryPlugin)
		CRYINTERFACE_ADD(CChrysalisCorePlugin)
	CRYINTERFACE_END()
	CRYGENERATE_SINGLETONCLASS(CChrysalisCorePlugin, "ChrysalisCore", 0x6CCC03C51C214ADA, 0x9F25AE9F5C644F68)
	
	// This shouldn't be needed, apparently, but calls to get the plugin from the plugin manager fail badly
	// returning a different unrelated plugin without it.	
	CRYINTERFACE_DECLARE(CChrysalisCorePlugin, 0x6CCC03C51C214ADA, 0x9F25AE9F5C644F68)

	virtual ~CChrysalisCorePlugin();

	//! Retrieve name of plugin.
	virtual const char* GetName() const override { return "ChrysalisCore"; }

	//! Retrieve category for the plugin.
	virtual const char* GetCategory() const override { return "Game"; }

	//! This is called to initialize the new plugin.
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;

	virtual void OnPluginUpdate(EPluginUpdateType updateType) override {}

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	static CChrysalisCorePlugin* Get();

	template<class T>
	struct CObjectCreator : public IGameObjectExtensionCreatorBase
	{
		IGameObjectExtension* Create(IEntity* pEntity)
		{
			return pEntity->CreateComponentClass<T>();
		}

		void GetGameObjectExtensionRMIData(void** ppRMI, size_t* nCount)
		{
			*ppRMI = nullptr;
			*nCount = 0;
		}
	};

	template<class T>
	static void RegisterEntityWithDefaultComponent(const char *name)
	{
		IEntityClassRegistry::SEntityClassDesc clsDesc;
		clsDesc.sName = name;

		static CObjectCreator<T> _creator;

		gEnv->pGameFramework->GetIGameObjectSystem()->RegisterExtension(name, &_creator, &clsDesc);
	}

	template<class T>
	static void RegisterEntityComponent(const char *name)
	{
		static CObjectCreator<T> _creator;

		gEnv->pGameFramework->GetIGameObjectSystem()->RegisterExtension(name, &_creator, nullptr);
		T::SetExtensionId(gEnv->pGameFramework->GetIGameObjectSystem()->GetID(name));
	}

	CChrysalisCore* GetChrysalisCore () const { return m_pChrysalisCore; };


private:
	CChrysalisCore* m_pChrysalisCore { nullptr };
};


struct IEntityRegistrator
{
	IEntityRegistrator()
	{
		if (g_pFirst == nullptr)
		{
			g_pFirst = this;
			g_pLast = this;
		}
		else
		{
			g_pLast->m_pNext = this;
			g_pLast = g_pLast->m_pNext;
		}
	}

	virtual void Register() = 0;
	virtual void Unregister() {};

public:
	IEntityRegistrator *m_pNext { nullptr };

	static IEntityRegistrator *g_pFirst;
	static IEntityRegistrator *g_pLast;
};