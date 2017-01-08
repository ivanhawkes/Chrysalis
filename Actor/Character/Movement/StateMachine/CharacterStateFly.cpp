
#include <StdAfx.h>

#include <CryRenderer/IRenderer.h>
#include <CryPhysics/physinterface.h>
#include "CharacterStateFly.h"
#include <Actor/Character/Character.h>
#include <Actor/Movement/StateMachine/ActorStateUtility.h>
//#include "CharacterInput.h"


CCharacterStateFly::CCharacterStateFly()
	: m_flyMode(0)
{}


void CCharacterStateFly::OnEnter(CCharacter& Character)
{
	// TODO: Add this back in.

	//pe_player_dynamics simPar;

	//IPhysicalEntity* piPhysics = Character.GetEntity ()->GetPhysics ();
	//if (!piPhysics || piPhysics->GetParams (&simPar) == 0)
	//{
	//	return;
	//}

	//Character.m_actorPhysics.velocity = Character.m_actorPhysics.velocityUnconstrained.Set (0, 0, 0);
	//Character.m_actorPhysics.speed = Character.GetActorState()->speedFlat = 0.0f;
	//Character.m_actorPhysics.groundMaterialIdx = -1;
	//Character.m_actorPhysics.gravity = simPar.gravity;

	//Character.GetActorState()->fallSpeed = 0.0f;
	//Character.GetActorState()->inFiring = 0;

	CCharacterStateUtil::PhySetFly(Character);
}


bool CCharacterStateFly::OnPrePhysicsUpdate(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime)
{
	// TODO: Add this back in.
	//	if (Character.GetActorState()->spectatorInfo.mode == 0)
	//{
	//	const uint8 flyMode = Character.GetFlyMode ();

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

	//ProcessFlyMode (Character, movementRequest);

	//return true;

	return false;
}


void CCharacterStateFly::OnExit(CCharacter& Character)
{
	Character.CreateScriptEvent("printhud", 0, "FlyMode/NoClip OFF");

	pe_player_dynamics simPar;

	IPhysicalEntity* piPhysics = Character.GetEntity()->GetPhysics();
	if (!piPhysics || piPhysics->GetParams(&simPar) == 0)
	{
		return;
	}

	CCharacterStateUtil::PhySetNoFly(Character, simPar.gravity);
}


void CCharacterStateFly::ProcessFlyMode(CCharacter& Character, const SActorMovementRequest& movementRequest)
{
	// TODO: Add this back in.

	//const Quat viewQuat = Character.GetViewQuat ();
	//Vec3 move = viewQuat * movementRequest.desiredVelocity;

	//float zMove (0.0f);
	//{
	//	const ICharacterInput* piCharacterInput = Character.GetCharacterInput ();
	//	if (piCharacterInput->GetType () == ICharacterInput::CHARACTER_INPUT)
	//	{
	//		if (Character.m_jumpButtonIsPressed)
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

	//move *= Character.GetTotalSpeedMultiplier () * Character.GetWeaponMovementFactor ();
	//move *= 30.0f;

	//if (Character.m_actions & ACTION_SPRINT)
	//	move *= 10.0f;

	//CCharacterStateUtil::PhySetFly (Character);

	//Character.GetMoveRequest ().type = eCMT_Fly;
	//Character.GetMoveRequest ().velocity = move;
}
