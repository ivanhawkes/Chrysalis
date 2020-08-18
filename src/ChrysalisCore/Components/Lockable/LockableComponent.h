#pragma once


/**
A lock extension.
**/
namespace Chrysalis
{
class CLockableComponent
	: public IEntityComponent
{
public:
	CLockableComponent() {}
	virtual ~CLockableComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CLockableComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{8DEFBD90-4A15-4B2A-B9CD-60AF386658C0}"_cry_guid;
		return id;
	}


	bool IsLocked() const { return m_isLocked; }
	void SetLocked(bool val) { m_isLocked = val; }

	virtual void OnResetState();

private:
	bool m_isLocked { true };
};
}