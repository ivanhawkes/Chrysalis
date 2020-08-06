#pragma once

#include <queue>
#include <Components/Player/Input/PlayerInputComponent.h>
#include <Entities/Interaction/IEntityInteraction.h>
#include <Spells/Spells.h>


namespace Chrysalis
{
/**
Allows an entity to have a spellbook which can hold spells both castable by the entity, as well as from outside
sources if desired.

\sa IEntityComponent
**/

class CSpellbookComponent
	: public IEntityComponent, public ISpellcastManager, public CPlayerInputComponent::IInputEventListener
{
protected:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

public:
	CSpellbookComponent() {}
	virtual ~CSpellbookComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CSpellbookComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{654A1648-663E-414B-97C1-0A7B12D3072F}"_cry_guid;
		return id;
	}


	// ISpellcastManager
	bool QueueSpellCast(std::shared_ptr<ISpellcasting> spellcasting) override;
	// ~ISpellcastManager


	// CPlayerInputComponent::IInputEventListener
	virtual void OnInputEscape(int activationMode) override;
	virtual void OnInputInteraction(int activationMode) override;
	virtual void OnInputActionBarUse(int activationMode, int buttonId) override;
	virtual void OnInputFunctionBarUse(int activationMode, int buttonId) override;
	// ~CPlayerInputComponent::IInputEventListener


	/** An entry in the spellbook. This should name the spell, and provide meta information on casting
	limitations that exist in addition to the limitations of the spell itself. */
	struct SSpellBookEntry
	{
		void Serialize(Serialization::IArchive& ar)
		{
			ar(spellName, "spellName", "Name");
			ar(isHidden, "isHidden", "Hidden?");
			ar(enabled, "enabled", "Enabled?");
		}

		/** The name of the spell must be found inside the spell registry in order for the spell to work correctly. */
		string spellName;

		/** The prototype for this spell. These will need to be looked up each time the spell name changes or
		whenever an entry is loaded. Attempts to use the spell should check for null spells which indicate
		some form of error has occurred or the spell prototype doesn't exist. */
		entt::entity prototype {entt::null};

		/** Provide a means of disabling spells. */
		bool isHidden {false};

		/** Provide a means of disabling spells. */
		bool enabled {true};
	};


	/*** A collection of spell entries. This is the soul of a spellbook. Every spell that can be cast
	should be found in this collection. */
	struct SSpellCollection
	{
		inline bool operator==(const SSpellCollection& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


		static void ReflectType(Schematyc::CTypeDesc<SSpellCollection>& desc)
		{
			desc.SetGUID("{CF8E39CE-CC1E-4E05-9536-B1403723CC5B}"_cry_guid);
		}


		void Serialize(Serialization::IArchive& ar)
		{
			ar(spells, "spells", "spells");
		}

		std::vector<SSpellBookEntry> spells;
	};


	std::vector<string> GetVerbs(bool includeHidden = false);

	void AddInteraction(IInteractionPtr interaction);
	void RemoveInteraction(string verb);
	IInteractionWeakPtr GetInteraction(string verb);
	IInteractionWeakPtr SelectInteractionVerb(string verb);
	void ClearInteractionVerb();

	void OnInteractionStart(IActor& actor);
	void OnInteractionTick(IActor& actor);
	void OnInteractionComplete(IActor& actor);

	const SSpellCollection& GetSpellColllection() const { return m_spellCollection; }

private:
	/** A collection of spells which can potentially be cast. */
	SSpellCollection m_spellCollection;

	// Older interaction system - looking at spell casting now instead.
	std::vector<IInteractionPtr> m_interactionQueue;
	IInteractionPtr m_selectedInteraction {IInteractionPtr()};

	// HACK: KISS - no queue for the moment.
	std::queue<std::shared_ptr<ISpellcasting>> m_spellQueue;
	std::shared_ptr<ISpellcasting> m_spellcasting;
};
}