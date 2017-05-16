#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include <Components/Geometry/GeometryComponent.h>


class CControlledAnimationComponent final : public CDesignerEntityComponent<>, public IEntityPropertyGroup,
	public CGeometryComponent::IGeometryListener
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CControlledAnimationComponent, "ControlledAnimationComponent", 0x4940688ACE964790, 0x889D5691727340B0)
	virtual ~CControlledAnimationComponent() {}

public:
	// IEntityComponent
	virtual void Initialize() final;
	virtual void ProcessEvent(SEntityEvent& event) final;
	virtual uint64 GetEventMask() const final { return CDesignerEntityComponent::GetEventMask() | BIT64(ENTITY_EVENT_HIDE) | BIT64(ENTITY_EVENT_UNHIDE); }
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "Animation Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// CGeometryComponent::IGeometryListener
	void OnGeometryResetState() override;
	// ~CGeometryComponent::IGeometryListener

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

private:
	typedef std::list<IControlledAnimationListener*> TListenersList;
	TListenersList m_ListenersList;

	/** Model for the geometry. */
	CGeometryComponent* m_pGeometryComponent { nullptr };

	/** The entity slot to load the model into.	**/
	int m_slotId { 1 };

	/**	Animation file to play. **/
	string m_animation;

	/**	Speed of the animation. **/
	float m_animationSpeed { 1.0f };

	/**	True to loop the animation. **/
	bool m_bLoopAnimation { true };
};
