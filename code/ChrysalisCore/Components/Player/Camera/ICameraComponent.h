#pragma once

#include <CryMath/Cry_Math.h>
#include <CryRenderer/Tarray.h>
#include <CrySystem/VR/IHMDDevice.h>
#include <CrySystem/VR/IHMDManager.h>
#include "DefaultComponents/Audio/ListenerComponent.h"


namespace Chrysalis
{
class CPlayerComponent;
struct SViewParams;


enum ECameraMode
{
	eCameraMode_NoCamera = -1,
	eCameraMode_FirstPerson = 0,
	eCameraMode_ActionRpg,
	eCameraMode_Examine,
	eCameraMode_Last
};

const Vec3 AverageEyePosition { 0.0f, 0.0f, 1.82f };


struct ICameraComponent
	: public IEntityComponent
	//, public IHmdDevice::IAsyncCameraCallback
{
protected:
	//// IAsyncCameraCallback
	//virtual bool OnAsyncCameraCallback(const HmdTrackingState& state, IHmdDevice::AsyncCameraContext& context) final;
	//// ~IAsyncCameraCallback

public:
	ICameraComponent() {}
	virtual ~ICameraComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<ICameraComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{899996BC-9269-4840-9C0A-6C81CDFA5E09}"_cry_guid;
		return id;
	}


	struct AnimationSettings
	{
		AnimationSettings() : positionFactor(0.0f), rotationFactor(0.0f), applyResult(false), stableBlendOff(false) {}

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
	Vec3 const GetPosition() const { return m_cameraMatrix.GetTranslation(); }


	/**
	Gets the rotation.

	\return The rotation.
	**/
	Quat const GetRotation() const { return Quat(m_cameraMatrix); }


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

	CCamera& GetCamera() { return m_camera; }
	const CCamera& GetCamera() const { return m_camera; }

	/** Set the field of view for this camera. */
	void SetFieldOfView(float fov) { 
		m_fieldOfView = fov;
	}

	/** Called by an active camera during it's update to update the camera view. */
	virtual void UpdateView();

	/**
	Is the view defined by this camera in first person? This will be true whenever the view is from the actor's /
	player's point of view or eyes.
	
	\return True if view first person, false if not.
	**/
	virtual bool IsViewFirstPerson() const = 0;

protected:
	/** The camera matrix. **/
	Matrix34 m_cameraMatrix { ZERO, IDENTITY };

	Schematyc::Range<0, 32768> m_nearPlane = 0.25f;
	CCamera m_camera;

	// TODO: Make this a schematyc range. Allow changing through schematyc functions.
	//CryTransform::CClampedAngle<20, 120> m_fieldOfView = 75.0_degrees;
	float m_fieldOfView {60.0f};

	/** Is debugging allowed? */
	bool m_isDebugEnabled { false };

	/** Is blending switched off? */
	bool m_bBlendingOff { false };
};
}