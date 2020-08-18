#pragma once

#include "Components/Animation/ActorAnimationComponent.h"


/**
A Door extension.
**/
namespace Chrysalis
{
class CDoorComponent
	: public CActorAnimationComponent
{
public:
	CDoorComponent() {}
	virtual ~CDoorComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CDoorComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{70B41B19-8560-4125-8DE0-1E477811A78F}"_cry_guid;
		return id;
	}


	virtual void Serialize(Serialization::IArchive& ar) override
	{
		Serialization::SContext context(ar, this);
		ar(m_DoorProperties, "Joints", "Joints");
	}


	struct SDoorProperties
	{
		inline bool operator==(const SDoorProperties& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


		static void ReflectType(Schematyc::CTypeDesc<SDoorProperties>& desc)
		{
			desc.SetGUID("{82513DCE-FE89-43C4-80B1-0FB04B48D61F}"_cry_guid);
			desc.SetLabel("Door Properties");
			desc.SetDescription("Door stuff.");
		}

		
		virtual void Serialize(Serialization::IArchive& ar)
		{
			ICharacterInstance* pCharacterInstance { nullptr };
			 
			if (const CDoorComponent* jointContext = ar.context<CDoorComponent>())
			{
				pCharacterInstance = jointContext->GetCharacter();
			}

			Serialization::SContext characterInstanceContext(ar, pCharacterInstance);
			ar(Serialization::JointName(jointName), "JointName", "Joint Name");
			ar(Serialization::AttachmentName(attachmentName), "AttachmentName", "Attachment Name");
		}

		string jointName;
		string attachmentName;

		Vec3 axis{ 0.0f, 1.0f, 0.0f };
		Schematyc::Range<0, 360> needleValue = 0.0f;
	};

	virtual void Update(SEntityUpdateContext* pCtx);

protected:
	SDoorProperties m_DoorProperties;
	string jointName; // HACK:
	string attachmentName; // HACK:
};
}