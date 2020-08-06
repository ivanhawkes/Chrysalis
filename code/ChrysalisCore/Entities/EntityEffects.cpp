#include <StdAfx.h>

#include "EntityEffects.h"
#include <CryParticleSystem/ParticleParams.h>
#include <CryAnimation/ICryAnimation.h>


namespace Chrysalis
{
// ***
// *** Particle effects.
// ***

IParticleEmitter* SpawnParticleFX(const char* effectName, const SEffectSpawnParams& spawnParams, const char* requester /*= nullptr*/)
{
	IParticleEffect* pParticleEffect = gEnv->pParticleManager->FindEffect(effectName, requester ? requester : "");

	return SpawnParticleFX(pParticleEffect, spawnParams);
}


IParticleEmitter* SpawnParticleFX(IParticleEffect* pParticleEffect, const SEffectSpawnParams& spawnParams)
{
	if (pParticleEffect)
	{
		SpawnParams sp;
		sp.bPrime = spawnParams.isPrime;
		IParticleEmitter* pEmitter = pParticleEffect->Spawn(ParticleLoc(spawnParams.position, spawnParams.direction, spawnParams.scale), &sp);

		if (spawnParams.speed > 0.0f)
		{
			ParticleParams particleParams = pParticleEffect->GetParticleParams();
			particleParams.fSpeed = spawnParams.speed;
			pParticleEffect->SetParticleParams(particleParams);
		}

		return pEmitter;
	}

	return nullptr;
}


void SpawnParticleWithEntity(const IEntity* pTargetEntity, const int targetSlot, const char* effectName, const char* helperName, const SEffectSpawnParams& spawnParams)
{
	IParticleEffect* pParticleEffect = gEnv->pParticleManager->FindEffect(effectName);

	SpawnParticleWithEntity(pTargetEntity, targetSlot, pParticleEffect, helperName, spawnParams);
}


void SpawnParticleWithEntity(const IEntity* pTargetEntity, const int targetSlot, IParticleEffect* pParticleEffect, const char* helperName, const SEffectSpawnParams& spawnParams)
{
	SEffectSpawnParams newSpawnParams = spawnParams;

	if (pTargetEntity)
	{
		SEntitySlotInfo slotInfo;
		if (pTargetEntity->GetSlotInfo(targetSlot, slotInfo))
		{
			if (slotInfo.pStatObj)
			{
				// Get helper position from static object.
				const Vec3 localHelperPosition = slotInfo.pStatObj->GetHelperPos(helperName);
				newSpawnParams.position = pTargetEntity->GetSlotWorldTM(targetSlot).TransformPoint(localHelperPosition);
			}
			else if (slotInfo.pCharacter)
			{
				// Get helper position from character.
				IAttachmentManager* pAttachmentManager = slotInfo.pCharacter->GetIAttachmentManager();

				if (IAttachment* pAttachment = pAttachmentManager->GetInterfaceByName(helperName))
				{
					newSpawnParams.position = pAttachment->GetAttWorldAbsolute().t;
				}
				else
				{
					IDefaultSkeleton& rIDefaultSkeleton = slotInfo.pCharacter->GetIDefaultSkeleton();
					ISkeletonPose* pSkeletonPose = slotInfo.pCharacter->GetISkeletonPose();

					Vec3 localJointPosition = ZERO;
					const int16 jointId = rIDefaultSkeleton.GetJointIDByName(helperName);
					if (jointId >= 0)
					{
						localJointPosition = pSkeletonPose->GetAbsJointByID(jointId).t;
					}

					newSpawnParams.position = pTargetEntity->GetSlotWorldTM(targetSlot).TransformPoint(localJointPosition);
				}
			}
		}
	}

	SpawnParticleFX(pParticleEffect, newSpawnParams);
}


// ***
// *** 
// ***

CEffectsController::CEffectsController()
	: m_effectGeneratorId(0)
{
}


void CEffectsController::Init(EntityId entityId)
{
	CRY_ASSERT_MESSAGE(gEnv->pEntitySystem->GetEntity(entityId), "Init Effect controller with nullptr entity, this will crash!");
	m_ownerEntityId = entityId;
}


void CEffectsController::FreeAllEffects()
{
	int index = (int)m_attachedEffects.size() - 1;

	while (index >= 0)
	{
		CRY_ASSERT(index < (int)m_attachedEffects.size());

		DetachEffect(m_attachedEffects[index].id);
		index--;
	}

	stl::free_container(m_attachedEffects);
	m_effectGeneratorId = 0;
}


int CEffectsController::FindSafeSlot(int firstSafeSlot)
{
	int i = -1;
	if (firstSafeSlot >= 0)
	{
		SEntitySlotInfo dummy;
		i = firstSafeSlot;
		auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
		while (pOwnerEntity->GetSlotInfo(i, dummy))
		{
			i++;
		}
	}

	return i;
}


TAttachedEffectId CEffectsController::AttachParticleEffect(IParticleEffect* pParticleEffect, const SEffectAttachParams& attachParams)
{
	if (pParticleEffect)
	{
		SEffectInfo effectInfo;
		int attachSlot = FindSafeSlot(attachParams.firstSafeSlot);

		auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
		CRY_ASSERT(pOwnerEntity);

		// Offset particle to desired location.
		effectInfo.entityEffectSlot = pOwnerEntity->LoadParticleEmitter(attachSlot, pParticleEffect, 0, attachParams.isPrime, false);
		Matrix34 localEffectMtx(IParticleEffect::ParticleLoc(attachParams.offset, attachParams.direction, attachParams.scale));
		pOwnerEntity->SetSlotLocalTM(effectInfo.entityEffectSlot, localEffectMtx);

		++m_effectGeneratorId;
		effectInfo.id = m_effectGeneratorId;
		m_attachedEffects.push_back(effectInfo);

		return m_effectGeneratorId;
	}

	return EFFECTID_INVALID;
}


TAttachedEffectId CEffectsController::AttachParticleEffect(const char* effectName, const SEffectAttachParams& attachParams)
{
	auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
	CRY_ASSERT(pOwnerEntity);

	IParticleEffect* pParticleEffect = gEnv->pParticleManager->FindEffect(effectName);

	return AttachParticleEffect(pParticleEffect, attachParams);
}


TAttachedEffectId CEffectsController::AttachParticleEffect(IParticleEffect* pParticleEffect, const int targetSlot, const char* helperName, const SEffectAttachParams& attachParams)
{
	auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
	CRY_ASSERT(pOwnerEntity);

	if (pParticleEffect)
	{
		SEntitySlotInfo slotInfo;
		SEffectInfo effectInfo;

		const bool validSlot = pOwnerEntity->GetSlotInfo(targetSlot, slotInfo);

		if (!validSlot || slotInfo.pStatObj)
		{
			// Get helper position on static object (if any).
			Vec3 localHelperPosition = attachParams.offset;
			if (validSlot)
			{
				const Matrix34& localSlotMtx = pOwnerEntity->GetSlotLocalTM(targetSlot, false);

				localHelperPosition = slotInfo.pStatObj->GetHelperPos(helperName) + attachParams.offset;
				localHelperPosition = localSlotMtx.TransformPoint(localHelperPosition);
			}

			int attachSlot = FindSafeSlot(attachParams.firstSafeSlot);

			// Offset particle to desired location. 
			effectInfo.entityEffectSlot = pOwnerEntity->LoadParticleEmitter(attachSlot, pParticleEffect, 0, attachParams.isPrime, false);
			Matrix34 localEffectMtx(IParticleEffect::ParticleLoc(localHelperPosition, attachParams.direction, attachParams.scale));
			pOwnerEntity->SetSlotLocalTM(effectInfo.entityEffectSlot, localEffectMtx);

			++m_effectGeneratorId;
			effectInfo.id = m_effectGeneratorId;
			m_attachedEffects.push_back(effectInfo);

			return m_effectGeneratorId;
		}
		else if (slotInfo.pCharacter)
		{
			IAttachmentManager* pAttachmentManager = slotInfo.pCharacter->GetIAttachmentManager();
			IAttachment* pAttachment = pAttachmentManager->GetInterfaceByName(helperName);

			if (pAttachment)
			{
				CEffectAttachment* pEffectAttachment = new CEffectAttachment(pParticleEffect, attachParams.offset, attachParams.direction, attachParams.scale, attachParams.isPrime);
				pAttachment->AddBinding(pEffectAttachment);
				pEffectAttachment->ProcessAttachment(pAttachment);
			}
			else
			{
				GameWarning("[EntityEffects] Can not attach '%s' to entity '%s', attachment point helper '%s' does not exist", pParticleEffect->GetName(), pOwnerEntity->GetName(), helperName);
				return EFFECTID_INVALID;
			}

			++m_effectGeneratorId;
			effectInfo.id = m_effectGeneratorId;
			effectInfo.characterEffectSlot = targetSlot;
			effectInfo.helperName = helperName;
			m_attachedEffects.push_back(effectInfo);

			return m_effectGeneratorId;
		}
	}

	return EFFECTID_INVALID;
}


TAttachedEffectId CEffectsController::AttachParticleEffect(const char* effectName, const int targetSlot, const char* helperName, const SEffectAttachParams& attachParams)
{
	auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
	CRY_ASSERT(pOwnerEntity);

	IParticleEffect* pParticleEffect = gEnv->pParticleManager->FindEffect(effectName);

	return AttachParticleEffect(pParticleEffect, targetSlot, helperName, attachParams);
}


TAttachedEffectId CEffectsController::AttachLight(const int targetSlot, const char* helperName, Vec3 offset, Vec3 direction, eGeometrySlot firstSafeSlot,
	const ECS::RenderLight& renderLight)
{
	auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
	CRY_ASSERT(pOwnerEntity);

	SRenderLight light;

	light.m_nLightStyle = renderLight.animations.m_style;
	light.SetAnimSpeed(renderLight.animations.m_speed);

	light.SetPosition(ZERO);
	light.m_Flags = DLF_DEFERRED_LIGHT | DLF_PROJECT;

	light.m_fLightFrustumAngle = renderLight.fovAngle.ToDegrees();
	light.m_fProjectorNearPlane = renderLight.projectorOptions.m_nearPlane;

	light.SetLightColor(renderLight.color.m_color * renderLight.color.m_diffuseMultiplier);
	light.SetSpecularMult(renderLight.color.m_specularMultiplier);

	light.m_fHDRDynamic = 0.f;

	if (renderLight.options.m_bAffectsOnlyThisArea)
		light.m_Flags |= DLF_THIS_AREA_ONLY;

	if (renderLight.options.m_bIgnoreVisAreas)
		light.m_Flags |= DLF_IGNORES_VISAREAS;

	if (renderLight.options.m_bVolumetricFogOnly)
		light.m_Flags |= DLF_VOLUMETRIC_FOG_ONLY;

	if (renderLight.options.m_bAffectsVolumetricFog)
		light.m_Flags |= DLF_VOLUMETRIC_FOG;

	if (renderLight.options.m_bLinkToSkyColor)
		light.m_Flags |= DLF_LINK_TO_SKY_COLOR;

	if (renderLight.options.m_bAmbient)
		light.m_Flags |= DLF_AMBIENT;

	// TODO: Automatically add DLF_FAKE when using beams or flares.

	bool shouldCastShadows = false;
	if (renderLight.shadows.m_castShadowSpec != Cry::DefaultComponents::EMiniumSystemSpec::Disabled)
	{
		const int sysSpec = gEnv->pSystem->GetConfigSpec();
		if (sysSpec != CONFIG_CUSTOM)
		{
			shouldCastShadows = sysSpec >= static_cast<int>(renderLight.shadows.m_castShadowSpec);
		}
		else
		{
			if (ICVar* const pSysSpecShadow = gEnv->pConsole->GetCVar("sys_spec_shadow"))
				shouldCastShadows = pSysSpecShadow->GetIVal() >= static_cast<int>(renderLight.shadows.m_castShadowSpec);
		}
	}

	if (shouldCastShadows)
	{
		light.m_Flags |= DLF_CASTSHADOW_MAPS;

		light.SetShadowBiasParams(renderLight.shadows.m_shadowBias, renderLight.shadows.m_shadowSlopeBias);
		light.m_fShadowUpdateMinRadius = light.m_fRadius;

		float shadowUpdateRatio = 1.f;
		light.m_nShadowUpdateRatio = max((uint16)1, (uint16)(shadowUpdateRatio * (1 << DL_SHADOW_UPDATE_SHIFT)));
	}
	else
	{
		light.m_Flags &= ~DLF_CASTSHADOW_MAPS;
	}

	light.SetRadius(renderLight.radius, renderLight.options.m_attenuationBulbSize);

	light.m_fFogRadialLobe = renderLight.options.m_fogRadialLobe;

	const char* szProjectorTexturePath = renderLight.projectorOptions.GetTexturePath();
	if (szProjectorTexturePath[0] == '\0')
	{
		szProjectorTexturePath = "%ENGINE%/EngineAssets/Textures/lights/softedge.dds";
	}

	const char* pExt = PathUtil::GetExt(szProjectorTexturePath);
	if (!stricmp(pExt, "swf") || !stricmp(pExt, "gfx") || !stricmp(pExt, "usm") || !stricmp(pExt, "ui"))
	{
		light.m_pLightDynTexSource = gEnv->pRenderer->EF_LoadDynTexture(szProjectorTexturePath, false);
	}
	else
	{
		light.m_pLightImage = gEnv->pRenderer->EF_LoadTexture(szProjectorTexturePath, 0);
	}

	if ((light.m_pLightImage == nullptr || !light.m_pLightImage->IsTextureLoaded()) && light.m_pLightDynTexSource == nullptr)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Light projector texture %s not found, disabling projector component for entity %s", szProjectorTexturePath, pOwnerEntity->GetName());
		return EFFECTID_INVALID;
	}

