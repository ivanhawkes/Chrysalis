#pragma once


namespace Chrysalis
{
class CDRSInteractionEntity
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	// ~IEntityComponent

public:
	CDRSInteractionEntity() {}
	virtual ~CDRSInteractionEntity() {}

	static void ReflectType(Schematyc::CTypeDesc<CDRSInteractionEntity>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{4300CD23-0892-4334-9128-2123FFF2242D}"_cry_guid;
		return id;
	}

	virtual void OnResetState() final;
};
}