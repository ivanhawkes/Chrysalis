#pragma once

#include <CryDynamicResponseSystem/IDynamicResponseAction.h>


namespace Chrysalis
{
class CActionSwitch final : public DRS::IResponseAction
{
public:
	CActionSwitch() {}
	CActionSwitch(const string& triggerName) : m_targetName(triggerName) {}
	virtual ~CActionSwitch() {}

	// IResponseAction
	virtual DRS::IResponseActionInstanceUniquePtr Execute(DRS::IResponseInstance* pResponseInstance) override;
	virtual string GetVerboseInfo() const override;
	virtual void Serialize(Serialization::IArchive& ar) override;
	virtual const char* GetType() const override { return "interaction_switch"; }
	// ~IResponseAction

private:
	string m_targetName;
};


class CActionSwitchInstance final : public DRS::IResponseActionInstance
{
public:
	CActionSwitchInstance() = default;
	virtual ~CActionSwitchInstance() = default;

	// IResponseActionInstance
	virtual eCurrentState Update() override;
	virtual void Cancel() override;
	// ~IResponseActionInstance
};
}