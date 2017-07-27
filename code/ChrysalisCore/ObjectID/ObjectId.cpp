#include <StdAfx.h>

#include "ObjectId.h"
#include <atomic>
#include <CryCore/Assert/CryAssert.h>
#include <time.h>
#include <CryMath/Random.h>


CObjectIdFactory::CObjectIdFactory(uint32 instanceId)
	: m_instanceId(instanceId),
	m_randomVariant(0),
	m_secondsSinceEpoch(0)
{}


ObjectId CObjectIdFactory::CreateObjectId()
{
	// Validation during testing / debug. For speed reasons we will not validate
	// input in a release build. This decision might be wise to reconsider if you need the validation.
	CRY_ASSERT(m_instanceId < MaxInstanceId);
	CRY_ASSERT(m_randomVariant < MaxRandomVariant);

	ObjectId objectId = InvalidId;

	std::atomic<bool> spinLock { false };
	{
		// Spin on this until we can get a lock.
		while (spinLock.exchange(true)) {}

		// We're rounding our time down to 32 bit, so this code here is susceptible to the Y2038 problem in 2038 when
		// the value will roll over. It will still provide a 1 second window, but the dates derived from using this
		// number will be nonsense. Still, it will take 70+ years before it starts to clash - so plenty of time to
		// switch to 64 bit numbers or another method.
		auto now = static_cast<int32> (time(nullptr));

		if (m_secondsSinceEpoch == now)
		{
			// Check for overflow on the variant.
			m_randomVariant = (m_randomVariant + 1) & MaxRandomVariant;
			if (m_lastRandomVariantSeed == m_randomVariant)
			{
				// #TODO: Don't let this assert happen for the dedicated server, since no-one will be
				// there to notice.
				CryLogAlways("Too many ObjectIds requested at once. See docs for limitations on ObjectId generation.");
				CRY_ASSERT_MESSAGE(false, "Too many ObjectIds requested at once. See docs for limitations on ObjectId generation.");
				return InvalidId;
			}
		}
		else
		{
			// Bring the seconds up to date.
			m_secondsSinceEpoch = now;

			// We generate a new random seed to make keys harder to guess. It's fairly weak but better
			// than nothing at all.
			m_lastRandomVariantSeed = m_randomVariant = cry_random_uint32() & MaxRandomVariant;
		}

		// Pack the values tightly into our data member. We're using the max values as a way to mask the raw
		// data value packed in. This is a cheap operation to perform and should help prevent dirty data
		// slipping in.
		objectId = (static_cast<uint64_t>(m_secondsSinceEpoch) << (InstanceIdBits + RandomVariantBits))
			+ (static_cast<uint64_t>((m_instanceId & MaxInstanceId)) << RandomVariantBits)
			+ (m_randomVariant & MaxRandomVariant);

		// Unlock!
		spinLock = false; 
	}

	return objectId;
}


uint32 CObjectIdFactory::GetSecondsSinceEpoch(ObjectId objectId)
{
	return uint32(objectId >> (InstanceIdBits + RandomVariantBits));
}


uint32 CObjectIdFactory::GetInstanceId(ObjectId objectId)
{
	return (objectId >> RandomVariantBits) & MaxInstanceId;
}


uint32 CObjectIdFactory::GetRandomVariant(ObjectId objectId)
{
	return objectId & MaxRandomVariant;
}
