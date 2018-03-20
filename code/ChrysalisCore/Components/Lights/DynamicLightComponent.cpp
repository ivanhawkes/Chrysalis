#include "StdAfx.h"

#include "DynamicLightComponent.h"


namespace Chrysalis
{
void CDynamicLightComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CDynamicLightComponent::ReflectType(Schematyc::CTypeDesc<CDynamicLightComponent>& desc)
{
	desc.SetGUID(CDynamicLightComponent::IID());
	desc.SetEditorCategory("Lights");
	desc.SetLabel("Dynamic Light");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

	//desc.AddMember(&CDynamicLightComponent::m_bActive, 'actv', "Active", "Active", "Determines whether the light is enabled", true);
	//desc.AddMember(&CDynamicLightComponent::m_radius, 'radi', "Radius", "Radius", "Determines whether the range of the point light", 10.f);
	//desc.AddMember(&CDynamicLightComponent::m_color, 'colo', "Color", "Color", "Color emission information", CDynamicLightComponent::SColor());
	//desc.AddMember(&CDynamicLightComponent::m_shadows, 'shad', "Shadows", "Shadows", "Shadow casting settings", CDynamicLightComponent::SShadows());
	//desc.AddMember(&CDynamicLightComponent::m_options, 'opt', "Options", "Options", "Specific Light Options", CDynamicLightComponent::SOptions());
	//desc.AddMember(&CDynamicLightComponent::m_animations, 'anim', "Animations", "Animations", "Light style / animation properties", CDynamicLightComponent::SAnimations());
}



//YASLI_ENUM_BEGIN_NESTED(CDynamicLightComponent, ECastShadowsSpec, "CastShadowsSpec")
//YASLI_ENUM_VALUE_NESTED(CDynamicLightComponent, eCastShadowsSpec_No, "None")
//YASLI_ENUM_VALUE_NESTED(CDynamicLightComponent, eCastShadowsSpec_Low, "Low")
//YASLI_ENUM_VALUE_NESTED(CDynamicLightComponent, eCastShadowsSpec_Medium, "Medium")
//YASLI_ENUM_VALUE_NESTED(CDynamicLightComponent, eCastShadowsSpec_High, "High")
//YASLI_ENUM_VALUE_NESTED(CDynamicLightComponent, eCastShadowsSpec_VeryHigh, "VeryHigh")
//YASLI_ENUM_END()


void CDynamicLightComponent::OnResetState()
{
	IEntity& entity = *GetEntity();

	// Check if we have to unload first
	if (m_slot != -1)
	{
		entity.FreeSlot(m_slot);
		m_slot = -1;
	}

	// Check if the light is active
	if (!m_isActive)
		return;

	m_light.SetPosition(ZERO);
	m_light.m_fLightFrustumAngle = 45;
	m_light.m_fProjectorNearPlane = 0;
	m_light.m_Flags = DLF_DEFERRED_LIGHT | DLF_THIS_AREA_ONLY;

	if (m_bAffectsThisAreaOnly)
	{
		m_light.m_Flags |= DLF_THIS_AREA_ONLY;
	}
	if (m_bIgnoreVisAreas)
	{
		m_light.m_Flags |= DLF_IGNORES_VISAREAS;
	}
	if (m_bAmbient)
	{
		m_light.m_Flags |= DLF_AMBIENT;
	}
	if (m_bFake)
	{
		m_light.m_Flags |= DLF_FAKE;
	}
	if (m_bAffectVolumetricFog)
	{
		m_light.m_Flags |= DLF_VOLUMETRIC_FOG;
	}
	if (m_bAffectVolumetricFogOnly)
	{
		m_light.m_Flags |= DLF_VOLUMETRIC_FOG_ONLY;
	}

	m_light.m_ProbeExtents(m_light.m_fRadius, m_light.m_fRadius, m_light.m_fRadius);

	m_light.SetShadowBiasParams(1.f, 1.f);
	m_light.m_fShadowUpdateMinRadius = m_light.m_fRadius;

	float shadowUpdateRatio = 1.f;
	m_light.m_nShadowUpdateRatio = max((uint16)1, (uint16)(shadowUpdateRatio * (1 << DL_SHADOW_UPDATE_SHIFT)));

	if (m_castShadowSpec != eCastShadowsSpec_No && (int)gEnv->pSystem->GetConfigSpec() >= (int)m_castShadowSpec)
		m_light.m_Flags |= DLF_CASTSHADOW_MAPS;
	else
		m_light.m_Flags &= ~DLF_CASTSHADOW_MAPS;

	m_light.SetAnimSpeed(m_animSpeed);

	m_light.SetLightColor(m_diffuseColor * m_diffuseMultiplier);

	m_light.m_nSortPriority = 0;
	m_light.SetFalloffMax(1.0f);
	m_light.m_fProjectorNearPlane = 0.f;

	m_light.m_fFogRadialLobe = 0.f;

	SAFE_RELEASE(m_light.m_pLightImage);
	SAFE_RELEASE(m_light.m_pLightDynTexSource);

	if (!m_projectorTexturePath.IsEmpty())
	{
		const char* pExt = PathUtil::GetExt(m_projectorTexturePath);
		if (!stricmp(pExt, "swf") || !stricmp(pExt, "gfx") || !stricmp(pExt, "usm") || !stricmp(pExt, "ui"))
		{
			m_light.m_pLightDynTexSource = gEnv->pRenderer->EF_LoadDynTexture(m_projectorTexturePath, false);
		}
		else
		{
			m_light.m_pLightImage = gEnv->pRenderer->EF_LoadTexture(m_projectorTexturePath, FT_DONT_STREAM);
		}

		if ((!m_light.m_pLightImage || !m_light.m_pLightImage->IsTextureLoaded()) && !m_light.m_pLightDynTexSource)
		{
			CryWarning(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_WARNING, 0, m_projectorTexturePath.c_str(),
				"Light projector texture not found: %s", m_projectorTexturePath.c_str());
			m_light.m_pLightImage = gEnv->pRenderer->EF_LoadTexture("Textures/defaults/red.dds", FT_DONT_STREAM);
		}
	}

	if (!m_flareTexturePath.IsEmpty())
	{
		int nLensOpticsId;

		if (gEnv->pOpticsManager->Load(m_flareTexturePath, nLensOpticsId))
		{
			if (IOpticsElementBase* pOptics = gEnv->pOpticsManager->GetOptics(nLensOpticsId))
			{
				m_light.SetLensOpticsElement(pOptics);

				if (m_flareFieldOfView != 0)
				{
					int modularAngle = ((int)m_flareFieldOfView) % 360;
					if (modularAngle == 0)
						m_light.m_LensOpticsFrustumAngle = 255;
					else
						m_light.m_LensOpticsFrustumAngle = (uint8)(m_flareFieldOfView * (255.0f / 360.0f));
				}
				else
				{
					m_light.m_LensOpticsFrustumAngle = 0;
				}
			}
			else
			{
				m_light.SetLensOpticsElement(nullptr);
			}
		}
		else
		{
			CryWarning(VALIDATOR_MODULE_ENTITYSYSTEM, VALIDATOR_ERROR, "Flare lens optics %s in light %s doesn't exist!", m_flareTexturePath.c_str(), GetEntity()->GetName());
			m_light.SetLensOpticsElement(nullptr);
		}
	}

	if (!(m_light.m_Flags & DLF_AREA_LIGHT) && m_light.m_fLightFrustumAngle && (m_light.m_pLightImage != NULL) && m_light.m_pLightImage->IsTextureLoaded() || m_light.m_pLightDynTexSource)
	{
		m_light.m_Flags |= DLF_PROJECT;
	}
	else
	{
		SAFE_RELEASE(m_light.m_pLightImage);
		SAFE_RELEASE(m_light.m_pLightDynTexSource);
		m_light.m_Flags |= DLF_POINT;
	}

	// Load the light source into the entity
	m_slot = entity.LoadLight(1, &m_light);
}


void CDynamicLightComponent::SetLocalTM(Matrix34 localMatrix)
{
	GetEntity()->SetSlotLocalTM(m_slot, localMatrix);
}


//void CDynamicLightComponent::SerializeProperties(Serialization::IArchive& archive)
//{
//	archive(m_isActive, "Active", "Active");
//	archive(m_light.m_fRadius, "Radius", "Radius");
//	archive(m_light.m_fAttenuationBulbSize, "AttenuationBulbSize", "AttenuationBulbSize");
//
//	if (archive.openBlock("Color", "Color"))
//	{
//		archive(m_diffuseColor, "DiffuseColor", "DiffuseColor");
//		archive(m_diffuseMultiplier, "DiffuseMultiplier", "DiffuseMultiplier");
//
//		archive.closeBlock();
//	}
//
//	if (archive.openBlock("Options", "Options"))
//	{
//		archive(m_bIgnoreVisAreas, "IgnoreVisAreas", "IgnoreVisAreas");
//		archive(m_bAffectsThisAreaOnly, "AffectsThisAreaOnly", "AffectsThisAreaOnly");
//		archive(m_bAmbient, "Ambient", "Ambient");
//		archive(m_bFake, "FakeLight", "FakeLight");
//		archive(m_bAffectVolumetricFog, "AffectVolumetricFog", "AffectVolumetricFog");
//		archive(m_bAffectVolumetricFogOnly, "AffectVolumetricFogOnly", "AffectVolumetricFogOnly");
//		archive(m_light.m_fFogRadialLobe, "FogRadialLobe", "FogRadialLobe");
//
//		archive.closeBlock();
//	}
//
//	if (archive.openBlock("Shadows", "Shadows"))
//	{
//		archive(m_castShadowSpec, "CastShadows", "CastShadows");
//
//		archive.closeBlock();
//	}
//
//	if (archive.openBlock("Animation", "Animation"))
//	{
//		archive(m_light.m_nLightStyle, "Style", "Style");
//		archive(m_animSpeed, "Speed", "Speed");
//
//		archive.closeBlock();
//	}
//
//	if (archive.openBlock("Projector", "Projector"))
//	{
//		archive(Serialization::TextureFilename(m_projectorTexturePath), "ProjectionTexture", "ProjectionTexture");
//		archive(m_light.m_fLightFrustumAngle, "ProjectionFieldOfView", "ProjectionFieldOfView");
//		archive(m_light.m_fProjectorNearPlane, "ProjectionNearPlane", "ProjectionNearPlane");
//
//		archive.closeBlock();
//	}
//
//	if (archive.openBlock("Flare", "Flare"))
//	{
//		archive(Serialization::TextureFilename(m_flareTexturePath), "FlareTextures", "FlareTextures");
//		archive(m_flareFieldOfView, "FlareFieldOfView", "FlareFieldOfView");
//
//		archive.closeBlock();
//	}
//
//	if (archive.isInput())
//	{
//		OnResetState();
//	}
//}
}