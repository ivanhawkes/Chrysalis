/**
Provides functionality for a bare bones player entity. For Chrysalis we envision the player as an entity that represents the person
at the keyboard playing the game. The entity is responsible for gathering player input, overall camera management, and attaching to
character entities. It should be fairly lightweight and instead delegate the work out to other components.

In places where CRYENGINE makes implicit assumptions about the player class also being the character in play, we will proxy as best
as possible to the character class.

At present, entering game mode without being attached to an existing character entity will likely crash or cause unwanted effects.
*/
#pragma once

#include <DefaultComponents/Physics/CharacterControllerComponent.h>


namespace Chrysalis
{
struct ICameraComponent;
class CCameraManagerComponent;
struct CActorComponent;
class CPlayerInputComponent;


/** Defines the player class. Used to provide the player a way to control a character / puppet in the game. */
class CPlayerComponent
	: public IEntityComponent
{
protected:
	// IEntityComponent
	void Initialize() override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Update | EEntityEvent::GameplayStarted; }
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


	struct SInteractionState
	{
		enum class EInteractionMode
		{
			/** No special states apply. */
			eNoMode,

			/** The player is handling an item in their hands. */
			eHandlingEntity,

			/** The player is zoomed into an entity, examining / using it in some special way e.g. security pad, computer system. */
			eExamineZoom
		};


		bool IsCharacterMovementAllowed() const { return m_allowCharacterMovement; }
		bool IsCharacterRotationAllowed() const { return m_allowCharacterRotation; }
		bool IsCameraMovementAllowed() const { return m_allowCameraMovement; }

		void SetInteractionMode(EInteractionMode playerInteractionMode)
		{
			m_playerInteractionMode = playerInteractionMode;

			switch (m_playerInteractionMode)
			{
				case EInteractionMode::eNoMode:
					m_allowCharacterMovement = true;
					m_allowCharacterRotation = true;
					m_allowCameraMovement = true;
					break;

				case EInteractionMode::eHandlingEntity:
					m_allowCharacterMovement = false;
					m_allowCharacterRotation = false;
					m_allowCameraMovement = false;
					break;

				case EInteractionMode::eExamineZoom:
					m_allowCharacterMovement = false;
					m_allowCharacterRotation = false;
					m_allowCameraMovement = true;
					break;
			}
		}

	private:
		EInteractionMode m_playerInteractionMode {EInteractionMode::eNoMode};

		void SetAllowCharacterMovement(bool val) { m_allowCharacterMovement = val; }
		void SetAllowCharacterRotation(bool val) { m_allowCharacterRotation = val; }
		void SetAllowCameraMovement(bool val) { m_allowCameraMovement = val; }

		/** Is the player allowed to move their character? */
		bool m_allowCharacterMovement {true};

		/** Is the player allowed to rotate their character? */
		bool m_allowCharacterRotation {true};

		/** Is the player allowed to move the camera? */
		bool m_allowCameraMovement {true};
	};


	SInteractionState& GetinteractionState()
	{
		return m_playerInteractionState;
	}

	/**
	Query if the attached character is in third person mode.

	\return True if third person, false if not.
	**/
	bool IsViewFirstPerson() const;


	/**
	Attach to the character with this EntityID. Player will then be in control of that character. The camera will
	switch to following the newly attached entity.

	\param	characterId	Identifier for the character.
	*/
	void AttachToCharacter(EntityId characterId);


	/** Attach to a character with the default name 'Hero'. */
	void AttachToHero();


	/**
	Gets the player input handler.

	\return	null if it fails, else the player input.
	*/
	CPlayerInputComponent* GetPlayerInput() const { return m_pPlayerInput; }


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
	Gets IActor for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	CActorComponent* GetAttachedActor() const;


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
	Query if this instance is the local player. The local player is the client who is local to the machine the game
	is running on.

	\return True if local player, false if not.
	**/
	ILINE bool IsLocalPlayer()
	{
		return (gEnv->pGameFramework->GetClientActorId() == GetEntityId());
	}


	/**
	Gets local player, if there is one for this client.	It is possible there is no local player. The local player is
	the client who is local to the machine the game is running on.

	Convenience function.

	\return null if it fails, else the local player.
	**/
	ILINE static CPlayerComponent* GetLocalPlayer()
	{
		auto actorId = gEnv->pGameFramework->GetClientActorId();
		auto pActor = gEnv->pEntitySystem->GetEntity(actorId);
		CRY_ASSERT_MESSAGE(pActor, "The local actor Id %d did not return an entity.", actorId);

		return pActor->GetComponent<CPlayerComponent>();
	}


	/**
	Gets the character for the character / actor who is acting as our pawn. It is possible there is no local player
	or they are not presently attached to a character.

	Convenience function.

	\return	null if it fails, else the local character.
	*/
	static CActorComponent* GetLocalActor();


	/** Revives the atatched character, if there is one. */
	void OnRevive();

	/** A network client has connected to the player. */
	void NetworkClientConnect();

private:
	/** The camera that this instance uses. */
	CCameraManagerComponent* m_pCameraManager {nullptr};

	/**
	EntityId for the character we are presently attached onto. This is the actor who will receive movement requests
	as well as being the actor to query for position and rotation.
	*/
	EntityId m_attachedCharacterId {INVALID_ENTITYID};

	/** EntityId for the camera target. This is the entity which the player camera is targeting currently. */
	EntityId m_cameraTargetId {INVALID_ENTITYID};

	/** The player input handler. The player is responsible for ensuring the correct character entity / UI gets the input
	as appropriate. */
	CPlayerInputComponent* m_pPlayerInput {nullptr};

	/** Tracks interaction modes and allows you to query for conditions they impose e.g. no camera movement. */
	SInteractionState m_playerInteractionState;
};
}