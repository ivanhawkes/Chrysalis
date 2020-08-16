#pragma once

#ifdef IMGUI

#include "entt/entt.hpp"


namespace Chrysalis
{

/** Provide performance metrics which can be displayed by Imgui. */
class CEntityEditor
{
public:
	CEntityEditor() = delete;
	~CEntityEditor() = default;

	CEntityEditor(entt::registry& registry, string windowName)
		: m_registry(registry), m_windowName(windowName) {};

	void Draw();
	void EnumerateEntities();

private:
	entt::registry& m_registry;
	string m_windowName;
};
}

#endif