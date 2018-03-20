#pragma once

#include <SharedParameters/SharedParameters.h>

class XmlNodeRef;

namespace Chrysalis
{
struct SFogVolume : public ISharedParams
{
	SHARED_PARAMS_BODY(SFogVolume);

	SFogVolume() = default;
	virtual ~SFogVolume() = default;

	/** Resets this object to it's default state. */
	virtual void Reset();

	/**
	Reads the given node.

	\param	node	The node to read.

	\return	true if it succeeds, false if it fails.
	*/
	virtual bool Read(const XmlNodeRef& node);

	/** Simple serialization of all of the members for this feature. Override if you need anything more complex. */
	void SerializeProperties(Serialization::IArchive& archive);

	/** The color (RGB).*/
	Vec3 color;

	/** Global density.*/
	float globalDensity;

	/** Density offset.*/
	float densityOffset;
};

DECLARE_SHARED_POINTERS(SFogVolume);
}