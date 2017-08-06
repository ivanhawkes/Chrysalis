#pragma once

class XmlNodeRef;


namespace Chrysalis
{
class CItemAccessoryParameter
{
public:
	CItemAccessoryParameter();
	~CItemAccessoryParameter();


	/** Resets this object to it's default state. */
	virtual void OnResetState();

	/**
	Reads the given node.

	\param	node	The node to read.

	\return	true if it succeeds, false if it fails.
	*/
	bool Read(const XmlNodeRef& node);
};
}