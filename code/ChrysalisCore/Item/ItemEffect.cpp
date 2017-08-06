#include <StdAfx.h>

#include <Components/Items/ItemComponent.h>


namespace Chrysalis
{
EntityEffects::TAttachedEffectId CItemComponent::AttachLight(const int targetSlot, const char* helperName, Vec3 offset, Vec3 direction, eGeometrySlot firstSafeSlot,
	const SDynamicLightConstPtr attachParams)
{
	return m_effectsController.AttachLight(targetSlot, helperName, offset, direction, firstSafeSlot, attachParams);
}


void CItemComponent::DetachEffect(const EntityEffects::TAttachedEffectId effectId)
{
	m_effectsController.DetachEffect(effectId);
}
}