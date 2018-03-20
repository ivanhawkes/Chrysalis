#pragma once

#include <SharedParameters/SharedParameters.h>

class XmlNodeRef;

namespace Chrysalis
{
struct SDynamicLight : public ISharedParams
{
	SHARED_PARAMS_BODY(SDynamicLight);

	SDynamicLight() = default;
	virtual ~SDynamicLight() = default;

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

	/** Specifies how far from the source the light affects the surrounding area.*/
	float radius { 10.0f };

	/** Control the strength of the specular brightness.*/
	float specularMultiplier;

	/** Control the strength of the diffuse color.*/
	float diffuseMultiplier;

	/** Specifies the radius of the area light bulb.*/
	float attenuationRadius { 5.0f };

	/** Diffuse color expressed as RGB e.g. 128, 255, 128*/
	Vec3 diffuseColor;

	/** Specifies the Angle on which the light texture is projected.*/
	float projectorFoV { 90.0f };

	/** Set the near plane for the projector, any surfaces closer to the light source than this value will not be projected on.*/
	float projectorNearPlane { 0.1f };

	/** A texture to used for custom falloff.*/
	string projectorTexture;

	/** A material. */
	string material;

	/** Style variation (flickering, waxing / wanning / etc)*/
	uint8 lightStyle;

	/** Rate at which the style animation will play.*/
	uint8 animationSpeed { 1 };

	/** Point in the cycle (style) at which light animation begins.*/
	uint8 lightPhase;

	/** Moves the shadow cascade toward or away from the shadow-casting object.*/
	float shadowBias { 1.0f };

	/** Allows you to adjust the gradient (slope-based) bias used to compute the shadow bias.*/
	float shadowSlopeBias { 1.0f };

	/** */
	float shadowResolutionScale;

	/** Percentage of the shadow pool the light should use for its shadows.*/
	float shadowMinimumResolutionPercent { 1.0f };

	/** Define the minimum radius from the light source to the player camera that the ShadowUpdateRatio setting will be ignored.*/
	float shadowUpdateMinimumRadius { 1.0f };

	/** Define the update ratio for shadow maps cast from this light.*/
	float shadowUpdateRatio { 0.01f };

};

DECLARE_SHARED_POINTERS(SDynamicLight);
}