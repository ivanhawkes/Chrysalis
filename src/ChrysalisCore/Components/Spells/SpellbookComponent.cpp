#include <StdAfx.h>

#include "SpellbookComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include "Components/Player/PlayerComponent.h"
#include <ECS/ECS.h>


namespace Chrysalis
{
static void RegisterSpellbookComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CSpellbookComponent));
		// Functions
		{
		}
	}
}


void CSpellbookComponent::ReflectType(Schematyc::CTypeDesc<CSpellbookComponent>& desc)
{
	desc.SetGUID(CSpellbookComponent::IID());
	desc.SetEditorCategory("Spells");
	desc.SetLabel("Spellbook");
	desc.SetDescription("Maintains a book of spells this entity can cast.");
	desc.SetIcon("icons:ObjectTypes/light.ico");

	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});

	// Keep a collection of spells available for use.
	desc.AddMember(&CSpellbookComponent::m_spellCollection, 'spcl', "SpellCollection", "Spell Collection", "List of spells which may be cast.", SSpellCollection {});
}


// ***
// *** IEntityComponent
// ***


void CSpellbookComponent::Initialize()
{
}


void CSpellbookComponent::ProcessEvent(const SEntityEvent& event)
{
}


// ***
// *** ISpellcastManager
// ***


bool CSpellbookComponent::QueueSpellCast(std::shared_ptr<ISpellcasting> spellcasting)
{
	// Just let them queue up one extra spell at a time, to help alleviate network lag issues.
	if (m_spellQueue.size() <= 1)
	{
		m_spellQueue.push(spellcasting);
		return true;
	}

	// We didn't queue the spell, let them know.
	return false;
}


// ***
// *** CPlayerInputComponent::IInputEventListener
// ***


void CSpellbookComponent::OnInputEscape(int activationMode)
{
	CryLogAlways("CSpellbookComponent::OnInputEscape");
}


void CSpellbookComponent::OnInputInteraction(int activationMode)
{
	switch (activationMode)
	{
		case eAAM_OnPress:
			CryLogAlways("CSpellbookComponent::OnInputInteraction - Press");
			break;

		case eAAM_OnHold:
			break;

		case eAAM_OnRelease:
			CryLogAlways("CSpellbookComponent::OnInputInteraction - Release");
			break;
	}
}


void CSpellbookComponent::OnInputActionBarUse(int activationMode, int buttonId)
{
	switch (activationMode)
	{
		case eAAM_OnPress:
			CryLogAlways("CSpellbookComponent::OnInputActionBarUse - Press: %d", buttonId);
			break;

		case eAAM_OnHold:
			break;

		case eAAM_OnRelease:
			CryLogAlways("CSpellbookComponent::OnInputActionBarUse - Release: %d", buttonId);
			break;
	}
}


void CSpellbookComponent::OnInputFunctionBarUse(int activationMode, int buttonId)
{
 	switch (activationMode)
	{
		case eAAM_OnPress:
			CryLogAlways("CSpellbookComponent::OnInputFunctionBarUse - Press: %d", buttonId);
			break;

		case eAAM_OnHold:
			break;

		case eAAM_OnRelease:
			CryLogAlways("CSpellbookComponent::OnInputFunctionBarUse - Release: %d", buttonId);
			break;
	}
}


// ***
// *** CSpellbookComponent
// ***


void CSpellbookComponent::AddInteraction(IInteractionPtr interaction)
{
	m_interactionQueue.push_back(interaction);
}


void CSpellbookComponent::RemoveInteraction(string verb)
{
	m_interactionQueue.erase(std::remove_if(m_interactionQueue.begin(), m_interactionQueue.end(),
		[&](IInteractionPtr i) { return i->GetVerb().compare(verb) == 0; }),
		m_interactionQueue.end());
}


std::vector<string> CSpellbookComponent::GetVerbs(bool includeHidden)
{
	std::vector<string> verbs;

	for (auto& it : m_interactionQueue)
	{
		if (it->IsEnabled())
		{
			if ((!it->IsHidden()) || ((it->IsHidden()) && includeHidden))
			{
				verbs.push_back(it->GetVerb());
			}
		}
	}

	return verbs;
}


IInteractionWeakPtr CSpellbookComponent::GetInteraction(string verb)
{
	for (auto& it : m_interactionQueue)
	{
		if ((it->GetVerb().compare(verb) == 0) && (it->IsEnabled()))
		{
			return it;
		}
	}

	CryLogAlways("There's no interaction verb for %s", verb.c_str());

	return std::weak_ptr<IInteraction>();
}


IInteractionWeakPtr CSpellbookComponent::SelectInteractionVerb(string verb)
{
	for (auto& it : m_interactionQueue)
	{
		if ((it->GetVerb().compare(verb) == 0) && (it->IsEnabled()))
		{
			m_selectedInteraction = it;
			return it;
		}
	}

	return std::weak_ptr<IInteraction>();
}


void CSpellbookComponent::ClearInteractionVerb()
{
	m_selectedInteraction = nullptr;
}


void CSpellbookComponent::OnInteractionStart(IActor& actor)
{
	CRY_ASSERT_MESSAGE(m_selectedInteraction, "Be sure to set an interaction before attempting to call it.");
	m_selectedInteraction->OnInteractionStart(actor);

	if (!m_spellQueue.empty())
	{
		m_spellcasting = m_spellQueue.front();
		m_spellQueue.pop();
	}
	else
	{
		m_spellcasting.reset();
	}

	if (m_spellcasting)
	{
		m_spellcasting->OnSpellStart();
	}
}


void CSpellbookComponent::OnInteractionTick(IActor& actor)
{
	CRY_ASSERT_MESSAGE(m_selectedInteraction, "Be sure to set an interaction before attempting to call it.");
	m_selectedInteraction->OnInteractionTick(actor);

	if (m_spellcasting)
	{
		m_spellcasting->OnSpellTick();
	}
}


void CSpellbookComponent::OnInteractionComplete(IActor& actor)
{
	CRY_ASSERT_MESSAGE(m_selectedInteraction, "Be sure to set an interaction before attempting to call it.");
	m_selectedInteraction->OnInteractionComplete(actor);

	if (m_spellcasting)
	{
		m_spellcasting->OnSpellComplete();
	}

	// We're done with this spell now.
	m_spellcasting.reset();
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSpellbookComponent)
}