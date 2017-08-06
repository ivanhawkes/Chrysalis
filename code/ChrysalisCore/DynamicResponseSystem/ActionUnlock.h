#pragma once

#include <CryDynamicResponseSystem/IDynamicResponseAction.h>


namespace Chrysalis
{
class CActionUnlock final : public DRS::IResponseAction
{
public:
	CActionUnlock() {}
	CActionUnlock(const string& triggerName) : m_targetName(triggerName) {}
	virtual ~CActionUnlock() {}

	// IResponseAction
	virtual DRS::IResponseActionInstanceUniquePtr Execute(DRS::IResponseInstance* pResponseInstance) override;
	virtual string GetVerboseInfo() const override;
	virtual void Serialize(Serialization::IArchive& ar) override;
	virtual const char* GetType() const override { return "Unlock"; }
	// ~IResponseAction

private:
	string m_targetName;
};


class CActionUnlockInstance final : public DRS::IResponseActionInstance
{
public:
	CActionUnlockInstance();
	virtual ~CActionUnlockInstance();

	// IResponseActionInstance
	virtual eCurrentState Update() override;
	virtual void Cancel() override;
	// ~IResponseActionInstance
};
}