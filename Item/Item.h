#pragma once

#include <CryCore/BoostHelpers.h>
#include <IActorSystem.h>
#include <IItemSystem.h>
#include <Item/Parameters/ItemBaseParameter.h>
#include <Entity/EntityEffects.h>


DECLARE_SHARED_POINTERS(IActor);


class CItem :public CGameObjectExtensionHelper<CItem, IItem>, public IGameObjectProfileManager
{
public:

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


	// ***
	// *** IGameObjectExtension
	// ***

public:
	void GetMemoryUsage(ICrySizer *pSizer) const override;
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
	void InitClient(int channelId) override;
	void PostInitClient(int channelId) override;
	bool ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	void PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	bool GetEntityPoolSignature(TSerialize signature) override;
	void Release() override;
	void FullSerialize(TSerialize ser) override;
	bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) override;
	void PostSerialize() override {};
	void SerializeSpawnInfo(TSerialize ser) override {};
	ISerializableInfoPtr GetSpawnInfo() override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void HandleEvent(const SGameObjectEvent& event) override;
	void ProcessEvent(SEntityEvent& event) override;
	void SetChannelId(uint16 id) override;
	void SetAuthority(bool auth) override;
	const void* GetRMIBase() const override;
	void PostUpdate(float frameTime) override;
	void PostRemoteSpawn() override;

	// ***
	// *** IGameObjectProfileManager
	// ***

	uint8 GetDefaultProfile(EEntityAspects aspect) override;
	bool SetAspectProfile(EEntityAspects aspect, uint8 profile) override;


	// ***
	// *** IItem
	// ***

	/**
	Returns a string based identifier for this type of item.

	\return	This should never return a NULL or empty string.
	*/
	const char* GetType() const override { return "CItem"; }


	// *** OWNERSHIP

	/**
	Retrieves the owner identifier.

	\return	The owner identifier.
	*/
	EntityId GetOwnerId() const override { return INVALID_ENTITYID; }


	/**
	Sets the owner identifier.

	\param	ownerId	The entity that owns this item.
	*/
	void SetOwnerId(EntityId ownerId) override {};


	// *** UPDATES

	/**
	Enables updates.

	\param	enable	true to enable, false to disable.
	\param	slot  	The slot.
	*/
	void EnableUpdate(bool enable, int slot = -1) override {};


	/**
	Require update.

	\param	slot	The slot.
	*/
	void RequireUpdate(int slot) override {};


	// *** ACTIONS

	/**
	Force pending actions.

	\param	blockedActions	The blocked actions.
	*/
	void ForcePendingActions(uint8 blockedActions = 0) override {};


	// *** EVENTS

	/**
	Executes the action action.

	\param	actorId		  	Identifier for the actor.
	\param	actionId	  	Identifier for the action.
	\param	activationMode	The activation mode.
	\param	value		  	The value.
	*/
	void OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value) override {};


	/**
	Executes the parent select action.

	\param	select	true to select, false to deselect.
	*/
	void OnParentSelect(bool select) override {};


	/**
	Executes the attach action.

	\param	attach	true to attach.
	*/
	void OnAttach(bool attach) override {};


	/**
	Executes the picked up action.

	\param	actorId  	Identifier for the actor.
	\param	destroyed	true if destroyed.
	*/
	void OnPickedUp(EntityId actorId, bool destroyed) override {};


	/**
	Executes the hit action.

	\param	damage 	The damage.
	\param	hitType	Type of the hit.
	*/
	void OnHit(float damage, int hitType) override {};


	// *** HOLDING


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
			int attach = (int) attachment;
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
				attachment = (eItemAttachment) attach;
				pickable = pick;
			}
		}

		Vec3 mount_dir { 0.0f, 1.0f, 0.0f };
		Vec3 mount_last_aimdir { 0.0f, 0.0f, 0.0f };

		eItemAttachment attachment { eIA_None };
		eGeometrySlot physicalisedSlot { eIGS_Last };

		float health { 0.0f };
		int hand { eIH_Right };
		int viewmode { 0 };
		bool fp { false };
		bool mounted { false };
		bool pickable { true };
		bool selected { false };
		bool dropped { false };
		bool detached { false };
		bool brandnew { true };
		bool flying { false };
		bool used { false };
		bool sound_enabled { true };
		bool first_selection { true };
	};

	SItemStatus m_itemStatus;


	/**
	Set the slot drawing states.
	
	\param	slot    The slot.
	\param	render  true to render the slot.
	\param	nearest true for who knows what reason.
	**/
	void DrawSlot(int slot, bool render, bool nearest = false);


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
	void PickUp(EntityId actorId, bool playSound, bool select = true, bool keepHistory = true, const char *setup = NULL) override;

	void Select(bool select) override {};


	bool IsSelected() const override { return false; };
	bool CanSelect() const override { return false; };
	bool CanDeselect() const override { return false; };
	void RemoveOwnerAttachedAccessories() override {};

	void Physicalize(bool enable, bool rigid) override {};
	bool CanDrop() const override { return false; };
	void Drop(float impulseScale = 1.0f, bool selectNext = true, bool byDeath = false) override {};
	void UpdateFPView(float frameTime) override {};
	Vec3 GetMountedAngleLimits() const override { return Vec3(0.0f, 0.0f, 0.0f); }
	void MountAtEntity(EntityId entityId, const Vec3 &pos, const Ang3 &angles) override {};
	bool FilterView(struct SViewParams &viewParams) override { return false; };
	void RemoveAllAccessories() override;
	void DetachAllAccessories() override {};
	void AttachAccessory(IEntityClass* pClass, bool attach, bool noanim, bool force = false, bool firstTimeAttached = false, bool initialLoadoutSetup = false) override {};
	void SetCurrentActionController(class IActionController* pActionController) override {};
	void UpdateCurrentActionController() override {};
	const string GetAttachedAccessoriesString(const char* separator = ",") override { return "SOME STRING"; }

	void SetHand(int hand) override {};

	void StartUse(EntityId userId) override {};
	void StopUse(EntityId userId) override {};
	void SetBusy(bool busy) override {};
	bool IsBusy() const override { return false; }
	bool CanUse(EntityId userId) const override { return false; }
	bool IsUsed() const override { return false; };
	void Use(EntityId userId) override {};

	bool AttachToHand(bool attach, bool checkAttachment = false) override { return false; };
	bool AttachToBack(bool attach) override { return false; }


	// *** SETTINGS

	bool IsModifying() const override { return false; }
	bool CheckAmmoRestrictions(IInventory * pInventory) override { return false; }
	void Reset() override;
	bool ResetParams() override;
	void PreResetParams() override;
	bool GivesAmmo() override { return false; }
	const char *GetDisplayName() const override { return "NOT IMPLEMENTED"; }
	void HideItem(bool hide) override {};

	void SetSubContextID(int tagContext) override {}
	int GetSubContextID() override { return 0; }


	// *** INTERFACES

	IWeapon *GetIWeapon() override { return nullptr; }
	const IWeapon *GetIWeapon() const override { return nullptr; }


	// ***
	// *** MISC
	// ***

	/**
	Query if this object is an accessory.

	\return	true if an accessory, false if not.
	*/
	bool IsAccessory() override { return false; }


	/**
	Used to serialize item attachment when loading next level.

	\param	ser	The serializer.
	*/
	void SerializeLTL(TSerialize ser) override {};


	/**
	Gets the original (not current) direction vector of a mounted weapon.

	\return	The direction.
	*/
	Vec3 GetMountedDir() const override { return Vec3(0.0f, 0.0f, 0.0f); }


	// ***
	// *** Accessories: Implementation code for these declarations can be found in the ItemAccessory.cpp file.
	// ***

