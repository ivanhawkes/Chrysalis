#pragma once

#include <CryScriptSystem/IScriptSystem.h>
#include <CryEntitySystem/IEntity.h>

class CItem;


/**
The item script bind. Used to expose all game functionality To Lua. All methods and variables that you want to
expose to Lua that pertain to the game should be added here. Every function and variable that this class has will be
accessible to the scripts as long as they are registered with the script system.

\sa					 	CScriptableBase
\sa	T:CScriptableBase
*/
class CScriptBindItem : public CScriptableBase
{
public:

	/**
	This instance's default constructor. Used to create the "Item" Lua table and to register all this instance's
	methods to the script system. They will then be accessible from the "Item" Lua table.
	*/
	CScriptBindItem();


	/**
	This instance's default destructor.
	*/
	~CScriptBindItem();


	// ***
	// *** Helper functions for setting up scripting.
	// ***


	/**
	Takes a CItem and attaches the script bind onto that entity.
	
	\param [in,out]	pItem	If non-null, the item.
	*/
	void AttachTo(CItem *pItem);


	// ***
	// *** Functions that are exposed to the script environment.
	// ***
	


private:
	/** Registers global variables for Lua to make use of. */
	void RegisterGlobals();

	/** Registers the methods. */
	void RegisterMethods();
};