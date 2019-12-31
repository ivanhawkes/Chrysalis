#include <StdAfx.h>

#include "XMLSerializer.h"
#include "ECS/Components/Components.h"


namespace Chrysalis::ECS
{
void SaveComponent(const XmlNodeRef& node, const IComponent& component)
{
	// Create a node for the component and mark it with the GUID so we can lookup the class on loading.
	XmlNodeRef componentNode = node->newChild(component.GetHashedName().data());
	//componentNode->setAttr("typeGUID", component.GetGuid());

	// Serialise the properties into a child node to keep it fairly neat and tidy.
	//XmlNodeRef propertiesNode = componentNode->newChild("properties");
	//Serialization::SaveXmlNode(propertiesNode, Serialization::SStruct(component));
	Serialization::SaveXmlNode(componentNode, Serialization::SStruct(component));
}


void LoadECSFromXML(string fileName, entt::registry& registry)
{
	// Load the file into an in-memory structure.
	if (XmlNodeRef entitiesNode = GetISystem()->LoadXmlFromFile(fileName))
	{
		// Iterate through all the children which should be individual entities.
		for (int i = 0, n = entitiesNode->getChildCount(); i < n; ++i)
		{
			// Grab the entity, which is basically an empty wrapper.
			XmlNodeRef entityNode = entitiesNode->getChild(i);

			// We should create a fresh entity for the components to attach onto.
			auto entity = registry.create();

			// Get the properties node - this holds all the properties for this component.
			if (XmlNodeRef componentsNode = entityNode->findChild("components"))
			{
				// Iterate through all the components.
				for (int i = 0, n = componentsNode->getChildCount(); i < n; ++i)
				{
					// Grab the entity, which is basically an empty wrapper.
					XmlNodeRef componentNode = componentsNode->getChild(i);

					// Using the tag as a unique ID for the class for now.
					auto hash = entt::hashed_string {componentNode->getTag()};

					// Ask the system for the class and default construct a component of that type.
					if (auto component = entt::resolve(hash))
					{
						// Uses the registry to construct a component, assign it to the entity, and then return a reference for us to use.
						auto any = component.construct(entity, &registry);

						// Get the properties node - this holds all the properties for this component.
						//XmlNodeRef propertiesNode = componentNode->findChild("properties");

						// Serialise the properties across to the component.
						auto& iComponent = any.cast<ECS::IComponent>();
						//Serialization::LoadXmlNode(iComponent, propertiesNode);
						Serialization::LoadXmlNode(iComponent, componentNode);
					}
				}
			}
		}
	}
}
}