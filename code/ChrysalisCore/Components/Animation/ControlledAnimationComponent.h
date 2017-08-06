#pragma once

#include <DefaultComponents/Geometry/StaticMeshComponent.h>
#include <CrySchematyc/ResourceTypes.h>


namespace Chrysalis
{
/** A controlled animation component. */
class CControlledAnimationComponent
	: public IEntityComponent
	//, public Cry::DefaultComponents::CStaticMeshComponent::IGeometryListener
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	virtual void Initialize() final;
	virtual void ProcessEvent(SEntityEvent& event) final;
	virtual uint64 GetEventMask() const final { return BIT64(ENTITY_EVENT_HIDE) | BIT64(ENTITY_EVENT_UNHIDE); }
	// ~IEntityComponent

public:
	CControlledAnimationComponent() {}
	virtual ~CControlledAnimationComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CControlledAnimationComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{925EB7EA-734C-42DC-A55D-7023BC3BE053}"_cry_guid;
		return id;
	}

	// Cry::DefaultComponents::CStaticMeshComponent::IGeometryListener
	void OnGeometryResetState();
	// ~Cry::DefaultComponents::CStaticMeshComponent::IGeometryListener

	// CControlledAnimationComponent
	virtual void OnResetState() final;
	// CControlledAnimationComponent

	struct IControlledAnimationListener
	{
		virtual ~IControlledAnimationListener() {};

		virtual void OnControlledAnimationResetState() = 0;
	};

	void AddEventListener(IControlledAnimationListener* pListener)
	{
		assert(pListener);
		if (pListener)
			stl::push_back_unique(m_ListenersList, pListener);
	}

	void RemoveEventListener(IControlledAnimationListener* pListener)
	{
		assert(pListener);
		m_ListenersList.remove(pListener);
	}

	/** It's critical to know which slot this animation will play on. **/
	int GetSlotId() { return m_slotId; }
	void SetSlotId(int slotId) { m_slotId = slotId; }

protected:
	typedef std::list<IControlledAnimationListener*> TListenersList;
	TListenersList m_ListenersList;

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
};
}