	if (renderLight.flare.HasTexturePath())
	{
		int nLensOpticsId;

		if (gEnv->pOpticsManager->Load(renderLight.flare.GetTexturePath(), nLensOpticsId))
		{
			IOpticsElementBase* pOptics = gEnv->pOpticsManager->GetOptics(nLensOpticsId);
			CRY_ASSERT(pOptics != nullptr);

			if (pOptics != nullptr)
			{
				light.SetLensOpticsElement(pOptics);

				float flareAngle = renderLight.flare.m_angle.ToDegrees();

				if (flareAngle != 0)
				{
					int modularAngle = ((int)flareAngle) % 360;
					if (modularAngle == 0)
						light.m_LensOpticsFrustumAngle = 255;
					else
						light.m_LensOpticsFrustumAngle = (uint8)(flareAngle * (255.0f / 360.0f));
				}
				else
				{
					light.m_LensOpticsFrustumAngle = 0;
				}
			}
		}
		else
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Flare lens optics %s for projector component in entity %s doesn't exist!", renderLight.flare.GetTexturePath(), pOwnerEntity->GetName());
			light.SetLensOpticsElement(nullptr);
		}
	}

	if (renderLight.optics.m_flareEnable && !renderLight.optics.m_lensFlareName.empty())
	{
		int32 opticsIndex = 0;
		if (gEnv->pOpticsManager->Load(renderLight.optics.m_lensFlareName.c_str(), opticsIndex))
		{
			IOpticsElementBase* pOpticsElement = gEnv->pOpticsManager->GetOptics(opticsIndex);
			light.SetLensOpticsElement(pOpticsElement);

			const int32 modularAngle = renderLight.optics.m_flareFOV % 360;
			if (modularAngle == 0)
				light.m_LensOpticsFrustumAngle = 255;
			else
				light.m_LensOpticsFrustumAngle = (uint8)(renderLight.optics.m_flareFOV * (255.0f / 360.0f));

			if (renderLight.optics.m_attachToSun)
			{
				light.m_Flags |= DLF_ATTACH_TO_SUN | DLF_FAKE | DLF_IGNORES_VISAREAS;
				light.m_Flags &= ~DLF_THIS_AREA_ONLY;
			}
		}
	}

	pOwnerEntity->UpdateLightClipBounds(light);

	// Need a custom material?
	IMaterial* pMaterial = nullptr;
	if (renderLight.effectSlotMaterial.value.length() > 0)
	{
		pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(renderLight.effectSlotMaterial.value);
	}

	SEntitySlotInfo slotInfo;
	SEffectInfo effectInfo;

	const bool validSlot = pOwnerEntity->GetSlotInfo(targetSlot, slotInfo);

	if (!validSlot || slotInfo.pStatObj)
	{
		// Get helper position on static object (if any).
		Vec3 helperPos(ZERO);
		Vec3 localHelperPosition = offset;

		if (validSlot)
		{
			helperPos = slotInfo.pStatObj->GetHelperPos(helperName);

			if (helperPos.IsZero())
			{
				const int childCount = pOwnerEntity->GetChildCount();

				for (int i = 0; i < childCount; ++i)
				{
					if (IEntity* pChild = pOwnerEntity->GetChild(i))
					{
						if (IStatObj* statObj = pChild->GetStatObj(targetSlot))
						{
							helperPos = statObj->GetHelperPos(helperName);

							if (!helperPos.IsZero())
							{
								helperPos += pChild->GetPos();
								break;
							}
						}
					}
				}
			}

			localHelperPosition = helperPos + offset;
			localHelperPosition = pOwnerEntity->GetSlotLocalTM(targetSlot, false).TransformPoint(localHelperPosition);
		}

		int attachSlot = FindSafeSlot(firstSafeSlot);

		++m_effectGeneratorId;
		effectInfo.id = m_effectGeneratorId;
		effectInfo.entityEffectSlot = pOwnerEntity->LoadLight(attachSlot, &light);

		if ((effectInfo.entityEffectSlot >= 0) && pMaterial)
		{
			pOwnerEntity->SetSlotMaterial(effectInfo.entityEffectSlot, pMaterial);
		}

		// HACK: I am applying a rotation to orient the light down the Y axis instead of the engine system of the X axis.
		//Matrix34 localEffectMtx = Matrix34(Matrix33::CreateRotationVDir(direction));
		//Matrix34 localEffectMtx = Matrix34(Matrix33::CreateRotationVDir(direction * Matrix33::CreateRotationZ(gf_PI * -0.5f)));
		Matrix34 localEffectMtx = Matrix34(Matrix33::CreateRotationVDir(direction));
		localEffectMtx.SetTranslation(localHelperPosition);
		pOwnerEntity->SetSlotLocalTM(effectInfo.entityEffectSlot, localEffectMtx);


		m_attachedEffects.push_back(effectInfo);

		return m_effectGeneratorId;
	}
	else if (slotInfo.pCharacter)
	{
		IAttachmentManager* pAttachmentManager = slotInfo.pCharacter->GetIAttachmentManager();
		IAttachment* pAttachment = pAttachmentManager->GetInterfaceByName(helperName);

		if (pAttachment)
		{
			CLightAttachment* pLightAttachment = new CLightAttachment();
			pLightAttachment->LoadLight(light);

			ILightSource* pLightSource = pLightAttachment->GetLightSource();
			if (pLightSource)
			{
				pLightSource->SetMaterial(pMaterial);
				// #TODO: Find out what this does and how to get it.
				// pLightSource->SetCastingException(renderLight.pCasterException);
			}
			pAttachment->AddBinding(pLightAttachment);

			const bool customOffset = (offset != Vec3Constants<float>::fVec3_Zero) || (direction != Vec3Constants<float>::fVec3_OneY);
			if (customOffset)
			{
				// HACK: I am applying a rotation to orient the light down the Y axis instead of the engine system of the X axis.
				//pAttachment->SetAttRelativeDefault(QuatT(Quat::CreateRotationVDir(direction), offset));
				//pAttachment->SetAttRelativeDefault(QuatT(Quat::CreateRotationVDir(direction * Matrix33::CreateRotationZ(gf_PI * -0.5f)), offset));
				pAttachment->SetAttRelativeDefault(QuatT(Quat::CreateRotationVDir(direction), offset));
			}
		}
		else
		{
			GameWarning("[EntityEffects] Entity '%s' trying to attach light to attachment '%s' which does not exist!", pOwnerEntity->GetName(), helperName);
			return EFFECTID_INVALID;
		}

		++m_effectGeneratorId;
		effectInfo.id = m_effectGeneratorId;
		effectInfo.helperName = helperName;
		effectInfo.characterEffectSlot = targetSlot;

		m_attachedEffects.push_back(effectInfo);

		return m_effectGeneratorId;
	}

	return EFFECTID_INVALID;
}


