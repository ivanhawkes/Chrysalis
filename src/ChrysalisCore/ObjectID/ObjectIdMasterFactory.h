/**
\file	ObjectID\ObjectIdMasterFactory.h

Declares the object identifier master factory class. This is provided as a convenience for keeping the 
modest number of ObjectIdfactory instances that we are likely to require.
*/
#pragma once

namespace Chrysalis
{
class CObjectIdFactory;


/** An object identifier master factory. This class is just used as a wrapper for creating a number
of unique objectId factories. It's primary use is simply to remove clutter from the CGame namespace.*/
class CObjectIdMasterFactory
{
public:
	/**
	Constructor.

	\param	instanceId	Identifier for the instance.
	*/
	CObjectIdMasterFactory(uint32 instanceId);

	/** Destructor. */
	~CObjectIdMasterFactory();

	CObjectIdFactory* GetCharacter() const { return m_pCharacterFactory; }
	CObjectIdFactory* GetAccount() const { return m_pAccountFactory; }
	CObjectIdFactory* GetFaction() const { return m_pFactionFactory; }
	CObjectIdFactory* GetItem() const { return m_pItemFactory; }

private:
	uint32 m_instanceId;

	// Declare one for every factory instance we require.
	CObjectIdFactory* m_pCharacterFactory;
	CObjectIdFactory* m_pAccountFactory;
	CObjectIdFactory* m_pFactionFactory;
	CObjectIdFactory* m_pItemFactory;

	// DO NOT IMPLEMENT.
	CObjectIdMasterFactory();
};
}