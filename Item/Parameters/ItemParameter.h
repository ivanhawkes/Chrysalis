#pragma once

#include "Item/Item.h"

class CItemAccessoryParameter;
class CItemBaseParameter;
//class CItemFlashlightParameter;
class CItemGeometryParameter;
class CItemLaserParameter;

// TODO: this was a stupid idea for implementing shared params - remove once I have the actual one done.


struct CItemParameter
{
	CItemParameter();
	~CItemParameter();


	/** Resets this object to it's default state. */
	void Reset();


	/**
	Reads the given node.

	\param	node	The node to read.

	\return	true if it succeeds, false if it fails.
	*/
	bool Read(const XmlNodeRef& node);

	const CItemAccessoryParameter* GetItemAccessoryParameter() { return m_pItemAccessoryParameter; };
	const CItemBaseParameter* GetItemBaseParameter() { return m_pItemBaseParameter; };
	//const CItemFlashlightParameter* GetItemFlashlightParameter() { return m_pItemFlashlightParameter; };
	const CItemGeometryParameter* GetItemGeometryParameter() { return m_pItemGeometryParameter; };
	const CItemLaserParameter* GetItemLaserParameter() { return m_pItemLaserParameter; };

private:
	CItemAccessoryParameter* m_pItemAccessoryParameter = nullptr;
	CItemBaseParameter* m_pItemBaseParameter = nullptr;
	//CItemFlashlightParameter* m_pItemFlashlightParameter = nullptr;
	CItemGeometryParameter* m_pItemGeometryParameter = nullptr;
	CItemLaserParameter* m_pItemLaserParameter = nullptr;
};