void CEffectsController::DetachEffect(const TAttachedEffectId effectId)
{
	auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
	CRY_ASSERT(pOwnerEntity);

	TAttachedEffects::iterator effectIt = std::find(m_attachedEffects.begin(), m_attachedEffects.end(), effectId);

	if (effectIt != m_attachedEffects.end())
	{
		const SEffectInfo& effectInfo = *effectIt;

		if (effectInfo.entityEffectSlot >= 0)
		{
			pOwnerEntity->FreeSlot(effectInfo.entityEffectSlot);
		}
		else
		{
			ICharacterInstance* pCharacter = pOwnerEntity->GetCharacter(effectInfo.characterEffectSlot);
			if (pCharacter)
			{
				IAttachmentManager* pAttachmentManager = pCharacter->GetIAttachmentManager();
				IAttachment* pAttachment = pAttachmentManager->GetInterfaceByName(effectInfo.helperName.c_str());
				if (pAttachment)
				{
					pAttachment->ClearBinding();
				}
			}
		}

		m_attachedEffects.erase(effectIt);
	}
}


IParticleEmitter* CEffectsController::GetEffectEmitter(const TAttachedEffectId effectId) const
{
	auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
	CRY_ASSERT(pOwnerEntity);

	TAttachedEffects::const_iterator effectCit = std::find(m_attachedEffects.begin(), m_attachedEffects.end(), effectId);

	if (effectCit != m_attachedEffects.end())
	{
		const SEffectInfo& effectInfo = *effectCit;

		if (effectInfo.entityEffectSlot >= 0)
		{
			SEntitySlotInfo slotInfo;
			if (pOwnerEntity->GetSlotInfo(effectInfo.entityEffectSlot, slotInfo) && slotInfo.pParticleEmitter)
			{
				return slotInfo.pParticleEmitter;
			}
		}

		if (effectInfo.characterEffectSlot >= 0)
		{
			SEntitySlotInfo slotInfo;
			if (pOwnerEntity->GetSlotInfo(effectInfo.characterEffectSlot, slotInfo) && slotInfo.pCharacter)
			{
				IAttachmentManager* pAttachmentManager = slotInfo.pCharacter->GetIAttachmentManager();
				IAttachment* pAttachment = pAttachmentManager->GetInterfaceByName(effectInfo.helperName.c_str());
				if (pAttachment)
				{
					IAttachmentObject* pAttachmentObject = pAttachment->GetIAttachmentObject();
					if (pAttachmentObject != nullptr && (pAttachmentObject->GetAttachmentType() == IAttachmentObject::eAttachment_Effect))
					{
						return static_cast<CEffectAttachment*>(pAttachmentObject)->GetEmitter();
					}
				}
			}
		}
	}

	return nullptr;
}


