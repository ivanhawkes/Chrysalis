#include <StdAfx.h>

#include "ICameraComponent.h"
#include <CryRenderer/IRenderer.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include <Player/Player.h>


void ICameraComponent::GetCameraAnimationFactor(float& position, float& rotation)
{
	// TODO: At some point this needs to be written.
	position = 0.0f;
	rotation = 0.0f;
}


const Vec3 ICameraComponent::GetAimTarget(const IEntity* pRayCastingEntity) const
{
	// Use a mid-length vector in the camera's forward direction as an initial target to ray-trace towards.
	// We should keep it as short as practical to lower the cost of using it. That said, it should probably be at
	// least the maximum distance for a GTAOE to help positioning the effect correctly.
	const Vec3 aimDirection = m_cameraPose.GetRotation() * FORWARD_DIRECTION * 100.0f;

	// Try and update where the player is aiming.
	// TODO: need to skip the player's geometry and if they are in a vehicle, that needs skipping too.
	ray_hit rayhit;
	static IPhysicalEntity* pSkipEnts [10];
	int skipCount { 0 };

	// Skip the target actor for this.
	if (pRayCastingEntity)
	{
		auto pPhysicalEntity = pRayCastingEntity->GetPhysics();
		if (pPhysicalEntity)
		{
			pSkipEnts [0] = pRayCastingEntity->GetPhysics();
			skipCount++;
		}
	}

	int hits = gEnv->pPhysicalWorld->RayWorldIntersection(m_cameraPose.GetPosition(),
		aimDirection,
		ent_all,
		rwi_stop_at_pierceable | rwi_colltype_any,
		&rayhit,
		1, pSkipEnts, skipCount);

	if (hits)
	{
//#if defined(_DEBUG)
//		gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(rayhit.pt, 0.04f, ColorB(128, 0, 0));
//#endif

		// There's a hit, so return that as the aim target.
		return rayhit.pt;
	}

//#if defined(_DEBUG)
//	gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(m_cameraPose.GetPosition() + aimDirection, 0.04, ColorB(0, 0, 128));
//#endif

	// Default is to return a position a set distance from the camera in the direction it is facing.
	return m_cameraPose.GetPosition() + aimDirection;
}


//void ICameraComponent::ActivateMode (const CPlayer &clientPlayer)
//{
//	 TODO: Figure out what this does and if we need it now.
//	m_bBlendingOff = false;
//
//	if (m_disableDrawNearest)
//	{
//		SetDrawNearestFlag (clientPlayer, false);
//	}
//
//	Activate (clientPlayer);
//}


//void ICameraComponent::DeactivateMode (const CPlayer &clientPlayer)
//{
//	 TODO: Figure out what this does and if we need it now.
//	m_bBlendingOff = true;
//
//	if (m_disableDrawNearest)
//	{
//		SetDrawNearestFlag (clientPlayer, true);
//	}
//
//	Deactivate (clientPlayer);
//}


//void ICameraComponent::SetDrawNearestFlag (const CPlayer & clientPlayer, bool drawNearestFlag)
//{
//	// TODO: Remove this or find out what it does.
//	/*	if (!clientPlayer.IsThirdPerson ())
//		{
//		IEntity* pPlayerEntity = clientPlayer.GetEntity ();
//		if (pPlayerEntity)
//		{
//		uint32 entitySlotFlags = pPlayerEntity->GetSlotFlags (eIGS_FirstPerson);
//		if (drawNearestFlag)
//		{
//		entitySlotFlags |= ENTITY_SLOT_RENDER_NEAREST;
//		}
//		else
//		{
//		entitySlotFlags &= ~ENTITY_SLOT_RENDER_NEAREST;
//		}
//		pPlayerEntity->SetSlotFlags (eIGS_FirstPerson, entitySlotFlags);
//		}
//		}*/
//}
