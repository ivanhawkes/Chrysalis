#pragma once

#include <DefaultComponents/Geometry/BaseMeshComponent.h>
#include <DefaultComponents/Geometry/AdvancedAnimationComponent.h>
#include <bitset>
#include <ICryMannequin.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryCore/Containers/CryArray.h>
#include <CryGame/IGameFramework.h>
#include <Animation/PoseAligner/PoseAligner.h>
#include <Actor/Animation/ActorAnimation.h>

// TEST: entity effect adding a light.
#include <Entities/EntityEffects.h>
#include <entt/entt.hpp>
#include <ECS/Systems/Simulation.h>
#include <ECS/Components/RenderLight.h>
#include <ECS/Components/Spells/Spell.h>
#include <ECS/ECS.h>


namespace Chrysalis
{
class CActorAnimationComponent
	: public Cry::DefaultComponents::CBaseMeshComponent, public IActorAnimationControl
{
protected:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EventFlags GetEventMask() const override;
	// ~IEntityComponent

	// IEditorEntityComponent
	virtual bool SetMaterial(int slotId, const char* szMaterial) override;
	// ~IEditorEntityComponent

public:
	// ***
	// *** ActorAnimationControl
	// ***

	bool HasActionController() const override
	{
		return m_pActionController != nullptr;
	}


	IActionController* GetActionController() const override
	{
		return m_pActionController;
	}

	// ***
	// *** ~ActorAnimationControl
	// ***

	struct SDefaultScopeSettings
	{
		inline bool operator==(const SDefaultScopeSettings& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


		static void ReflectType(Schematyc::CTypeDesc<SDefaultScopeSettings>& desc)
		{
			desc.SetGUID("{B032C89F-15FD-48A1-8B16-24CC9EAE011A}"_cry_guid);
			desc.SetLabel("Actor Animation Component Default Scope");
			desc.SetDescription("Settings for the default Mannequin scope");
		}


		void Serialize(Serialization::IArchive& ar)
		{
			ar(Serialization::MannequinControllerDefinitionPath(m_controllerDefinitionPath), "ControllerDefPath", "Controller Definition");
			ar.doc("Path to the Mannequin controller definition");

			std::shared_ptr<Serialization::SMannequinControllerDefResourceParams> pParams;

			// Load controller definition for the context and fragment selectors.
			if (ar.isEdit())
			{
				pParams = std::make_shared<Serialization::SMannequinControllerDefResourceParams>();

				IAnimationDatabaseManager &animationDatabaseManager = gEnv->pGameFramework->GetMannequinInterface().GetAnimationDatabaseManager();
				if (m_controllerDefinitionPath.size() > 0)
				{
					pParams->pControllerDef = animationDatabaseManager.LoadControllerDef(m_controllerDefinitionPath);
				}
			}

			ar(Serialization::MannequinScopeContextName(m_contextName, pParams), "DefaultScope", "Default Scope Context Name");
			ar.doc("The Mannequin scope context to activate by default");

			ar(Serialization::MannequinFragmentName(m_fragmentName, pParams), "DefaultFragment", "Default Fragment Name");
			ar.doc("The fragment to play by default");
		}

		string m_controllerDefinitionPath;
		string m_contextName;
		string m_fragmentName;
	};

	CActorAnimationComponent() = default;
	virtual ~CActorAnimationComponent();


	static void ReflectType(Schematyc::CTypeDesc<CActorAnimationComponent>& desc)
	{
		desc.SetGUID("{DDBD2129-A579-466F-96A1-7B378A35CAB3}"_cry_guid);
		desc.SetEditorCategory("Geometry");
		desc.SetLabel("Actor Animation");
		desc.SetDescription("Exposes playback of Actor animation using Mannequin.");
		desc.SetIcon("icons:General/Mannequin.ico");
		desc.SetComponentFlags({IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach});

		desc.AddMember(&CActorAnimationComponent::m_type, 'type', "Type", "Type", "Determines the behavior of the static mesh", Cry::DefaultComponents::EMeshType::RenderAndCollider);
		desc.AddMember(&CActorAnimationComponent::m_characterFile, 'file', "Character", "Character", "Determines the character to load", "");
		desc.AddMember(&CActorAnimationComponent::m_materialPath, 'mat', "Material", "Material", "Specifies the override material for the selected object", "");
		desc.AddMember(&CActorAnimationComponent::m_renderParameters, 'rend', "Render", "Rendering Settings", "Settings for the rendered representation of the component", Cry::DefaultComponents::SRenderParameters());

		desc.AddMember(&CActorAnimationComponent::m_databasePath, 'dbpa', "DatabasePath", "Animation Database", "Path to the Mannequin .adb file", "");
		desc.AddMember(&CActorAnimationComponent::m_defaultScopeSettings, 'defs', "DefaultScope", "Default Scope Context Name", "Default Mannequin scope settings", CActorAnimationComponent::SDefaultScopeSettings());
		desc.AddMember(&CActorAnimationComponent::m_bAnimationDrivenMotion, 'andr', "AnimDriven", "Animation Driven Motion", "Whether or not to use root motion in the animations", true);
		desc.AddMember(&CActorAnimationComponent::m_bGroundAlignment, 'grou', "GroundAlign", "Use Ground Alignment", "Enables adjustment of leg positions to align to the ground surface", false);

		desc.AddMember(&CActorAnimationComponent::m_physics, 'phys', "Physics", "Physics", "Physical properties for the object, only used if a simple physics or character controller is applied to the entity.", Cry::DefaultComponents::SPhysicsParameters());
	}


	virtual void ActivateContext(const Schematyc::CSharedString& contextName)
	{
		if (m_pCachedCharacter == nullptr)
		{
			return;
		}

		const TagID scopeContextId = m_pAnimationContext->controllerDef.m_scopeContexts.Find(contextName.c_str());
		if (scopeContextId == TAG_ID_INVALID)
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to find scope context %s in controller definition.", contextName.c_str());
			return;
		}

		// Setting Scope contexts can happen at any time, and what entity or character instance we have bound to a particular scope context
		// can change during the lifetime of an action controller.
		m_pActionController->SetScopeContext(scopeContextId, *m_pEntity, m_pCachedCharacter, m_pDatabase);
	}


	virtual void QueueFragment(const Schematyc::CSharedString& fragmentName)
	{
		if (m_pAnimationContext == nullptr)
		{
			return;
		}

		FragmentID fragmentId = m_pAnimationContext->controllerDef.m_fragmentIDs.Find(fragmentName.c_str());
		if (fragmentId == FRAGMENT_ID_INVALID)
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to find Mannequin fragment %s in controller definition %s", fragmentName.c_str(), m_pAnimationContext->controllerDef.m_filename.c_str());
			return;
		}

		return QueueFragmentWithId(fragmentId);
	}


