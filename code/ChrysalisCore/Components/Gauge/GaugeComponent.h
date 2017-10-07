#pragma once

#include "Entities/Interaction/IEntityInteraction.h"
#include <DefaultComponents/Geometry/AnimatedMeshComponent.h>
#include <DefaultComponents/Geometry/BaseMeshComponent.h>


class Cry::DefaultComponents::CAnimatedMeshComponent;

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
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return Cry::DefaultComponents::CBaseMeshComponent::GetEventMask() | BIT64(ENTITY_EVENT_UPDATE); }
	// ~IEntityComponent

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

		Schematyc::Range<0, 360> m_needle = 0.0f;
	};

	virtual void Update(SEntityUpdateContext* pCtx);

	virtual void SetCharacterFile(const char* szPath) { m_filePath = szPath; };
	const char* SetCharacterFile() const { return m_filePath.value.c_str(); }

	// Loads character and mannequin data from disk
	virtual void LoadFromDisk();

	// Applies the character to the entity
	virtual void ResetObject();

	// Helper to allow exposing derived function to Schematyc
	virtual void SetMeshType(Cry::DefaultComponents::EMeshType type) { SetType(type); }

	void SetNeedle(const float needle)
	{
		m_gaugeProperties.m_needle = needle;
	}

protected:
	Schematyc::CharacterFileName m_filePath;
	_smart_ptr<ICharacterInstance> m_pCachedCharacter = nullptr;
	SGaugeProperties m_gaugeProperties;
	Vec3 m_axis { 0.0f, 1.0f, 0.0f };
};


static void ReflectType(Schematyc::CTypeDesc<CGaugeComponent::SGaugeProperties>& desc)
{
	desc.SetGUID("{FEE816B5-15F3-4A63-A74B-E6597B555C18}"_cry_guid);
	desc.AddMember(&CGaugeComponent::SGaugeProperties::m_needle, 'need', "Needle", "Needle", nullptr, 0.0f);
}
}