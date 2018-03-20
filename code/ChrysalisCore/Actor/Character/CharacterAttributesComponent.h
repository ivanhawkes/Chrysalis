#pragma once


namespace Chrysalis
{
/** A CharacterAttributes extension. */
class CCharacterAttributesComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

public:
	static void ReflectType(Schematyc::CTypeDesc<CCharacterAttributesComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{9A65029F-3D2D-4754-9A7A-C9AAAED49C3D}"_cry_guid;
		return id;
	}

	// IEntityComponent
	void Initialize() override;
	// ~IEntityComponent

	// CCharacterAttributesComponent
	CCharacterAttributesComponent() = default;
	virtual ~CCharacterAttributesComponent() = default;

	// Called on entity spawn, or when the state of the entity changes in Editor
	virtual void OnResetState();
};
}