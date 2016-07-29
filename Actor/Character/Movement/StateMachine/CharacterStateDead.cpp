#include <StdAfx.h>

#include "CharacterStateDead.h"
#include <Game/Game.h>
#include <ConsoleVariables/ConsoleVariables.h>
#include <Actor/Character/Character.h>
/*#include "IActorEventListener.h"
#include "HitDeathReactions.h"
#include "GameRules.h"
#include "GameCodeCoverage/GameCodeCoverageTracker.h"
#include "Audio/Announcer.h"
#include "ActorImpulseHandler.h"
#include "PickAndThrowProxy.h"
#include "AI/AICorpse.h"*/


CCharacterStateDead::CCharacterStateDead()
	: m_swapToCorpseTimeout(10.0f)
	, m_corpseUpdateStatus(eCorpseStatus_WaitingForSwap)
{}


CCharacterStateDead::~CCharacterStateDead()
{}


void CCharacterStateDead::OnEnter(CCharacter& Character)
{
	/*	Character.StopLoopingSounds ();
		Character.SetDeathTimer ();

		if (Character.m_pPickAndThrowProxy)
		{
		Character.m_pPickAndThrowProxy->Unphysicalize ();
		}

		const bool bIsClient = Character.IsClient ();

		if (gEnv->bMultiCharacter)
		{
		if (bIsClient)
		CAudioSignalCharacter::JustPlay ("CharacterDeath_MP_Marine", Character.GetEntityId ());
		}

		CGameRules* pGameRules = g_pGame->GetGameRules ();

		if (bIsClient)
		{
		Character.SetClientSoundmood (CCharacter::ESoundmood_Dead);

		const bool dropHeavyWeapon = true;
		Character.SetBackToNormalWeapon (dropHeavyWeapon);

		bool  itemIsUsed = false;
		bool  itemIsDroppable = false;
		if (!gEnv->bMultiCharacter || (pGameRules->GetCurrentItemForActorWithStatus (&Character, &itemIsUsed, &itemIsDroppable) && !itemIsDroppable))
		{
		Character.HolsterItem (true);
		}

		CCCPOINT (CharacterState_LocalCharacterDied);
		}
		else
		{
		CCCPOINT (CharacterState_NonLocalCharacterDied);
		}

		// Report normal death.
		if (Character.m_CharacterEventListeners.empty () == false)
		{
		CCharacter::TCharacterEventListeners::const_iterator iter = Character.m_CharacterEventListeners.begin ();
		CCharacter::TCharacterEventListeners::const_iterator cur;
		while (iter != Character.m_CharacterEventListeners.end ())
		{
		cur = iter;
		++iter;
		(*cur)->OnDeath (&Character, false);
		}
		}

		pGameRules->OnActorDeath (&Character);
		if (gEnv->bMultiCharacter == false && Character.IsClient ())
		{
		g_pGame->GetIGameFramework ()->GetIGameplayRecorder ()->Event (Character.GetEntity (), eGE_Death);
		}

		Character.SendMusicLogicEvent (eMUSICLOGICEVENT_CHARACTER_KILLED);

		m_swapToCorpseTimeout = g_pGameCVars->g_aiCorpses_DelayTimeToSwap;
		m_corpseUpdateStatus = eCorpseStatus_WaitingForSwap;*/
}


void CCharacterStateDead::OnLeave(CCharacter& Character)
{
	//Character.GetEntity ()->KillTimer (RECYCLE_AI_ACTOR_TIMER_ID);
}


void CCharacterStateDead::OnPrePhysicsUpdate(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime)
{
	// No-one has any idea why this is needed but without the Character animation doesn't execute correctly.
	if (Character.IsClient())
	{
		// STAP must be updated here to ensure that the most recent values are there for the animation processing pass.
		/*const Vec3 cameraPositionForTorso = Character.GetFPCameraPosition (false);
		Character.UpdateFPIKTorso (frameTime, Character.GetCurrentItem (), cameraPositionForTorso);*/
	}
}


void CCharacterStateDead::OnUpdate(CCharacter& Character, const CCharacterStateDead::UpdateCtx& updateCtx)
{
	/*if (Character.m_pHitDeathReactions)
	{
	Character.m_pHitDeathReactions->Update (updateCtx.frameTime);
	}

	if (Character.IsClient ())
	{
	// Need to create this even when Character is dead, otherwise spectators don't see tank turrets rotate etc until they spawn in.
	Character.UpdateClient (updateCtx.frameTime);
	}
	else if (gEnv->bMultiCharacter == false)
	{
	UpdateAICorpseStatus (Character, updateCtx);
	}

	if (Character.GetImpulseHander ())
	{
	Character.GetImpulseHander ()->UpdateDeath (updateCtx.frameTime);
	}

	Character.UpdateBodyDestruction (updateCtx.frameTime);
	Character.GetDamageEffectController ().UpdateEffects (updateCtx.frameTime);*/
}


void CCharacterStateDead::Serialize(TSerialize& serializer)
{
	int corpseUpdateStatus = m_corpseUpdateStatus;

	serializer.Value("AICorpseUpdateStatus", corpseUpdateStatus);
	serializer.Value("AICorpseSwapTimeOut", m_swapToCorpseTimeout);

	if (serializer.IsReading())
	{
		m_corpseUpdateStatus = (EAICorpseUpdateStatus) corpseUpdateStatus;
	}
}


void CCharacterStateDead::UpdateAICorpseStatus(CCharacter& Character, const CCharacterStateDead::UpdateCtx& updateCtx)
{
	/*	const EAICorpseUpdateStatus updateStatus = m_corpseUpdateStatus;

		if (updateStatus == eCorpseStatus_WaitingForSwap)
		{
		const float newForcedRemovalTimeOut = m_swapToCorpseTimeout - updateCtx.frameTime;
		m_swapToCorpseTimeout = newForcedRemovalTimeOut;

		const	bool removeCorpse = (newForcedRemovalTimeOut < 0.0f);
		if (removeCorpse)
		{
		if (Character.GetEntity ()->IsHidden () == false)
		{
		CAICorpseManager* pAICorpseManager = CAICorpseManager::GetInstance ();
		if (pAICorpseManager != NULL)
		{
		CAICorpseManager::SCorpseParameters corpseParams;
		corpseParams.priority = CAICorpseManager::GetPriorityForClass (Character.GetEntity ()->GetClass ());
		pAICorpseManager->SpawnAICorpseFromEntity (*Character.GetEntity (), corpseParams);
		}

		// Hide after spawning the corpse to make sure it is properly recreated
		Character.GetEntity ()->Hide (true);

		IAnimatedCharacter *pAnimatedCharacter = Character.GetAnimatedCharacter ();
		if (pAnimatedCharacter)
		{
		pAnimatedCharacter->UpdateCharacterPtrs ();
		}
		}

		// Schedule recycling (doing during update might end in a crash)
		Character.GetEntity ()->SetTimer (RECYCLE_AI_ACTOR_TIMER_ID, 1000);

		m_corpseUpdateStatus = eCorpseStatus_SwapDone;
		}
		}*/
}