	virtual void QueueFragmentWithId(const FragmentID& fragmentId)
	{
		if (m_pAnimationContext == nullptr)
		{
			return;
		}

		if (m_pActiveAction)
		{
			m_pActiveAction->Stop();
		}

		const int priority = 0;
		m_pActiveAction = new TAction<SAnimationContext>(priority, fragmentId);
		m_pActionController->Queue(*m_pActiveAction);
	}


	virtual void QueueAction(IAction& action)
	{
		if (m_pActionController)
			m_pActionController->Queue(action);
	}


	virtual void SetTag(const Schematyc::CSharedString& tagName, bool bSet)
	{
		SetTagWithId(GetTagId(tagName.c_str()), bSet);
	}


	virtual void SetMotionParameter(EMotionParamID motionParam, float value)
	{
		CRY_ASSERT(m_pCachedCharacter != nullptr);
		if (m_pCachedCharacter != nullptr)
		{
			m_pCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(motionParam, value, 0.f);
			m_overriddenMotionParams.set(motionParam);
		}
	}


	TagID GetTagId(const char* szTagName) const
	{
		return m_pControllerDefinition->m_tags.Find(szTagName);
	}


	FragmentID GetFragmentId(const char* szFragmentName) const
	{
		return m_pControllerDefinition->m_fragmentIDs.Find(szFragmentName);
	}


