#pragma once

#include "Helpers/DesignerEntityComponent.h"


class CGeometryComponent final : public CDesignerEntityComponent<IGeometryEntityComponent>, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_CLASS(CGeometryComponent, IGeometryEntityComponent, "GeometryComponent", 0xB430544945AC4302, 0x99FA7A0C2203DE09);

	virtual ~CGeometryComponent() {}

public:
	enum EPhysicalizationType
	{
		ePhysicalizationType_None = 0,
		ePhysicalizationType_Static,
		ePhysicalizationType_Rigid
	};

	struct IGeometryListener
	{
		virtual ~IGeometryListener() {};

		virtual void OnGeometryResetState() = 0;
	};

	void AddEventListener(IGeometryListener* pListener)
	{
		assert(pListener);
		if (pListener)
			stl::push_back_unique(m_ListenersList, pListener);
	}

	void RemoveEventListener(IGeometryListener* pListener)
	{
		assert(pListener);
		m_ListenersList.remove(pListener);
	}

public:
	// IEntityComponent
	virtual void Initialize() final;
	virtual void ProcessEvent(SEntityEvent& event) final;
	virtual uint64 GetEventMask() const final { return CDesignerEntityComponent::GetEventMask() | BIT64(ENTITY_EVENT_COLLISION) | BIT64(ENTITY_EVENT_HIDE) | BIT64(ENTITY_EVENT_UNHIDE); }
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "Geometry Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// IGeometryEntityComponent
	virtual void OnResetState() final;
	virtual void SetGeometry(const char* szFilePath) override;
	// IGeometryEntityComponent

public:
	// Helper for loading geometry or characters.
	int LoadMesh(const char *path, int slot = -1)
	{
		if (IsCharacterFile(path))
		{
			return GetEntity()->LoadCharacter(slot, path);
		}
		else
		{
			return GetEntity()->LoadGeometry(slot, path);
		}
	}

	/** Gets the slot the geometry is loaded into or -1 if there is no loaded geometry. **/
	int GetSlotId() { return m_slotId; }

private:
	typedef std::list<IGeometryListener*> TListenersList;
	TListenersList m_ListenersList;

	/** The model. */
	string m_model;

	/** The entity slot to load the model into.	**/
	int m_slotId { -1 };

	/**	The mass. **/
	float m_mass { 1.0f };

	/**	Physicalisation type. **/
	EPhysicalizationType m_physicalizationType { ePhysicalizationType_Static };

	/**	True to receive collision events. **/
	bool m_bReceiveCollisionEvents { false };
};
