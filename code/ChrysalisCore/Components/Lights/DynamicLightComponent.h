#pragma once

#include "Helpers/DesignerEntityComponent.h"


class CDynamicLightComponent final : public CDesignerEntityComponent<>, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CDynamicLightComponent, "DynamicLightComponent", 0xBFAD8CB384564A3B, 0x8AE6F05F5D21BBFA);

	virtual ~CDynamicLightComponent() {}

public:
	enum ECastShadowsSpec
	{
		eCastShadowsSpec_No = 0,
		eCastShadowsSpec_Low,
		eCastShadowsSpec_Medium,
		eCastShadowsSpec_High,
		eCastShadowsSpec_VeryHigh
	};

	struct IDynamicLightListener
	{
		virtual ~IDynamicLightListener() {};

		virtual void OnDynamicLightResetState() = 0;
	};

	void AddEventListener(IDynamicLightListener* pListener)
	{
		assert(pListener);
		if (pListener)
			stl::push_back_unique(m_ListenersList, pListener);
	}

	void RemoveEventListener(IDynamicLightListener* pListener)
	{
		assert(pListener);
		m_ListenersList.remove(pListener);
	}

public:
	// CDesignerEntityComponent
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	virtual void OnResetState() override;
	// ~CDesignerEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "Dynamic Light Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	void SetActive(bool bActive)
	{
		if (m_isActive == bActive)
			return;

		m_isActive = bActive;
		OnResetState();
	}

	/** Gets the slot the light is loaded into or -1 if there is no loaded light. **/
	int GetSlotId() { return m_slot; }

	void SetLocalTM(Matrix34 localMatrix);

private:
	typedef std::list<IDynamicLightListener*> TListenersList;
	TListenersList m_ListenersList;

	bool m_isActive { true };
	int m_slot { -1 };
	CDLight m_light;
	bool m_bIgnoreVisAreas { false };
	bool m_bAffectsThisAreaOnly { true };
	bool m_bAmbient { false };
	bool m_bFake { false };
	bool m_bAffectVolumetricFog { true };
	bool m_bAffectVolumetricFogOnly { false };
	ECastShadowsSpec m_castShadowSpec { eCastShadowsSpec_No };
	float m_animSpeed { 1.0f };
	ColorF m_diffuseColor = ColorF(1, 1, 1, 1);
	float m_diffuseMultiplier { 1.0f };
	string m_projectorTexturePath;
	string m_flareTexturePath;
	float m_flareFieldOfView { 360.0f };
};
