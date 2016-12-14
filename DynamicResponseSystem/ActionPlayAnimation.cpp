#include "StdAfx.h"

#include "ActionPlayAnimation.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include <CryDynamicResponseSystem/IDynamicResponseAction.h>


// NOTE: This is a cheap and dirty way to get a token that should always be unique for long enough to not matter any more.
#define PLAY_ANIMATION_TOKEN_BASE 0x0010

static uint32 GetNextToken()
{
	static uint8 s_currentToken { 0 };
	s_currentToken++;
	return PLAY_ANIMATION_TOKEN_BASE + s_currentToken;
}


DRS::IResponseActionInstanceUniquePtr CActionPlayAnimation::Execute(DRS::IResponseInstance* pResponseInstance)
{
	IEntity* pEntity = pResponseInstance->GetCurrentActor()->GetLinkedEntity();
	if (pEntity)
	{
		auto pContextVariables = pResponseInstance->GetContextVariables();

		// DO SOMETHING HERE e.g. populate variables and execute action.
		StartAnimation(pResponseInstance);

		return DRS::IResponseActionInstanceUniquePtr(new CActionPlayAnimationInstance());
	}

	return nullptr;
}


string CActionPlayAnimation::GetVerboseInfo() const
{
	return string("'") + m_targetName + "'";
}


void CActionPlayAnimation::Serialize(Serialization::IArchive& ar)
{
	ar(m_targetName, "TargetName", "^ TargetName");
}


CActionPlayAnimationInstance::CActionPlayAnimationInstance()
{
}


CActionPlayAnimationInstance::~CActionPlayAnimationInstance()
{
}


DRS::IResponseActionInstance::eCurrentState CActionPlayAnimationInstance::Update()
{
	return DRS::IResponseActionInstance::CS_FINISHED;
}


void CActionPlayAnimationInstance::Cancel()
{
}


// Helper functions to make getting values from context variables.

int GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const int default)
{
	auto variable = pContextVariables->GetVariable(name);
	return variable ? variable->GetValueAsInt() : default;
}


float GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const float default)
{
	auto variable = pContextVariables->GetVariable(name);
	return variable ? variable->GetValueAsFloat() : default;
}


CHashedString GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const CHashedString default)
{
	auto variable = pContextVariables->GetVariable(name);
	return variable ? variable->GetValueAsHashedString() : default;
}


bool GetValueOrDefault(const DRS::IVariableCollectionSharedPtr pContextVariables, const CHashedString& name, const bool default)
{
	auto variable = pContextVariables->GetVariable(name);
	return variable ? variable->GetValueAsBool() : default;
}


void CActionPlayAnimation::StartAnimation(DRS::IResponseInstance* pResponseInstance)
{
	if (auto pResponseActor = pResponseInstance->GetCurrentActor())
	{
		IEntity* const pEntity = pResponseActor->GetLinkedEntity();
		ICharacterInstance* pCharacterInstance = (pEntity != nullptr) ? pEntity->GetCharacter(0) : nullptr;
		if (pCharacterInstance)
		{
			CryCharAnimationParams aparams;

			auto pContextVariables = pResponseInstance->GetContextVariables();
			if (pContextVariables)
			{
				// The animation to play.
				CHashedString animationFile = GetValueOrDefault(pContextVariables, "PlayAnimationFile", CHashedString(""));
				
				// Playback parameters.
				aparams.m_fPlaybackSpeed = GetValueOrDefault(pContextVariables, "PlayAnimationSpeed", 1.0f);
				aparams.m_fTransTime = GetValueOrDefault(pContextVariables, "PlayAnimationBlendTime", 0.2f);
				bool isMovementControlled = GetValueOrDefault(pContextVariables, "PlayAnimationMovementIsControled", false);
				aparams.m_nLayerID = m_animationLayer = CLAMP(GetValueOrDefault(pContextVariables, "PlayAnimationLayer", 0), 0, 15);
				aparams.m_nUserToken = GetNextToken();

				// Playback flags.
				bool isLooped = GetValueOrDefault(pContextVariables, "PlayAnimationLooped", false);
				bool shouldRepeatLastFrame = GetValueOrDefault(pContextVariables, "PlayAnimationRepeatLastFrame", false);
				if (isLooped)
				{
					aparams.m_nFlags |= CA_LOOP_ANIMATION;
				}
				else if (shouldRepeatLastFrame)
				{
					aparams.m_nFlags |= CA_REPEAT_LAST_KEY;
				}
				aparams.m_nFlags |= (isMovementControlled && (m_animationLayer == 0)) ? CA_FULL_ROOT_PRIORITY : 0;

				//m_isEntityActivationForced = false;
				//if (GetPortBool(pActInfo, IN_FORCE_UPDATE))
				//{
				//	aparams.m_nFlags |= CA_FORCE_SKELETON_UPDATE;
				//	if (pEntity->IsActive() == false)
				//	{
				//		m_isEntityActivationForced = true;
				//		pEntity->Activate(true); // maybe unforce update as well
				//	}
				//}

				// Get the skeleton and start the animation.
				ISkeletonAnim* pISkeletonAnim = pCharacterInstance->GetISkeletonAnim();
				const bool isStarted = pISkeletonAnim->StartAnimation(animationFile.GetText(), aparams);

				//// TODO: IF we need to send a notification of starting it can go here.


				//if (isStarted)
				//{
				//	m_token = aparams.m_nUserToken;
				//}
				//else
				//{
				//	//make sure game logic continues nevertheless
				//	//ActivateOutput(pActInfo, OUT_ALMOST_DONE, SFlowSystemVoid());
				//	//ActivateOutput(pActInfo, OUT_DONE, SFlowSystemVoid());
				//}

				//pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, true);
				//m_firedAlmostDone = false;

				//if (animationMovementControl)
				//{
				//	bool needsManualUpdate = (m_animationLayer == 0);
				//	if (IGameObject* pGameObject = CCryAction::GetCryAction()->GetGameObject(pEntity->GetId()))
				//	{
				//		if (IAnimatedCharacter* pAnimatedCharacter = static_cast<IAnimatedCharacter*>(pGameObject->QueryExtension("AnimatedCharacter")))
				//		{
				//			needsManualUpdate = false;
				//		}
				//	}

				//	if (needsManualUpdate)
				//	{
				//		pISkeletonAnim->SetAnimationDrivenMotion(1);
				//		m_manualAnimationControlledMovement = true;
				//	}
				//}
			}
			else
			{
				CryLogAlways("You must supply context variables to play an animation.");
			}
		}
	}
}

