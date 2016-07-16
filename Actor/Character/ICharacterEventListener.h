/**
\file	Actor\Character\ICharacterEventListener.h

Declares the ICharacterEventListener interface. This interface handles events that are raised from the
character movement code and provides the means for that code to notify the character of useful events.
*/
#pragma once

#include <CryAISystem/IAgent.h>
#include <CryEntitySystem/IComponent.h>

struct IActor;
struct IEntityClass;


struct ICharacterEventListener
{
	enum ESpecialMove
	{
		eSM_Jump = 0,
		eSM_SpeedSprint,
	};

	virtual ~ICharacterEventListener() {}

	virtual void OnDeath(IActor* pActor, bool bIsGod) {};
	virtual void OnEnterVehicle(IActor* pActor, const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson) {};
	virtual void OnExitVehicle(IActor* pActor) {};
	virtual void OnHealthChanged(IActor* pActor, float newHealth) {};
	virtual void OnItemDropped(IActor* pActor, EntityId itemId) {};
	virtual void OnItemPickedUp(IActor* pActor, EntityId itemId) {};
	virtual void OnItemUsed(IActor* pActor, EntityId itemId) {};
	virtual void OnObjectGrabbed(IActor* pActor, bool bIsGrab, EntityId objectId, bool bIsNPC, bool bIsTwoHanded) {};
	virtual void OnPickedUpPickableAmmo(IActor* pActor, IEntityClass* pAmmoType, int count) {}
	virtual void OnRevive(IActor* pActor, bool bIsGod) {};
	virtual void OnSpectatorModeChanged(IActor* pActor, uint8 mode) {};
	virtual void OnSpecialMove(IActor* pActor, ICharacterEventListener::ESpecialMove move) {};
	virtual void OnSprintStaminaChanged(IActor* pActor, float newStamina) {};
	virtual void OnStanceChanged(IActor* pActor, EStance stance) {};
	virtual void OnToggleThirdPerson(IActor* pActor, bool bThirdPerson) {};
};
