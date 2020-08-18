#pragma once


namespace Chrysalis
{
/** A key extension. */
class CKeyringComponent
	: public IEntityComponent
{
	// IEntityComponent
	void Initialize() override {};
	// ~IEntityComponent

public:
	CKeyringComponent() {}
	virtual ~CKeyringComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CKeyringComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{12CACED6-E8E6-4363-867B-9B59ACBD5D12}"_cry_guid;
		return id;
	}

	virtual void OnResetState();

private:
	string m_keys;
	bool m_bActive { true };
};
}