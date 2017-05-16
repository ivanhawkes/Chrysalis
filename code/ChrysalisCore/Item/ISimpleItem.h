#pragma once

#include <IItemSystem.h>


// Helper struct to avoid having to implement all IGameObjectExtension functions every time
struct ISimpleItem : public IItem
{
	// IGameObjectExtension
	virtual bool Init(IGameObject* pGameObject) override { SetGameObject(pGameObject); return true; }
	virtual void PostInit(IGameObject* pGameObject) override {}
	virtual void HandleEvent(const SGameObjectEvent& event) override {}
	virtual void ProcessEvent(SEntityEvent& event) override {}
	virtual void InitClient(int channelId) override {}
	virtual void PostInitClient(int channelId) override {}
	virtual bool ReloadExtension(IGameObject* pGameObject, const SEntitySpawnParams& params) override { return true; }
	virtual void PostReloadExtension(IGameObject* pGameObject, const SEntitySpawnParams& params) override {}
	virtual void FullSerialize(TSerialize ser) override {}
	virtual bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) override { return true; }
	virtual void PostSerialize() override {}
	virtual void SerializeSpawnInfo(TSerialize ser) override {}
	virtual ISerializableInfoPtr GetSpawnInfo() override { return nullptr; }
	virtual void Update(SEntityUpdateContext& ctx, int updateSlot) override {}
	virtual void SetChannelId(uint16 id) override {}
	virtual void SetAuthority(bool auth) override {}
	virtual void PostUpdate(float frameTime) override {}
	virtual void PostRemoteSpawn() override {}
	virtual void GetMemoryUsage(ICrySizer* pSizer) const override {}
	virtual ComponentEventPriority GetEventPriority(const int eventID) const override { return EEntityEventPriority_GameObject; }
	// ~IGameObjectExtension


	// IItem
	const char* GetType() const override { return "CItem"; }

	// *** OWNERSHIP
	EntityId GetOwnerId() const override { return INVALID_ENTITYID; }
	void SetOwnerId(EntityId ownerId) override {};

	void EnableUpdate(bool enable, int slot = -1) override {};
	void RequireUpdate(int slot) override {};


	// *** ACTIONS
	void ForcePendingActions(uint8 blockedActions = 0) override {};

	// *** EVENTS
	void OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value) override {};
	void OnParentSelect(bool select) override {};
	void OnAttach(bool attach) override {};
	void OnPickedUp(EntityId actorId, bool destroyed) override {};
	void OnHit(float damage, int hitType) override {};

	// *** HOLDING
	void DrawSlot(int slot, bool render, bool nearest = false)
	{
		uint32 flags = GetEntity()->GetSlotFlags(slot);

		if (render)
			flags |= ENTITY_SLOT_RENDER;
		else
			flags &= ~ENTITY_SLOT_RENDER;

		if (nearest)
			flags |= ENTITY_SLOT_RENDER_NEAREST;
		else
			flags &= ~ENTITY_SLOT_RENDER_NEAREST;

		GetEntity()->SetSlotFlags(slot, flags);
	}

	void EnablePicking(bool enable, bool dropped) {};
	void PickUp(EntityId actorId, bool playSound, bool select = true, bool keepHistory = true, const char *setup = NULL) override {};

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
	void RemoveAllAccessories() override {};
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
	void Reset() override {};
	bool ResetParams() override { return true; }
	void PreResetParams() override {};
	bool GivesAmmo() override { return false; }
	const char *GetDisplayName() const override { return "NOT IMPLEMENTED"; }
	void HideItem(bool hide) override {};

	void SetSubContextID(int tagContext) override {}
	int GetSubContextID() override { return 0; }

	// *** INTERFACES
	IWeapon *GetIWeapon() override { return nullptr; }
	const IWeapon *GetIWeapon() const override { return nullptr; }

	bool IsAccessory() override { return false; }
	void SerializeLTL(TSerialize ser) override {};
	Vec3 GetMountedDir() const override { return FORWARD_DIRECTION; }
	// ~IItem

	ISimpleItem() {}
	virtual ~ISimpleItem() {}
};