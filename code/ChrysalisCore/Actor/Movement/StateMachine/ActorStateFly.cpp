
#include <StdAfx.h>

#include <CryRenderer/IRenderer.h>
#include <CryPhysics/physinterface.h>
#include "ActorStateFly.h"
#include <Actor/Actor.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
//#include "CharacterInput.h"


namespace Chrysalis
{
CActorStateFly::CActorStateFly()
	: m_flyMode(0)
{}


void CActorStateFly::OnEnter(IActorComponent& actorComponent)
{
	// #TODO: Add this back in.

	//pe_player_dynamics simPar;

	//IPhysicalEntity* piPhysics = actorComponent.GetEntity ()->GetPhysics ();
	//if (!piPhysics || piPhysics->GetParams (&simPar) == 0)
	//{
	//	return;
	//}

	//actorComponent.m_actorPhysics.velocity = actorComponent.m_actorPhysics.velocityUnconstrained.Set (0, 0, 0);
	//actorComponent.m_actorPhysics.speed = actorComponent.GetActorState()->speedFlat = 0.0f;
	//actorComponent.m_actorPhysics.groundMaterialIdx = -1;
	//actorComponent.m_actorPhysics.gravity = simPar.gravity;

	//actorComponent.GetActorState()->fallSpeed = 0.0f;
	//actorComponent.GetActorState()->inFiring = 0;

	CActorStateUtility::PhySetFly(actorComponent);
}


bool CActorStateFly::OnPrePhysicsUpdate(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, float frameTime)
{
	// #TODO: Add this back in.
	//	if (actorComponent.GetActorState()->spectatorInfo.mode == 0)
	//{
	//	const uint8 flyMode = actorComponent.GetFlyMode ();

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

	//ProcessFlyMode (actorComponent, movementRequest);

	//return true;

	return false;
}


void CActorStateFly::OnExit(IActorComponent& actorComponent)
{
	//	actorComponent.CreateScriptEvent("printhud", 0, "FlyMode/NoClip OFF");

	pe_player_dynamics simPar;

	IPhysicalEntity* piPhysics = actorComponent.GetEntity()->GetPhysics();
	if (!piPhysics || piPhysics->GetParams(&simPar) == 0)
	{
		return;
	}

	CActorStateUtility::PhySetNoFly(actorComponent, simPar.gravity);
}


void CActorStateFly::ProcessFlyMode(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest)
{
	// #TODO: Add this back in.

	//const Quat viewQuat = actorComponent.GetViewQuat ();
	//Vec3 move = viewQuat * movementRequest.desiredVelocity;

	//float zMove (0.0f);
	//{
	//	const ICharacterInput* piCharacterInput = actorComponent.GetCharacterInput ();
	//	if (piCharacterInput->GetType () == ICharacterInput::CHARACTER_INPUT)
	//	{
	//		if (actorComponent.m_jumpButtonIsPressed)
	//			zMove += 1.0f;
	//		if (static_cast<const CCharacterInput*> (piCharacterInput)->IsCrouchButtonDown ())
	//			zMove -= 1.0f;
	//	}
	//}
	//
	//move += viewQuat.GetColumn2 () * zMove;

	//// Cap the movement vector to max 1
	//float moveModule (move.len ());

	//if (moveModule > 1.0f)
	//	move /= moveModule;

	//move *= actorComponent.GetTotalSpeedMultiplier () * actorComponent.GetWeaponMovementFactor ();
	//move *= 30.0f;

	//if (actorComponent.m_actions & ACTION_SPRINT)
	//	move *= 10.0f;

	//CActorStateUtility::PhySetFly (actorComponent);

	//actorComponent.GetMoveRequest ().type = eCMT_Fly;
	//actorComponent.GetMoveRequest ().velocity = move;
}
}