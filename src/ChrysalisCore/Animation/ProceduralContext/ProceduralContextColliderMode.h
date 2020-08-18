#pragma once

//#include <ICryMannequin.h>
//#include <CryExtension/CryCreateClassInstance.h>
//#include <CryExtension/ClassWeaver.h>
//#include <CryExtension/ICryFactoryRegistryImpl.h>
//#include <CryExtension/RegFactoryNode.h>
//#include <IAnimatedCharacter.h>
//#include "ProceduralContextHelpers.h"
//
//
//namespace Chrysalis
//{
//#define PROCEDURAL_CONTEXT_COLLIDER_MODE_NAME "ProceduralContextColliderMode"
//
//
//class CProceduralContextColliderMode
//	: public IProceduralContext
//{
//public:
//	PROCEDURAL_CONTEXT(CProceduralContextColliderMode, PROCEDURAL_CONTEXT_COLLIDER_MODE_NAME, "{1B14E1BC-0970-4E95-ACB7-A11255C77458}"_cry_guid);
//
//	virtual ~CProceduralContextColliderMode() {}
//
//	// IProceduralContext
//	virtual void Update(float timePassedSeconds) override;
//	// ~IProceduralContext
//
//	uint32 RequestColliderMode(const EColliderMode colliderMode);
//	void CancelRequest(const uint32 requestId);
//
//private:
//	IAnimatedCharacter* GetAnimatedCharacter() const;
//
//private:
//	struct SColliderModeRequest
//	{
//		uint32 id;
//		EColliderMode mode;
//	};
//
//	typedef ProceduralContextHelpers::CRequestList< SColliderModeRequest > TColliderModeRequestList;
//	TColliderModeRequestList m_requestList;
//};
//}