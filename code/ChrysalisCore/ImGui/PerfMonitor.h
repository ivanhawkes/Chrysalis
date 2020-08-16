#pragma once

namespace Cry::Imgui
{
/** Provide performance metrics which can be displayed by Imgui. */
class CPerformanceMonitor
{
public:
	void Update();

protected:
	void Draw();

	//SDebugFPSInfo m_fpsInfo;

	std::vector<float> m_gpuFrameTimes;
	size_t m_gpuFrameTimeSamples {100};

	std::vector<float> m_frameTimes;
	size_t m_frameTimeSamples {100};

	std::vector<float> m_fps;
	size_t m_fpsSamples {100};
};
}
