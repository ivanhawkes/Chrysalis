#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include <Entities/Interaction/IEntityInteraction.h>

class CEntityInteractionComponent;


/**
Supplies interaction ability to game world items.

\sa CDesignerEntityComponent
\sa IEntityPropertyGroup
\sa IInteractionItem
**/
class CItemInteractionComponent : public CDesignerEntityComponent<>, public IEntityPropertyGroup, public IInteractionItem
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CItemInteractionComponent, "ItemInteractionComponent", 0xd7c88abd16d44f0f, 0xbf9c328ae7623a89)

public:
	// IEntityComponent
	void Initialize() override;
	uint64 GetEventMask() const override { return CDesignerEntityComponent::GetEventMask() | BIT64(ENTITY_EVENT_UPDATE); }
	void ProcessEvent(SEntityEvent& event) override;
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Item Interaction Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// IInteractionItem
	void OnInteractionItemInspect() override;;
	void OnInteractionItemPickup() override;;
	void OnInteractionItemDrop() override;;
	void OnInteractionItemToss() override;;
	// ~IInteractionItem


	// ***
	// *** CItemInteractionComponent
	// ***

	CItemInteractionComponent() {};
	virtual ~CItemInteractionComponent() {};

private:
	/** An instance of an interaction component. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** Speed at which object 'jump' towards player when being inspected (m/sec). */
	const float kJumpToPlayerSpeed = 4.0f;
	
	/** Factor the speed at which inspected items are rotated, in comparison to player character rotation. */
	const float kInspectionRotationFactor = 5.0f;

	/** Factor the velocity at which items are tossed from inventory. */
	const float kTossFactor = 0.01f;

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

	void OnResetState() override;
	void Update();

	void OnPickingUpUpdate(const float frameTime);

	void OnInspectingUpdate(const float frameTime);
};