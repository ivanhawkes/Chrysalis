////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001.
// -------------------------------------------------------------------------
//  File name:   birdsflock.cpp
//  Version:     v1.00
//  Created:     72010 by Luciano Morpurgo (refactored from flock.cpp).
//  Compilers:   Visual C++ 7.0
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <StdAfx.h>

#include "AvianFlock.h"
#include <IActorSystem.h>
#include "BoidAvian.h"
#include <CryAISystem/IAIObject.h>


#define START_LANDING_TIME 3


CAvianFlock::CAvianFlock(IEntity *pEntity) : CFlock(pEntity, EFLOCK_BIRDS), m_bAttractOutput(false)
{ 
	m_boidEntityName = "AvianBoid";  
	m_landCollisionCallback = functor(*this,&CAvianFlock::LandCollisionCallback);

};

void CAvianFlock::CreateBoids(SBoidsCreateContext &ctx)
{
	CFlock::CreateBoids(ctx);
	m_boidDefaultAnimName =  m_bc.GetAnimationName(Avian::ANIM_FLY);

	m_bc.bAutoTakeOff = false;

	for (uint32 i = 0; i < m_RequestedBoidsCount; i++)
	{
		CBoidObject *boid = new CBoidAvian( m_bc );
		if (m_bc.bSpawnFromPoint)
		{
			CBoidAvian *pBoid = (CBoidAvian *)boid;
			float radius	= 0.5; // Spawn not far from origin
			float z				= /*gEnv->p3DEngine->GetTerrainElevation(m_origin.x,m_origin.y)*/ + 7.f + Boid::Frand()*2.f;	// z = terrain height + [5-9]
			pBoid->m_pos	= m_origin + Vec3(radius*Boid::Frand(),radius*Boid::Frand(), radius*Boid::Frand());
			z = .25f*Boid::Frand() + .25f;	// z-heading = 0.0 - 0.5
			pBoid->m_heading		= ( Vec3(Boid::Frand(),Boid::Frand(),z) ).GetNormalized(); 
			pBoid->m_scale			= m_bc.boidScale + Boid::Frand()*m_bc.boidRandomScale;
			boid->m_speed				= m_bc.MinSpeed + (Boid::Frand()+1)/2.0f*(m_bc.MaxSpeed - m_bc.MinSpeed);
			pBoid->m_dead				= 0;
			pBoid->m_currentAccel(0,0,0);
			pBoid->SetSpawnFromPt(true);
			pBoid->m_fNoCenterAttract = m_bc.factorAttractToOrigin;
			pBoid->m_fNoKeepHeight		= m_bc.factorKeepHeight;
			pBoid->SetAttracted(false);
		}
		else
		{
			float radius = m_bc.fSpawnRadius;
			boid->m_pos = m_origin + Vec3(radius*Boid::Frand(),radius*Boid::Frand(),Boid::Frand()*radius);
			boid->m_heading = ( Vec3(Boid::Frand(),Boid::Frand(),0) ).GetNormalized();
			boid->m_scale = m_bc.boidScale + Boid::Frand()*m_bc.boidRandomScale;
			boid->m_speed = m_bc.MinSpeed + (Boid::Frand()+1)/2.0f*(m_bc.MaxSpeed - m_bc.MinSpeed);
		}

		AddBoid(boid);
	}
	m_lastUpdatePosTimePassed = 1;
	m_avgBoidPos = GetPos();
	//
	m_hasLanded = false;
	TakeOff();
	
	m_terrainPoints = 0;
}


void CAvianFlock::Reset()
{
	CFlock::Reset();
	m_LandingPoints.clear();
	m_landCollisionInfo.Reset();
	m_terrainPoints = 0;
	m_hasLanded = false;

}

void CAvianFlock::Update(CCamera *pCamera)
{
	
	UpdateLandingPoints();

	m_isPlayerNearOrigin = GetEntity()!=NULL && IsPlayerInProximity(GetEntity()->GetPos());

	CFlock::Update(pCamera);
	
	float dt = gEnv->pTimer->GetFrameTime();

	// set the ai object in the middle of the boids
	UpdateAvgBoidPos(dt);

	if(m_status == Avian::FLYING && !m_bc.noLanding && 
		(gEnv->pTimer->GetFrameStartTime()- m_flightStartTime).GetSeconds() > m_flightDuration)
	{
		if(GetEntity() && !m_isPlayerNearOrigin)
			Land();
	}
	
	if (m_status == Avian::LANDING)
	{
		float timePassed = (gEnv->pTimer->GetFrameStartTime()- m_flightStartTime).GetSeconds(); 
		if(timePassed < START_LANDING_TIME+0.5f)
		{
			int n = m_boids.size();
			int numBoidsLandingNow = int(float(n*timePassed)/START_LANDING_TIME);
			if(numBoidsLandingNow >n)
				numBoidsLandingNow = n;
			for(int i = 0; i<numBoidsLandingNow ;++i)
			{
				CBoidAvian* bird = static_cast<CBoidAvian*>(m_boids[i]);
				if(bird && !bird->IsDead() && !bird->IsLanding())
					bird->Land();
			}
		}

	}

	
	if (m_status == Avian::LANDING || m_status == Avian::ON_GROUND)
	{
		if(IsPlayerInProximity(GetAvgBoidPos()))
			TakeOff();
	}

}


