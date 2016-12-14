#pragma once

#include <CryDynamicResponseSystem/IDynamicResponseAction.h>


class CActionLock final : public DRS::IResponseAction
{
public:
	CActionLock() {}
	CActionLock(const string& triggerName) : m_targetName(triggerName) {}
	virtual ~CActionLock() {}

	// IResponseAction
	virtual DRS::IResponseActionInstanceUniquePtr Execute(DRS::IResponseInstance* pResponseInstance) override;
	virtual string GetVerboseInfo() const override;
	virtual void Serialize(Serialization::IArchive& ar) override;
	virtual const char* GetType() const override { return "Lock"; }
	// ~IResponseAction

private:
	string m_targetName;
};


class CActionLockInstance final : public DRS::IResponseActionInstance
{
public:
	CActionLockInstance();
	virtual ~CActionLockInstance();

	// IResponseActionInstance
	virtual eCurrentState Update() override;
	virtual void Cancel() override;
	// ~IResponseActionInstance
};
