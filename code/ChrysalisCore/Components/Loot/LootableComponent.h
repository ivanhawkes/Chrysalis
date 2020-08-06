#pragma once


namespace Chrysalis
{
/** A key extension. */
class CLootableComponent
	: public IEntityComponent
{
protected:
	// IEntityComponent
	void Initialize() override {};
	// ~IEntityComponent

public:
	CLootableComponent() {}
	virtual ~CLootableComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CLootableComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{7B1BA680-DF6D-433F-9CB9-FA14A55CF000}"_cry_guid;
		return id;
	}

	virtual void OnResetState();

	struct SLootTableEntry
	{
		inline bool operator==(const SLootTableEntry& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


		static void ReflectType(Schematyc::CTypeDesc<SLootTableEntry>& desc)
		{
			desc.SetGUID("{625EAC5E-143D-4C70-A0AE-100C32352DE7}"_cry_guid);
			desc.SetLabel("Loot Table Entry");
			desc.SetDescription("An entry from the master loot table.");
		}


		void Serialize(Serialization::IArchive& ar)
		{
			ar(m_lootTable, "LootTable", "Loot table entry.");
			ar.doc("A uniquely identifiable entry on the master loot table.");
		}

		string m_lootTable;
	};

private:
	/** A uniquely identifying entry into a loot table. This will be used to distribute the loot when
	characters interact with this component. */
	SLootTableEntry m_lootTableEntry;

	bool m_bActive {true};
};
}