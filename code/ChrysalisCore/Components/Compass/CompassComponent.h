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
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

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
	void OnInteractionItemInspect(IActorComponent& actor) override { CryLogAlways("OnInteractionItemInspect fired."); };
	void OnInteractionItemPickup(IActorComponent& actor) override { CryLogAlways("OnInteractionItemPickup fired."); };
	void OnInteractionItemDrop(IActorComponent& actor) override { CryLogAlways("OnInteractionItemDrop fired."); };
	void OnInteractionItemToss(IActorComponent& actor) override { CryLogAlways("OnInteractionItemToss fired."); };
	// IInteractionItem

	virtual void OnResetState() final;

private:
	/** Model for the geometry. */
	Cry::DefaultComponents::CStaticMeshComponent* m_pGeometryComponent { nullptr };

	///** Animation for the geometry. */
	//CControlledAnimationComponent* m_pControlledAnimationComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };
};
}