#pragma once


namespace Chrysalis
{
/**
A Inventory extension.

**/
class CInventoryComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	// ~IEntityComponent

public:
	CInventoryComponent() {}
	virtual ~CInventoryComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CInventoryComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{D9CD9BE7-F17F-4331-BEBF-E0ED9567D1DE}"_cry_guid;
		return id;
	}

	// Called on entity spawn, or when the state of the entity changes in Editor
	virtual void OnResetState();
};
}