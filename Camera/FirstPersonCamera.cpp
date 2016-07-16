#include <StdAfx.h>

#include "FirstPersonCamera.h"
#include <Actor/Player/Player.h>
#include <Actor/Character/Character.h>
#include <Game/Game.h>
#include <IViewSystem.h>


// ***
// *** IGameObjectExtension
// ***


void CFirstPersonCamera::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}


bool CFirstPersonCamera::Init(IGameObject * pGameObject)
{
	// Critical this is called.
	SetGameObject(pGameObject);

	// It's a good idea to use the entity as a default for our target entity.
	m_targetEntityID = GetEntityId();

	return true;
}


void CFirstPersonCamera::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);

	// Create a new view and link it to this entity.
	auto pViewSystem = gEnv->pGame->GetIGameFramework()->GetIViewSystem();
	m_pView = pViewSystem->CreateView();
	m_pView->LinkTo(GetGameObject());
}


bool CFirstPersonCamera::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	ResetGameObject();

	// If we're meant to be active, capture the view.
	OnActivate();

	return true;
}


void CFirstPersonCamera::Release()
{
	// Release the view.
	GetGameObject()->ReleaseView(this);
	gEnv->pGame->GetIGameFramework()->GetIViewSystem()->RemoveView(m_pView);
	m_pView = nullptr;

	delete this;
}


void CFirstPersonCamera::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	// Default on failure is to return a cleanly constructed blank camera pose.
	CCameraPose newCameraPose { CCameraPose() };

	// Resolve the entity.
	auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
	if (pEntity)
	{
		// HACK: need to fix this sort of thing.
		Vec3 localEyePosition { 0.0f, 0.0f, 1.82f };

		auto pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_targetEntityID);
		if (pActor)
			localEyePosition = pActor->GetLocalEyePos();

		// Pose is based on entity position and the eye position.
		// HACK: should get eye rotation as well.
		newCameraPose = CCameraPose(pEntity->GetPos() + localEyePosition, pEntity->GetRotation());
	}

	// We set the pose, regardless of the result.
	SetCameraPose(newCameraPose);
}


// ***
// *** ICamera
// ***


void CFirstPersonCamera::OnActivate()
{
	gEnv->pGame->GetIGameFramework()->GetIViewSystem()->SetActiveView(m_pView);
	GetGameObject()->CaptureView(this);
}


void CFirstPersonCamera::OnDeactivate()
{
	GetGameObject()->ReleaseView(this);
}


// ***
// *** IGameObjectView
// ***


void CFirstPersonCamera::UpdateView(SViewParams& params)
{
	// The last update call should have given us a new updated position and rotation.
	// We now pass those off to the view system. 
	params.SaveLast();
	params.position = GetCameraPose().GetPosition();
	params.rotation = GetCameraPose().GetRotation();

	// TODO: Need to set a FoV or weird stuff happens. Set up a cvar or something.
	// VR devices might need to be queried.
	params.fov = DEG2RAD(60.0f);
}


// ***
// *** CFirstPersonCamera
// ***


void CFirstPersonCamera::AttachToEntity(EntityId entityId)
{
	// Store the target entity.
	m_targetEntityID = entityId;

	// TODO: Figure out if we really need to be linking to the target entity. It seems like it's a good idea but it
	// always messes up the view.
	//auto pEntity = gEnv->pEntitySystem->GetEntity(m_targetEntityID);
	//if (pEntity)
	//	m_pView->LinkTo(pEntity);
}
