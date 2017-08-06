#include <StdAfx.h>

#include "ItemBaseParameter.h"
#include <GameXmlParamReader.h>
#include <Game/Cache/GameCache.h>
#include <Item/Parameters/ItemParameter.h>


namespace Chrysalis
{
DEFINE_SHARED_PARAMS_TYPE_INFO(SItemBaseParameter)


SItemBaseParameter::SItemBaseParameter()
{
}


SItemBaseParameter::~SItemBaseParameter()
{
}


void SItemBaseParameter::OnResetState()
{
	*this = SItemBaseParameter();
}


bool SItemBaseParameter::Read(const XmlNodeRef& node)
{
	CGameXmlParamReader reader(node);

	// Read the parameters.
	itemClass = reader.ReadParamValue("itemClass");
	displayName = reader.ReadParamValue("displayName");
	reader.ReadParamValue("isSelectable", isSelectable);
	reader.ReadParamValue("isDroppable", isDroppable);
	reader.ReadParamValue("isAutoDroppable", isAutoDroppable);
	reader.ReadParamValue("isPickable", isPickable);
	reader.ReadParamValue("isAutoPickable", isAutoPickable);
	reader.ReadParamValue("isMountable", isMountable);
	reader.ReadParamValue("isUsable", isUsable);
	reader.ReadParamValue("isGiveable", isGiveable);
	reader.ReadParamValue("isUsableUnderWater", isUsableUnderWater);
	reader.ReadParamValue("isConsumable", isConsumable);
	reader.ReadParamValue("mass", mass);
	reader.ReadParamValue("dropImpulse", dropImpulse);
	reader.ReadParamValue("shouldRemoveOnDrop", shouldRemoveOnDrop);
	reader.ReadParamValue("selectTimeMultiplier", selectTimeMultiplier);
	reader.ReadParamValue("isWeapon", isWeapon);
	reader.ReadParamValue("isHeavyWeapon", isHeavyWeapon);
	reader.ReadParamValue("canOvercharge", canOvercharge);
	reader.ReadParamValue("autoReloadDelay", autoReloadDelay);
	reader.ReadParamValue("scopeAttachment", scopeAttachment);
	reader.ReadParamValue("selectOverride", selectOverride);
	reader.ReadParamValue("isUnique", isUnique);
	reader.ReadParamValue("doesAttachmentGiveAmmo", doesAttachmentGiveAmmo);
	reader.ReadParamValue("isAttachedToBack", isAttachedToBack);
	reader.ReadParamValue("sprintToFireDelay", sprintToFireDelay);
	reader.ReadParamValue("sprintToZoomDelay", sprintToZoomDelay);
	reader.ReadParamValue("sprintToMeleeDelay", sprintToMeleeDelay);
	reader.ReadParamValue("runToSprintBlendTime", runToSprintBlendTime);
	reader.ReadParamValue("sprintToRunBlendTime", sprintToRunBlendTime);
	tag = reader.ReadParamValue("tag");

	return true;
}
}