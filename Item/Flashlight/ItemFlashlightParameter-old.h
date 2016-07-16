#pragma once

#include "Utility\ItemString.h"


class CItemFlashlightParameter
{
public:
	CItemFlashlightParameter();
	~CItemFlashlightParameter();

	/** Resets this object to it's default state. */
	void Reset();

	/**
	Reads the given node.

	\param	node	The node to read.

	\return	true if it succeeds, false if it fails.
	*/
	bool Read(const XmlNodeRef& node);


	/** The style of light which will be created e.g. steady, flickering, etc. */
	uint8 style;
	
	/** The color. */
	Vec3 color;
	
	/** The diffuse multiplier. */
	float diffuseMultiplier;
	
	/** The specular multiplier. */
	float specularMultiplier;
	
	/** High dynamic range. */
	float hdrDynamic;
	
	/** The distance. */
	float distance;
	
	/** Field of view. */
	float fov;
	
	/** The animation speed. */
	float animSpeed;
	
	/** The fog volume color. */
	Vec3 fogVolumeColor;
	
	/** The fog volume radius. */
	float fogVolumeRadius;
	
	/** Size of the fog volume. */
	float fogVolumeSize;
	
	/** The fog volume density. */
	float fogVolumeDensity;
	
	/** The light cookie. */
	ItemString lightCookie;
};
