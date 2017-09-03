#include <StdAfx.h>

#include "ActorStateDead.h"
#include <Actor/Actor.h>
/*#include "IActorEventListener.h"
#include "HitDeathReactions.h"
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


void CActorStateDead::OnEnter(IActorComponent& actorComponent)
{
	/*	actorComponent.StopLoopingSounds ();
		actorComponent.SetDeathTimer ();

		if (actorComponent.m_pPickAndThrowProxy)
		{
		actorComponent.m_pPickAndThrowProxy->Unphysicalize ();
		}

		const bool bIsClient = actorComponent.IsClient ();

		if (gEnv->bMultiCharacter)
		{
		if (bIsClient)
		CAudioSignalCharacter::JustPlay ("CharacterDeath_MP_Marine", actorComponent.GetEntityId ());
		}

		CGameRules* pGameRules = gEnv->pGameFramework->GetGameRules ();

		if (bIsClient)
		{
		actorComponent.SetClientSoundmood (IActorComponent::ESoundmood_Dead);

		const bool dropHeavyWeapon = true;
		actorComponent.SetBackToNormalWeapon (dropHeavyWeapon);

		bool  itemIsUsed = false;
		bool  itemIsDroppable = false;
		if (!gEnv->bMultiCharacter || (pGameRules->GetCurrentItemForActorWithStatus (&actorComponent, &itemIsUsed, &itemIsDroppable) && !itemIsDroppable))
		{
		actorComponent.HolsterItem (true);
		}

		CCCPOINT (ActorState_LocalCharacterDied);
		}
		else
		{
		CCCPOINT (ActorState_NonLocalCharacterDied);
		}

		// Report normal death.
		if (!actorComponent.m_CharacterEventListeners.empty ())
		{
		IActorComponent::TCharacterEventListeners::const_iterator iter = actorComponent.m_CharacterEventListeners.begin ();
		IActorComponent::TCharacterEventListeners::const_iterator cur;
		while (iter != actorComponent.m_CharacterEventListeners.end ())
		{
		cur = iter;
		++iter;
		(*cur)->OnDeath (&actorComponent, false);
		}
		}

		pGameRules->OnActorDeath (&actorComponent);
		if (!gEnv->bMultiCharacter && actorComponent.IsClient ())
		{
			gEnv->pGameFramework->GetIGameplayRecorder ()->Event (actorComponent.GetEntity (), eGE_Death);
		}

		actorComponent.SendMusicLogicEvent (eMUSICLOGICEVENT_CHARACTER_KILLED);

		m_swapToCorpseTimeout = g_pGameCVars->g_aiCorpses_DelayTimeToSwap;
		m_corpseUpdateStatus = eCorpseStatus_WaitingForSwap;*/
}


void CActorStateDead::OnLeave(IActorComponent& actorComponent)
{
	//actorComponent.GetEntity ()->KillTimer (RECYCLE_AI_ACTOR_TIMER_ID);
}


void CActorStateDead::OnPrePhysicsUpdate(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, float frameTime)
{
	//// No-one has any idea why this is needed but without the actorComponent animation doesn't execute correctly.
	//if (actorComponent.IsClient())
	//{
	//	// STAP must be updated here to ensure that the most recent values are there for the animation processing pass.
	//	/*const Vec3 cameraPositionForTorso = actorComponent.GetFPCameraPosition (false);
	//	actorComponent.UpdateFPIKTorso (frameTime, actorComponent.GetCurrentItem (), cameraPositionForTorso);*/
	//}
}


void CActorStateDead::OnUpdate(IActorComponent& actorComponent, const CActorStateDead::UpdateCtx& updateCtx)
{
	/*if (actorComponent.m_pHitDeathReactions)
	{
	actorComponent.m_pHitDeathReactions->Update (updateCtx.frameTime);
	}

	if (actorComponent.IsClient ())
	{
	// Need to create this even when actorComponent is dead, otherwise spectators don't see tank turrets rotate etc until they spawn in.
	actorComponent.UpdateClient (updateCtx.frameTime);
	}
	else if (!gEnv->bMultiCharacter)
	{
	UpdateAICorpseStatus (actorComponent, updateCtx);
	}

	if (actorComponent.GetImpulseHander ())
	{
	actorComponent.GetImpulseHander ()->UpdateDeath (updateCtx.frameTime);
	}

	actorComponent.UpdateBodyDestruction (updateCtx.frameTime);
	actorComponent.GetDamageEffectController ().UpdateEffects (updateCtx.frameTime);*/
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


void CActorStateDead::UpdateAICorpseStatus(IActorComponent& actorComponent, const CActorStateDead::UpdateCtx& updateCtx)
{
	/*	const EAICorpseUpdateStatus updateStatus = m_corpseUpdateStatus;

		if (updateStatus == eCorpseStatus_WaitingForSwap)
		{
		const float newForcedRemovalTimeOut = m_swapToCorpseTimeout - updateCtx.frameTime;
		m_swapToCorpseTimeout = newForcedRemovalTimeOut;

		const	bool removeCorpse = (newForcedRemovalTimeOut < 0.0f);
		if (removeCorpse)
		{
		if (!actorComponent.GetEntity ()->IsHidden ())
		{
		CAICorpseManager* pAICorpseManager = CAICorpseManager::GetInstance ();
		if (pAICorpseManager != NULL)
		{
		CAICorpseManager::SCorpseParameters corpseParams;
		corpseParams.priority = CAICorpseManager::GetPriorityForClass (actorComponent.GetEntity ()->GetClass ());
		pAICorpseManager->SpawnAICorpseFromEntity (*actorComponent.GetEntity (), corpseParams);
		}

		// Hide after spawning the corpse to make sure it is properly recreated
		actorComponent.GetEntity ()->Hide (true);

		IAnimatedCharacter *pAnimatedCharacter = actorComponent.GetAnimatedCharacter ();
		if (pAnimatedCharacter)
		{
		pAnimatedCharacter->UpdateCharacterPtrs ();
		}
		}

		// Schedule recycling (doing during update might end in a crash)
		actorComponent.GetEntity ()->SetTimer (RECYCLE_AI_ACTOR_TIMER_ID, 1000);

		m_corpseUpdateStatus = eCorpseStatus_SwapDone;
		}
		}*/
}
}