#pragma once

#include <CryEntitySystem/IEntity.h>
#include <CryExtension/CryGUID.h>


namespace Chrysalis
{
struct ISnaplock;

DECLARE_SHARED_POINTERS(ISnaplock);

// Make it easier and more consitant to declare snaplock types.
#define DECLARE_SNAPLOCK_TYPE(class_name, type_name, guid_hi, guid_lo) const static ISnaplockType class_name { ISnaplockType(type_name, CryGUID { guid_hi, guid_lo }) };


/** Each type needs to be registered with the Snaplock system. */
struct ISnaplockType
{
	/** Required by serialisation code. */
	ISnaplockType() = default;

	ISnaplockType(string typeName, CryGUID typeId) :
		m_typeName(typeName),
		m_typeId(typeId)
	{
	}
	virtual ~ISnaplockType() = default;

	// We will consider types to be identical if their typeIds are identical.
	bool operator==(const ISnaplockType& rhs) const { return m_typeId == rhs.m_typeId; }

	// Sorting.
	bool operator<(const ISnaplockType& rhs) const { return m_typeId < rhs.m_typeId; }

	virtual void Serialize(Serialization::IArchive& ar)
	{
		ar(m_typeId, "typeId", "Type Id");
		ar.doc(m_typeName);
	}

	/** Unique Id for this type of snaplock. */
	const CryGUID& GetTypeId() const { return m_typeId; }
	const string GetName() const { return m_typeName; }

private:
	string m_typeName;
	CryGUID m_typeId { CryGUID::Null() };
};


struct ISnaplock
{
	/** Required by serialisation code. */
	ISnaplock() = default;

	ISnaplock(ISnaplockType snaplockType, bool isMale, bool isRoot = false) :
		m_snaplockType(snaplockType),
		m_isMale(isMale),
		m_isRoot(isRoot)
	{
	}
	virtual ~ISnaplock() = default;

	virtual void Serialize(Serialization::IArchive& ar);

	/** The type of snaplock. */
	const ISnaplockType& GetType() const { return m_snaplockType; }

	/** Is this a male snaplock. Males plug into females. */
	bool IsMale() const { return m_isMale; }

	/** Is this a female snaplock. Males plug into females. */
	bool IsFemale() const { return m_isMale == true; }

	std::vector<ISnaplock> GetChildren();

	/** Add a new snaplock as a child of this one. */
	void AddSnaplock(ISnaplock snaplock);

private:
	/** Type of the snaplock */
	ISnaplockType m_snaplockType;

	/** Is this the root of the tree? */
	bool m_isRoot { false };

	/** Males socket into females, this indicates if the snaplock is male or female. */
	bool m_isMale;

	/** Is this snaplock currently in use? */
	bool m_isInUse { false };

	/** It's possible to nest snaplocks recursively. These are the children of this snaplock. */
	std::vector<ISnaplock> m_children;
};


DECLARE_SNAPLOCK_TYPE(SLT_ROOT, "Root", 0xBA5EBA5EBA5EBA5E, 0xBA5EBA5EBA5EBA5E)
}