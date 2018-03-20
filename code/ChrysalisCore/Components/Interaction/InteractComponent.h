#pragma once

#include <Components/Interaction/EntityInteractionComponent.h>
#include <Actor/Animation/Actions/ActorAnimationActionInteration.h>


namespace Chrysalis
{
class CInteractComponent
	: public IEntityComponent
	, public IInteractionInteract
	, public IAnimationEventListener
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void Initialize() override;
	// ~IEntityComponent

public:
	CInteractComponent() {}
	virtual ~CInteractComponent() {}

	struct SAnimationTag
	{
		inline bool operator==(const SAnimationTag &rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
		inline bool operator!=(const SAnimationTag &rhs) const { return 0 != memcmp(this, &rhs, sizeof(rhs)); }

		Schematyc::CSharedString tag;

		static void ReflectType(Schematyc::CTypeDesc<SAnimationTag>& desc)
		{
			desc.SetGUID("{C07C367C-106F-4FD4-B7D5-E40C1A21F9F3}"_cry_guid);
			desc.AddMember(&SAnimationTag::tag, 'tag', "Tag", "Tag", "An animation tag for the fragment.", "");
		}
	};

	typedef Schematyc::CArray<SAnimationTag> TagCollection;

	static void ReflectType(Schematyc::CTypeDesc<CInteractComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{C03D46F4-D1C9-45BE-AAC6-2362EFD3ABCC}"_cry_guid;
		return id;
	}

	const string kQueueSignal { "interaction_interact" };
	const string kInteractStartVerb { "interaction_interact_start" };
	const string kInteractTickVerb { "interaction_interact_tick" };
	const string kInteractCompleteVerb { "interaction_interact_complete" };

	const string kInteractAnimationEnterVerb { "interaction_animation_enter" };
	const string kInteractAnimationFailVerb { "interaction_animation_fail" };
	const string kInteractAnimationExitVerb { "interaction_animation_exit" };
	const string kInteractAnimationEventVerb { "interaction_animation_event" };

	/** A signal that indicates the user has pressed down the interact key. */
	struct SInteractStartSignal
	{
		SInteractStartSignal() = default;
	};


	/** A signal that indicates the user is holding down the interact key. */
	struct SInteractTickSignal
	{
		SInteractTickSignal() = default;
		SInteractTickSignal(float deltaPitch, float deltaYaw) : m_deltaPitch(deltaPitch), m_deltaYaw(deltaYaw) {};

		float m_deltaPitch { 0.0f };
		float m_deltaYaw { 0.0f };
	};


	/**
	A signal that indicates the user has released the interact key. NOTE: The actual interaction may still be ongoing
	e.g. an animation might be playing.
	**/
	struct SInteractCompleteSignal
	{
		SInteractCompleteSignal() = default;
	};


	/** A signal which is sent at the very beginning of playing an animation. */
	struct SInteractAnimationEnterSignal
	{
		SInteractAnimationEnterSignal() = default;
	};


	/** A signal to indicate playback of the animation has failed. */
	struct SInteractAnimationFailSignal
	{
		SInteractAnimationFailSignal() = default;
	};


	/** A signal to indicate playback of animation has completed. */
	struct SInteractAnimationExitSignal
	{
		SInteractAnimationExitSignal() = default;
	};


	/** An signal which informs the system when an animation event has been reached. */
	struct SInteractAnimationEventSignal
	{
		SInteractAnimationEventSignal() = default;
		SInteractAnimationEventSignal(Schematyc::CSharedString eventName,
			uint32 eventNameLowercaseCRC32,
			Schematyc::CSharedString customParameter,
			float time, float endTime,
			Schematyc::CSharedString bonePathName,
			Vec3 boneDirection,
			Vec3 boneOffset)
			: m_eventName(eventName),
			m_eventNameLowercaseCRC32(eventNameLowercaseCRC32),
			m_customParameter(customParameter),
			m_time(time), m_endTime(endTime),
			m_bonePathName(bonePathName),
			m_boneDirection(boneDirection),
			m_boneOffset(boneOffset)
		{}

