#include "StdAfx.h"

#include "ActorAnimationComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include <Cry3DEngine/IRenderNode.h>


// HACK: Am I seriously copying this code segment from CryDefaultEntities in order to get rid of the YASLI
// serialisation issue? Yes, yes I am. Dirty, dirty, dirty.

namespace Cry
{
namespace DefaultComponents
{
YASLI_ENUM_BEGIN_NESTED(SPhysicsParameters, EWeightType, "WeightType")
YASLI_ENUM_VALUE_NESTED(SPhysicsParameters, EWeightType::Mass, "Mass")
YASLI_ENUM_VALUE_NESTED(SPhysicsParameters, EWeightType::Density, "Density")
YASLI_ENUM_VALUE_NESTED(SPhysicsParameters, EWeightType::Immovable, "Immovable")
YASLI_ENUM_END()
}
}


namespace Chrysalis
{
static void RegisterActorAnimationComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CActorAnimationComponent));
		// Functions
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CActorAnimationComponent::ActivateContext, "{932F9FBF-DCEC-405C-B593-FB9AF34211A7}"_cry_guid, "ActivateContext");
			pFunction->SetDescription("Activates a Mannequin context");
			pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
			pFunction->BindInput(1, 'cont', "Context Name");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CActorAnimationComponent::QueueFragment, "{F3A04F87-9B9D-463D-AAFF-D4B6621EDED3}"_cry_guid, "QueueFragment");
			pFunction->SetDescription("Queues a Mannequin fragment for playback");
			pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
			pFunction->BindInput(1, 'frag', "Fragment Name");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CActorAnimationComponent::SetTag, "{5F93657F-6817-4185-84A4-A982E1177402}"_cry_guid, "SetTag");
			pFunction->SetDescription("Sets a Mannequin tag's state to true or false");
			pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
			pFunction->BindInput(1, 'tagn', "Tag Name");
			pFunction->BindInput(2, 'set', "Set");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CActorAnimationComponent::SetMotionParameter, "{C7364382-A603-4EEB-8751-A6D0E775C71B}"_cry_guid, "SetMotionParameter");
			pFunction->SetDescription("Sets a motion parameter to affect a blend space");
			pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
			pFunction->BindInput(1, 'mtnp', "Motion Parameter");
			pFunction->BindInput(2, 'val', "Value");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CActorAnimationComponent::SetMeshType, "{64BC1CCD-E1E9-4A6A-9FB3-D8F4C0CA8C95}"_cry_guid, "SetType");
			pFunction->BindInput(1, 'type', "Type");
			pFunction->SetDescription("Changes the type of the object");
			pFunction->SetFlags({Schematyc::EEnvFunctionFlags::Member});
			componentScope.Register(pFunction);
		}
	}
}


CActorAnimationComponent::~CActorAnimationComponent()
{
	SAFE_RELEASE(m_pActionController);
}


void CActorAnimationComponent::Initialize()
{
	LoadFromDisk();

	ResetCharacter();
}


void CActorAnimationComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		case EEntityEvent::EditorPropertyChanged:
		{
			LoadFromDisk();
			ResetCharacter();

			// Update Editor UI to show the default object material
			if (m_materialPath.value.empty() && m_pCachedCharacter != nullptr)
			{
				if (IMaterial* pMaterial = m_pCachedCharacter->GetMaterial())
				{
					m_materialPath = pMaterial->GetName();
				}
			}
		}
		break;

		case EEntityEvent::Update:
		{
			SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];
			Update(pCtx);
		}
		break;

		case EEntityEvent::AnimationEvent:
		{
			if (m_pActionController != nullptr)
			{
				const AnimEventInstance *pAnimEvent = reinterpret_cast<const AnimEventInstance*>(event.nParam[0]);
				ICharacterInstance *pCharacter = reinterpret_cast<ICharacterInstance*>(event.nParam[1]);

				m_pActionController->OnAnimationEvent(pCharacter, *pAnimEvent);
			}
		}
		break;
	}

	CBaseMeshComponent::ProcessEvent(event);
}


