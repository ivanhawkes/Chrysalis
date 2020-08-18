#pragma once


namespace Chrysalis::ECS
{
template<typename Type>
void SaveComponent(const XmlNodeRef& node, const entt::entity entity, const Type& component)
{
	// Components need a prop meta value that allows us to associate their type with a string used to serialise them.
	if (auto prop = entt::resolve<Type>().prop("name-hs"_hs))
	{
		// Create a node for the component and mark it with the hashed name so we can lookup the class on loading.
		auto hashedName = prop.value().cast<entt::hashed_string>();
		XmlNodeRef componentNode = node->newChild(hashedName.data());

		// Serialise it to the node we just made.
		Serialization::SaveXmlNode(componentNode, Serialization::SStruct(component));
		componentNode->setAttr("entityId", static_cast<std::underlying_type_t<entt::entity>>(entity));
	}
	else
	{
		CryLogAlways("The component's hashed name (.prop(\"name-hs\") isn't in the registry");
	}
}


template<typename Type>
void LoadComponent(const XmlNodeRef& node, const Type& component)
{
	Serialization::LoadXmlNode(component, node);
}


// Used by ECS snapshot code to handle calls to serialise each entity and component in a registry.

struct SerialiseECSInput
{
	// We need to create a DOM for the entities to attach onto.
	SerialiseECSInput()
	{
	}


	// ***
	// *** Input
	// ***

	/** Called for each type to announce the number of entities of that type. */
	void operator()(std::underlying_type_t<entt::entity>& componentCount)
	{
		if (isFirstTime)
		{
			// The first time this runs it is looking for a count of the number of entities.
			componentCount = m_entitiesNode->getChildCount();
			isFirstTime = false;
		}
		else
		{
			if (auto node = m_metaComponentTypesNode->getChild(m_currentType))
			{
				// Dirty way to get the attribute out, since I don't have a primitive for getting an entity attribute.
				std::underlying_type_t<entt::entity> count {0};
				node->getAttr("count", count);
				componentCount = count;
			}

			m_currentType++;
		}
	}


	/** Called for each entity. */
	void operator()(entt::entity& entity)
	{
		// Each entity get's a couple of nodes made for storing it's components.
		XmlNodeRef node = m_entitiesNode->getChild(m_currentEntity);

		// Dirty way to get the attribute out, since I don't have a primitive for getting an entity attribute.
		std::underlying_type_t<entt::entity> entityId {0};
		node->getAttr("entityId", entityId);
		entity = static_cast<entt::entity>(entityId);

		m_currentEntity++;
	}


	/** Called for each entity and component type combination. */
	template<typename Type>
	void operator()(entt::entity& entity, Type& component)
	{
		if (XmlNodeRef componentNode = m_componentsNode->getChild(m_componentCounter))
		{
			// Dirty way to get the attribute out, since I don't have a primitive for getting an entity attribute.
			std::underlying_type_t<entt::entity> val {0};
			componentNode->getAttr("entityId", val);
			entity = static_cast<entt::entity>(val);

			// Each entity get's a couple of nodes made for storing it's components.	
			if (auto prop = entt::resolve<Type>().prop("name-hs"_hs))
			{
				auto hashedName = prop.value().cast<entt::hashed_string>();
				LoadComponent(componentNode, component);
			}
		}

		// Walk through the component list in order.
		m_componentCounter++;
	}


	void LoadFromFile(string fileName)
	{
		isFirstTime = true;

		if (m_essNode = GetISystem()->LoadXmlFromFile(fileName))
		{
			m_entitiesNode = m_essNode->findChild("entities");
			m_componentsNode = m_essNode->findChild("components");
			m_metaNode = m_essNode->findChild("meta");
			m_metaComponentTypesNode = m_metaNode->findChild("component-types");

			m_currentType = 0;
			m_currentEntity = 0;
		}
	}

private:
	bool isFirstTime {true};
	XmlNodeRef m_essNode;
	XmlNodeRef m_entitiesNode;
	XmlNodeRef m_componentsNode;
	XmlNodeRef m_metaNode;
	XmlNodeRef m_metaComponentTypesNode;
	std::underlying_type_t<entt::entity> m_currentType {0};
	std::underlying_type_t<entt::entity> m_currentEntity {0};
	std::underlying_type_t<entt::entity> m_componentCounter {0};
};


// Used by ECS snapshot code to handle calls to serialise each entity and component in a registry.

struct SerialiseECSOutput
{
	// We need to create a DOM for the entities to attach onto.
	SerialiseECSOutput()
	{
		m_essNode = GetISystem()->CreateXmlNode("ecs");
		if (m_essNode)
		{
			// Node for all the component types.
			m_entitiesNode = m_essNode->newChild("entities");
			m_componentsNode = m_essNode->newChild("components");

			// Create a meta node if one doesn't exist.
			m_metaNode = m_essNode->newChild("meta");
			m_metaComponentTypesNode = m_metaNode->newChild("component-types");
		}
	}


	// ***
	// *** Output
	// ***


	/** Called for each type to announce the number of entities of that type. */
	void operator()(std::underlying_type_t<entt::entity> componentCount)
	{
		if (isFirstTime)
		{
			isFirstTime = false;

			if (m_metaNode)
			{
				// Each entity get's a couple of nodes made for storing it's components.
				XmlNodeRef node = m_metaNode->newChild("entities");

				// This static cast should be fine with common underlying types.
				node->setAttr("count", static_cast<std::underlying_type_t<entt::entity>>(componentCount));
			}
		}
		else
		{
			if (m_metaComponentTypesNode)
			{
				// Each entity get's a couple of nodes made for storing it's components.
				XmlNodeRef node = m_metaComponentTypesNode->newChild("component-type");

				// This static cast should be fine with common underlying types.
				node->setAttr("count", static_cast<std::underlying_type_t<entt::entity>>(componentCount));
			}
		}
	}


	/** Called for each entity. */
	void operator()(entt::entity entity)
	{
		if (m_essNode)
		{
			// Each entity get's a couple of nodes made for storing it's components.
			XmlNodeRef entityNode = m_entitiesNode->newChild("entity");

			// This static cast should be fine with common underlying types.
			entityNode->setAttr("entityId", static_cast<std::underlying_type_t<entt::entity>>(entity));
		}
	}


	/** Called for each entity and component type combination. */
	template<typename Type>
	void operator()(entt::entity entity, const Type& component)
	{
		SaveComponent(m_componentsNode, entity, component);
	}


	void SaveToFile(string fileName)
	{
		isFirstTime = true;
		m_essNode->saveToFile(fileName);
	}

private:
	bool isFirstTime {true};
	XmlNodeRef m_essNode;
	XmlNodeRef m_entitiesNode;
	XmlNodeRef m_componentsNode;
	XmlNodeRef m_metaNode;
	XmlNodeRef m_metaEntitiesNode;
	XmlNodeRef m_metaComponentTypesNode;
};
}