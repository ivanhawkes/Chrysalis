/**
\file d:\CRYENGINE\Code\ChrysalisSDK\Actor\Player\Input\IPlayerInputComponent.h

Declares the IPlayerInputComponent interface.
**/
#pragma once


namespace Chrysalis
{
enum class EInputFlagType
{
	Hold = 0,
	Toggle
};

typedef uint8 TInputFlags;

enum class EInputFlag
	: TInputFlags
{
	None = 0,
	Left = 1 << 0,
	Right = 1 << 1,
	Forward = 1 << 2,
	Backward = 1 << 3
};


// HACK: This should be generically useful and so should get moved to a utility file.
template<typename T>
struct TListener
{
	void AddEventListener(T* pListener)
	{
		assert(pListener);
		if (pListener)
			stl::push_back_unique(m_ListenersList, pListener);
	}

	void RemoveEventListener(T* pListener)
	{
		assert(pListener);
		m_ListenersList.remove(pListener);
	}

	std::list<T*> GetListeners() { return m_ListenersList; }

private:
	std::list<T*> m_ListenersList;
};


struct IPlayerInputComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;

	static void Register(Schematyc::CEnvRegistrationScope& componentScope)
	{
	}

public:
	IPlayerInputComponent() {}
	virtual ~IPlayerInputComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<IPlayerInputComponent>& desc)
	{
		desc.SetGUID(IPlayerInputComponent::IID());
		desc.SetEditorCategory("Hidden");
		desc.SetLabel("Player Input Component Interface");
		desc.SetDescription("No description.");
		desc.SetIcon("icons:ObjectTypes/light.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::None });
	}

	static CryGUID& IID()
	{
		static CryGUID id = "{9A2DDB06-ED3C-4070-A95E-3F49E82FAE97}"_cry_guid;
		return id;
	}


	enum class EInputType
	{
		Null,
		Player,
		NetPlayer,
		Ai,
		DedicatedServer,
		_LAST,
	};


	/** Listen for 'special' keys and be notified when they are input e.g. ESC, Examine. */
	struct IInputSpecialListener
	{
		virtual ~IInputSpecialListener() {};

		virtual void OnInputSpecialEsc() = 0;
		virtual void OnInputSpecialExamine() = 0;
	};

	virtual TListener<IInputSpecialListener> GetSpecialListener() = 0;

	/**
	Reset the movements to zero state.
	*/
	virtual void ResetMovementState() = 0;


	/**
	Reset the actions resulting from movement state to zero state.
	*/
	virtual void ResetActionState() = 0;


	/**
	Given the current input state, calculate a vector that represents the direction the player wishes their
	character to move. The vector is normalised. Movement will only be affected along the X and Y axis,
	since we are not presently processing Z input (up). This should be sufficient for most RPG style games.

	\param	baseRotation	The vector will be calculated using this rotation as a base and then applying the input
	requests relative to this direction.

	\return The calculated movement direction.
	**/
	virtual Vec3 GetMovement(const Quat& baseRotation) = 0;


	/**
	Given the current input state, calculate an angular vector that represents the rotation the player has requested
	using the mouse / xbox controller / etc.

	This is typically used to rotate the character and the camera.

	\return The rotation.
	**/
	virtual Ang3 GetRotationDelta() = 0;


	/**
	Gets the combined pitch delta from all devices.

	\return The pitch delta.
	**/
	virtual float GetPitchDelta() = 0;


	/**
	Gets the combined yaw delta from all devices.

	\return The yaw delta.
	**/
	virtual float GetYawDelta() = 0;


	/**
	Gets the pitch delta from the mouse.

	\return The pitch delta.
	**/
	virtual float GetMousePitchDelta() = 0;


	/**
	Gets the yaw delta from the mouse.

	\return The yaw delta.
	**/
	virtual float GetMouseYawDelta() = 0;


	/**
	Gets the pitch delta from the XBOX controller.

	\return The pitch delta.
	**/
	virtual float GetXiPitchDelta() = 0;


	/**
	Gets the yaw delta from the XBOX controller.

	\return The yaw delta.
	**/
	virtual float GetXiYawDelta() = 0;

	/**
	Gets number of times the player has requested a change in zoom level since the last frame. Cameras can query this
	value and use it to adjust their zoom. Value is set in stone on PrePhysicsUpdate () so it can be consistently
	queried during the Update() routine. It's provided as a float, even though currently implementation is integral
	steps. This will give us more fine control if needed later.


	\return The zoom delta.
	**/
	virtual float GetZoomDelta() = 0;


	/**
	Gets movement state flags.

	\return The movement state flags.
	**/
	virtual TInputFlags GetMovementDirectionFlags() const = 0;


	// ***
	// *** HMD based head tracking. Provide the ability to handle head movement separate to body movement.
	// ***

	virtual Vec3 GetHeadMovement(const Quat& baseRotation) = 0;

	virtual Ang3 GetHeadRotationDelta() = 0;

	virtual float GetHeadPitchDelta() = 0;

	virtual float GetHeadYawDelta() = 0;
};
}