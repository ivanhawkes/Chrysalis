#include <StdAfx.h>

#include "Qi.h"
#include "CrySerialization/Enum.h"


namespace Chrysalis::ECS
{
#ifdef IMGUI
void Qi::ImGuiRender()
{
	qi.ImGuiRender();
}


void UtiliseQi::ImGuiRender()
{
	ImGui::InputFloat("Quantity:", &quantity);
}


void UtiliseQiOverTime::ImGuiRender()
{
	ImGui::InputFloat("Quantity:", &quantity);
	ImGui::InputFloat("Duration:", &duration);
	ImGui::InputFloat("Interval:", &interval);
}


void ReplenishQi::ImGuiRender()
{
	ImGui::InputFloat("Quantity:", &quantity);
}


void ReplenishQiOverTime::ImGuiRender()
{
	ImGui::InputFloat("Quantity:", &quantity);
	ImGui::InputFloat("Duration:", &duration);
	ImGui::InputFloat("Interval:", &interval);
}
#endif

}
