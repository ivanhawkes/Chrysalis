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
//#define PROCEDURAL_CONTEXT_MOVEMENT_CONTROL_METHOD_NAME "ProceduralContextMCM"
//
//
//class CProceduralContextMovementControlMethod
//	: public IProceduralContext
//{
//public:
//	PROCEDURAL_CONTEXT(CProceduralContextMovementControlMethod, PROCEDURAL_CONTEXT_MOVEMENT_CONTROL_METHOD_NAME, "{12B1667D-FE25-4046-A2A8-EBECF3048ADE}"_cry_guid);
//
//	virtual ~CProceduralContextMovementControlMethod() {}
//
//	// IProceduralContext
//	virtual void Update(float timePassedSeconds) override;
//	// ~IProceduralContext
//
//	uint32 RequestMovementControlMethod(const EMovementControlMethod horizontal, const EMovementControlMethod vertical);
//	void CancelRequest(const uint32 requestId);
//
//private:
//	IAnimatedCharacter* GetAnimatedCharacter() const;
//
//private:
//	struct SMCMRequest
//	{
//		uint32 id;
//		EMovementControlMethod horizontal;
//		EMovementControlMethod vertical;
//	};
//
//	typedef ProceduralContextHelpers::CRequestList< SMCMRequest > TMCMRequestList;
//	TMCMRequestList m_requestList;
//};
//}