#pragma once

#include <Entities/EntityEffects.h>
#include <Components/Actor/ActorComponent.h>
#include <Interfaces/IItem.h>


namespace Chrysalis
{
class CSnaplockComponent;


class CItemComponent
	: public IEntityComponent
{
protected:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Update | EEntityEvent::PrePhysicsUpdate; }
	// ~IEntityComponent

public:
	CItemComponent() {}
	virtual ~CItemComponent() { 
		// TEST: writing out classes test data.
		//Serialization::SaveJsonFile("chrysalis/parameters/items/item_classes.json", m_itemClassCollection);
	}

	static void ReflectType(Schematyc::CTypeDesc<CItemComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{ADF96E1C-FDED-403B-A625-1D9FCCFAD68D}"_cry_guid;
		return id;
	}

	/** This class tracks owner information for the item. It used to return a CActorWeakPtr but I have switched
	it to use the more generic IActorWeakPtr until I find an actual requirement for the CActorWeakPtr instead. */
	class COwnerInfo
	{
	public:
		COwnerInfo() : m_id(INVALID_ENTITYID) {}

		ILINE EntityId GetId() const { return m_id; }
		ILINE const IActorWeakPtr& GetActorWeakPtr() const { return m_pIActor; }

		void Set(EntityId id, const IActorWeakPtr& pIActor)
		{
			m_id = id;
			m_pIActor = pIActor;
		}

		void Reset()
		{
			m_id = 0;
			m_pIActor.reset();
		}

	private:
		EntityId m_id;
		IActorWeakPtr m_pIActor;
	};

	/** Resets the item to an initial state. */
	virtual void OnResetState();

	// ***
	// *** HOLDING
	// ***

	/**
	Enables / disables this entity for being picked up.

	\param	enable  true to enable, false to disable.
	\param	dropped true if dropped.
	**/
	void EnablePicking(bool enable, bool dropped);


	/**
	Tries to pick up the entity.

	\param	actorId	    The actor who is trying to pick up the entity.
	\param	playSound   true to play a sound.
	\param	select	    unknown.
	\param	keepHistory true to keep history.
	\param	setup	    The setup.
	**/
	virtual void PickUp(EntityId actorId, bool playSound, bool select = true, bool keepHistory = true, const char *setup = nullptr);


	// ***
	// *** Accessories: Implementation code for these declarations can be found in the ItemAccessory.cpp file.
	// ***

public:
	virtual void SetParentId(EntityId parentId) { m_parentId = parentId; }
	virtual EntityId GetParentId() const { return m_parentId; }

	//CItemComponent* AddAccessory(IEntityClass* pClass);
	//bool HasAccessory(IEntityClass* pClass);
	//CItemComponent* GetAccessory(IEntityClass* pClass);
	//void RemoveAccessory(IEntityClass* pClass);
	//void RemoveAllAccessories();

	//virtual void AttachAccessory(IEntityClass* pClass, bool attach, bool noanim, bool force = false, bool firstTimeAttached = false, bool initialLoadoutSetup = false);
	//void ReAttachAccessory(IEntityClass* pClass);

private:
	/** The maximum number of accessories which an item can have attached onto it. */
	static const int MaxNumberOfAccessories = 4;

	/** Information about the accessory. */
	struct SAccessoryInfo
	{
		SAccessoryInfo() : pClass(nullptr), accessoryId(INVALID_ENTITYID) {};
		SAccessoryInfo(IEntityClass* _pClass, EntityId _id) : pClass(_pClass), accessoryId(_id) {};

		IEntityClass* pClass;
		EntityId accessoryId;
	};

	/** Defines an alias representing array of accessories. */
	typedef CryFixedArray<SAccessoryInfo, MaxNumberOfAccessories> TAccessoryArray;

	TAccessoryArray m_accessories;

	/** Identifier for the parent entity. */
	EntityId m_parentId = INVALID_ENTITYID;


	// ***
	// *** Item effects.
	// ***

public:
	ILINE CEffectsController& GetEffectsController() { return m_effectsController; }

	///**
	// Attach a light to this entity.