Cry::Entity::EventFlags CActorAnimationComponent::GetEventMask() const
{
	Cry::Entity::EventFlags bitFlags = CBaseMeshComponent::GetEventMask() | EEntityEvent::EditorPropertyChanged;

	if (m_pPoseAligner != nullptr)
	{
		bitFlags |= EEntityEvent::Update;
	}

	if (m_pActionController != nullptr)
	{
		bitFlags |= EEntityEvent::Update | EEntityEvent::AnimationEvent;
	}

	return bitFlags;
}


void CActorAnimationComponent::Update(SEntityUpdateContext* pCtx)
{
	// TEST:!!!
	//auto count = GetEntity()->GetSlotCount();
	//auto slotId = GetEntitySlotId();
	if (ICharacterInstance* pCharacter = m_pEntity->GetCharacter(GetEntitySlotId()))
	{
		//auto pISkeletonAnim = pCharacter->GetISkeletonAnim();
	}

	if (m_pActionController != nullptr)
	{
		m_pActionController->Update(pCtx->fFrameTime);
	}

	Matrix34 characterTransform = GetWorldTransformMatrix();

	// Set turn rate as the difference between previous and new entity rotation
	m_turnAngle = Ang3::CreateRadZ(characterTransform.GetColumn1(), m_prevForwardDir) / pCtx->fFrameTime;
	m_prevForwardDir = characterTransform.GetColumn1();

	if (m_pCachedCharacter != nullptr)
	{
		if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysicalEntity())
		{
			pe_status_dynamics dynStatus;
			if (pPhysicalEntity->GetStatus(&dynStatus))
			{
				float travelAngle = Ang3::CreateRadZ(characterTransform.GetColumn1(), dynStatus.v.GetNormalized());
				float travelSpeed = dynStatus.v.GetLength2D();

				// Set the travel speed based on the physics velocity magnitude
				// Keep in mind that the maximum number for motion parameters is 10.
				// If your velocity can reach a magnitude higher than this, divide by the maximum theoretical account and work with a 0 - 1 ratio.
				if (!m_overriddenMotionParams.test(eMotionParamID_TravelSpeed))
				{
					m_pCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TravelSpeed, travelSpeed, 0.f);
				}

				// Update the turn speed in CryAnimation, note that the maximum motion parameter (10) applies here too.
				if (!m_overriddenMotionParams.test(eMotionParamID_TurnAngle))
				{
					m_pCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TurnAngle, m_turnAngle, 0.f);
				}

				if (!m_overriddenMotionParams.test(eMotionParamID_TravelAngle))
				{
					m_pCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TravelAngle, travelAngle, 0.f);
				}
			}
		}

		if (m_pPoseAligner != nullptr && m_pPoseAligner->Initialize(*m_pEntity, m_pCachedCharacter))
		{
			m_pPoseAligner->SetBlendWeight(1.f);
			m_pPoseAligner->Update(m_pCachedCharacter, QuatT(characterTransform), pCtx->fFrameTime);
		}
	}

	m_overriddenMotionParams.reset();
}


void CActorAnimationComponent::SetCharacterFile(const char* szPath, bool applyImmediately)
{
	m_characterFile = szPath;
	LoadFromDisk();

	if (applyImmediately)
	{
		ResetCharacter();
	}
}


void CActorAnimationComponent::SetMannequinAnimationDatabaseFile(const char* szPath)
{
	m_databasePath = szPath;
}


void CActorAnimationComponent::SetControllerDefinitionFile(const char* szPath)
{
	m_defaultScopeSettings.m_controllerDefinitionPath = szPath;
}


void CActorAnimationComponent::SetDefaultScopeContextName(const char* szName)
{
	m_defaultScopeSettings.m_contextName = szName;
}


void CActorAnimationComponent::SetDefaultFragmentName(const char* szName)
{
	m_defaultScopeSettings.m_fragmentName = szName;
}


bool CActorAnimationComponent::SetMaterial(int slotId, const char* szMaterial)
{
	if (slotId == GetEntitySlotId())
	{
		if (IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(szMaterial, false))
		{
			m_materialPath = szMaterial;
			m_pEntity->SetSlotMaterial(GetEntitySlotId(), pMaterial);
		}
		else if (szMaterial[0] == '\0')
		{
			m_materialPath.value.clear();
			m_pEntity->SetSlotMaterial(GetEntitySlotId(), nullptr);
		}

		return true;
	}

	return false;
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterActorAnimationComponent)
}
