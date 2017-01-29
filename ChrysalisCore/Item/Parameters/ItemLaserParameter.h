#pragma once

class XmlNodeRef;


class CItemLaserParameter
{
public:
	CItemLaserParameter();
	~CItemLaserParameter();


	/** Resets this object to it's default state. */
	void OnResetState();

	/**
	Reads the given node.

	\param	node	The node to read.

	\return	true if it succeeds, false if it fails.
	*/
	bool Read(const XmlNodeRef& node);
};

