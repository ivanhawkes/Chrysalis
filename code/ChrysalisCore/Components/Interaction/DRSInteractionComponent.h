#pragma once

#include "Entities/Interaction/IEntityInteraction.h"


namespace Chrysalis
{
class CEntityInteractionComponent;

class CDRSInteractionComponent
	: public IEntityComponent
	, public IInteractionDRS
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

public:
	CDRSInteractionComponent() {}
	virtual ~CDRSInteractionComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CDRSInteractionComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{25963340-C513-405A-9C44-3CC6EDE66B5A}"_cry_guid;
		return id;
	}

	struct SDRSProperties
	{
		inline bool operator==(const SDRSProperties &rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		string key;
		string value;

		virtual void Serialize(Serialization::IArchive& ar)
		{
			ar(key, "DRSKey", "DRS Key");
			ar(value, "DRSValue", "DRS Value");
		}
	};

	static void ReflectType(Schematyc::CTypeDesc<CDRSInteractionComponent::SDRSProperties>& desc)
	{
		desc.SetGUID("{C07C367C-106F-4FD4-B7D5-E40C1A21F9F3}"_cry_guid);
		// TODO: CRITICAL: HACK: BROKEN: !!
//		desc.AddMember(&CDRSInteractionComponent::SDRSProperties::key, 'key', "Key", "Key", nullptr, "");
//		desc.AddMember(&CDRSInteractionComponent::SDRSProperties::value, 'valu', "Value", "Value", nullptr, "");
	}


	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	// ~IEntityComponent

	// IDRSInteractionEntityComponent
	virtual void OnResetState() final;
	// IDRSInteractionEntityComponent

	// IInteractionDRS
	void OnInteractionDRS() override;
	// ~IInteractionDRS

private:
	/** An instance of an interaction component. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** The main verb for the DRS response. */
	string m_drsResponse;

	/** Properties. */
	std::vector<SDRSProperties> m_drsProperties;
};
}