// TODO: When it stabilises, copy these over from GameSDK and re-work them for template based flowgraph nodes.

///*************************************************************************
//Crytek Source File.
//Copyright (C), Crytek Studios, 2001-2015.
//-------------------------------------------------------------------------
//Description:
//- This node includes different generic VR functionality
//History:
//- 02.02.2015   Created by Dario Sancho
//*************************************************************************/
//
//#include <StdAfx.h>
//
//#include "Nodes/G2FlowBaseNode.h"
//#include <IHMDDevice.h>
//#include "ICryAnimation.h"
//#include <IHMDDevice.h>
//#include <IHMDManager.h>
//#include "Player.h"
//
//class CVRTools : public CFlowBaseNode<eNCT_Singleton>
//{
//	enum INPUTS
//	{
//		EIP_RecenterPose = 0
//	};
//
//	enum OUTPUTS
//	{
//		EOP_Done = 0,
//		EOP_Triggered,
//		EOP_Failed,
//	};
//
//public:
//	CVRTools( SActivationInfo * pActInfo )
//	{
//	}
//
//	virtual void GetMemoryUsage(ICrySizer * s) const
//	{
//		s->Add(*this);
//	}
//
//	virtual void GetConfiguration( SFlowNodeConfig &config )
//	{
//		static const SInputPortConfig in_config[] = {
//			InputPortConfig_Void( "RecentrePose",_HELP("Resets the tracking origin to the headset's current location, and sets the yaw origin to the current headset yaw value") ),
//			{0}
//		};
//		static const SOutputPortConfig out_config[] = {
//			OutputPortConfig_AnyType("Done", _HELP("The selected operation has been acknoledge. This output will always get triggered.") ),
//			OutputPortConfig_AnyType("Triggered", _HELP("The selected operation has been triggered.") ),
//			OutputPortConfig_AnyType("Failed", _HELP("The selected operation did not work as expected (e.g. the VR operation was not supported).") ),
//			{0}
//		};
//		config.sDescription = _HELP( "Various VR-specific utils" );
//		config.pInputPorts = in_config;
//		config.pOutputPorts = out_config;
//		config.SetCategory(EFLN_APPROVED);
//	}
//
//	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
//	{
//		switch (event)
//		{
//		case eFE_Activate:
//			{
//				if(IsPortActive(pActInfo, EIP_RecenterPose))
//				{
//					bool triggered = false;
//					IHMDManager* pHmDManager = gEnv->pSystem->GetHMDManager();
//					IHMDDevice* pDev = pHmDManager ? pHmDManager->GetHMDDevice() : nullptr;
//					if (pDev && pHmDManager->IsStereoSetupOk())
//					{
//						const HMDTrackingState& sensorState = pDev->GetTrackingState();
//						if (sensorState.CheckStatusFlags(HS_IsUsable))
//						{
//							pDev->RecenterPose();
//							triggered = true;
//						}
//					}
//
//					ActivateOutput(pActInfo, triggered ? EOP_Triggered : EOP_Failed, true);
//					ActivateOutput(pActInfo, EOP_Done, true);
//				}
//			}
//			break;
//		}
//	}
//};
//
//class CVRTransformInfo : public CFlowBaseNode<eNCT_Singleton>
//{
//	enum INPUTS
//	{
//		EIP_Enabled = 0,
//	};
//
//	enum OUTPUTS
//	{
//		EOP_CamPos,
//		EOP_CamRot,
//		EOP_CamDataValid,
//		EOP_HmdPos,
//		EOP_HmdRot,
//		EOP_HmdDataValid,
//		EOP_PlayerPos,
//		EOP_PlayerViewRot,
//		EOP_PlayerDataValid
//	};
//
//public:
//	CVRTransformInfo( SActivationInfo * pActInfo )
//	{
//	}
//
//	virtual void GetMemoryUsage(ICrySizer * s) const
//	{
//		s->Add(*this);
//	}
//
//	virtual void GetConfiguration( SFlowNodeConfig &config )
//	{
//		static const SInputPortConfig in_config[] = {
//			InputPortConfig<bool>( "Enabled", true, _HELP("Enable / disable the node") ),
//			{0}
//		};
//		static const SOutputPortConfig out_config[] = {
//			OutputPortConfig<Vec3>("Camera pos", _HELP("The position of the current camera in world coordinates")),
//			OutputPortConfig<Vec3>("Camera rot (PRY)", _HELP("The orientation of the current camera in world coordinates (Pitch,Roll,Yaw) in Degrees")),
//			OutputPortConfig<bool>("Camera valid", _HELP("The camera data shown is valid")),
//			OutputPortConfig<Vec3>("HMD pos", _HELP("The position of the HMD with respect to the recentred pose of the tracker")),
//			OutputPortConfig<Vec3>("HMD rot (PRY)", _HELP("The orientation of the HMD in world coordinates (Pitch,Roll,Yaw) in Degrees")),
//			OutputPortConfig<bool>("HMD valid", _HELP("The HMD data shown is valid")),
//			OutputPortConfig<Vec3>("Player pos", _HELP("The position of the local player in world coordinates")),
//			OutputPortConfig<Vec3>("Player view rot (PRY)", _HELP("The orientation of the player view (Pitch,Roll,Yaw) in Degrees")),
//			OutputPortConfig<bool>("Player valid", _HELP("The HMD data shown is valid")),
//			{0}
//		};
//
//		config.sDescription = _HELP( "This node provides information about the orientation and position of the camera, player and HMD" );
//		config.pInputPorts = in_config;
//		config.pOutputPorts = out_config;
//		config.SetCategory(EFLN_APPROVED);
//	}
//
//	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
//	{
//		switch (event)
//		{
//		case eFE_Initialize:
//			{
//				if (pActInfo->pGraph != nullptr)
//				{
//					const bool enabled = GetPortBool( pActInfo, EIP_Enabled );
//					pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, enabled );
//				}
//			}
//			break;
//
//		case eFE_Activate:
//			{
//				if (IsPortActive(pActInfo, EIP_Enabled)) 
//				{
//					const bool enabled = GetPortBool( pActInfo, EIP_Enabled );
//					pActInfo->pGraph->SetRegularlyUpdated( pActInfo->myID, enabled );
//				}
//			}
//			break;
//
//		case eFE_Update:
//			{
//				// Camera info
//				IRenderer * pRenderer = gEnv->pRenderer;
//				if (pRenderer)
//				{
//					const CCamera &rCam = pRenderer->GetCamera();
//					const Ang3 angles = RAD2DEG(rCam.GetAngles());
//
//					ActivateOutput(pActInfo, EOP_CamPos, rCam.GetPosition());
//					ActivateOutput(pActInfo, EOP_CamRot, Vec3(angles));
//				}
//				
//				ActivateOutput(pActInfo, EOP_CamDataValid, pRenderer != nullptr);
//				
//				// HMD info
//				IHMDManager * pHmdManager = gEnv->pSystem->GetHMDManager();
//				bool bHmdOk = false;
//				if (pHmdManager != nullptr)
//				{	
//					const HMDTrackingState& sensorState = pHmdManager->GetHMDDevice()->GetWorldTrackingState();
//					if (sensorState.CheckStatusFlags(HS_IsUsable))
//					{
//						bHmdOk = true;
//						Ang3 hmdAngles(sensorState.pose.orientation);
//						ActivateOutput(pActInfo, EOP_HmdRot, Vec3(RAD2DEG(hmdAngles)));
//						ActivateOutput(pActInfo, EOP_HmdPos, sensorState.pose.position);
//					}
//				}
//
//				ActivateOutput(pActInfo, EOP_HmdDataValid, bHmdOk);
//
//				// Player Info
//				CPlayer* pLocalPlayer = static_cast<CPlayer*>(g_pGame->GetIGameFramework()->GetClientActor());
//				if (pLocalPlayer)
//				{
//					Vec3 entityRotationInDegrees(RAD2DEG(Ang3(pLocalPlayer->GetViewQuat())));
//					ActivateOutput(pActInfo, EOP_PlayerViewRot, entityRotationInDegrees);
//					ActivateOutput(pActInfo, EOP_PlayerPos, pLocalPlayer->GetEntity()->GetWorldPos());
//				}
//
//				ActivateOutput(pActInfo, EOP_PlayerDataValid, pLocalPlayer != nullptr);
//			}
//			break;
//		}
//	}
//};
//
//
//REGISTER_FLOW_NODE( "VR:Tools", CVRTools);
//REGISTER_FLOW_NODE( "VR:TransformInfo", CVRTransformInfo);
