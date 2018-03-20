#pragma once

#include <Entities/Interaction/IEntityInteraction.h>

namespace Chrysalis
{
class CEntityInteractionComponent;


/**
Supplies interaction ability to game world items.

**/

class CItemInteractionComponent
	: public IEntityComponent
	, public IInteractionItem
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE); }
	void ProcessEvent(SEntityEvent& event) override;
	// ~IEntityComponent

public:
	CItemInteractionComponent() {}
	virtual ~CItemInteractionComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CItemInteractionComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{B79309D8-50E2-4351-866C-2AA29AC9ABC5}"_cry_guid;
		return id;
	}

	// IInteractionItem
	void OnInteractionItemInspect(IActorComponent& actor) override;;
	void OnInteractionItemPickup(IActorComponent& actor) override;;
	void OnInteractionItemDrop(IActorComponent& actor) override;;
	void OnInteractionItemToss(IActorComponent& actor) override;;
	// ~IInteractionItem


protected:
	/** An instance of an interaction component. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** Speed at which object 'jump' towards player when being inspected (m/sec). */
	const float kJumpToPlayerSpeed = 4.0f;

	/** Factor the speed at which inspected items are rotated, in comparison to player character rotation. */
	const float kInspectionRotationFactor = 5.0f;

	/** The number of newtons of force applied when items are tossed from inventory. */
	const float kTossNewtons = 10.0f;

	enum class InspectionState
	{
		eNone,
		eInspecting,
		ePickingUp,
		eDroping,
		eTossing,
		eCancelled,
	};

	InspectionState m_inspectionState { InspectionState::eNone };
	float m_timeInAirRequired;
	float m_timeInAir;
	Vec3 m_initialPosition;
	Vec3 m_targetPosition;
	Quat m_initialRotation;

	virtual void OnResetState();
	void Update();

	void OnPickingUpUpdate(const float frameTime);

	void OnInspectingUpdate(const float frameTime);
};
}