	virtual void SetTagWithId(TagID id, bool bSet)
	{
		m_pAnimationContext->state.Set(id, bSet);
	}


	SAnimationContext& GetContext() const override
	{
		return m_pActionController->GetContext();
	}


	IProceduralContext* FindOrCreateProceduralContext(CryClassID classId) const override
	{
		return m_pActionController->FindOrCreateProceduralContext(classId);
	}


	void FlushActions() const
	{
		m_pActionController->Flush();
	}


	void ResumeActions() const
	{
		m_pActionController->Resume();
	}


	void ResetActions() const
	{
		m_pActionController->Reset();
	}


	ICharacterInstance* GetCharacter() const { return m_pCachedCharacter; }


	// Loads character and mannequin data from disk.
	virtual void LoadFromDisk()
	{
		if (m_characterFile.value.size() > 0)
		{
			m_pCachedCharacter = gEnv->pCharacterManager->CreateInstance(m_characterFile.value);
			if (m_pCachedCharacter == nullptr)
			{
				CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load character %s!", m_characterFile.value.c_str());
				return;
			}

			if (m_bGroundAlignment && m_pCachedCharacter != nullptr)
			{
				if (m_pPoseAligner == nullptr)
				{
					CryCreateClassInstance(CPoseAlignerC3::GetCID(), m_pPoseAligner);
				}

				m_pPoseAligner->Clear();
			}
			else
			{
				m_pPoseAligner.reset();
			}
		}
		else
		{
			m_pCachedCharacter = nullptr;
		}

		if (m_defaultScopeSettings.m_controllerDefinitionPath.size() > 0 && m_databasePath.value.size() > 0)
		{
			// Now start loading the Mannequin data.
			IMannequin& mannequinInterface = gEnv->pGameFramework->GetMannequinInterface();
			IAnimationDatabaseManager& animationDatabaseManager = mannequinInterface.GetAnimationDatabaseManager();

			// Load the Mannequin controller definition. This is owned by the animation database manager.
			m_pControllerDefinition = animationDatabaseManager.LoadControllerDef(m_defaultScopeSettings.m_controllerDefinitionPath);
			if (m_pControllerDefinition == nullptr)
			{
				CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load controller definition %s!", m_defaultScopeSettings.m_controllerDefinitionPath.c_str());
				return;
			}

			// Load the animation database.
			m_pDatabase = animationDatabaseManager.Load(m_databasePath.value);
			if (m_pDatabase == nullptr)
			{
				CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Failed to load animation database %s!", m_databasePath.value.c_str());
				return;
			}
		}
	}


	// Resets the actor and Mannequin.
	virtual void ResetCharacter()
	{
		m_pActiveAction = nullptr;

		// Release previous controller and context.
		SAFE_RELEASE(m_pActionController);
		m_pAnimationContext.reset();

		if (m_pCachedCharacter == nullptr)
		{
			return;
		}

		m_pEntity->SetCharacter(m_pCachedCharacter, GetOrMakeEntitySlotId(), false);

		if (!m_materialPath.value.empty())
		{
			if (IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(m_materialPath.value, false))
			{
				m_pEntity->SetSlotMaterial(GetEntitySlotId(), pMaterial);
			}
		}

		SetAnimationDrivenMotion(m_bAnimationDrivenMotion);

		if (m_pControllerDefinition != nullptr)
		{
			// Create a new animation context for the controller definition we loaded above.
			m_pAnimationContext = stl::make_unique<SAnimationContext>(*m_pControllerDefinition);

			// Now create the controller that will be handling animation playback.
			IMannequin& mannequinInterface = gEnv->pGameFramework->GetMannequinInterface();
			m_pActionController = mannequinInterface.CreateActionController(GetEntity(), *m_pAnimationContext);
			CRY_ASSERT(m_pActionController != nullptr);

			if (m_defaultScopeSettings.m_contextName.size() > 0)
			{
				ActivateContext(m_defaultScopeSettings.m_contextName);
			}

			if (m_defaultScopeSettings.m_fragmentName.size() > 0)
			{
				QueueFragment(m_defaultScopeSettings.m_fragmentName);
			}

			m_pEntity->UpdateComponentEventMask(this);
		}

		// TEST: entity effect adding a light.
		// Provide them with an effects controller for this entity.
		ECS::RenderLight renderLight;
		renderLight.projectorOptions.m_texturePath = "chrysalis/textures/lights/flashlight_projector.dds";
		m_effectsController.Init(GetEntityId());
		
		// HACK: Try and get the params from a spell for now.
		entt::entity spellEntity = ECS::Simulation.GetSpellByName("Flashlight");
		if (spellEntity != entt::null)
		{
			auto* spellRegistry = ECS::Simulation.GetSpellRegistry();
			renderLight = spellRegistry->get<ECS::RenderLight>(spellEntity);
		}

		//int slotId = GetOrMakeEntitySlotId();
		int slotId = 0;
		m_effectsController.AttachLight(slotId, "Flashlight", Vec3(0.0f, 0.6f, 1.0f), Vec3(0.0f, 1.0f, 0.0f).normalized(), eGeometrySlot::eIGS_Aux0, renderLight);
	}


