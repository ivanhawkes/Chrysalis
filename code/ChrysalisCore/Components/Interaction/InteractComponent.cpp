#include "StdAfx.h"

#include "InteractComponent.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include <Components/Player/Input/PlayerInputComponent.h>


namespace Chrysalis
{
static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractAnimationEnterSignal>& desc)
{
	desc.SetGUID("{9F8551C1-3DC5-42A3-B0D4-8473D1445DDC}"_cry_guid);
	desc.SetLabel("Animation Enter");
}


static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractAnimationFailSignal>& desc)
{
	desc.SetGUID("{D1F80772-E8CF-4CD4-B9A9-B25BD35A0A9F}"_cry_guid);
	desc.SetLabel("Animation Fail");
}


static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractAnimationExitSignal>& desc)
{
	desc.SetGUID("{60CA3693-111F-4104-9ABD-866B54DDB8AE}"_cry_guid);
	desc.SetLabel("Animation Exit");
}


static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractAnimationEventSignal>& desc)
{
	desc.SetGUID("{3DA983C8-0794-4176-8CD6-B92B1A0868D4}"_cry_guid);
	desc.SetLabel("Animation Event");

	desc.AddMember(&CInteractComponent::SInteractAnimationEventSignal::m_eventName, 'evnm', "EventName", "Event Name", "The name of this animation event.", "");
	desc.AddMember(&CInteractComponent::SInteractAnimationEventSignal::m_eventNameLowercaseCRC32, 'ncrc', "NameCRC32", "Event Name CRC32", "A CRC32 of the animation event name.", 0);
	desc.AddMember(&CInteractComponent::SInteractAnimationEventSignal::m_customParameter, 'cstp', "CustomParameter", "Custom Parameter", "A custom parameter from the animation event.", "");
	desc.AddMember(&CInteractComponent::SInteractAnimationEventSignal::m_time, 'time', "Time", "Time", "Time the event was started.", 0.0f);
	desc.AddMember(&CInteractComponent::SInteractAnimationEventSignal::m_endTime, 'endt', "EndTime", "End Time", "Time the event will end.", 0.0f);
	desc.AddMember(&CInteractComponent::SInteractAnimationEventSignal::m_bonePathName, 'bnpt', "BonePathName", "Bone path name", "The path to a bone for this event.", "");
	desc.AddMember(&CInteractComponent::SInteractAnimationEventSignal::m_boneDirection, 'bndr', "BoneDirection", "Bone Direction", "Bone direction.", Vec3(ZERO));
	desc.AddMember(&CInteractComponent::SInteractAnimationEventSignal::m_boneOffset, 'bnof', "BoneOffset", "Bone offset", "Bone offset.", Vec3(ZERO));
}


void CInteractComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractStartSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractTickSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractCompleteSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractAnimationEnterSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractAnimationFailSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractAnimationExitSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractAnimationEventSignal));
}


void CInteractComponent::ReflectType(Schematyc::CTypeDesc<CInteractComponent>& desc)
{
	desc.SetGUID(CInteractComponent::IID());
	desc.SetEditorCategory("Interact");
	desc.SetLabel("Generic Interaction");
	desc.SetDescription("An interaction between a player and an entity.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::None });

	// Mark the entity interaction component as a hard requirement.
	desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, CEntityInteractionComponent::IID());

	desc.AddMember(&CInteractComponent::m_isEnabled, 'isen', "IsEnabled", "IsEnabled", "Is this interaction currently enabled.", true);
	desc.AddMember(&CInteractComponent::m_isSingleUseOnly, 'issi', "IsSingleUseOnly", "Single Use Only", "Is this Interact only able to be used once.", false);
	desc.AddMember(&CInteractComponent::m_queueSignal, 'alts', "InteractVerb", "Interact Verb (Override)", "Send an alternative queue signal to DRS if the string is not empty. ('interaction_Interact').", "");
	
	// HACK: For now, there's no way to set the list of tags. Release 5.5 should have vector<string> support.
	desc.AddMember(&CInteractComponent::m_tags, 'tags', "Tags", "Mannequin Tags", "Set these tags when playing the animation.", TagCollection{});
}


void CInteractComponent::Initialize()
{
	// We want to supply interaction verbs.
	m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactPtr = std::make_shared<CInteractionInteract>(this);
		m_interactor->AddInteraction(m_interactPtr);
	}
}


void CInteractComponent::OnResetState()
{
	m_interactPtr->SetEnabled(m_isEnabled);
}


void CInteractComponent::OnInteractionInteractStart(IInteraction& pInteraction, IActorComponent& actor)
{
	if (m_isEnabled)
	{
		m_pInteractionActor = &actor;
		m_interaction = &pInteraction;

		// Inform the actor we are taking control of an interaction.
		m_pInteractionActor->InteractionStart(m_interaction);

		// HACK: We need to convert the tags from a Schematyc format to a more general one. In 5.5 / 5.6 we should be able
		// to use the generic containers instead.
		std::vector<string> tags;
		for (int i = 0; i < m_tags.Size(); i++)
		{
			tags.push_back(m_tags.At(i).tag.c_str());
		}

		// We should queue an animation for this action.
		auto action = new CActorAnimationActionInteraction(tags);
		action->AddEventListener(this);
		actor.QueueAction(*action);

		// Push the signal out using DRS.
		InformAllLinkedEntities(kInteractStartVerb, true);

		// Push the signal out using schematyc.
		ProcessSchematycSignalStart();

		// Disable after a single use.
		if (m_isSingleUseOnly)
			m_isEnabled = false;
	}
}


