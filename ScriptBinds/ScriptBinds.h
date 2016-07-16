#pragma once

class CScriptBindGame;
class CScriptBindBoids;
class CScriptBindItem;


class CScriptBinds
{
public:
	/** Default constructor. */
	CScriptBinds();

	/** Destructor. */
	virtual ~CScriptBinds();

	/** Initialises this object. */
	void Init();

	/** Releases this object. */
	void Release();

	/**
	Automatically called by various systems to get the memory statistics for this instance. Add your own usage via
	'*this'. Be sure to add any memory you have allocated on the heap.

	\param [in,out]	s	The ICrySizer interface used to "size-up" this instance.
	*/
	virtual void GetMemoryUsage(ICrySizer * s);

	CScriptBindGame* GetGame() { return m_pScriptBindGame; }

	CScriptBindBoids* GetBoids() { return m_pScriptBindBoids; }

	CScriptBindItem* GetItem() { return m_pScriptBindItem; }


private:
	/** The game script bind. Used to provide scripts access to this instance's functionality. */
	CScriptBindGame* m_pScriptBindGame = nullptr;

	/** The script bind for boids. */
	CScriptBindBoids* m_pScriptBindBoids = nullptr;

	/** The script bind for items. */
	CScriptBindItem* m_pScriptBindItem = nullptr;
};