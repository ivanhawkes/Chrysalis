#pragma once

#include <CryDynamicResponseSystem/IDynamicResponseAction.h>


class CActionClose final : public DRS::IResponseAction
{
public:
	CActionClose() {}
	CActionClose(const string& triggerName) : m_targetName(triggerName) {}
	virtual ~CActionClose() {}

	// IResponseAction
	virtual DRS::IResponseActionInstanceUniquePtr Execute(DRS::IResponseInstance* pResponseInstance) override;
	virtual string GetVerboseInfo() const override;
	virtual void Serialize(Serialization::IArchive& ar) override;
	virtual const char* GetType() const override { return "Close"; }
	// ~IResponseAction

private:
	string m_targetName;
};


class CActionCloseInstance final : public DRS::IResponseActionInstance
{
public:
	CActionCloseInstance();
	virtual ~CActionCloseInstance();

	// IResponseActionInstance
	virtual eCurrentState Update() override;
	virtual void Cancel() override;
	// ~IResponseActionInstance
};
