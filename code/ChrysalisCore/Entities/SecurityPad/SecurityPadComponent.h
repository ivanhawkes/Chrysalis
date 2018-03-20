#pragma once

#include <Components/Interaction/EntityInteractionComponent.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>


namespace Chrysalis
{
class CSecurityPadComponent
	: public IEntityComponent
	, public IInteractionExamine
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void Initialize() final;
	// ~IEntityComponent

public:
	CSecurityPadComponent() {}
	virtual ~CSecurityPadComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CSecurityPadComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{AE870662-9826-4945-AEC8-3CEA54C2159B}"_cry_guid;
		return id;
	}

	// ISecurityPadEntityComponent
	virtual void OnResetState() final;
	// ISecurityPadEntityComponent

	// IInteractionExamine
	void OnInteractionExamineStart(IActorComponent& actor) override;;
	void OnInteractionExamineComplete(IActorComponent& actor) override;;
	// IInteractionExamine

private:
	/** Model for the geometry. */
	Cry::DefaultComponents::CStaticMeshComponent* m_pGeometryComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };
};
}