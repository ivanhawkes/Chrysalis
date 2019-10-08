#include <StdAfx.h>

#include "JointManipulationComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"


namespace Chrysalis
{
static void RegisterJointManipulationComponent(Schematyc::IEnvRegistrar& registrar)
{
	// TODO: This is not how we chain the resistration. I think I'm meant to set it's parent instead.
	//CActorAnimationComponent::Register(componentScope);

	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CJointManipulationComponent));
		// Functions
		{
		}
	}
}


void CJointManipulationComponent::ReflectType(Schematyc::CTypeDesc<CJointManipulationComponent>& desc)
{
	desc.SetGUID(CJointManipulationComponent::IID());
	desc.SetEditorCategory("Miscellaneous");
	desc.SetLabel("Joint Manipulation");
	desc.SetDescription("Control joint movement and rotation.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });

	// HACK: Declared in the base class - they require adding here. It's not DRY principle, it's just what we have.
	desc.AddMember(&CJointManipulationComponent::m_type, 'type', "Type", "Type", "Determines the behavior of the static mesh", Cry::DefaultComponents::EMeshType::RenderAndCollider);
	desc.AddMember(&CJointManipulationComponent::m_characterFile, 'file', "Character", "Character", "Determines the character to load", "");
	desc.AddMember(&CJointManipulationComponent::m_renderParameters, 'rend', "Render", "Rendering Settings", "Settings for the rendered representation of the component", Cry::DefaultComponents::SRenderParameters());
	desc.AddMember(&CJointManipulationComponent::m_databasePath, 'dbpa', "DatabasePath", "Animation Database", "Path to the Mannequin .adb file", "");
	desc.AddMember(&CJointManipulationComponent::m_defaultScopeSettings, 'defs', "DefaultScope", "Default Scope Context Name", "Default Mannequin scope settings", CActorAnimationComponent::SDefaultScopeSettings());
	desc.AddMember(&CJointManipulationComponent::m_bAnimationDrivenMotion, 'andr', "AnimDriven", "Animation Driven Motion", "Whether or not to use root motion in the animations", true);
	desc.AddMember(&CJointManipulationComponent::m_bGroundAlignment, 'grou', "GroundAlign", "Use Ground Alignment", "Enables adjustment of leg positions to align to the ground surface", false);
	desc.AddMember(&CJointManipulationComponent::m_physics, 'phys', "Physics", "Physics", "Physical properties for the object, only used if a simple physics or character controller is applied to the entity.", Cry::DefaultComponents::SPhysicsParameters());

	// Joint manipulation.
	//desc.AddMember(&CJointManipulationComponent::m_jointManipulationProperties, 'jtmp', "JointManipulation", "Joint Manipulation", "Joint Manipulation", CJointManipulationComponent::SJointManipulationProperties());
}


void CJointManipulationComponent::Update(SEntityUpdateContext* pCtx)
{
	CActorAnimationComponent::Update(pCtx);

	// TEST:
	if (auto pCharacterInstance = GetCharacter())
	{
		//auto pISkeletonAnim = pCharacterInstance->GetISkeletonAnim();

		// Get the default skeleton, a read-only construct in which we can get joint identifiers from names
		const IDefaultSkeleton& defaultSkeleton = pCharacterInstance->GetIDefaultSkeleton();

		// Get the internal identifier of our joint, labeled in the DCC (Max / Maya for example)
		const int32 jointId = defaultSkeleton.GetJointIDByName(m_jointManipulationProperties.jointName);

//		const QuatT& jointOrientationModelSpace = pCharacterInstance->GetISkeletonPose()->GetAbsJointByID(jointId);

		// If we can access the attachments, we can try and find the rotation for each hand.	
		if (const IAttachmentManager * pAttachmentManager = pCharacterInstance->GetIAttachmentManager())
		{
			// Is there a needle joint?
			const auto needleIndex = pAttachmentManager->GetIndexByName(m_jointManipulationProperties.attachmentName); // TODO: switch this to needle or have a widget to pick it out of a list
			if (IAttachment * pNeedleAttachment = pAttachmentManager->GetInterfaceByIndex(needleIndex))
			{
				QuatT trans = pNeedleAttachment->GetAttAbsoluteDefault();
				//const float radians = DEG2RAD(m_jointManipulationProperties.needleValue);
				//trans.q = Quat::CreateRotationXYZ(m_jointManipulationProperties.axis * radians);
				//pNeedleAttachment->SetAttAbsoluteDefault(trans);
			}
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterJointManipulationComponent)
}