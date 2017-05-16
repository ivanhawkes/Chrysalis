/**
\file	Actor\Inventory\CEntityInventory.h

Class will perform instantiation and management of inventory systems for all entities.

*/
#pragma once

#include "IGameObject.h"
#include "IItemSystem.h"


/**
An entity inventory.

\sa CGameObjectExtensionHelper<CEntityInventory, IInventory>
**/
class CEntityInventory : public CGameObjectExtensionHelper <CEntityInventory, IInventory>
{
	// ***
	// *** IGameObjectExtension
	// ***

	void GetMemoryUsage(ICrySizer *pSizer) const override;
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
	void InitClient(int channelId) override {};
	void PostInitClient(int channelId) override {};
	void FullSerialize(TSerialize ser) override {};
	bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) override { return true; };
	void PostSerialize() override {};
	void SerializeSpawnInfo(TSerialize ser) override {};
	ISerializableInfoPtr GetSpawnInfo() override { return nullptr; };
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void HandleEvent(const SGameObjectEvent& event) override {};
	void ProcessEvent(SEntityEvent& event) override {};
	void SetChannelId(uint16 id) override {};
	void SetAuthority(bool auth) override {};
	void PostRemoteSpawn() override {};


	// ***
	// *** IInventory
	// ***


	/**
	Adds an item.

	\param	itemId	Identifier for the item.

	\return	true if it succeeds, false if it fails.
	*/
	virtual bool AddItem(EntityId itemId);


	/**
	Removes the item described by itemId.

	\param	itemId	Identifier for the item.

	\return	true if it succeeds, false if it fails.
	*/
	virtual bool RemoveItem(EntityId itemId);


	/** Removes all items. */
	virtual void RemoveAllItems(bool forceClear = false);


	/**
	Searches for the first item.
	
	\param	itemId	Identifier for the item.
	
	\return	The found item.
	*/
	// #TODO: Warning! You are shadowing the exiting function in IInventory. Is this expected behaviour?
	// Are you planning to chain the call downwards?
	virtual int FindItem(EntityId itemId) const;


	/** Destroys this object. */
	virtual void Destroy();


	/** Clears this object to its blank/initial state. */
	virtual void Clear(bool forceClear = false);


	// ***
	// *** CEntityInventory
	// ***

public:

	/** Values that represent entity equipment slots. */
	enum EntityEquipmentSlots
	{
		eEquipmentSlot_helmet = 0,
		eEquipmentSlot_chest,
		eEquipmentSlot_hands,
		eEquipmentSlot_legs,
		eEquipmentSLot_feet,
		eEquipmentSlot_belt,
		eEquipmentSlot_leftRing,
		eEquipmentSlot_rightRing,
		eEquipmentSlot_amulate,
	};

	/** Values that represent entity quick slots. */
	enum EntityQuickSlots
	{
		eQuickSlot_0 = 0,
		eQuickSLot_1,
		eQuickSLot_2,
		eQuickSLot_3,
		eQuickSlot_4,
		eQuickSlot_5,
		eQuickSlot_6,
		eQuickSlot_7,
	};

	/** Values that represent entity bag expansion slots. */
	enum EntityBagExpansionSlots
	{
		eBagExpansionSlot_0 = 0,
		eBagExpansionSlot_1,
		eBagExpansionSlot_2,
		eBagExpansionSlot_3,
		eBagExpansionSlot_4,
	};

	/** Information about the actor backpack. */
	struct ActorBackpackInfo
	{
	public:
		// default backpack init constructor
		ActorBackpackInfo() {}

		// Passing in existing data about the actors backpack used for loading from save.
		ActorBackpackInfo(int capacity,
			int currentItemCount,
			int expansionCounter,
			bool bCanBeExpanded)
			: m_capacity(capacity)
			, m_currentItemCount(currentItemCount)
			, m_expansionCounter(expansionCounter)
			, m_maxAllowedExpansions(5)
			, m_bCanBeExpanded(bCanBeExpanded)
		{}

		// getters
		ILINE void SetCurrentItemCount(int _currentCount) { m_currentItemCount = _currentCount; }
		ILINE void SetMaxInventoryCapacity(int _maxCpacity) { m_capacity = _maxCpacity; }
		ILINE void SetExpansionCounter(int _expansionCounter);
		ILINE void SetBackpackExpandableState(bool _canBeExpanded) { m_bCanBeExpanded = _canBeExpanded; }
		ILINE void SetExpansion1Size(int expansion0Size) { m_expansion0Size = expansion0Size; }
		ILINE void SetExpansion2Size(int expansion1Size) { m_expansion1Size = expansion1Size; }
		ILINE void SetExpansion3Size(int expansion2Size) { m_expansion2Size = expansion2Size; }
		ILINE void SetExpansion4Size(int expansion3Size) { m_expansion3Size = expansion3Size; }
		ILINE void SetExpansion5Size(int expansion4Size) { m_expansion4Size = expansion4Size; }

		// setters
		ILINE int GetCurrentItemCount() const { return m_currentItemCount; }
		ILINE int GetMaxInventoryCapacity() const { return m_capacity; }
		ILINE int GetExpansionCounter() const { return m_expansionCounter; }
		ILINE int GetMaxAllowedExpansions() const { return m_maxAllowedExpansions; }
		ILINE int GetExpansion1Size() const { return m_expansion0Size; }
		ILINE int GetExpansion2Size() const { return m_expansion1Size; }
		ILINE int GetExpansion3Size() const { return m_expansion2Size; }
		ILINE int GetExpansion4Size() const { return m_expansion3Size; }
		ILINE int GetExpansion5Size() const { return m_expansion4Size; }

		ILINE bool CheckIfBackpackExpandable() const { return m_bCanBeExpanded; }

	private:
		int m_capacity { 0 };
		int m_currentItemCount { 0 };
		int m_expansionCounter { 0 };
		int m_maxAllowedExpansions { 5 };
		int m_expansion0Size { 0 };
		int m_expansion1Size { 0 };
		int m_expansion2Size { 0 };
		int m_expansion3Size { 0 };
		int m_expansion4Size { 0 };
		bool m_bCanBeExpanded { true };
	};

	/**
	This instance's default constructor.
	*/
	CEntityInventory();


	/**
	This instance's default destructor.
	*/
	~CEntityInventory();


private:
	typedef std::vector<EntityId> EntityVector;
	typedef std::vector<IItem> InventoryItemVector;
};
