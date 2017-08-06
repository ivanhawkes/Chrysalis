#pragma once

#include <CryNetwork/ISerialize.h>


class XmlNodeRef;

namespace Chrysalis
{
class CItemFlashlightParameter
{
public:
	CItemFlashlightParameter() {};
	virtual ~CItemFlashlightParameter() {};

	/** Resets this object to it's default state. */
	virtual void OnResetState();

	/**
	Reads the given node.

	\param	node	The node to read.

	\return	true if it succeeds, false if it fails.
	*/
	virtual bool Read(const XmlNodeRef& node);


	/** Simple serialization of all of the members for this feature. Override if you need anything more complex. */
	virtual void MemberSerialize(TSerialize ser);


	/** Prototype which provides a foundation for instances of this entity.*/
	string prototype = "";

	/** The amount of time the present batteries will last.*/
	int batteryRemaining = 21600;
};
}