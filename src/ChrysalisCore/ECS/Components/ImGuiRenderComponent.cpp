#include <StdAfx.h>

#include "ImGuiRenderComponent.h"


namespace Chrysalis::ECS
{
#ifdef IMGUI

// Attributes.

void ImGuiRenderComponent(AttributeType ar)
{
	ImGui::InputFloat("Base:", &ar.base);
	ImGui::InputFloat("Base Modifiers:", &ar.baseModifiers);
	ImGui::InputFloat("Modifiers:", &ar.modifiers);
}


// Name and prototypes for references.

void ImGuiRenderComponent(Name ar)
{
	ImGui::Text(ar.name);
}


void ImGuiRenderComponent(Prototype ar)
{
	// TODO: Need a better way to handle this.
	int32_t protoId = to_underlying(ar.prototypeEntityId);
	ImGui::InputInt("Prototype Id:", &protoId);
	ar.prototypeEntityId = entt::entity {protoId};
}


// Source and target entitiy Ids.

void ImGuiRenderComponent(SourceEntity ar)
{
	// TODO: Need a better way to handle this.
	int32_t sourceId = to_underlying(ar.sourceEntityId);
	ImGui::Text("Source Id: %u", sourceId);
	ImGui::Text("Cry Source Id: %u", ar.crySourceEntityId);

}


void ImGuiRenderComponent(TargetEntity ar)
{
	// TODO: Need conversion from the base entity type.
	int32_t targetId = to_underlying(ar.targetEntityId);
	ImGui::Text("Target Id: %u", targetId);
	ImGui::Text("Cry Target Id: %u", ar.cryTargetEntityId);
}


// Health.

void ImGuiRenderComponent(Health ar)
{
	ImGuiRenderComponent(ar.health);
	ImGui::Checkbox("Dead?:", &ar.isDead);
	ImGui::Checkbox("Immortal:", &ar.isImmortal);
}


void ImGuiRenderComponent(Damage ar)
{
	ImGui::InputFloat("Quantity:", &ar.quantity);
}


void ImGuiRenderComponent(DamageOverTime ar)
{
	ImGui::InputFloat("Quantity:", &ar.quantity);
	ImGui::InputFloat("Duration:", &ar.duration);
	ImGui::InputFloat("Interval:", &ar.interval);
}


void ImGuiRenderComponent(SelfHarm ar)
{
	ImGui::InputFloat("Quantity:", &ar.quantity);
}


void ImGuiRenderComponent(Heal ar)
{
	ImGui::InputFloat("Quantity:", &ar.quantity);
}


void ImGuiRenderComponent(HealOverTime ar)
{
	ImGui::InputFloat("Quantity:", &ar.quantity);
	ImGui::InputFloat("Duration:", &ar.duration);
	ImGui::InputFloat("Interval:", &ar.interval);
}


// Qi.

void ImGuiRenderComponent(Qi ar)
{
	ImGuiRenderComponent(ar.qi);
}


void ImGuiRenderComponent(UtiliseQi ar)
{
	ImGui::InputFloat("Quantity:", &ar.quantity);
}


void ImGuiRenderComponent(UtiliseQiOverTime ar)
{
	ImGui::InputFloat("Quantity:", &ar.quantity);
	ImGui::InputFloat("Duration:", &ar.duration);
	ImGui::InputFloat("Interval:", &ar.interval);
}


void ImGuiRenderComponent(ReplenishQi ar)
{
	ImGui::InputFloat("Quantity:", &ar.quantity);
}


void ImGuiRenderComponent(ReplenishQiOverTime ar)
{
	ImGui::InputFloat("Quantity:", &ar.quantity);
	ImGui::InputFloat("Duration:", &ar.duration);
	ImGui::InputFloat("Interval:", &ar.interval);
}


// Spellcasts.

void ImGuiRenderComponent(Spell ar)
{
	for (auto& prototypeId : ar.fragments)
	{
		ImGuiRenderComponent(prototypeId);
	}
}


void ImGuiRenderComponent(SpellFragment ar)
{
}


void ImGuiRenderComponent(SpellTargetType ar)
{
}


void ImGuiRenderComponent(SpellTargetAggressionType ar)
{
}


void ImGuiRenderComponent(SpellcastExecution ar)
{
	ImGui::InputFloat("Execution Time:", &ar.executionTime);
}


void ImGuiRenderComponent(SpellCastDuration ar)
{
	ImGui::InputFloat("Value:", &ar.value);
}


// Spell actions.

void ImGuiRenderComponent(SpellActionSchematyc ar)
{
}


void ImGuiRenderComponent(SpellActionDRS ar)
{
}


void ImGuiRenderComponent(SpellActionInspect ar)
{
}


void ImGuiRenderComponent(SpellActionExamine ar)
{
}


void ImGuiRenderComponent(SpellActionTake ar)
{
}


void ImGuiRenderComponent(SpellActionDrop ar)
{
}


void ImGuiRenderComponent(SpellActionThrow ar)
{
}


void ImGuiRenderComponent(SpellActionSwitch ar)
{
}


void ImGuiRenderComponent(SpellActionOpen ar)
{
}


void ImGuiRenderComponent(SpellActionClose ar)
{
}


void ImGuiRenderComponent(SpellActionUnlock ar)
{
}


void ImGuiRenderComponent(SpellActionLock ar)
{
}


// Environment.

void ImGuiRenderComponent(RenderLight ar)
{
	ImGui::Text("Optics");
	//ar(desc.m_lensFlareName, "lensFlareName", "Diffuse color expressed as RGB e.g. 128, 255, 128.");
	ImGui::Checkbox("Attach to the Sun:", &ar.optics.m_attachToSun);
	ImGui::Checkbox("Flare Enable:", &ar.optics.m_flareEnable);
	ImGui::InputInt("Flare FoV:", &ar.optics.m_flareFOV);
	ImGui::Separator();

	ImGui::Text("Options");
	//Schematyc::Range<0, 64000> m_attenuationBulbSize = SRenderLight().m_fAttenuationBulbSize;
	ImGui::Checkbox("Ignore Vis Areas:", &ar.options.m_bIgnoreVisAreas);
	ImGui::Checkbox("Volumetric Fog Only:", &ar.options.m_bVolumetricFogOnly);
	ImGui::Checkbox("Affects Volumetric Fog:", &ar.options.m_bAffectsVolumetricFog);
	ImGui::Checkbox("Affects Only This Area:", &ar.options.m_bAffectsOnlyThisArea);
	ImGui::Checkbox("Link To Sky Color:", &ar.options.m_bLinkToSkyColor);
	ImGui::Checkbox("Ambient:", &ar.options.m_bAmbient);
	//ar(desc.m_fogRadialLobe, "fogRadialLobe", "Fog Radial Lobe");
	//ar(desc.m_giMode, "GIMode", "Global Illumination");
	ImGui::Separator();

	ImGui::Text("Color");
	ImGui::InputFloat("Diffuse Multiplier:", &ar.color.m_diffuseMultiplier.value);
	ImGui::InputFloat("Specular Multiplier:", &ar.color.m_specularMultiplier.value);
	ImGui::Separator();

	ImGui::Text("Shadows");
	//ar(desc.m_castShadowSpec, "shadowSpec", "Minimum Shadow Graphics");
	ImGui::InputFloat("Shadow Bias:", &ar.shadows.m_shadowBias);
	ImGui::InputFloat("Shadow Slope Bias:", &ar.shadows.m_shadowSlopeBias);
	ImGui::InputFloat("Shadow Resolution Scale:", &ar.shadows.m_shadowResolutionScale);
	ImGui::InputFloat("Shadow UpdateMin Radius:", &ar.shadows.m_shadowUpdateMinRadius);
	ImGui::InputInt("Shadow Min Resolution:", &ar.shadows.m_shadowMinResolution);
	ImGui::InputInt("Shadow Update Ratio:", &ar.shadows.m_shadowUpdateRatio);
	ImGui::Separator();

	ImGui::Text("Animations");
	ImGui::InputInt("Style:", reinterpret_cast<int*>(&ar.animations.m_style));
	ImGui::InputFloat("Speed:", &ar.animations.m_speed);
	ImGui::Separator();

	ImGui::Text("Shape");
	//ar(desc.m_areaShape, "shape", "Shape");
	//ar(desc.m_twoSided, "twoSided", "Two sided light contribution");
	//ar(desc.m_texturePath, "texturePath", "Texture path");
	ImGui::InputFloat("Width:", &ar.shape.m_width.value);
	ImGui::InputFloat("Height:", &ar.shape.m_height.value);
	ImGui::Separator();

	ImGui::Text("Projector Options");
	ImGui::InputFloat("Near Plane:", &ar.projectorOptions.m_nearPlane.value);
	//ar(desc.m_texturePath, "texturePath", "Texture Path");
	//ar(desc.m_materialPath, "materialPath", "Material Path");
	ImGui::Separator();

	ImGui::Text("Flare");
	//ar(flare, "flare", "flare");
	ImGui::Separator();

	// Members that are directly on the class.
	ImGui::Text("Misc");
	ImGui::InputFloat("radius:", &ar.radius.value);
	//ImGui::InputFloat("FoV Angle:", &ar.fovAngle.);
	//ImGui::Text(&effectSlotMaterial.value.c_str());
}


// Items.

void ImGuiRenderComponent(ItemClass ar)
{
}


// Range checking.

void ImGuiRenderComponent(Range ar)
{
	ImGui::InputFloat("Minimum:", &ar.minRange);
	ImGui::InputFloat("Maximum:", &ar.maxRange);
}


// Animation components.

void ImGuiRenderComponent(AnimationFragmentSpellCast ar)
{
	ImGui::Text(ar.value);
}


void ImGuiRenderComponent(AnimationFragmentEmote ar)
{
	ImGui::Text(ar.value);
}


void ImGuiRenderComponent(AnimationTag ar)
{
	ImGui::Text(ar.value);
}


// Simple components.

void ImGuiRenderComponent(Timer ar)
{
	ImGui::InputFloat("Value:", &ar.value);
}


void ImGuiRenderComponent(Duration ar)
{
	ImGui::InputFloat("Value:", &ar.value);
}


void ImGuiRenderComponent(Aura ar)
{
}


void ImGuiRenderComponent(Buff ar)
{
}


void ImGuiRenderComponent(Debuff ar)
{
}


void ImGuiRenderComponent(Channelled ar)
{
	ImGui::Checkbox("Value:", &ar.value);
}


void ImGuiRenderComponent(MovementFactor ar)
{
	ImGui::InputFloat("Value:", &ar.value);
}


void ImGuiRenderComponent(CancelOnMovement ar)
{
	ImGui::Checkbox("Value:", &ar.value);
}


void ImGuiRenderComponent(AreaOfEffect ar)
{
	ImGui::InputFloat("Value:", &ar.value);
}


// Crowd control.

void ImGuiRenderComponent(CrowdControlBlind ar)
{
}


void ImGuiRenderComponent(CrowdControlDisarm ar)
{
}


void ImGuiRenderComponent(CrowdControlMovementRestricted ar)
{
}


void ImGuiRenderComponent(CrowdControlRotationRestricted ar)
{
}


void ImGuiRenderComponent(CrowdControlFlee ar)
{
}


void ImGuiRenderComponent(CrowdControlMindControl ar)
{
}


void ImGuiRenderComponent(CrowdControlPull ar)
{
}


void ImGuiRenderComponent(CrowdControlTaunt ar)
{
}


void ImGuiRenderComponent(CrowdControlThrow ar)
{
}


void ImGuiRenderComponent(CrowdControlKockback ar)
{
}


void ImGuiRenderComponent(CrowdControlKnockdown ar)
{
}


void ImGuiRenderComponent(CrowdControlPolymorph ar)
{
}


void ImGuiRenderComponent(CrowdControlSilence ar)
{
}


// Cooldowns.

void ImGuiRenderComponent(Cooldown ar)
{
	ImGui::InputFloat("Value:", &ar.value);
}

#endif
}
