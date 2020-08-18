#pragma once

#include <Components/Player/Input/PlayerInputComponent.h>
#include <Entities/Interaction/IEntityInteraction.h>
#include <entt/entt.hpp>
#include <Spells/Spells.h>
#include <ECS/Components/Health.h>
#include <ECS/Components/Qi.h>
#include <ECS/Components/Spells/SpellActions.h>


namespace Chrysalis
{
/**
Allows the entity to participate in spell casting and having spells cast upon them.

We're adding the health and mana ECS components using this component because they will be commonly required, even by
things that might not seem like they are living or actors e.g. a chest could be opened with a fireball blast.

\sa IEntityComponent
**/

class CSpellParticipantComponent
	: public IEntityComponent, public ISpellParticipant
{
protected:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

public:
	CSpellParticipantComponent() {}
	virtual ~CSpellParticipantComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CSpellParticipantComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{E71EABC2-7FFC-47DB-80EB-6418823F0062}"_cry_guid;
		return id;
	}


	// ISpellParticipant
	const entt::entity GetECSEntity() const override { return m_ecsEntity; };
	// ~ISpellParticipant

private:
	/** The identifier for this spell participant in the ECS. */
	entt::entity m_ecsEntity {entt::null};

	// HACK: Initialise is being called twice. Use this to filter it till I figure out why.
	bool m_isInit {false};
};
}