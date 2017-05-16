#pragma once

#include <CryMath/Cry_Math.h>
#include <CryRenderer/Tarray.h>
#include <Entities/Helpers/ISimpleExtension.h>


class CPlayer;
struct SViewParams;


enum ECameraMode
{
	eCameraMode_NoCamera = -1,
	eCameraMode_FirstPerson = 0,
	eCameraMode_ActionRpg,
	eCameraMode_FirstPersonHmd,
	eCameraMode_Last
};

const Vec3 AverageEyePosition { 0.0f, 0.0f, 1.82f };


/**
A camera pose provides both a position and rotation which may be used to represent the position of a camera
within the game world. Simple member functions provides helpers for composing, scaling and constructing these
representations.
**/
struct CCameraPose
{
public:

	/** Constructs a camera with zero position and an identity quaternion for it's rotation component. */
	CCameraPose() :
		m_position(ZERO),
		m_rotation(IDENTITY)
	{}


	/**
	Constructor.
	
	\param	position The position.
	\param	rotation The rotation.
	**/
	CCameraPose(Vec3 position, Quat rotation) :
		m_position(position),
		m_rotation(rotation)
	{}


	/**
	A constructor which copies it's position and rotation values from another CCameraPose object.
	
	\param	copy The object from which we will copy the position and rotation members.
	**/
	CCameraPose(const CCameraPose& copy) :
		m_position(copy.m_position),
		m_rotation(copy.m_rotation)
	{}


	/**
	The left hand camera pose is composed with the right hand camera pose.
	
	\param	lhs The left hand side.
	\param	rhs The right hand side.
	
	\return A CCameraPose that is the result of composing the lhs and the rhs.
	**/
	static CCameraPose Compose(const CCameraPose& lhs, const CCameraPose& rhs)
	{
		return CCameraPose(
			lhs.GetPosition() + rhs.GetPosition(),
			lhs.GetRotation() * rhs.GetRotation());
	}


	/**
	Scales both the vector and the rotation for the camera pose by the factor provided. The rotation is scaled using
	a SLERP between the identity quaternion and the lhs rotation.
	
	\param	lhs    The left hand side.
	\param	factor The factor.
	
	\return A CCameraPose that is a copy of the lhs scaled by factor.
	**/
	static CCameraPose Scale(const CCameraPose& lhs, float factor)
	{
		if (factor == 1.0f)
			return lhs;
		else
			return CCameraPose(
				lhs.GetPosition() * factor,
				Quat::CreateSlerp(IDENTITY, lhs.GetRotation(), factor));
	}


	/**
	Linear interpolation between two camera poses.
	
	\param	from   A valid camera pose that defines a position and rotation from which you wish to interpolate.
	\param	to	   A valid camera pose that defines a position and rotation towards which you wish to interpolate.
	\param	stride The stride.
	
	\return A camera pose that represents a linear interpolation between the 'from' and 'to' parameters.
	**/
	static CCameraPose Lerp(const CCameraPose& from, const CCameraPose& to, float stride)
	{
		return CCameraPose(
			LERP(from.GetPosition(), to.GetPosition(), stride),
			Quat::CreateSlerp(from.GetRotation(), to.GetRotation(), stride));
	}


	/**
	Gets the position.
	
	\return The position.
	**/
	Vec3 GetPosition() const { return m_position; }


	/**
	Gets the rotation.
	
	\return The rotation.
	**/
	Quat GetRotation() const { return m_rotation; }

private:

	/** Position of the camera. */
	Vec3 m_position;

	/** Rotation of the camera. */
	Quat m_rotation;
};


struct ICameraComponent : public ISimpleExtension
//struct ICameraComponent : public IEntityComponent
{
	struct AnimationSettings
	{
		AnimationSettings() : positionFactor(0.0f), rotationFactor(0.0f), applyResult(false), stableBlendOff(false)
		{}

		float positionFactor;
		float rotationFactor;
		bool  applyResult;
		bool  stableBlendOff;
	};


	/**
	Player cameras generally need to follow an actor. This allows us to switch which entity represents the actor that
	the camera is following.
	
	\param	entityID If non-null, the entity.
	**/
	virtual void AttachToEntity(EntityId entityID) = 0;


	/**
	Query if this camera allows debugging.
	
	\return true if debug allowed, false if not.
	**/
	bool IsDebugEnabled(void) const { return (m_isDebugEnabled); };


	/**
	Enables / disable debugging helpers.
	
	\param	enabled True if it should be enabled.
	**/
	void SetDebugEnabled(const bool enabled) { m_isDebugEnabled = enabled; };


	/**
	Is this camera capable of transitioning?
	
	\return true if we can transition, false if not.
	**/
	virtual bool CanTransition() { return false; };


	/**
	Sets camera animation factor.
	
	\param	animationSettings The animation settings.
	**/
	virtual void SetCameraAnimationFactor(const AnimationSettings& animationSettings) {};


	/**
	Gets camera animation factor.
	
	\param [in,out]	position The position.
	\param [in,out]	rotation The rot.
	**/
	virtual void GetCameraAnimationFactor(float& position, float& rotation);


	/**
	Query if the ability to blend between frames is off.
	
	\return true if blending off, false if not.
	**/
	const bool IsBlendingOff() const
	{
		return m_bBlendingOff;
	}


	/**
	Gets the position.
	
	\return The position.
	**/
	Vec3 const GetPosition() const { return m_cameraPose.GetPosition(); }


	/**
	Gets the rotation.
	
	\return The rotation.
	**/
	Quat const GetRotation() const { return m_cameraPose.GetRotation(); }


	/**
	Gets the previously stored camera pose.
	
	\param	cameraPose The camera pose.
	
	\return The camera pose.
	**/
	const CCameraPose& GetCameraPose() { return m_cameraPose; }


	/**
	Gets a vector representing a point at which the camera is presently aiming. This will involve a ray-cast
	operation in the forward direction using camera space. Since it's primary use will be for weapon targeting it should
	cast a reasonable distance and hit living creatures, terrain, and physical entities.
	
	\param	pRayCastingEntity	The entity from which we are ray-casting. We need this to exclude it's physics from the
								ray-cast.
	
	\return The aim target.
	**/
	const Vec3 GetAimTarget(const IEntity* pRayCastingEntity) const;


	/** Executes the activate action. */
	virtual void OnActivate() = 0;

	/** Executes the deactivate action. */
	virtual void OnDeactivate() = 0;


protected:

	/**
	Sets camera pose. This stores the position and rotation for this camera. This should be called during a camera's
	update function after the new camera pose is calculated.
	
	\param	cameraPose The camera pose.
	**/
	void SetCameraPose(const CCameraPose& cameraPose) { m_cameraPose = cameraPose; }


	/** The position and rotation of the camera. This should be updated every frame during the update function. */
	CCameraPose m_cameraPose { CCameraPose() };

	/** Is debugging allowed? */
	bool m_isDebugEnabled { false };

	/** Is blending switched off? */
	bool m_bBlendingOff { false };
};