		Schematyc::CSharedString m_eventName { "" };
		uint32 m_eventNameLowercaseCRC32 { 0 };
		Schematyc::CSharedString m_customParameter { "" };
		float m_time { 0.0f };
		float m_endTime { 0.0f };
		Schematyc::CSharedString m_bonePathName { "" };
		Vec3 m_boneDirection { Vec3(ZERO) };
		Vec3 m_boneOffset { Vec3(ZERO) };
	};


	// IInteractionInteract
	virtual void OnInteractionInteractStart(IInteraction& pInteraction, IActorComponent& actor) override;
	virtual void OnInteractionInteractTick(IInteraction& pInteraction, IActorComponent& actor) override;
	virtual void OnInteractionInteractComplete(IInteraction& pInteraction, IActorComponent& actor) override;
	// ~IInteractionInteract

	// IAnimationEventListener
	virtual void OnActionAnimationEnter() override;
	virtual void OnActionAnimationFail(EActionFailure actionFailure) override;
	virtual void OnActionAnimationExit() override;
	virtual void OnActionAnimationEvent(ICharacterInstance* pCharacter, const AnimEventInstance& event) override;
	// ~IAnimationEventListener

protected:
	/** Sends the Schematyc start signal. */
	virtual void ProcessSchematycSignalStart() { GetEntity()->GetSchematycObject()->ProcessSignal(SInteractStartSignal(), GetGUID()); };

	/**
	Sends the Schematyc tick signal.
	
	\param	deltaPitch The delta pitch.
	\param	deltaYaw   The delta yaw.
	**/
	virtual void ProcessSchematycSignalTick(float deltaPitch, float deltaYaw) { GetEntity()->GetSchematycObject()->ProcessSignal(SInteractTickSignal(deltaPitch, deltaYaw), GetGUID()); };
	
	/** Sends the Schematyc complete signal. */
	virtual void ProcessSignalComplete() { GetEntity()->GetSchematycObject()->ProcessSignal(SInteractCompleteSignal(), GetGUID()); };


	/**
	Raises a DRS event and sends it to all the entities linked to this one.
	
	\param	verb		   The DRS verb.
	\param	isInteractedOn True if this instance is interacted on.
	**/
	virtual void InformAllLinkedEntities(string verb, bool isInteractedOn);

	virtual void OnResetState();

	/** True if this Interact is able to be used. */
	bool m_isEnabled { true };

	/** True if this Interact can only be used once. */
	bool m_isSingleUseOnly { false };

	/** Keep track of these so we can enable / disable as needed. */
	CInteractionInteractPtr m_interactPtr { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** Send an alternative queue signal to DRS if the string is not empty. */
	Schematyc::CSharedString m_queueSignal;

	/** During the processing cycle for an interaction, this will hold the actor that initiated the interaction. It will
	be invalid at all other times. */
	IActorComponent* m_pInteractionActor { nullptr };

	/** The interaction being run by this component. */
	IInteraction* m_interaction { nullptr };

	/** A set of tags which will be added to the fragment when it plays. */
	TagCollection m_tags;
};


static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractStartSignal>& desc)
{
	desc.SetGUID("{D7834D96-13FB-41C4-90D1-F3D977CA0AC7}"_cry_guid);
	desc.SetLabel("Interact Start");
}


static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractTickSignal>& desc)
{
	desc.SetGUID("{C11053C3-0CB4-4316-A643-F53BECBA07B5}"_cry_guid);
	desc.SetLabel("Interact Tick");
	desc.AddMember(&CInteractComponent::SInteractTickSignal::m_deltaPitch, 'dpit', "DeltaPitch", "Delta Pitch", "Player input requested change in pitch.", 0.0f);
	desc.AddMember(&CInteractComponent::SInteractTickSignal::m_deltaYaw, 'dyaw', "DeltaYaw", "Delta Yaw", "Player input requested change in yaw.", 0.0f);
}


static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractCompleteSignal>& desc)
{
	desc.SetGUID("{6E153DFF-B21B-4E92-8D50-976B17802556}"_cry_guid);
	desc.SetLabel("Interact Complete");
}

bool Serialize(Serialization::IArchive& archive, CInteractComponent::SAnimationTag& value, const char* szName, const char* szLabel);
}