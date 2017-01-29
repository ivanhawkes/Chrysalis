#pragma once

//#include <IGamePhysicsSettings.h>
//#include <CryPhysics/physinterface.h>
//#include <CryString/CryString.h>
//#include <map>
//
//
///**
//An implementation of the IGamePhysicsSettings interface. Used to provide game-specific collision settings such as
//collision filtering (what objects should collide with other objects). Every CRYENGINE game needs to have an
//implementation of the IGamePhysicsSettings interface. If not, collision will not be possible.
//
//\sa	IGamePhysicsSettings
//*/
//class CGamePhysicsSettings : public IGamePhysicsSettings
//{
//public:
//
//	CGamePhysicsSettings();
//	~CGamePhysicsSettings();
//
//
//	// ***
//	// *** IGamePhysicsSettings
//	// ***
//
//
//	/**
//	Gets the name of the specified collision class.
//
//	\param	bitIndex	Zero-based bit-index of the bit that defines the collision class you want to get the name of.
//
//	\return	The name of the specified collision class.
//	*/
//	virtual const char* GetCollisionClassName(unsigned int bitIndex) override;
//
//
//	// ***
//	// *** CGamePhysicsSettings
//	// ***
//
//private:
//
//	/**
//	Initializes this instance. Used to create all available collision filters. These are then exported to Lua by
//	calling this instance's ExportToLua() method.
//	
//	Exposing these classes to Lua is important, as Lua expects them to exist. If these classes are not exposed to Lua,
//	some physics scripts would fail (eg. RigidbodyEx).
//	*/
//	void Init();
//
//
//	/**
//	Exports all available collision filters to Lua scripts.
//	
//	Exposing the collision classes to lua is important, as lua expects them to exists. If these classes are not exposed
//	to Lua,  some physics scripts would fail (namely the RigidbodyEx script and other collision dependent scripts). There
//	is a c++ way of setting collision filtering, and if you go this route, you should erase all physics script entities
//	and create your own using your own c++ methods.
//	*/
//	void ExportToLua();
//
//
//	/**
//	The available collision class filters. Used to decide what objects can collide with each other.
//	*/
//	std::map<pe_collision_class, string> m_CollisionClassFilters;
//
//
//	/**
//	The maximum amount of collision classes possible. The max is 23 because Lua doesn't support integers, it uses
//	floats. This means that we can only use 23 bits out of the 32 in a float as the other 9 bits are used for floating
//	point precision.
//	*/
//	static const uint32 MAX_COLLISION_CLASSES = 23;
//};