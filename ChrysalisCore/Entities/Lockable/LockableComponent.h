#pragma once


/**
A lock extension.

\sa IEntityComponent
\sa IEntityPropertyGroup
**/
class CLockableComponent : public IEntityComponent, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CLockableComponent, "Lockable", 0x819A4630CA1840F9, 0xB4C7F93B3F13A69A)

public:
	enum EInputPorts
	{
		eInputPort_Unlock = 0,
		eInputPort_Lock
	};

	// IEntityComponent
	void Initialize() override {};
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_START_LEVEL) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_EDITOR_PROPERTY_CHANGED) | BIT64(ENTITY_EVENT_XFORM_FINISHED_EDITOR); }
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Lockable Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// ***
	// *** CLockableComponent
	// ***

	CLockableComponent() {};
	virtual ~CLockableComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

private:
	bool m_isLocked { true };

	void OnResetState();
};