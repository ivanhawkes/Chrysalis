#include "StdAfx.h"
#include "PerfMonitor.h"
#include "Imgui/imgui.h"
#include <numeric>


void Cry::Imgui::CPerformanceMonitor::Update()
{
	gEnv->p3DEngine->FillDebugFPSInfo(m_fpsInfo);
	
	
	float gpuFrameTime = gEnv->pRenderer->GetGPUFrameTime();
	float frameTime = gEnv->pSystem->GetITimer()->GetRealFrameTime();
	float fps = gEnv->pSystem->GetITimer()->GetFrameRate();
	

	auto queueValue = [](float value, size_t maxSamples, std::vector<float>& container) {
		if (container.size() >= maxSamples)
			container.erase(container.begin());

		container.push_back(value);
	};

	queueValue(gpuFrameTime * 1000, m_gpuFrameTimeSamples, m_gpuFrameTimes);
	queueValue(fps, m_fpsSamples, m_fps);
	queueValue(frameTime * 1000, m_frameTimeSamples, m_frameTimes);

	Draw();
}

void Cry::Imgui::CPerformanceMonitor::Draw()
{
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::Begin("PerfMon", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

	float avaragedFPS = std::accumulate(m_fps.begin(), m_fps.end(), 0) / m_fps.size();

	string statString;
	statString.Format("%.1f fps", avaragedFPS);
	ImGui::PlotLines("", m_fps.data(), m_fps.size(), 0, statString.c_str());

	statString.Format("%.1f ms", m_frameTimes.back());
	ImGui::PlotHistogram("", m_frameTimes.data(), m_frameTimes.size(), 0, statString.c_str(), 0, 100);

	statString.Format("%.1f ms (GPU)", m_gpuFrameTimes.back());
	ImGui::PlotHistogram("", m_gpuFrameTimes.data(), m_gpuFrameTimes.size(), 0, statString.c_str(),0, 100);
	
	ImGui::Text("Drawcalls: %d",gEnv->pRenderer->GetCurrentNumberOfDrawCalls());

	ImGui::End();
}

