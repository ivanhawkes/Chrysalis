#pragma once


namespace Chrysalis::ECS
{
struct IComponent;

// Serialise a component into an XML node.
void SaveComponent(const XmlNodeRef& node, const IComponent& component);

// Loads the entities and components from an XML file.
void LoadECSFromXML(string fileName, entt::registry& registry);


// Used by ECS snapshot code to handle calls to serialise each entity and component in a registry.

struct SerialiseECS
{
	// We need to create a DOM for the entities to attach onto.
	SerialiseECS()
	{
		m_entitiesNode = GetISystem()->CreateXmlNode("entities");
	}


	// Input.
	// NOTE: Missing one function, but we're not doing input yet, so haven't chased it up yet.
	template<typename Type>
	void operator()(entt::entity& entity, Type& component)
	{
	}


	// Output.
	void operator()(unsigned int entity)
	{
		// Not really sure why this is getting called. Need it defined to prevent compile errors.
	}


	void operator()(entt::entity entity)
	{
		if (m_entitiesNode)
		{
			// Each entity get's a couple of nodes made for storing it's components.
			XmlNodeRef entityNode = m_entitiesNode->newChild("entity");
			XmlNodeRef componentsNode = entityNode->newChild("components");

			// Store a copy in a map to enable fast lookup when iterating the components.
			m_nodeMap[entity] = componentsNode;
		}
	}


	template<typename Type>
	void operator()(entt::entity entity, const Type& component)
	{
		// Grab the components node from the map and then inject the component into the XML DOM.
		if (XmlNodeRef componentsNode = m_nodeMap[entity])
		{
			SaveComponent(componentsNode, component);
		}
	}


	void SaveToFile(string fileName)
	{
		m_entitiesNode->saveToFile(fileName);
	}

private:
	XmlNodeRef m_entitiesNode;
	std::map<entt::entity, XmlNodeRef> m_nodeMap;
};

}