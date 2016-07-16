#include <StdAfx.h>

#include "ICamera.h"
#include <CryRenderer/IRenderer.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include <Actor/Player/Player.h>


ICamera::ICamera()
{}


ICamera::~ICamera()
{}


void ICamera::GetCameraAnimationFactor(float& position, float& rotation)
{
	position = 0.0f;
	rotation = 0.0f;
}


// TODO: Copy newly learnt lessons from entity sensing if no other way.
const Vec3 ICamera::GetAimTarget() const
{
	// Use a mid-length vector in the camera's forward direction as an initial target to ray-trace towards.
	// We should keep it as short as practical to lower the cost of using it. That said, it should probably be at
	// least the maximum distance for a GTAOE to help positioning the effect correctly.
	const Vec3 aimDirection = m_cameraPose.GetRotation() * FORWARD_DIRECTION * 60.0f;

	// Try and update where the player is aiming.
	// TODO: need to skip the player's geometry and if they are in a vehicle, that needs skipping too.
	// IMPORTANT: until we have the code to do that, it will intersect the player's own capsule giving
	// odd looking results.
	ray_hit rayhit;
	static IPhysicalEntity* pSkipEnts [10];
	int nskip = 0;

	int hits = gEnv->pPhysicalWorld->RayWorldIntersection(m_cameraPose.GetPosition(),
		aimDirection,
		ent_all,
		rwi_stop_at_pierceable | rwi_colltype_any,
		&rayhit,
		1, pSkipEnts, nskip + 1);

	if (hits)
	{
		// There's a hit, so return that as the aim target.
#if defined(_DEBUG)
		//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(rayhit.pt, 0.05f, ColorB(128, 0, 0));
#endif
		return rayhit.pt;
	}

#if defined(_DEBUG)
	//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(m_cameraPose.GetPosition() + aimDirection, 0.05f, ColorB(0, 0, 128));
#endif

	// Default is to return a position a set distance from the camera in the direction it is facing.
	return m_cameraPose.GetPosition() + aimDirection;
}


//void ICamera::ActivateMode (const CPlayer &clientPlayer)
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


//void ICamera::DeactivateMode (const CPlayer &clientPlayer)
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


//void ICamera::SetDrawNearestFlag (const CPlayer & clientPlayer, bool drawNearestFlag)
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