void CAvianFlock::SetAttractionPoint(const Vec3 &point)
{
	m_bc.attractionPoint	= point;
	SetAttractOutput(false);

	for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
	{
		((CBoidAvian *)*it)->SetAttracted();
	}
}

void CAvianFlock::SetEnabled( bool bEnabled )
{
	if (!m_bc.bSpawnFromPoint)
		CFlock::SetEnabled(bEnabled);
	else
	{
		if (m_bEnabled != bEnabled)
		{
			SetAttractOutput(false);
			m_bEnabled	= bEnabled;
			for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
			{
				CBoidAvian* pBoid = (CBoidAvian *)*it;
				float radius	= 0.5; // Spawn not far from origin
				float z				= /*gEnv->p3DEngine->GetTerrainElevation(m_origin.x,m_origin.y) + */7.f + Boid::Frand()*2.f;	// z = terrain height + [5-9]
				pBoid->m_pos	= m_origin + Vec3(radius*Boid::Frand(),radius*Boid::Frand(), radius*Boid::Frand());
				z = .25f*Boid::Frand() + .25f;	// z-heading = 0.0 - 0.5
				pBoid->m_heading		= ( Vec3(Boid::Frand(),Boid::Frand(),z) ).GetNormalized(); 
				pBoid->m_scale			= m_bc.boidScale + Boid::Frand()*m_bc.boidRandomScale;
				pBoid->m_speed				= m_bc.MinSpeed + (Boid::Frand()+1)/2.0f*(m_bc.MaxSpeed - m_bc.MinSpeed);
				pBoid->m_dead				= 0;
				pBoid->m_currentAccel(0,0,0);
				pBoid->SetSpawnFromPt(true);
				pBoid->m_fNoCenterAttract = m_bc.factorAttractToOrigin;
				pBoid->m_fNoKeepHeight		= m_bc.factorKeepHeight;
				pBoid->SetAttracted(false);
			}

			if (!bEnabled && m_bEntityCreated)
				DeleteEntities(false);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void CAvianFlock::OnAIEvent(EAIStimulusType type, const Vec3& pos, float radius, float threat, EntityId sender)
{
	CFlock::OnAIEvent(type,pos,radius,threat,sender);
	if(type==AISTIM_SOUND)
	{
		if(threat > 0 && Distance::Point_PointSq(pos, m_bc.flockPos) < radius*radius * threat * threat)
			TakeOff();
	}
}

//////////////////////////////////////////////////////////////////////////
Vec3& CAvianFlock::FindLandSpot()
{
	return m_defaultLandSpot;
}


//////////////////////////////////////////////////////////////////////////

void CAvianFlock::Land()
{
/*
	for (Boids::iterator it = m_boids.begin(),itEnd = m_boids.end(); it != itEnd; ++it)
	{
		CBoidAvian* pBoid = static_cast<CBoidAvian*>(*it);
		if(pBoid && !pBoid->IsDead())
			pBoid->Land();
	}*/

	m_status = Avian::LANDING;
	m_flightStartTime = gEnv->pTimer->GetFrameStartTime();
}

//////////////////////////////////////////////////////////////////////////

void CAvianFlock::TakeOff()
{
	for (Boids::iterator it = m_boids.begin(),itEnd = m_boids.end(); it != itEnd; ++it)
	{
		CBoidAvian* pBoid = static_cast<CBoidAvian*>(*it);
		if(pBoid && !pBoid->IsDead())
			pBoid->TakeOff(m_bc);
	}

	m_flightDuration = GetFlightDuration();
	m_flightStartTime = gEnv->pTimer->GetFrameStartTime();
	m_status = Avian::FLYING;
	m_birdsOnGround = 0;
}

//////////////////////////////////////////////////////////////////////////

float CAvianFlock::GetFlightDuration()
{
	if(!m_hasLanded && !m_bc.noLanding && m_bc.bStartOnGround)
		return 2.f;
	return m_bc.flightTime * (1.f + Boid::Frand()*0.2f);
}

//////////////////////////////////////////////////////////////////////////

int CAvianFlock::GetNumAliveBirds()
{
	int numAliveBoids = 0;
	for (Boids::iterator it = m_boids.begin(),itEnd = m_boids.end(); it != itEnd; ++it)
	{
		CBoidAvian* pBoid = static_cast<CBoidAvian*>(*it);
		if(pBoid && !pBoid->IsDead())
			++numAliveBoids;
	}
	return numAliveBoids;

}

//////////////////////////////////////////////////////////////////////////

void CAvianFlock::NotifyBirdLanded()
{
	++m_birdsOnGround;
	if(m_status != Avian::ON_GROUND) // just optimization
	{
		if(m_birdsOnGround > 2 * GetNumAliveBirds() / 3)
			m_status = Avian::ON_GROUND;
	}
	m_hasLanded = true;
}

//////////////////////////////////////////////////////////////////////////

void CAvianFlock::UpdateLandingPoints()
{
	int numPoints = m_boids.size()*3 /2;
	if(m_LandingPoints.size() < (size_t) numPoints )
	{
		if(!m_landCollisionInfo.IsRequestingRayCast())
		{
			float maxradius = m_bc.fSpawnRadius *0.6f;
			Vec3 origin(m_bc.flockPos);
			float angle = Boid::Frand()*gf_PI;
			float radius = (Boid::Frand()+1)/2 * maxradius;

			origin += Vec3(cos_tpl(angle)*radius, sin_tpl(angle)*radius, 5.f);

			Vec3 vDir = Vec3(0,0,-10);
			m_landCollisionInfo.QueueRaycast(m_pEntity->GetId(),origin,vDir,&m_landCollisionCallback);
		}
	}
}

////////////////////////////////////////////////

void CAvianFlock::LandCollisionCallback(const QueuedRayID& rayID, const RayCastResult& result)
{
	if (result.hitCount)
	{
		Vec3 hitpt = result.hits[0].pt;

		hitpt.z += m_bc.groundOffset;

		for(TLandingPoints::iterator it = m_LandingPoints.begin(), itEnd = m_LandingPoints.end(); it != itEnd; ++it)
		{
			if(Distance::Point_PointSq(*it,hitpt)< 0.04f)
				return;
		}

		SLandPoint lpt(hitpt);
//  			if(hit.bTerrain >0)
// 				++m_terrainPoints;
		
		m_LandingPoints.push_back(lpt);
/*
		// prevent birds to walk if there are too many non-terrain objects in the area
		if(m_LandingPoints.size() == numPoints)
		{
			if(m_terrainPoints < 2 * numPoints / 3)
				m_bc.walkSpeed = 0;
		}*/

	}
}

/////////////////////////////////////////////////////////////////////////

Vec3 CAvianFlock::GetLandingPoint(Vec3& fromPos)
{
	float minDist2 = 100000000.f;
	TLandingPoints::iterator itEnd = m_LandingPoints.end(), itFound = m_LandingPoints.end();
	for(TLandingPoints::iterator it = m_LandingPoints.begin(); it != itEnd; ++it)
	{
		if(!it->bTaken)
		{
			Vec3 landPoint = *it;
			float dist2 = Distance::Point_PointSq(landPoint,fromPos);
			if(dist2< minDist2)
			{
				minDist2 = dist2;
				itFound = it;
			}		
		}
	}	
	if(itFound!=itEnd )
	{
		itFound->bTaken = true;
		return *itFound;
	}
	return ZERO;
}

/////////////////////////////////////////////////////////////////////////////////////

void CAvianFlock::LeaveLandingPoint(Vec3& point)
{
	TLandingPoints::iterator itEnd = m_LandingPoints.end();
	for(TLandingPoints::iterator it = m_LandingPoints.begin(); it != itEnd; ++it)
		if(point == Vec3(*it))
		{
			it->bTaken = false;
			return;
		}
}

/////////////////////////////////////////////////////////////////////////////////////

bool CAvianFlock::IsPlayerInProximity(const Vec3& pos) const
{
	const IActor* pClientPlayer = gEnv->pGame->GetIGameFramework()->GetClientActor();
	if(pClientPlayer != NULL && pClientPlayer->GetEntity())
	{
		if(Distance::Point_PointSq(pos,pClientPlayer->GetEntity()->GetPos()) <
			m_bc.fSpawnRadius * m_bc.fSpawnRadius )
			return true;
	}
	return false;
}
