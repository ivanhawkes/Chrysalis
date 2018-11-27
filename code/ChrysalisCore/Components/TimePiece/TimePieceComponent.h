#pragma once

#include "Entities/Interaction/IEntityInteraction.h"
#include <DefaultComponents/Geometry/BaseMeshComponent.h>


namespace Chrysalis
{
/**
A TimePiece extension.

**/
class CTimePieceComponent
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
	CTimePieceComponent() {}
	virtual ~CTimePieceComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CTimePieceComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{64F5CEFC-3098-4166-8860-4C82DEAFEA11}"_cry_guid;
		return id;
	}

	struct STimePieceProperties
	{
		inline bool operator==(const STimePieceProperties &rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		Vec3 axis { 0.0f, 1.0f, 0.0f };
		Schematyc::Range<0, 24> hour = 0.0f;
		Schematyc::Range<0, 60> minute = 0.0f;
		Schematyc::Range<0, 60> second = 0.0f;
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

	void SetHour(const float hour)
	{
		m_timePieceProperties.hour = hour;
	}

	void SetMinute(const float minute)
	{
		m_timePieceProperties.minute = minute;
	}

	void SetSecond(const float second)
	{
		m_timePieceProperties.second = second;
	}

protected:
	Schematyc::CharacterFileName m_filePath;
	Schematyc::MaterialFileName m_materialPath;
	_smart_ptr<ICharacterInstance> m_pCachedCharacter = nullptr;
	STimePieceProperties m_timePieceProperties;
};


static void ReflectType(Schematyc::CTypeDesc<CTimePieceComponent::STimePieceProperties>& desc)
{
	desc.SetGUID("{C0D6D7AC-9D54-48AA-A9F9-DC85CF579632}"_cry_guid);
	desc.AddMember(&CTimePieceComponent::STimePieceProperties::axis, 'axis', "Axis", "Axis", "Axis around which the hands will rotate", Vec3(0.0f, 1.0f, 0.0f));
	desc.AddMember(&CTimePieceComponent::STimePieceProperties::hour, 'hour', "Hours", "Hours", nullptr, 0.0f);
	desc.AddMember(&CTimePieceComponent::STimePieceProperties::minute, 'mins', "Minutes", "Minutes", nullptr, 0.0f);
	desc.AddMember(&CTimePieceComponent::STimePieceProperties::second, 'secs', "Seconds", "Seconds", nullptr, 0.0f);
}
}