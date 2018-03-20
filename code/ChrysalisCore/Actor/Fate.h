/** This class provides a randomly generated seed that is specific to the actor / object and lasts for their entire
lifetime. It's intention is for use as a seed for pseudo-random outcomes that have been pre-determined at or close
to the time of the actor's creation. Anything attributable to 'Deus Ex Machina' should be derived from this value.
By doing this, an actor's fate is literally sealed at their birth / creation for events based on this value.

The value can also be used for simple things like innate aptitude for various subjects. It can act as a simple
form of DNA, locking in attributes specific to a character.

Because it's possible to use a memory sniffer to determine this value after creation, it's suggested you either
start with a 0 value "no fate" or a random value that is switched once or more at later stages of a character's
development. */

#pragma once

#include <CryMath/Random.h>


namespace Chrysalis
{
class CFate
{
public:

	CFate() = default;
	virtual ~CFate() = default;

	/**
	Changes the seed value of fate for a character. All fate based tests after this point in time
	will be affected and based off this new seed.

	\param	fate	The fate.
	*/
	void ChangeYourFate() { m_fate = (static_cast<uint64_t>(cry_random_uint32()) << 32) + static_cast<uint64_t>(cry_random_uint32()); }


	/**
	Sets a fate. Intended for use in games where they don't want to make use of a randomised fate.

	\param	fate	The fate.
	*/
	void SetFate(uint64 fate) { m_fate = fate; }

	/**
	Gets the character's complete fate.

	\return	The fate.
	*/
	uint64 GetFate() const { return m_fate; }


	/**
	Gets the character's greater fate. This portion of the fate seed is intended for large scale events
	in the character's life.

	\return	The greater fate.
	*/
	uint32 GetGreaterFate() const { return static_cast<uint32_t>(m_fate >> 32); }


	/**
	Gets the character's lesser fate. This portion of the fate seed is intended for small scale events
	in the character's life.

	\return	The lesser fate.
	*/
	uint32 GetLesserFate() const { return static_cast<uint32_t>(m_fate & 0x0000FFFF); }

private:

	/** The fate seed. */
	uint64 m_fate { 0 };
};
}