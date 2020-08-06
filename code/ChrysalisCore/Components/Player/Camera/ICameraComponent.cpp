#include <StdAfx.h>

#include "ICameraComponent.h"
#include <CryRenderer/IRenderer.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include "Components/Player/PlayerComponent.h"


namespace Chrysalis
{
void ICameraComponent::ReflectType(Schematyc::CTypeDesc<ICameraComponent>& desc)
{
	desc.SetGUID(ICameraComponent::IID());
	desc.SetEditorCategory("Hidden");
	desc.SetLabel("Camera Component Interface");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


//bool ICameraComponent::OnAsyncCameraCallback(const HmdTrackingState& sensorState, IHmdDevice::AsyncCameraContext& context)
//{
//	context.outputCameraMatrix = m_cameraMatrix;
//
//	Matrix33 orientation = Matrix33(context.outputCameraMatrix);
//	Vec3 position = context.outputCameraMatrix.GetTranslation();
//
//	context.outputCameraMatrix.AddTranslation(orientation * sensorState.pose.position);
//	context.outputCameraMatrix.SetRotation33(orientation * Matrix33(sensorState.pose.orientation));
//
//	return true;
//}


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
	const Vec3 aimDirection = Quat(m_cameraMatrix) * FORWARD_DIRECTION * 100.0f;

	// Try and update where the player is aiming.
	// #TODO: need to skip the player's geometry and if they are in a vehicle, that needs skipping too.
	ray_hit rayhit;
	static IPhysicalEntity* pSkipEnts[10];
	int skipCount{ 0 };

	// Skip the target actor for this.
	if (pRayCastingEntity)
	{
		auto pPhysicalEntity = pRayCastingEntity->GetPhysics();
		if (pPhysicalEntity)
		{
			pSkipEnts[0] = pRayCastingEntity->GetPhysics();
			skipCount++;
		}
	}

	int hits = gEnv->pPhysicalWorld->RayWorldIntersection(m_cameraMatrix.GetTranslation(),
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
	//	gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(m_cameraMatrix.GetTranslation() + aimDirection, 0.04, ColorB(0, 0, 128));
	//#endif

		// Default is to return a position a set distance from the camera in the direction it is facing.
	return m_cameraMatrix.GetTranslation() + aimDirection;
}


void ICameraComponent::UpdateView()
{
	const CCamera& systemCamera = gEnv->pSystem->GetViewCamera();

	m_camera.SetFrustum(systemCamera.GetViewSurfaceX(), systemCamera.GetViewSurfaceZ(), 
		DEG2RAD(m_fieldOfView), 
		m_nearPlane,
		gEnv->p3DEngine->GetMaxViewDistance(), systemCamera.GetPixelAspectRatio());
	m_camera.SetMatrix(m_cameraMatrix);

	// Take control of the system camera.
	// TODO: This is a greedy inconsiderate way to do it. Only do this on activation and release on deactivation for the cameras.
	gEnv->pSystem->SetViewCamera(m_camera);
}
}