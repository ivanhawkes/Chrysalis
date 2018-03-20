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
		inline bool operator!=(const SDRSProperties &rhs) const { return 0 != memcmp(this, &rhs, sizeof(rhs)); }

		Schematyc::CSharedString key;
		Schematyc::CSharedString value;

		static void ReflectType(Schematyc::CTypeDesc<SDRSProperties>& desc)
		{
			desc.SetGUID("{C07C367C-106F-4FD4-B7D5-E40C1A21F9F3}"_cry_guid);
			desc.AddMember(&SDRSProperties::key, 'key', "Key", "Key", "DRS Key", "");
			
			// HACK: This is broken if there is more than one member added. I don't expect a working collection type until 5.5 or 5.6.
			// Consider this broken until then and *don't use this collection to store values*.
			//desc.AddMember(&SDRSProperties::value, 'valu', "Value", "Value", "DRS Value", "");
		}
	};

	typedef Schematyc::CArray<SDRSProperties> PropertyCollection;

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
	Schematyc::CSharedString m_drsResponse;

	/** Properties. */
	PropertyCollection m_drsProperties;	
};

bool Serialize(Serialization::IArchive& archive, CDRSInteractionComponent::SDRSProperties& value, const char* szName, const char* szLabel);
}