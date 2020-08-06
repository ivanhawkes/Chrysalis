#include <StdAfx.h>

#include "ActorStateDead.h"
#include <Components/Actor/ActorControllerComponent.h>
/*#include "HitDeathReactions.h"
#include "GameRules.h"
#include "GameCodeCoverage/GameCodeCoverageTracker.h"
#include "Audio/Announcer.h"
#include "ActorImpulseHandler.h"
#include "PickAndThrowProxy.h"
#include "AI/AICorpse.h"*/


namespace Chrysalis
{
CActorStateDead::CActorStateDead()
	: m_swapToCorpseTimeout(10.0f)
	, m_corpseUpdateStatus(eCorpseStatus_WaitingForSwap)
{}


CActorStateDead::~CActorStateDead()
{}


void CActorStateDead::OnEnter(CActorControllerComponent& actorControllerComponent)
{
	/*	actorControllerComponent.StopLoopingSounds ();
		actorControllerComponent.SetDeathTimer ();

		if (actorControllerComponent.m_pPickAndThrowProxy)
		{
		actorControllerComponent.m_pPickAndThrowProxy->Unphysicalize ();
		}

		const bool bIsClient = actorControllerComponent.IsClient ();

		if (gEnv->bMultiCharacter)
		{
		if (bIsClient)
		CAudioSignalCharacter::JustPlay ("CharacterDeath_MP_Marine", actorControllerComponent.GetEntityId ());
		}

		CGameRules* pGameRules = gEnv->pGameFramework->GetGameRules ();

		if (bIsClient)
		{
		actorControllerComponent.SetClientSoundmood (CActorControllerComponent::ESoundmood_Dead);

		const bool dropHeavyWeapon = true;
		actorControllerComponent.SetBackToNormalWeapon (dropHeavyWeapon);

		bool  itemIsUsed = false;
		bool  itemIsDroppable = false;
		if (!gEnv->bMultiCharacter || (pGameRules->GetCurrentItemForActorWithStatus (&actorControllerComponent, &itemIsUsed, &itemIsDroppable) && !itemIsDroppable))
		{
		actorControllerComponent.HolsterItem (true);
		}

		CCCPOINT (ActorState_LocalCharacterDied);
		}
		else
		{
		CCCPOINT (ActorState_NonLocalCharacterDied);
		}

		// Report normal death.
		if (!actorControllerComponent.m_CharacterEventListeners.empty ())
		{
		CActorControllerComponent::TCharacterEventListeners::const_iterator iter = actorControllerComponent.m_CharacterEventListeners.begin ();
		CActorControllerComponent::TCharacterEventListeners::const_iterator cur;
		while (iter != actorControllerComponent.m_CharacterEventListeners.end ())
		{
		cur = iter;
		++iter;
		(*cur)->OnDeath (&actorControllerComponent, false);
		}
		}

		pGameRules->OnActorDeath (&actorControllerComponent);
		if (!gEnv->bMultiCharacter && actorControllerComponent.IsClient ())
		{
			gEnv->pGameFramework->GetIGameplayRecorder ()->Event (actorControllerComponent.GetEntity (), eGE_Death);
		}

		actorControllerComponent.SendMusicLogicEvent (eMUSICLOGICEVENT_CHARACTER_KILLED);

		m_swapToCorpseTimeout = g_pGameCVars->g_aiCorpses_DelayTimeToSwap;
		m_corpseUpdateStatus = eCorpseStatus_WaitingForSwap;*/
}


void CActorStateDead::OnLeave(CActorControllerComponent& actorControllerComponent)
{
	//actorControllerComponent.GetEntity ()->KillTimer (RECYCLE_AI_ACTOR_TIMER_ID);
}


void CActorStateDead::OnPrePhysicsUpdate(CActorControllerComponent& actorControllerComponent, float frameTime)
{
	//// No-one has any idea why this is needed but without the actorControllerComponent animation doesn't execute correctly.
	//if (actorControllerComponent.IsClient())
	//{
	//	// STAP must be updated here to ensure that the most recent values are there for the animation processing pass.
	//	/*const Vec3 cameraPositionForTorso = actorControllerComponent.GetFPCameraPosition (false);
	//	actorControllerComponent.UpdateFPIKTorso (frameTime, actorControllerComponent.GetCurrentItem (), cameraPositionForTorso);*/
	//}
}


void CActorStateDead::OnUpdate(CActorControllerComponent& actorControllerComponent, const CActorStateDead::UpdateCtx& updateCtx)
{
	/*if (actorControllerComponent.m_pHitDeathReactions)
	{
	actorControllerComponent.m_pHitDeathReactions->Update (updateCtx.frameTime);
	}

	if (actorControllerComponent.IsClient ())
	{
	// Need to create this even when actorControllerComponent is dead, otherwise spectators don't see tank turrets rotate etc until they spawn in.
	actorControllerComponent.UpdateClient (updateCtx.frameTime);
	}
	else if (!gEnv->bMultiCharacter)
	{
	UpdateAICorpseStatus (actorControllerComponent, updateCtx);
	}

	if (actorControllerComponent.GetImpulseHander ())
	{
	actorControllerComponent.GetImpulseHander ()->UpdateDeath (updateCtx.frameTime);
	}

	actorControllerComponent.UpdateBodyDestruction (updateCtx.frameTime);
	actorControllerComponent.GetDamageEffectController ().UpdateEffects (updateCtx.frameTime);*/
}


void CActorStateDead::Serialize(TSerialize& serializer)
{
	int corpseUpdateStatus = m_corpseUpdateStatus;

	serializer.Value("AICorpseUpdateStatus", corpseUpdateStatus);
	serializer.Value("AICorpseSwapTimeOut", m_swapToCorpseTimeout);

	if (serializer.IsReading())
	{
		m_corpseUpdateStatus = (EAICorpseUpdateStatus)corpseUpdateStatus;
	}
}


void CActorStateDead::UpdateAICorpseStatus(CActorControllerComponent& actorControllerComponent, const CActorStateDead::UpdateCtx& updateCtx)
{
	/*	const EAICorpseUpdateStatus updateStatus = m_corpseUpdateStatus;

		if (updateStatus == eCorpseStatus_WaitingForSwap)
		{
		const float newForcedRemovalTimeOut = m_swapToCorpseTimeout - updateCtx.frameTime;
		m_swapToCorpseTimeout = newForcedRemovalTimeOut;

		const	bool removeCorpse = (newForcedRemovalTimeOut < 0.0f);
		if (removeCorpse)
		{
		if (!actorControllerComponent.GetEntity ()->IsHidden ())
		{
		CAICorpseManager* pAICorpseManager = CAICorpseManager::GetInstance ();
		if (pAICorpseManager != nullptr)
		{
		CAICorpseManager::SCorpseParameters corpseParams;
		corpseParams.priority = CAICorpseManager::GetPriorityForClass (actorControllerComponent.GetEntity ()->GetClass ());
		pAICorpseManager->SpawnAICorpseFromEntity (*actorControllerComponent.GetEntity (), corpseParams);
		}

		// Hide after spawning the corpse to make sure it is properly recreated
		actorControllerComponent.GetEntity ()->Hide (true);

		IAnimatedCharacter *pAnimatedCharacter = actorControllerComponent.GetAnimatedCharacter ();
		if (pAnimatedCharacter)
		{
		pAnimatedCharacter->UpdateCharacterPtrs ();
		}
		}

		// Schedule recycling (doing during update might end in a crash)
		actorControllerComponent.GetEntity ()->SetTimer (RECYCLE_AI_ACTOR_TIMER_ID, 1000);

		m_corpseUpdateStatus = eCorpseStatus_SwapDone;
		}
		}*/
}
}