ILightSource* CEffectsController::GetLightSource(const TAttachedEffectId effectId) const
{
	auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
	CRY_ASSERT(pOwnerEntity);

	TAttachedEffects::const_iterator effectCit = std::find(m_attachedEffects.begin(), m_attachedEffects.end(), effectId);

	if (effectCit != m_attachedEffects.end())
	{
		const SEffectInfo& effectInfo = *effectCit;

		if (effectInfo.entityEffectSlot >= 0)
		{
			SEntitySlotInfo slotInfo;
			if (pOwnerEntity->GetSlotInfo(effectInfo.entityEffectSlot, slotInfo) && slotInfo.pLight)
			{
				return slotInfo.pLight;
			}
		}

		if (effectInfo.characterEffectSlot >= 0)
		{
			SEntitySlotInfo slotInfo;
			if (pOwnerEntity->GetSlotInfo(effectInfo.characterEffectSlot, slotInfo) && slotInfo.pCharacter)
			{
				IAttachmentManager* pAttachmentManager = slotInfo.pCharacter->GetIAttachmentManager();
				IAttachment* pAttachment = pAttachmentManager->GetInterfaceByName(effectInfo.helperName.c_str());
				if (pAttachment)
				{
					IAttachmentObject* pAttachmentObject = pAttachment->GetIAttachmentObject();
					if (pAttachmentObject != nullptr && (pAttachmentObject->GetAttachmentType() == IAttachmentObject::eAttachment_Light))
					{
						return static_cast<CLightAttachment*>(pAttachmentObject)->GetLightSource();
					}
				}
			}
		}
	}

	return nullptr;
}


