/**
\file	Game\Registration\CGameRegistration.h

Declares the game registration class. The purpose of this class is to gather together all the sections
of code that are used to register factories, game objects, and other things which require registration.

By keeping it all here we help keep the other code clear and uncluttered.

This code is refactored from GameFactory.h / GameFactory.cpp.
*/
# pragma once

struct IGameFramework;
class CGameEntityNodeFactory;


struct SEntityScriptProperties
{
	SEntityScriptProperties()
		: pEntityTable(nullptr)
		, pEditorTable(nullptr)
		, pPropertiesTable(nullptr)
		, pInstancePropertiesTable(nullptr)
	{
	}

	IScriptTable* pEntityTable;
	IScriptTable* pEditorTable;
	IScriptTable* pPropertiesTable;
	IScriptTable* pInstancePropertiesTable;
};


class CGameRegistration
{
public:
	// Register all the components that this game needs to register.
	static void RegisterGameObjects(IGameFramework *pFramework);

private:
	enum eGameObjectRegistrationFlags
	{
		eGORF_None = 0x0,
		eGORF_HiddenInEditor = 0x1,
		eGORF_NoEntityClass = 0x2,
		eGORF_InstanceProperties = 0x4,
	};


	/** Registers the entity flow nodes. The flow node registration data is disposed of after registration. */
	// TODO: This is not currently being called anywhere. Find out where it needs to be hooked in.
	static void RegisterEntityFlowNodes();


	/**
	Creates a set of empty script tables.

	\param [in,out]	out	The out.
	\param	flags	   	The flags.
	*/
	static void CreateScriptTables(SEntityScriptProperties& out, uint32 flags);


	/**
	Registers a game object with a script.
	
	\tparam	T	Generic type parameter.
	\param	name  	The name of the game object type.
	\param	script	The Lua script for this type of entity.
	\param	flags 	The flags.
	*/
	template<class T>
	static void RegisterGameObject(const string& name, const string& script, uint32 flags = 0);


	/**
	Registers the no script game object.
	
	\tparam	T	Generic type parameter.
	\param	name	  	The name of the game object type.
	\param	editorPath	Full pathname of the file.
	\param	flags	  	The flags.
	*/
	template<class T>
	static void RegisterNoScriptGameObject(const string& name, const string& editorPath, uint32 flags = 0);


	/**
	 Registers a game object factory making it accessible to the system to spawn objects of that class.
	
	 \tparam	T	Generic type parameter.
	 \param [in,out]	pFramework	A pointer to the framework.
	 \param	className			  	The class name to register this factory under.
	 \param	isAi				  	Is this an AI object class.
	 */
	template<class T>
	static void RegisterFactory(IGameFramework *pFramework, const string& className, bool isAi = false);


	// Most functions are static, so this is too.
	static std::map<string, CGameEntityNodeFactory*> s_flowNodeFactories;
};
