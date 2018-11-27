#pragma once

#include "Entities/Interaction/IEntityInteraction.h"
#include <DefaultComponents/Geometry/BaseMeshComponent.h>


namespace Chrysalis
{
/**
A Gauge extension.

**/
class CGaugeComponent
	: public Cry::DefaultComponents::CBaseMeshComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EntityEventMask GetEventMask() const override { return Cry::DefaultComponents::CBaseMeshComponent::GetEventMask() | EntityEventMask(EEntityEvent::Update); }
	// ~IEntityComponent

	// IEditorEntityComponent
	//virtual bool SetMaterial(int slotId, const char* szMaterial) override;
	// ~IEditorEntityComponent

public:
	CGaugeComponent() {}
	virtual ~CGaugeComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CGaugeComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{5EE0586B-81B4-467C-B86F-B6727246687D}"_cry_guid;
		return id;
	}

	struct SGaugeProperties
	{
		inline bool operator==(const SGaugeProperties &rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		Vec3 axis { 0.0f, 1.0f, 0.0f };
		Schematyc::Range<0, 360> needleValue = 0.0f;
	};

	virtual void Update(SEntityUpdateContext* pCtx);

	virtual void SetCharacterFile(const char* szPath) { m_filePath = szPath; };
	const char* GetCharacterFile() const { return m_filePath.value.c_str(); }

	// Loads character and mannequin data from disk.
	virtual void LoadFromDisk();

	// Applies the character to the entity.
	virtual void ResetObject();

	// Helper to allow exposing derived function to Schematyc.
	virtual void SetMeshType(Cry::DefaultComponents::EMeshType type) { SetType(type); }

	void SetNeedle(const float needleValue)
	{
		m_gaugeProperties.needleValue = needleValue;
	}

protected:
	Schematyc::CharacterFileName m_filePath;
	Schematyc::MaterialFileName m_materialPath;
	_smart_ptr<ICharacterInstance> m_pCachedCharacter = nullptr;
	SGaugeProperties m_gaugeProperties;
};


static void ReflectType(Schematyc::CTypeDesc<CGaugeComponent::SGaugeProperties>& desc)
{
	desc.SetGUID("{FEE816B5-15F3-4A63-A74B-E6597B555C18}"_cry_guid);
	desc.AddMember(&CGaugeComponent::SGaugeProperties::axis, 'axis', "Axis", "Axis", "Axis around which the hands will rotate", Vec3(0.0f, 1.0f, 0.0f));
	desc.AddMember(&CGaugeComponent::SGaugeProperties::needleValue, 'need', "Needle", "Needle", nullptr, 0.0f);
}
}