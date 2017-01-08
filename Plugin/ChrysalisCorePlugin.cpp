#include "StdAfx.h"

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

#include "Plugin/ChrysalisCorePlugin.h"
#include "Plugin/ChrysalisCore.h"
#include <CryExtension/ICryPluginManager.h>


IEntityRegistrator *IEntityRegistrator::g_pFirst = nullptr;
IEntityRegistrator *IEntityRegistrator::g_pLast = nullptr;


CChrysalisCorePlugin::~CChrysalisCorePlugin()
{
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	IEntityRegistrator* pTemp = IEntityRegistrator::g_pFirst;
	while (pTemp != nullptr)
	{
		pTemp->Unregister();
		pTemp = pTemp->m_pNext;
	}

	SAFE_DELETE(m_pChrysalisCore);
}


bool CChrysalisCorePlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);

	m_pChrysalisCore = new CChrysalisCore();

	return (m_pChrysalisCore != nullptr);
}


void CChrysalisCorePlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		case ESYSTEM_EVENT_GAME_POST_INIT:
		{
			// Register entities
			IEntityRegistrator* pTemp = IEntityRegistrator::g_pFirst;
			while (pTemp != nullptr)
			{
				pTemp->Register();
				pTemp = pTemp->m_pNext;
			}

			gEnv->pConsole->ExecuteString("map example", false, true);
		}
		break;
	}
}


CChrysalisCorePlugin* CChrysalisCorePlugin::Get()
{
	static CChrysalisCorePlugin* plugIn { nullptr };

	if (!plugIn)
		plugIn = gEnv->pSystem->GetIPluginManager()->QueryPlugin<CChrysalisCorePlugin>();

	if (!plugIn)
		CRY_ASSERT_MESSAGE(plugIn, "Chrysalis Core plugin was not found.");

	return plugIn;
}

CRYREGISTER_SINGLETON_CLASS(CChrysalisCorePlugin)