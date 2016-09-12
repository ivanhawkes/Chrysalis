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

#include <Game/Game.h>
#include <Actor/ISimpleActor.h>
#include <Actor/Actor.h>


struct ICameraComponent;
struct ICameraManagerComponent;
struct IPlayerInputComponent;
class CCharacter;
class CItem;
class CWeapon;
class CActor;


/**
An implementation of the IActor interface. Defines the player class. Used to provide the player a way to control
a character in the game. This class provides heath / armour functionality, player camera / movement functionality and any
other functionality needed for a fully playable character. Every CRYENGINE game NEEDS to have an IActor
implementation (e.g. "Player" class (client actor)).

\sa	CGameObjectExtensionHelper&lt;CPlayer, IActor&gt;
*/
class CPlayer : public CGameObjectExtensionHelper <CPlayer, ISimpleActor>
{
public:
	struct SExternalCVars
	{
		float m_mass;
	};

	// ISimpleActor
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
	bool ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	void HandleEvent(const SGameObjectEvent& event) override;

	bool IsThirdPerson() const override;
	void ToggleThirdPerson() override;
	bool IsPlayer() const override { return true; }
	bool IsClient() const override;
	void InitLocalPlayer() override;
	const char* GetActorClassName() const override { return "CPlayer"; }
	ActorClass GetActorClass() const override { return EACT_PLAYER; }
	// ~ISimpleActor


	// ***
	// *** CPlayer
	// ***

public:

	enum EPlayerUpdateSlot
	{
		ePlayerUpdateSlot_Main = 0,
		ePlayerUpdateSlot_CameraFirstPerson,
		ePlayerUpdateSlot_CameraThirdPerson
	};


	CPlayer() {};
	virtual ~CPlayer();


	/**
	Attach to the character with this EntityID. Player will then be in control of that character. The camera will
	switch to following the newly attached entity.

	\param	characterId	Identifier for the character.
	*/
	void AttachToCharacter(EntityId characterId);


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
	Gets IActor for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	IActor* GetAttachedActor() const;


	/**
	Gets CCharacter for the character / actor who is acting as our pawn.

	\return	The character identifier.
	*/
	CCharacter* GetAttachedCharacter() const;


	/**
	Gets EntityId for actor who is the focus of our camera.

	\return	The character identifier.
	*/
	EntityId GetCameraTargetId() const;


	/**
	Gets camera manager.

	\return null if it fails, else the camera manager.
	**/
	ICameraManagerComponent* GetCameraManager() const;


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
	ILINE static CPlayer* GetLocalPlayer()
	{
		return static_cast<CPlayer*>(g_pGame->GetIGameFramework()->GetClientActor());
	}


	/**
	Gets the character for the character / actor who is acting as our pawn. It is possible there is no local player
	or they are not presently attached to a character.

	Convenience function.

	\return	null if it fails, else the local character.
	*/
	static CCharacter* GetLocalCharacter();


	/**
	Convenience function to get the local actor, if there is one. We are casting to a CActor
	since they should be the only form of actor returned from this call in any case.

	\return	null if it fails, else the local actor.
	*/
	ILINE static CActor* GetLocalActor()
	{
		return reinterpret_cast<CActor*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
	}


	/**
	If we have a local player and they are attached to a character, this will retrieve the
	item that character is using.

	\return	null if it fails, else the local item.
	*/
	static CItem* GetLocalItem();


	/**
	If we have a local player and they are attached to a character, this will retrieve the
	weapon that character is using.

	\return	null if it fails, else the local weapon.
	*/
	static CWeapon* GetLocalWeapon();


private:

	/**
	Registers this instance for GameObject event notifications (will receive HandleEvent () calls).
	*/
	void RegisterForGOEvents();

	/** Specifies whether this instance is the client actor. */
	bool m_isClient { false };

	/** The camera that this instance uses. */
	ICameraManagerComponent* m_pCameraManager { nullptr };

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
};