	// \param	targetSlot   	Target slot.
	// \param	helperName   	Name of the helper.
	// \param	offset		 	The offset.
	// \param	direction	 	The direction.
	// \param	firstSafeSlot	The first safe slot.
	// \param	attachParams 	Options for controlling the attach.

	// \return	An TAttachedEffectId.
	// */

	//TAttachedEffectId AttachLight(const int targetSlot, const char* helperName, Vec3 offset, Vec3 direction, eGeometrySlot firstSafeSlot,
	//	const ECS::RenderLight& renderLight, const ECS::ProjectorLight& projectorLight)
	//{
	//	return m_effectsController.AttachLight(targetSlot, helperName, offset, direction, firstSafeSlot, renderLight, projectorLight);
	//}


	//void DetachEffect(const TAttachedEffectId effectId)
	//{
	//	m_effectsController.DetachEffect(effectId);
	//}

private:
	CEffectsController m_effectsController;


	// ***
	// *** CItemComponent
	// ***

public:
	enum eItemHand
	{
		eIH_Right = 0, // indicates the right hand of the actor
		eIH_Left,      // indicates the left hand of the actor
		eIH_Last,
	};


	enum ETimer
	{
		eIT_Flying = 0,
		eIT_Last,
	};


	enum eItemAttachment
	{
		eIA_None,
		eIA_WeaponEntity,
		eIA_WeaponCharacter,
		eIA_StowPrimary,
		eIA_StowSecondary,
	};


	struct SItemStatus
	{
		SItemStatus()
		{
		};

		void Serialize(TSerialize &ser)
		{
			bool fly = flying;
			bool sel = selected;
			bool mount = mounted;
			bool use = used;
			int han = hand;
			bool drop = dropped;
			bool detach = detached;
			bool bnew = brandnew;
			float fHealth = health;
			bool bfirstSelection = first_selection;
			int attach = (int)attachment;
			bool pick = pickable;

			ser.BeginGroup("ItemStats");
			ser.Value("flying", fly);
			ser.Value("selected", sel);
			ser.Value("mounted", mount);
			ser.Value("used", use);
			ser.Value("hand", han);
			ser.Value("dropped", drop);
			ser.Value("detached", detach);
			ser.Value("brandnew", bnew);
			ser.Value("health", fHealth);
			ser.Value("first_selection", bfirstSelection);
			ser.Value("attachment", attach);
			ser.Value("pickable", pick);
			ser.Value("mount_dir", mount_dir);
			ser.Value("mount_last_aimdir", mount_last_aimdir);
			ser.EndGroup();

			if (ser.IsReading())
			{
				flying = fly;
				selected = sel;
				mounted = mount;
				used = use;
				hand = han;
				dropped = drop;
				detached = detach;
				brandnew = bnew;
				health = fHealth;
				first_selection = bfirstSelection;
				attachment = (eItemAttachment)attach;
				pickable = pick;
			}
		}

		Vec3 mount_dir {0.0f, 1.0f, 0.0f};
		Vec3 mount_last_aimdir {0.0f, 0.0f, 0.0f};

		eItemAttachment attachment {eIA_None};
		eGeometrySlot physicalisedSlot {eIGS_Last};

		float health {0.0f};
		int hand {eIH_Right};
		int viewmode {0};
		bool fp {false};
		bool mounted {false};
		bool pickable {true};
		bool selected {false};
		bool dropped {false};
		bool detached {false};
		bool brandnew {true};
		bool flying {false};
		bool used {false};
		bool sound_enabled {true};
		bool first_selection {true};
	};

	SItemStatus m_itemStatus;

private:
	// Who currently owns this item, if anyone?
	COwnerInfo m_owner;

	IItem m_item;

	IItemClassCollection m_itemClassCollection;

	/**
	Determine if we should bind on initialise. This allows derived classes a chance to not bind on init by overriding
	this. Only seen used on vehicle mounted weapon in GameSDK.

	\return	true if it succeeds, false if it fails.
	*/
	virtual bool ShouldBindOnInit() const { return true; }

	/** A component that allows for management of snaplocks. */
	CSnaplockComponent* m_pSnaplockComponent {nullptr};
};
}