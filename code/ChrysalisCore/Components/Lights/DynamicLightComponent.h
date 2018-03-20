#pragma once


namespace Chrysalis
{
/** A dynamic light component. */
class CDynamicLightComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

public:
	CDynamicLightComponent() {}
	virtual ~CDynamicLightComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CDynamicLightComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{10EFBBAA-7355-431D-9977-F659164AB607}"_cry_guid;
		return id;
	}

	enum ECastShadowsSpec
	{
		eCastShadowsSpec_No = 0,
		eCastShadowsSpec_Low,
		eCastShadowsSpec_Medium,
		eCastShadowsSpec_High,
		eCastShadowsSpec_VeryHigh
	};

public:
	virtual void OnResetState();

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
}