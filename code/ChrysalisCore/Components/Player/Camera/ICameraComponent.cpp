#include <StdAfx.h>

#include "ICameraComponent.h"
#include <CryRenderer/IRenderer.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include <Components/Player/Player.h>


namespace Chrysalis
{
void ICameraComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void ICameraComponent::ReflectType(Schematyc::CTypeDesc<ICameraComponent>& desc)
{
	desc.SetGUID(ICameraComponent::IID());
	desc.SetEditorCategory("Hidden");
	desc.SetLabel("Camera Component Interface");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


void ICameraComponent::GetCameraAnimationFactor(float& position, float& rotation)
{
	// #TODO: At some point this needs to be written.
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
	// #TODO: need to skip the player's geometry and if they are in a vehicle, that needs skipping too.
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
}