void CEffectsController::GetMemoryStatistics(ICrySizer* pSizer) const
{
	pSizer->AddContainer(m_attachedEffects);
}


void CEffectsController::SetEffectWorldTM(const TAttachedEffectId effectId, const Matrix34& effectWorldTM)
{
	auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
	CRY_ASSERT(pOwnerEntity);

	TAttachedEffects::const_iterator effectCit = std::find(m_attachedEffects.begin(), m_attachedEffects.end(), effectId);

	if (effectCit != m_attachedEffects.end())
	{
		const SEffectInfo& effectInfo = *effectCit;
		SEntitySlotInfo slotInfo;

		if (effectInfo.entityEffectSlot >= 0)
		{
			if (pOwnerEntity->GetSlotInfo(effectInfo.entityEffectSlot, slotInfo) && (slotInfo.pParticleEmitter || slotInfo.pLight))
			{
				const Matrix34& worldMatrix = pOwnerEntity->GetWorldTM();
				Matrix34 localMatrix = worldMatrix.GetInverted() * effectWorldTM;

				pOwnerEntity->SetSlotLocalTM(effectInfo.entityEffectSlot, localMatrix);
			}
		}
	}
}


void CEffectsController::UpdateEntitySlotEffectLocationsFromHelpers()
{
	const int numEffects = m_attachedEffects.size();

	auto pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerEntityId);
	for (int i = 0; i < numEffects; ++i)
	{
		SEffectInfo& effectInfo = m_attachedEffects[i];

		if (effectInfo.entityEffectSlot >= 0 && effectInfo.characterEffectSlot >= 0 && !effectInfo.helperName.empty())
		{
			SEntitySlotInfo slotInfo;
			pOwnerEntity->GetSlotInfo(effectInfo.characterEffectSlot, slotInfo);
			if (slotInfo.pStatObj)
			{
				Matrix34 localMatrix = pOwnerEntity->GetSlotLocalTM(effectInfo.characterEffectSlot, false) * slotInfo.pStatObj->GetHelperTM(effectInfo.helperName.c_str());
				pOwnerEntity->SetSlotLocalTM(effectInfo.entityEffectSlot, localMatrix);
			}
		}
	}
}
}