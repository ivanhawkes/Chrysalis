#pragma once

#include <IActionMapManager.h>


#define DECL_ACTION(name) ActionId name;

class CGameActionMaps : public IActionMapEventListener
{
public:
	CGameActionMaps();
	~CGameActionMaps();

	// IActionMapEventListener
	virtual void OnActionMapEvent(const SActionMapEvent& event);
	// ~IActionMapEventListener


	// HACK: It's dirty, but it prevents missing actions out from the declarations.
#include "Actions.txt"


private:
	
	/** Creates a set of filters to use for action maps e.g. no mouse, vehicle use, cutscene playing. */
	void CreateActionFilters();
};

#undef DECL_ACTION

