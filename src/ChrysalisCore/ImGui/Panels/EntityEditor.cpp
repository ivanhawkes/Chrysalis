#include "StdAfx.h"

#ifdef IMGUI

#include "EntityEditor.h"
#include "ECS/ECS.h"
#include "ECS/Components/Components.h"
#include "ECS/Systems/Simulation.h"
#include "ECS/Components/Health.h"
#include <entt/entt.hpp>


namespace Chrysalis
{
void CEntityEditor::Draw()
{
	static int newId {0};

	ImGui::Begin(m_windowName, nullptr, ImGuiWindowFlags_MenuBar);

	// TODO: This should load the registry specific to the editor window.
	if (ImGui::Button("Load"))
	{
		ECS::Simulation.LoadPrototypeData();
	}

	// TODO: This should load the registry specific to the editor window.
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		ECS::Simulation.SavePrototypeData();
	}

	ImGui::SameLine();
	if (ImGui::Button("Add Entity"))
	{
		// Add a new blank entity.
		auto entity = m_registry.create();

		// Add a name component.
		ECS::Name& name = m_registry.emplace<ECS::Name>(entity);
		name.name.Format("Entity-%03d", newId);
		name.displayName = name.name;
		newId++;
	}

	EnumerateEntities();

	ImGui::End();
}


void CEntityEditor::EnumerateEntities()
{
	// Apply any damage to the damage modifiers.
	auto view = m_registry.view<ECS::Name>();
	for (auto& entity : view)
	{
		// Figure out if this is the presently selected entity.
		const auto selectedEntityFlag = ((entity == m_selectedEntity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		auto& name = view.get<ECS::Name>(entity);

		// Show the node and select it if it's clicked.
		bool isEntityNodeOpened = ImGui::TreeNodeEx(name.name.c_str(), selectedEntityFlag, "%s (%d)", name.name.c_str(), entity);
		if (ImGui::IsItemClicked())
		{
			m_selectedEntity = entity;
		}

		if (isEntityNodeOpened)
		{
			ImGui::SameLine();
			if (ImGui::Button("-"))
			{
				m_registry.destroy(entity);
			}

			if (ImGui::BeginPopupContextWindow())
			{
				auto& typeMap = ECS::Simulation.GetTypeMap();

				// Add a menu item for each component type.
				for (auto& registeredType : typeMap)
				{
					if (ImGui::MenuItem(registeredType.second.tag, NULL, false))
					{
						registeredType.second.emplaceFunction(m_registry, entity);
					}
				}

				ImGui::EndPopup();
			}

			// List all their components.
			m_registry.visit(entity, [this, entity](const auto type_id)
				{
					ECS::CSimulation::RegisteredType& registeredType = ECS::Simulation.GetTypeMap()[type_id.seq()];
					if (ImGui::TreeNode(registeredType.tag.data()))
					{
						registeredType.imgRenderFunction(m_registry, entity);

						ImGui::SameLine();
						if (ImGui::Button("-"))
						{
							registeredType.removeFunction(m_registry, entity);
						}

						ImGui::TreePop();
					}
				});

			ImGui::TreePop();
		}
	}
}
}

#endif
