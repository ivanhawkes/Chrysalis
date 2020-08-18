#pragma once

#include <CryDynamicResponseSystem/IDynamicResponseAction.h>


namespace Chrysalis
{
class CActionOpen final : public DRS::IResponseAction
{
public:
	CActionOpen() {}
	CActionOpen(const string& triggerName) : m_targetName(triggerName) {}
	virtual ~CActionOpen() {}

	// IResponseAction
	virtual DRS::IResponseActionInstanceUniquePtr Execute(DRS::IResponseInstance* pResponseInstance) override;
	virtual string GetVerboseInfo() const override;
	virtual void Serialize(Serialization::IArchive& ar) override;
	virtual const char* GetType() const override { return "Open"; }
	// ~IResponseAction

private:
	string m_targetName;
};


class CActionOpenInstance final : public DRS::IResponseActionInstance
{
public:
	CActionOpenInstance();
	virtual ~CActionOpenInstance();

	// IResponseActionInstance
	virtual eCurrentState Update() override;
	virtual void Cancel() override;
	// ~IResponseActionInstance
};
}