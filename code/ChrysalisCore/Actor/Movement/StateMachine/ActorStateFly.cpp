
#include <StdAfx.h>

#include <CryRenderer/IRenderer.h>
#include <CryPhysics/physinterface.h>
#include "ActorStateFly.h"
#include <Components/Actor/ActorControllerComponent.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
//#include "CharacterInput.h"


namespace Chrysalis
{
CActorStateFly::CActorStateFly()
	: m_flyMode(0)
{}


void CActorStateFly::OnEnter(CActorControllerComponent& actorControllerComponent)
{
	// #TODO: Add this back in.

	//pe_player_dynamics simPar;

	//IPhysicalEntity* piPhysics = actorControllerComponent.GetEntity ()->GetPhysics ();
	//if (!piPhysics || piPhysics->GetParams (&simPar) == 0)
	//{
	//	return;
	//}

	//actorControllerComponent.m_actorPhysics.velocity = actorControllerComponent.m_actorPhysics.velocityUnconstrained.Set (0, 0, 0);
	//actorControllerComponent.m_actorPhysics.speed = actorControllerComponent.GetActorState()->speedFlat = 0.0f;
	//actorControllerComponent.m_actorPhysics.groundMaterialIdx = -1;
	//actorControllerComponent.m_actorPhysics.gravity = simPar.gravity;

	//actorControllerComponent.GetActorState()->fallSpeed = 0.0f;
	//actorControllerComponent.GetActorState()->inFiring = 0;

	CActorStateUtility::PhySetFly(actorControllerComponent);
}


bool CActorStateFly::OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent, float frameTime)
{
	// #TODO: Add this back in.
	//	if (actorControllerComponent.GetActorState()->spectatorInfo.mode == 0)
	//{
	//	const uint8 flyMode = actorControllerComponent.GetFlyMode ();

	//	if (m_flyMode != flyMode)
	//	{
	//		if (flyMode == 0)
	//			return false;

	//		m_flyModeDisplayTime = 2.f;
	//		m_flyMode = flyMode;
	//	}
	//	else
	//	{
	//		if (m_flyModeDisplayTime > 0.f)
	//		{
	//			char buffer [20];

	//			switch (flyMode)
	//			{
	//				case 1:
	//					sprintf (buffer, "FlyMode ON");
	//					break;

	//				case 2:
	//					sprintf (buffer, "FlyMode/NoClip ON");
	//					break;
	//			}

	//			IRenderAuxText::Draw2dLabel (20.f, 20.f, 1.5f, Col_White, 0, buffer);
	//			m_flyModeDisplayTime -= gEnv->pTimer->GetFrameTime ();
	//		}
	//	}
	//}

	//ProcessFlyMode (actorControllerComponent);

	//return true;

	return false;
}


void CActorStateFly::OnExit(CActorControllerComponent& actorControllerComponent)
{
	//	actorControllerComponent.CreateScriptEvent("printhud", 0, "FlyMode/NoClip OFF");

	pe_player_dynamics simPar;

	IPhysicalEntity* piPhysics = actorControllerComponent.GetEntity()->GetPhysics();
	if (!piPhysics || piPhysics->GetParams(&simPar) == 0)
	{
		return;
	}

	CActorStateUtility::PhySetNoFly(actorControllerComponent, simPar.gravity);
}


void CActorStateFly::ProcessFlyMode(CActorControllerComponent& actorControllerComponent)
{
	// #TODO: Add this back in.

	//const Quat viewQuat = actorControllerComponent.GetViewQuat();
	//Vec3 move = viewQuat * actorControllerComponent.GetVelocity();

	//float zMove(0.0f);
	//{
	//	const ICharacterInput* piCharacterInput = actorControllerComponent.GetCharacterInput();
	//	if (piCharacterInput->GetType() == ICharacterInput::CHARACTER_INPUT)
	//	{
	//		if (actorControllerComponent.m_jumpButtonIsPressed)
	//			zMove += 1.0f;
	//		if (static_cast<const CCharacterInput*> (piCharacterInput)->IsCrouchButtonDown())
	//			zMove -= 1.0f;
	//	}
	//}

	//move += viewQuat.GetColumn2() * zMove;

	//// Cap the movement vector to max 1
	//float moveModule(move.len());

	//if (moveModule > 1.0f)
	//	move /= moveModule;

	//move *= actorControllerComponent.GetTotalSpeedMultiplier() * actorControllerComponent.GetWeaponMovementFactor();
	//move *= 30.0f;

	//if (actorControllerComponent.m_actions & ACTION_SPRINT)
	//	move *= 10.0f;

	//CActorStateUtility::PhySetFly(actorControllerComponent);

	//actorControllerComponent.GetMoveRequest().type = eCMT_Fly;
	//actorControllerComponent.GetMoveRequest().velocity = move;
}
}