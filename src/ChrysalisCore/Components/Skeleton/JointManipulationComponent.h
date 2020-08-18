#pragma once

#include "Components/Animation/ActorAnimationComponent.h"


/**
A joint manipulation extension.
**/
namespace Chrysalis
{
class CJointManipulationComponent
	: public CActorAnimationComponent
	//: public IEntityComponent
{
public:
	CJointManipulationComponent() {}
	virtual ~CJointManipulationComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CJointManipulationComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{F16D56F8-9EFD-47FB-A089-96277C31D4E7}"_cry_guid;
		return id;
	}


	virtual void Serialize(Serialization::IArchive& ar) override
	{
		Serialization::SContext context(ar, this);
		ar(m_jointManipulationProperties, "Joints", "Joints");
	}


	struct SJointManipulationProperties
	{
		inline bool operator==(const SJointManipulationProperties& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


		static void ReflectType(Schematyc::CTypeDesc<SJointManipulationProperties>& desc)
		{
			desc.SetGUID("{1193B2C2-B77B-4651-9A8E-AF468868F2C0}"_cry_guid);
			desc.SetLabel("Joint Manipulation Properties");
			desc.SetDescription("Perform operations on joints.");
		}


		virtual void Serialize(Serialization::IArchive& ar)
		{
			ICharacterInstance* pCharacterInstance { nullptr };
			 
			if (const CJointManipulationComponent* jointContext = ar.context<CJointManipulationComponent>())
			{
				pCharacterInstance = jointContext->GetCharacter();
				//m_pCharacterInstance = jointContext->GetCharacter();
			}

			Serialization::SContext characterInstanceContext(ar, pCharacterInstance);
			//Serialization::SContext characterInstanceContext(ar, m_pCharacterInstance);
			ar(Serialization::JointName(jointName), "JointName", "Joint Name");
			ar(Serialization::AttachmentName(attachmentName), "AttachmentName", "Attachment Name");
			ar(floatyMcFloatFace, "floatyMcFloatFace", "floatyMcFloatFace");			
		}

		string jointName;
		string attachmentName;

		float floatyMcFloatFace;
		Vec3 axis{ 0.0f, 1.0f, 0.0f };
		Schematyc::Range<0, 360> needleValue = 0.0f;
		ICharacterInstance* m_pCharacterInstance { nullptr };
	};

	virtual void Update(SEntityUpdateContext* pCtx);

protected:
	SJointManipulationProperties m_jointManipulationProperties;
	ICharacterInstance* m_pCharacterInstance { nullptr };
	string jointName; // HACK:
	string attachmentName; // HACK:
};
}