/**
\file	d:\Chrysalis\Source\Chrysalis\Actor\Player\Player.h

Provides functionality for a bare bones player entity. For Chrysalis we envision the player as an entity that represents the person
at the keyboard playing the game. The entity is responsible for gathering player input, overall camera management, and attaching to
character entities. It should be fairly lightweight and instead delegate the work out to other components.

In places where CRYENGINE makes implicit assumptions about the player class also being the character in play, we will proxy as best
as possible to the character class.

At present, entering game mode without being attached to an existing character entity will likely crash or cause unwanted effects.
Code should be added to armour against this.
*/
#pragma once


namespace Chrysalis
{
struct ICameraComponent;
struct IPlayerInputComponent;
class CCameraManagerComponent;
class CCharacterComponent;
class CItemComponent;
class CWeapon;


/** Defines the player class. Used to provide the player a way to control a character / puppet in the game. */
class CPlayerComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	// ~IEntityComponent

public:
	CPlayerComponent() {}
	virtual ~CPlayerComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{58A26B16-EC1D-4700-A76D-38CBB7B2B400}"_cry_guid;
		return id;
	}


	enum class EPlayerInteractionMode
	{
		eNoMode = 0,
		eHandlingEntity,
		eExamineZoom
	};


	/**
	Query if the attached character is in third person mode.

	\return True if third person, false if not.
	**/
	bool IsThirdPerson() const;


	/** The player has toggled between third person and first person view modes. */
	void OnToggleThirdPerson();


	/**
	Attach to the character with this EntityID. Player will then be in control of that character. The camera will
	switch to following the newly attached entity.

	\param	characterId	Identifier for the character.
	*/
	void AttachToCharacter(EntityId characterId);


	/** Attach to a character with the default name 'Hero'. */
	void AttachToCharacter();


	/**
	Gets the player input handler.

	\return	null if it fails, else the player input.
	*/
	IPlayerInputComponent* GetPlayerInput() const { return m_pPlayerInput; }


	/**
	Gets EntityId for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	EntityId GetAttachedEntityId() const;


	/**
	Gets IEntity for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	IEntity* GetAttachedEntity() const;


	/**
	Gets CCharacterComponent for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	CCharacterComponent* GetAttachedCharacter() const;


	/**
	Gets EntityId for actor who is the focus of our camera.

	\return	The character identifier.
	*/
	EntityId GetCameraTargetId() const;


	/**
	Gets camera manager.

	\return null if it fails, else the camera manager.
	**/
	CCameraManagerComponent* GetCameraManager() const;


	/**
	Gets the player camera currently in use.

	\return	null if it fails, else the current camera.
	*/
	ICameraComponent* GetCamera() const;


	/**
	Gets local player, if there is one for this client.	It is possible there is no local player.

	Convenience function.

	\return	null if it fails, else the local player.
	*/
	ILINE static CPlayerComponent* GetLocalPlayer()
	{
		auto actorId = gEnv->pGameFramework->GetClientActorId();
		auto pActor = gEnv->pEntitySystem->GetEntity(actorId);
		if (pActor)
			return pActor->GetComponent<CPlayerComponent>();

		return nullptr;
	}


	/**
	Gets the character for the character / actor who is acting as our pawn. It is possible there is no local player
	or they are not presently attached to a character.

	Convenience function.

	\return	null if it fails, else the local character.
	*/
	static CCharacterComponent* GetLocalCharacter();


	/**
	If we have a local player and they are attached to a character, this will retrieve the
	item that character is using.

	\return	null if it fails, else the local item.
	*/
	static CItemComponent* GetLocalItem();


	/**
	If we have a local player and they are attached to a character, this will retrieve the
	weapon that character is using.

	\return	null if it fails, else the local weapon.
	*/
	static CWeapon* GetLocalWeapon();

	bool GetAllowCharacterMovement() const { return m_allowCharacterMovement; }
	bool GetAllowCharacterRotation() const { return m_allowCharacterRotation; }
	bool GetAllowCameraMovement() const { return m_allowCameraMovement; }


	/**
	When true, the player is interacting with a useable entity.

	\param	val True to value.
	**/
	void SetObjectInteractionMode(bool val)
	{
		m_playerInteractionMode = val ? EPlayerInteractionMode::eHandlingEntity : EPlayerInteractionMode::eNoMode;

		//m_allowCharacterMovement = !val;
		//m_allowCharacterRotation = !val;
		//m_allowCameraMovement = !val;
	}


	/**
	When true, the player is zoomed in and examining an entity. Movement requests should cancel this mode.

	\param	val True to value.
	**/
	void SetExamineInteractionMode(bool val)
	{
		m_playerInteractionMode = val ? EPlayerInteractionMode::eExamineZoom : EPlayerInteractionMode::eNoMode;

		//m_allowCharacterMovement = !val;
		//m_allowCharacterRotation = !val;
		//m_allowCameraMovement = !val;
	}


	/** Revives the atatched character, if there is one. */
	void Revive();

private:
	void SetAllowCharacterMovement(bool val) { m_allowCharacterMovement = val; }
	void SetAllowCharacterRotation(bool val) { m_allowCharacterRotation = val; }
	void SetAllowCameraMovement(bool val) { m_allowCameraMovement = val; }

	EPlayerInteractionMode m_playerInteractionMode { EPlayerInteractionMode::eNoMode };

	/** Specifies whether this instance is the client actor. */
	bool m_isClient { false };

	/** The camera that this instance uses. */
	CCameraManagerComponent* m_pCameraManager { nullptr };

	/**
	EntityId for the character we are presently attached onto. This is the actor who will receive movement requests
	as well as being the actor to query for position and rotation.
	*/
	EntityId m_attachedCharacterId { INVALID_ENTITYID };

	/** EntityId for the camera target. This is the entity which the player camera is targeting currently. */
	EntityId m_cameraTargetId { INVALID_ENTITYID };

	/** The player input handler. The player is responsible for ensuring the correct character entity / UI gets the input
	as appropriate. */
	IPlayerInputComponent* m_pPlayerInput { nullptr };

	/** Identifier for the team. */
	int m_teamId { 0 };

	/** Is the player allowed to move their character? */
	bool m_allowCharacterMovement { true };

	/** Is the player allowed to rotate their character? */
	bool m_allowCharacterRotation { true };

	/** Is the player allowed to move the camera? */
	bool m_allowCameraMovement { true };
};
}