#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include "Entities/Interaction/IEntityInteraction.h"
#include <CrySerialization/Decorators/Resources.h>


class CEntityInteractionComponent;


class CDRSInteractionComponent final : public CDesignerEntityComponent<>, public IEntityPropertyGroup, public IInteractionDRS
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CDRSInteractionComponent, "DRSInteractionComponent", 0x5D1C5884B9AD48C3, 0x89F0A9138C04454E);

	virtual ~CDRSInteractionComponent() {}

public:
	struct SDRSProperties
	{
		string key;
		string value;

		virtual void Serialize(Serialization::IArchive& ar)
		{
			ar(key, "DRSKey", "DRS Key");
			ar(value, "DRSValue", "DRS Value");
		}
	};


	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "DRS Component Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

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
