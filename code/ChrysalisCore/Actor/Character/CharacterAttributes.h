#pragma once


// TODO: Work out what's needed here and make it into a template for floats, ints, etc

namespace Chrysalis
{
class CCharacterAttributes
{
public:

	/**
	Gets the name. Names should be localised as they can be displayed in the UI.

	\return The name.
	**/
	inline string GetName() const { return m_name; }


	/**
	Gets the value, which combines the base value with any bonuses to this attribute.

	\return The value.
	**/
	inline float GetValue() const { return m_baseValue + m_bonusValue; }


	/**
	Gets base value.

	\return The base value.
	**/
	inline float GetBaseValue() const { return m_baseValue; }


	/**
	Gets bonus value.

	\return The bonus value.
	**/
	inline float GetBonusValue() const { return m_bonusValue; }

private:
	string m_name;
	float m_baseValue;
	float m_bonusValue;
};
}