	// Enable / disable motion on entity being applied from animation on the root node.
	virtual void SetAnimationDrivenMotion(bool bSet)
	{
		m_bAnimationDrivenMotion = bSet;

		if (m_pCachedCharacter == nullptr)
		{
			return;
		}

		// Disable animation driven motion, note that the function takes the inverted parameter of what you would expect.
		m_pCachedCharacter->GetISkeletonAnim()->SetAnimationDrivenMotion(m_bAnimationDrivenMotion ? 0 : 1);
	}


	bool         IsAnimationDrivenMotionEnabled() const { return m_bAnimationDrivenMotion; }

	virtual void SetCharacterFile(const char* szPath, bool applyImmediately = true);
	const char*  GetCharacterFile() const { return m_characterFile.value.c_str(); }

	virtual void SetMannequinAnimationDatabaseFile(const char* szPath);
	const char*  GetMannequinAnimationDatabaseFile() const { return m_databasePath.value.c_str(); }

	virtual void SetControllerDefinitionFile(const char* szPath);
	const char*  GetControllerDefinitionFile() const { return m_defaultScopeSettings.m_controllerDefinitionPath.c_str(); }

	virtual void SetDefaultScopeContextName(const char* szName);
	const char*  GetDefaultScopeContextName() const { return m_defaultScopeSettings.m_fragmentName.c_str(); }

	virtual void SetDefaultFragmentName(const char* szName);
	const char*  GetDefaultFragmentName() const { return m_defaultScopeSettings.m_fragmentName.c_str(); }

	virtual void EnableGroundAlignment(bool bEnable) { m_bGroundAlignment = bEnable; }
	bool IsGroundAlignmentEnabled() const { return m_bGroundAlignment; }

	virtual bool IsTurning() const { return fabsf(m_turnAngle) > 0; }

	// Helper to allow exposing derived function to Schematyc.
	virtual void SetMeshType(Cry::DefaultComponents::EMeshType type) { SetType(type); }

protected:
	virtual void Update(SEntityUpdateContext* pCtx);

	bool m_bAnimationDrivenMotion = true;

	Schematyc::CharacterFileName m_characterFile;
	Schematyc::MannequinAnimationDatabasePath m_databasePath;
	Schematyc::MaterialFileName m_materialPath;

	SDefaultScopeSettings m_defaultScopeSettings;

	// Run-time info below.
	IActionController* m_pActionController {nullptr};
	std::unique_ptr<SAnimationContext> m_pAnimationContext;
	const IAnimationDatabase* m_pDatabase {nullptr};

	_smart_ptr<IAction> m_pActiveAction;
	std::bitset<eMotionParamID_COUNT> m_overriddenMotionParams;

	const SControllerDef* m_pControllerDefinition {nullptr};
	_smart_ptr<ICharacterInstance> m_pCachedCharacter {nullptr};

	IAnimationPoseAlignerPtr m_pPoseAligner;
	Vec3 m_prevForwardDir {ZERO};
	float m_turnAngle {0.f};

	bool m_bGroundAlignment {false};

	// TEST: entity effect adding a light.
	CEffectsController m_effectsController;
};
}