void CActionPlayAnimation::UpdateAnimation(DRS::IResponseInstance* pResponseInstance)
{
	if (auto pResponseActor = pResponseInstance->GetCurrentActor())
	{
		IEntity* const pEntity = pResponseActor->GetLinkedEntity();
		//bool tokenFound = false;
		//bool almostDone = false;

		//QuatT relativeAnimationMovement(IDENTITY, ZERO);
		//ICharacterInstance* pCharacterInstance = m_token && pEntity ? pEntity->GetCharacter(0) : nullptr;
		//ISkeletonAnim* pSkeletonAnimation = nullptr;
		//if (pCharacterInstance)
		//{
		//	pSkeletonAnimation = pCharacterInstance->GetISkeletonAnim();
		//	for (int i = 0; i < pSkeletonAnimation->GetNumAnimsInFIFO(m_animationLayer); ++i)
		//	{
		//		const CAnimation& animation = pSkeletonAnimation->GetAnimFromFIFO(m_animationLayer, i);
		//		if (animation.HasUserToken(m_token))
		//		{
		//			tokenFound = true;
		//			const float animationTime = pSkeletonAnimation->GetAnimationNormalizedTime(&animation);
		//			almostDone = animationTime > 0.85f;
		//		}
		//	}
		//	relativeAnimationMovement = pSkeletonAnimation->GetRelMovement();
		//}

		//if (almostDone && !m_firedAlmostDone)
		//{
		//	ActivateOutput(pActInfo, OUT_ALMOST_DONE, SFlowSystemVoid());
		//}
		//else if (!tokenFound)
		//{
		//	ActivateOutput(pActInfo, OUT_DONE, SFlowSystemVoid());

		//	pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, false);

		//	m_firedAlmostDone = false;

		//	if (pEntity != nullptr)
		//	{
		//		if (m_isEntityActivationForced)
		//		{
		//			pEntity->Activate(false);
		//			m_isEntityActivationForced = false;
		//		}

		//		if (m_manualAnimationControlledMovement)
		//		{
		//			if (pSkeletonAnimation != nullptr)
		//			{
		//				pSkeletonAnimation->SetAnimationDrivenMotion(0);
		//			}
		//			m_manualAnimationControlledMovement = false;
		//		}

		//		//IScriptTable* pEntityScript = pEntity->GetScriptTable();
		//		//const char* pFuncName = "OnFlowGraphAnimationEnd";
		//		//if (pEntityScript && pEntityScript->HaveValue(pFuncName))
		//		//	Script::CallMethod(pEntityScript, pFuncName);
		//	}
		//}
		//else
		//{
		//	const bool updateMovementManually = m_manualAnimationControlledMovement && (pEntity != nullptr);
		//	if (updateMovementManually)
		//	{
		//		QuatT newWorldLocation = QuatT(pEntity->GetWorldTM()) * relativeAnimationMovement;
		//		newWorldLocation.q.Normalize();

		//		pEntity->SetWorldTM(Matrix34(newWorldLocation));
		//	}
		//}
		//
	}
}


void CActionPlayAnimation::CancelAnimation(DRS::IResponseInstance* pResponseInstance)
{
	if (auto pResponseActor = pResponseInstance->GetCurrentActor())
	{
		IEntity* const pEntity = pResponseActor->GetLinkedEntity();
		if (pEntity)
		{
			if (m_isEntityActivationForced)
			{
				pEntity->Activate(false);
				m_isEntityActivationForced = false;
			}

			ICharacterInstance* pCharacterInstance = pEntity->GetCharacter(0);
			if (pCharacterInstance != nullptr)
			{
				ISkeletonAnim* pSkeletonAnimation = pCharacterInstance->GetISkeletonAnim();
				for (int i = 0; i < pSkeletonAnimation->GetNumAnimsInFIFO(m_animationLayer); ++i)
				{
					CAnimation& anim = pSkeletonAnimation->GetAnimFromFIFO(m_animationLayer, i);
					if (anim.HasUserToken(m_token))
					{
						// TODO: This may not be needed anymore, but we will need to check first before removing it.
						anim.ClearActivated();

						// Remove the animation that matches our token.
						pSkeletonAnimation->RemoveAnimFromFIFO(m_animationLayer, i);
						break;
					}
				}
			}

			// TODO: IF we need to send a notification of cancellation it can go here.
		}
	}
}

