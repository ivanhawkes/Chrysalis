#pragma once

#include <CryDynamicResponseSystem/IDynamicResponseAction.h>


class CActionPlayAnimation final : public DRS::IResponseAction
{
public:
	CActionPlayAnimation() {}
	CActionPlayAnimation(const string& triggerName) : m_targetName(triggerName) {}
	virtual ~CActionPlayAnimation() {}

	// IResponseAction
	virtual DRS::IResponseActionInstanceUniquePtr Execute(DRS::IResponseInstance* pResponseInstance) override;
	virtual string GetVerboseInfo() const override;
	virtual void Serialize(Serialization::IArchive& ar) override;
	virtual const char* GetType() const override { return "PlayAnimation"; }
	// ~IResponseAction

private:

	void StartAnimation(DRS::IResponseInstance* pResponseInstance);
	void UpdateAnimation(DRS::IResponseInstance* pResponseInstance);
	void CancelAnimation(DRS::IResponseInstance* pResponseInstance);

	bool m_isEntityActivationForced { false };
	uint32 m_animationLayer { 0 };
	uint32 m_token { 0xFFFF };

	string m_targetName;
};


class CActionPlayAnimationInstance final : public DRS::IResponseActionInstance
{
public:
	CActionPlayAnimationInstance();
	virtual ~CActionPlayAnimationInstance();

	// IResponseActionInstance
	virtual eCurrentState Update() override;
	virtual void Cancel() override;
	// ~IResponseActionInstance
};
