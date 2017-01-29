#include <StdAfx.h>

//#include "GamePhysicsSettings.h"
//
//
//CGamePhysicsSettings::CGamePhysicsSettings()
//{
//	// Initializes this instance by creating and exposing the available collision classes to the scripts
//	Init();
//}
//
//
//CGamePhysicsSettings::~CGamePhysicsSettings()
//{
//}
//
//
//const char* CGamePhysicsSettings::GetCollisionClassName(unsigned int bitIndex)
//{
//	// Make sure the specified bit is actually usable. If the specified bit is over 23 it cannot be used.
//	if (bitIndex > MAX_COLLISION_CLASSES)
//		return "";
//
//	// Find the name of the specified collision class by searching for it's bit.
//	auto pName = m_CollisionClassFilters.find((pe_collision_class) (1 << bitIndex));
//	if (pName == m_CollisionClassFilters.end())
//		return "";
//	else
//		return pName->second.c_str();
//}
//
//
//void CGamePhysicsSettings::Init()
//{
//	// Create all the available collision classes.
//	m_CollisionClassFilters.insert(std::make_pair(collision_class_terrain, "Terrain"));
//	m_CollisionClassFilters.insert(std::make_pair(collision_class_wheeled, "Wheeled"));
//	m_CollisionClassFilters.insert(std::make_pair(collision_class_living, "Living"));
//	m_CollisionClassFilters.insert(std::make_pair(collision_class_articulated, "Articulated"));
//	m_CollisionClassFilters.insert(std::make_pair(collision_class_soft, "Soft"));
//	m_CollisionClassFilters.insert(std::make_pair(collision_class_rope, "Rope"));
//	m_CollisionClassFilters.insert(std::make_pair(collision_class_particle, "Particle"));
//
//	// Export the created collision classes to the Lua scripts.
//	ExportToLua();
//}
//
//
//void CGamePhysicsSettings::ExportToLua()
//{
//	// Construct a global Lua table called g_PhysicsCollisionClass and store the collision classes in it.
//	// g_PhysicsCollisionClass is expected to exist by some scripts, so we need to create it. 
//	IScriptSystem * pScriptSystem = gEnv->pScriptSystem;
//	IScriptTable* physicsCollisionClassTable = pScriptSystem->CreateTable();
//	physicsCollisionClassTable->AddRef();
//
//	// Begins a Set / Get chain for the script. You could use just plain Set / Get but BeginSetGetChain() is faster when using Set / Get numerous times.
//	physicsCollisionClassTable->BeginSetGetChain();
//
//	// Add all of the available collision classes to the Lua table.
//	for (int i = 0; i < m_CollisionClassFilters.size(); i++)
//	{
//		// Prefix the collision class name with 'b' So that the CRYENGINE sandbox will know that it's a bool.
//		// This allows the sandbox editor to create a check-box for entity scripts that use collision filtering.
//		stack_string name;
//		name.Format("b%s", m_CollisionClassFilters [(pe_collision_class) (1 << i)].c_str());
//
//		// Set the collision class name in the Lua table. You can just use SetValue() but SetValueChain() must be used instead 
//		// when inside Begin/End SetGetChain() calls.
//		physicsCollisionClassTable->SetValueChain(name.c_str(), 1 << i);
//	}
//	// Ends a Set / Get chain for the script. You could use just plain Set / Get But EndSetGetChain() is faster when using Set / Get numerous times.
//	physicsCollisionClassTable->EndSetGetChain();
//
//	// Create a global Lua variable and set it's value to the created Lua table.
//	pScriptSystem->SetGlobalValue("g_PhysicsCollisionClass", physicsCollisionClassTable);
//
//	// The Lua table now exists in the Lua scripts, so there is no need to keep the c++ version in memory.
//	physicsCollisionClassTable->Release();
//}