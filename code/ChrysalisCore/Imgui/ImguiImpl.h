#pragma once
#include "CryInput\IHardwareMouse.h"
#include "CryInput\IInput.h"

class CImguiRenderer;
namespace Cry
{
	namespace Imgui
	{
		class CPerformanceMonitor;
	}
}

class  CImguiImpl : IHardwareMouseEventListener, ISystemEventListener, IInputEventListener
{
	friend class CImguiRenderNode;

public:

	CImguiImpl();
	~CImguiImpl();

	void InitImgui();

	void Update();

	static CImguiImpl* Get();

	virtual void OnHardwareMouseEvent(int iX, int iY, EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta = 0) override;


	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;


	virtual bool OnInputEvent(const SInputEvent& event) override;

	IMaterial* GetFontMaterial() { return m_pFontMaterial; }
protected:
	ITexture* GetFontTexture() { return m_pFontTexture; }

private:
	void OnCachedInputEvent(const SInputEvent &event);
	void OnCachedMouseEvent(int iX, int iY, EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta = 0);

	void DrawPerformance();

	int m_bShowDemoWindow = 0;

	void InitImguiFontTexture();

	_smart_ptr<ITexture>			m_pFontTexture = nullptr;
	IMaterial*			m_pFontMaterial = nullptr;
	IMaterial*			m_pImageTest = nullptr;

	std::unique_ptr<CImguiRenderer> m_pRenderer;

	std::vector<SInputEvent> m_cachedInputEvents;

	struct SHWMouseEvent
	{
		SHWMouseEvent(int iX, int iY, EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta)
			: iX(iX) , iY(iY), eHardwareMouseEvent(eHardwareMouseEvent), wheelDelta(wheelDelta) {}

		int iX;
		int iY;
		EHARDWAREMOUSEEVENT eHardwareMouseEvent;
		int wheelDelta;
	};

	std::vector<SHWMouseEvent> m_cachedMouseEvents;

	std::unique_ptr<Cry::Imgui::CPerformanceMonitor> m_pPerfMon;

	int m_showPerfWidget = 0;
};