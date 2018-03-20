#pragma once

#include <DefaultComponents/Geometry/StaticMeshComponent.h>
#include <CrySchematyc/ResourceTypes.h>


namespace Chrysalis
{
/** A simple animation component. */
class CSimpleAnimationComponent
	: public IEntityComponent
	//, public Cry::DefaultComponents::CStaticMeshComponent::IGeometryListener

{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void Initialize() final;
	// ~IEntityComponent

public:
	CSimpleAnimationComponent() {}
	virtual ~CSimpleAnimationComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CSimpleAnimationComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{7DB3FF61-17CC-4FBD-B571-2ECA41EBABC9}"_cry_guid;
		return id;
	}

	// Cry::DefaultComponents::CStaticMeshComponent::IGeometryListener
	void OnGeometryResetState();
	// ~Cry::DefaultComponents::CStaticMeshComponent::IGeometryListener

	// CSimpleAnimationComponent
	virtual void OnResetState();
	void OnPlayAnimation(Schematyc::LowLevelAnimationName overrideAnimation = "");
	// CSimpleAnimationComponent

	/** It's critical to know which slot this animation will play on. **/
	int GetSlotId() { return m_slotId; }
	void SetSlotId(int slotId) { m_slotId = slotId; }

protected:
	/** Model for the geometry. */
	Cry::DefaultComponents::CStaticMeshComponent* m_pGeometryComponent { nullptr };

	/** The entity slot to load the model into.	**/
	int m_slotId { 1 };

	/**	Animation file to play. **/
	Schematyc::LowLevelAnimationName m_animation;

	/**	Speed of the animation. **/
	float m_animationSpeed { 1.0f };

	/**	True to loop the animation. **/
	bool m_bLoopAnimation { true };

	/** Should the animation play when the level starts? **/
	bool m_bPlayOnLevelStart { true };
};
}