#pragma once

#include "Entities/Interaction/IEntityInteraction.h"
#include <DefaultComponents/Geometry/AnimatedMeshComponent.h>
#include <DefaultComponents/Geometry/BaseMeshComponent.h>


class Cry::DefaultComponents::CAnimatedMeshComponent;

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
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return Cry::DefaultComponents::CBaseMeshComponent::GetEventMask() | BIT64(ENTITY_EVENT_UPDATE); }
	// ~IEntityComponent

public:
	CTimePieceComponent() {}
	virtual ~CTimePieceComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CTimePieceComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{64F5CEFC-3098-4166-8860-4C82DEAFEA11}"_cry_guid;
		return id;
	}

	struct STimeComponents
	{
		inline bool operator==(const STimeComponents &rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		Schematyc::Range<0, 24> m_hour = 0.0f;
		Schematyc::Range<0, 60> m_minute = 0.0f;
		Schematyc::Range<0, 60> m_second = 0.0f;
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

	void SetHour(const float hour)
	{
		m_timeComponents.m_hour = hour;
	}

	void SetMinute(const float minute)
	{
		m_timeComponents.m_minute = minute;
	}

	void SetSecond(const float second)
	{
		m_timeComponents.m_second = second;
	}

protected:
	Schematyc::CharacterFileName m_filePath;
	_smart_ptr<ICharacterInstance> m_pCachedCharacter = nullptr;
	STimeComponents m_timeComponents;
};


static void ReflectType(Schematyc::CTypeDesc<CTimePieceComponent::STimeComponents>& desc)
{
	desc.SetGUID("{C0D6D7AC-9D54-48AA-A9F9-DC85CF579632}"_cry_guid);
	desc.AddMember(&CTimePieceComponent::STimeComponents::m_hour, 'hour', "Hours", "Hours", nullptr, 0.0f);
	desc.AddMember(&CTimePieceComponent::STimeComponents::m_minute, 'mins', "Minutes", "Minutes", nullptr, 0.0f);
	desc.AddMember(&CTimePieceComponent::STimeComponents::m_second, 'secs', "Seconds", "Seconds", nullptr, 0.0f);
}
}