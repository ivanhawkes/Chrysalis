#pragma once

#include <CryDynamicResponseSystem/IDynamicResponseCondition.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>

struct DRS::IResponseActor;
struct DRS::IVariableCollection;


class CConditionDistanceToEntity final : public DRS::IResponseCondition
{
public:
	CConditionDistanceToEntity();
	CConditionDistanceToEntity(const string& actorName);
	virtual ~CConditionDistanceToEntity();

	// IResponseCondition
	virtual bool IsMet(DRS::IResponseInstance* pResponseInstance) override;
	virtual void Serialize(Serialization::IArchive& ar) override;
	virtual string GetVerboseInfo() const override;
	virtual const char* GetType() const override { return "DistanceToEntity"; }
	// ~IResponseCondition

private:
	float m_squaredDistance { 100.0f };
	string m_entityName;
};
