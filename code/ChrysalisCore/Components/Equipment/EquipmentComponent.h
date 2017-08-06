#pragma once


namespace Chrysalis
{
/** An Equipment extension. */
class CEquipmentComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	// ~IEntityComponent

public:
	CEquipmentComponent() {}
	virtual ~CEquipmentComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CEquipmentComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{E288AB46-CC70-46C4-8264-6D4E8F0D832C}"_cry_guid;
		return id;
	}

	// Called on entity spawn, or when the state of the entity changes in Editor
	virtual void OnResetState();
};
}