public:
	virtual void SetParentId(EntityId parentId);
	virtual EntityId GetParentId() const;

	CItem* AddAccessory(IEntityClass* pClass);
	bool HasAccessory(IEntityClass* pClass);
	CItem* GetAccessory(IEntityClass* pClass);
	void RemoveAccessory(IEntityClass* pClass);

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
	ILINE EntityEffects::CEffectsController& GetEffectsController() { return m_effectsController; }


	/**
	 Attach a light to this entity.
	
	 \param	targetSlot   	Target slot.
	 \param	helperName   	Name of the helper.
	 \param	offset		 	The offset.
	 \param	direction	 	The direction.
	 \param	firstSafeSlot	The first safe slot.
	 \param	attachParams 	Options for controlling the attach.
	
	 \return	An EntityEffects::TAttachedEffectId.
	 */

	EntityEffects::TAttachedEffectId AttachLight(const int targetSlot, const char* helperName, Vec3 offset, Vec3 direction, eGeometrySlot firstSafeSlot,
		const SDynamicLightConstPtr attachParams);

	void DetachEffect(const EntityEffects::TAttachedEffectId effectId);

private:
	EntityEffects::CEffectsController m_effectsController;


	// ***
	// *** CItem
	// ***


public:
	CItem();
	~CItem();

	void GetSharedParameters(XmlNodeRef rootParams);


private:
	// Who currently owns this entity, if anyone?
	COwnerInfo m_owner;

	/**
	Determine if we should bind on initialise. This allows derived classes a chance to not bind on init by overriding
	this. Only seen used on vehicle mounted weapon in GameSDK.

	\return	true if it succeeds, false if it fails.
	*/
	virtual bool ShouldBindOnInit() const { return true; }


	static IGameFramework *m_pGameFramework;
	static IEntitySystem *m_pEntitySystem;
	static IItemSystem *m_pItemSystem;

	SItemBaseParameterConstPtr m_itemBaseParameter;
};
