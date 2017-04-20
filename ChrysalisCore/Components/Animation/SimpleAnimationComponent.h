#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include <Components/Geometry/GeometryComponent.h>


class CSimpleAnimationComponent final : public CDesignerEntityComponent<>, public IEntityPropertyGroup, public CGeometryComponent::IGeometryListener
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CSimpleAnimationComponent, "SimpleAnimationComponent", 0xB947F6CEBBE24DC5, 0x8E15F1E4F82161FB)
	virtual ~CSimpleAnimationComponent() {}

public:
	// IEntityComponent
	virtual void Initialize() final;
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "Animation Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// CGeometryComponent::IGeometryListener
	void OnGeometryResetState() override;
	// ~CGeometryComponent::IGeometryListener

	// CSimpleAnimationComponent
	void OnResetState() override;
	void OnPlayAnimation(string overrideAnimation = "");
	// CSimpleAnimationComponent

	struct ISimpleAnimationListener
	{
		virtual ~ISimpleAnimationListener() {};

		virtual void OnSimpleAnimationResetState() = 0;
	};

	void AddEventListener(ISimpleAnimationListener* pListener)
	{
		assert(pListener);
		if (pListener)
			stl::push_back_unique(m_ListenersList, pListener);
	}

	void RemoveEventListener(ISimpleAnimationListener* pListener)
	{
		assert(pListener);
		m_ListenersList.remove(pListener);
	}

	/** It's critical to know which slot this animation will play on. **/
	int GetSlotId() { return m_slotId; }
	void SetSlotId(int slotId) { m_slotId = slotId; }

private:
	typedef std::list<ISimpleAnimationListener*> TListenersList;
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

	/** Should the animation play when the level starts? **/
	bool m_bPlayOnLevelStart { true };
};