void CInteractComponent::OnInteractionInteractTick(IInteraction& pInteraction, IActorComponent& actor)
{
	if (m_isEnabled)
	{
		// Push the signal out using DRS.
		InformAllLinkedEntities(kInteractTickVerb, true);

		// Pass along the delta movement for input devices. This helps enable interactive control of entities.
		if (auto pPlayerInput = CPlayerComponent::GetLocalPlayer()->GetPlayerInput())
			ProcessSchematycSignalTick(pPlayerInput->GetPitchDelta(), pPlayerInput->GetYawDelta());
	}
}


void CInteractComponent::OnInteractionInteractComplete(IInteraction& pInteraction, IActorComponent& actor)
{
	if (m_isEnabled)
	{
		// Push the signal out using DRS.
		InformAllLinkedEntities(kInteractCompleteVerb, true);

		// Push the signal out using schematyc.
		ProcessSignalComplete();
	}
}


void CInteractComponent::OnActionAnimationEnter()
{
	// Push the signal out using DRS.
	InformAllLinkedEntities(kInteractAnimationEnterVerb, true);

	// Push the signal out using schematyc.
	if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
		pSchematycObject->ProcessSignal(SInteractAnimationEnterSignal(), GetGUID());
}


void CInteractComponent::OnActionAnimationFail(EActionFailure actionFailure)
{
	// Inform the actor we are finished with an interaction.
	m_pInteractionActor->InteractionEnd(m_interaction);

	// Push the signal out using DRS.
	InformAllLinkedEntities(kInteractAnimationFailVerb, true);

	// Push the signal out using schematyc.
	if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
		pSchematycObject->ProcessSignal(SInteractAnimationFailSignal(), GetGUID());

	m_pInteractionActor = nullptr;
	m_interaction = nullptr;
}


void CInteractComponent::OnActionAnimationExit()
{
	// Inform the actor we are finished with an interaction.
	m_pInteractionActor->InteractionEnd(m_interaction);

	// Push the signal out using DRS.
	InformAllLinkedEntities(kInteractAnimationExitVerb, true);

	// Push the signal out using schematyc.
	if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
		pSchematycObject->ProcessSignal(SInteractAnimationExitSignal(), GetGUID());

	m_pInteractionActor = nullptr;
	m_interaction = nullptr;
}


void CInteractComponent::OnActionAnimationEvent(ICharacterInstance * pCharacter, const AnimEventInstance & event)
{
	CryLogAlways("CInteractComponent::AnimEvent: %s", event.m_EventName);

	// Push the signal out using DRS.
	InformAllLinkedEntities(kInteractAnimationEventVerb, true);

	// Push the signal out using schematyc.
	if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
		pSchematycObject->ProcessSignal(SInteractAnimationEventSignal(Schematyc::CSharedString(event.m_EventName),
			event.m_EventNameLowercaseCRC32,
			Schematyc::CSharedString(event.m_CustomParameter),
			float(event.m_time),
			float(event.m_endTime),
			Schematyc::CSharedString(event.m_BonePathName),
			event.m_vDir,
			event.m_vOffset), GetGUID());
}


void CInteractComponent::InformAllLinkedEntities(string verb, bool isInteractedOn)
{
	// Notify every linked entity.
	auto* entityLinks = GetEntity()->GetEntityLinks();
	while (entityLinks)
	{
		auto pTargetEntity = gEnv->pEntitySystem->GetEntity(entityLinks->entityId);
		auto pDrsProxy = crycomponent_cast<IEntityDynamicResponseComponent*> (pTargetEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));

		// Create a context variable collection and populate it based on information from the target entity.
		DRS::IVariableCollectionSharedPtr pContextVariableCollection = gEnv->pDynamicResponseSystem->CreateContextCollection();

		// It might be useful to know which verb triggered the interaction.
		pContextVariableCollection->CreateVariable("Verb", CHashedString(verb));

		// The Interact value is always set, regardless of which verb was triggered.
		pContextVariableCollection->CreateVariable("IsInteractedOn", isInteractedOn);

		// Queue it and let the DRS handle it now.
		const string queueSignalVerb = m_queueSignal.empty() ? kQueueSignal : m_queueSignal.c_str();
		pDrsProxy->GetResponseActor()->QueueSignal(queueSignalVerb, pContextVariableCollection);

		// Next please.
		entityLinks = entityLinks->next;
	}
}


bool Serialize(Serialization::IArchive& archive, CInteractComponent::SAnimationTag& value, const char* szName, const char* szLabel)
{
	archive(value.tag, "tag", "Tag");
	archive.doc("An animation tag for the fragment.");

	return true;
}
}