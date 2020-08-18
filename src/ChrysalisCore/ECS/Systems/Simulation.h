#pragma once

#include <entt/entt.hpp>


namespace Chrysalis::ECS
{
template<typename TYPE>
void CloneComponent(const entt::registry& sourceRegistry, const entt::entity sourceEntity, entt::registry& targetRegistry, const entt::entity targetEntity)
{
	targetRegistry.emplace_or_replace<TYPE>(targetEntity, sourceRegistry.get<TYPE>(sourceEntity));
}


template<typename TYPE>
void EmplaceComponent(entt::registry& registry, entt::entity entity)
{
	TYPE newType;

	if (registry.any<TYPE>(entity))
		CryLogAlways("Entity already has that component type.");
	else
		registry.emplace<TYPE>(entity, newType);
}


template<typename TYPE>
void RemoveComponent(entt::registry& registry, entt::entity entity)
{
	registry.remove<TYPE>(entity);
}


#ifdef IMGUI
template<typename TYPE>
void ImGuiRenderComponent(entt::registry& registry, entt::entity entity)
{
	TYPE& component = registry.get<TYPE>(entity);

	ImGuiRenderComponent(component);
}
#endif


class CSimulation
{
public:
	// Required to clone components between registries.
	using StampFunction = void(const entt::registry&, const entt::entity, entt::registry&, const entt::entity);
	using EmplaceFunction = void(entt::registry&, const entt::entity);
	using RemoveFunction = void(entt::registry&, const entt::entity);

#ifdef IMGUI
	using ImGuiRenderFunction = void(entt::registry&, const entt::entity);
#endif

	struct RegisteredType
	{
		RegisteredType() = default;
		~RegisteredType() = default;

#ifdef IMGUI
		RegisteredType(entt::hashed_string tag, StampFunction* stampFunction, EmplaceFunction* emplaceFunction, RemoveFunction* removeFunction, ImGuiRenderFunction* imgRenderFunction)
			:tag(tag), stampFunction(stampFunction), emplaceFunction(emplaceFunction), removeFunction(removeFunction), imgRenderFunction(imgRenderFunction) {};
#else
		RegisteredType(entt::hashed_string tag, StampFunction* stampFunction, EmplaceFunction* emplaceFunction, RemoveFunction* removeFunction)
			:tag(tag), stampFunction(stampFunction), emplaceFunction(emplaceFunction), removeFunction(removeFunction) {};

#endif

		entt::hashed_string tag;
		StampFunction* stampFunction;
		EmplaceFunction* emplaceFunction;
		RemoveFunction* removeFunction;
#ifdef IMGUI
		ImGuiRenderFunction* imgRenderFunction;
#endif
	};


	using RegisteredTypeMap = std::unordered_map<entt::id_type, RegisteredType>;

	template <typename TYPE>
	void RegisterTypeWithSimulation(entt::hashed_string nameHash)
	{
#ifdef IMGUI
		m_functionDispatchMap[entt::type_info<TYPE>::id()] = RegisteredType(nameHash, &CloneComponent<TYPE>, &EmplaceComponent<TYPE>, &RemoveComponent<TYPE>, &ImGuiRenderComponent<TYPE>);
#else
		m_functionDispatchMap[entt::type_info<TYPE>::id()] = RegisteredType(nameHash, &CloneComponent<TYPE>, &EmplaceComponent<TYPE>, &RemoveComponent<TYPE>);
#endif
	}


	/** One time initialisation. */
	void Init();

	/** Update tick, called during every frame update. */
	void Update(const float deltaTime);

	/** Immediate Simulation. Responsible for things that require immediate attention each frame. */
	void UpdateImmediate(const float deltaTime);

	/** Tick updates are responsible for handling DoTs, HoTs, and other components that can tick at intervals. */
	void UpdateTick(const float deltaTime);

	/** World spellcasts are the various commands for interacting with the world. */
	void UpdateWorldSpellcasts(const float deltaTime);

	/** Updates to health, qi, etc */
	void UpdateActors(const float deltaTime);

	/** Populate the actor registry. */
	void LoadActorData();

	/** Save the prototype registry. */
	void SaveActorData();

	/** Populate the prototype registries. */
	void LoadPrototypeData();

	/** Save the prototype registries. */
	void SavePrototypeData();

	/** Get a reference to the registry for actors. */
	const entt::registry& GetActorRegistry() const { return m_actorRegistry; }
	entt::registry& GetActorRegistry() { return m_actorRegistry; }

	/** Get a reference to the spell registry, which keeps prototypes for all the spells. */
	const entt::registry& GetSpellRegistry() const { return m_spellRegistry; }
	entt::registry& GetSpellRegistry() { return m_spellRegistry; }

	/** Get a reference to the spell casting registry. This is where spells are executed. */
	const entt::registry& GetSpellCastingRegistry() const { return m_spellcastingRegistry; }
	entt::registry& GetSpellCastingRegistry() { return m_spellcastingRegistry; }

	/** Performs some important fixups to the spell entity, based on the type of spell. Source and target entities
	are rewired to work as expected. */
	void RewireSpell(entt::registry& registry, entt::entity spellEntity, entt::entity sourceEntity, entt::entity targetEntity,
		EntityId crySourceEntityId, EntityId cryTargetEntityId);

	// TODO: Make this function not be slow and full of suck.
	/** Get's a spell entity using it's name. This function is =SLOW= so use it carefully. */
	entt::entity GetSpellByName(const char* spellName);

	/** Casts a spell, using the spell name as an index into the spell registry. */
	void CastSpellByName(const char* spellName, entt::entity sourceEntity, entt::entity targetEntity,
		EntityId crySourceEntityId, EntityId cryTargetEntityId);

	const RegisteredTypeMap& GetTypeMap() const { return m_functionDispatchMap; }
	RegisteredTypeMap& GetTypeMap() { return m_functionDispatchMap; }

private:
	entt::registry m_actorRegistry;
	entt::registry m_spellRegistry;
	entt::registry m_spellcastingRegistry;

	RegisteredTypeMap m_functionDispatchMap;
};
}