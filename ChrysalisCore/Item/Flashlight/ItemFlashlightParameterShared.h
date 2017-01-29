#pragma once

#include <CryMath/Cry_Vector3.h>
#include <SharedParameters/SharedParameters.h>


class XmlNodeRef;


struct SItemFlashlightParameterShared : public ISharedParams
{
	SHARED_PARAMS_BODY(SItemFlashlightParameterShared);

	SItemFlashlightParameterShared() {};
	virtual ~SItemFlashlightParameterShared() {};

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
	string lightCookie;
};

DECLARE_SHARED_POINTERS(SItemFlashlightParameterShared);
