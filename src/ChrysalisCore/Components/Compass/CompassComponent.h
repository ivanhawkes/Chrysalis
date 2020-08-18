#pragma once

#include <Components/Interaction/EntityInteractionComponent.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>
#include <Components/Animation/ControlledAnimationComponent.h>


namespace Chrysalis
{
class CCompassComponent
	: public IEntityComponent
	, public IInteractionItem
{
protected:
	// IEntityComponent
	virtual void Initialize() final;
	// ~IEntityComponent

public:
	CCompassComponent() {}
	virtual ~CCompassComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CCompassComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{7B6E6249-4243-4E1F-A24E-8A1218CB6844}"_cry_guid;
		return id;
	}

	// IInteractionItem
	void OnInteractionItemInspect(IActor& actor) override { CryLogAlways("OnInteractionItemInspect fired."); };
	void OnInteractionItemPickup(IActor& actor) override { CryLogAlways("OnInteractionItemPickup fired."); };
	void OnInteractionItemDrop(IActor& actor) override { CryLogAlways("OnInteractionItemDrop fired."); };
	void OnInteractionItemToss(IActor& actor) override { CryLogAlways("OnInteractionItemToss fired."); };
	// IInteractionItem

	virtual void OnResetState() final;

private:
	/** Model for the geometry. */
	Cry::DefaultComponents::CStaticMeshComponent* m_pGeometryComponent {nullptr};

	///** Animation for the geometry. */
	//CControlledAnimationComponent* m_pControlledAnimationComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor {nullptr};
};
}