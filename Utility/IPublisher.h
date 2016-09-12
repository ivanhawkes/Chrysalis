#pragma once


/**
A naive implementation of the publisher design pattern. Because the vector that stores the list of listeners is
in-built, you cannot use two of these on the same class.

If you find yourself in the rare situration of needing two or more implementations of this for a single class then
supply the remaining implementations by hand.

\tparam	T Generic type parameter.
**/
template<class T>
class IPublisher
{
public:
	void AddListener(T pListener)
	{
		CRY_ASSERT(pListener);
		if (std::find(m_listeners.begin(), m_listeners.end(), pListener) == m_listeners.end())
		{
			m_listeners.push_back(pListener);
		}
	}


	void RemoveListener(T pListener)
	{
		CRY_ASSERT(pListener);
		stl::find_and_erase(m_listeners, pListener);
	}

	// TODO: Write an iterator that takes a lamba parameter to simplify using this to iterate
	// all elements and call a function for each.

protected:
	std::vector<T> m_listeners;
};


