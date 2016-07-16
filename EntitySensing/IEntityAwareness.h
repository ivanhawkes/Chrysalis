/**
\file d:\CRYENGINE\Code\ChrysalisSDK\EntitySensing\IEntityAwareness.h

Declares the IEntityAwareness interface.
**/
#pragma once

#include <IGameObject.h>

struct ray_hit;
typedef std::vector<EntityId> Entities;


struct IEntityAwareness : IGameObjectExtension
{
	/**
	Gets the position of the actor's eyes.

	\return The eye position.
	**/
	virtual const Vec3& GetPos() const = 0;


	/**
	Gets the normalised (fast) direction the actor's eye are gazing.

	\return The direction.
	**/
	virtual const Vec3& GetDir() const = 0;


	/**
	Sets proximity radius, which determines how far away from the actor we will search for entities.

	\param	proximityRadius The proximity radius.
	**/
	virtual void SetProximityRadius(float proximityRadius) = 0;


	/**
	Utilises the base ray-cast query to see if there is a hit within the "forwardCastDistance".
	If so, that hit is returned.
	
	\param	maxDistance The maximum distance at which a hit from the base ray is to be considered.
	\param	ignoreGlass true to ignore glass.
	
	\return null if it fails, else the look at point.
	**/
	virtual const ray_hit* GetLookAtPoint(const float maxDistance = 0.0f, bool ignoreGlass = false) = 0;


	/**
	Utilises the base ray-cast query to see if there is a hit within the "forwardCastDistance".
	
	\param	ignoreGlass true to ignore glass.
	
	\return The EntityId for whichever entity the actor is looking towards.
	**/
	virtual const EntityId GetLookAtEntityId(bool ignoreGlass = false) = 0;


	/**
	Utilises the base ray-cast query, but filters the result to only include a hit if there is a viable
	result within the proximity radius limit (m_proximityRadius).
	
	\return null if it fails, else the successful raycast data.
	**/
	virtual const ray_hit* RaycastQuery() = 0;


	/**
	A proximity based query that limits the results to only those entities which are considered
	to be in front of the actor. These are limited based on a line segment from the actor's eyes,
	forward in the direction they are looking.

	\return The entities in front of the actor.
	**/
	virtual const Entities& GetEntitiesInFrontOf() = 0;


	/**
	Based on the results from GetEntitiesInFrontOf(), it will return just one entity from the list.
	NOTE: the list is not sorted in any manner, so which one is returned in non-deterministic.
	
	\return	null if it fails, else the first entity in the list of those in front of the actor. 
	**/
	virtual IEntity* GetEntityInFrontOf() = 0;

	
	/** Creates an AABB around the actor and performs a query on entities within that box. The size of
	the box is based on m_proximityRadius. Any entities which are within the box will be made available
	in the m_entitiesInProximity container as a side effect	of running this query. No culling is
	performed on the entities returned from the query.

	You can use this as a base on which to build more nuanced and precise queries. */
	virtual Entities& ProximityQuery() = 0;


	/** A proximity based query that limits the results to a smaller range centered around the actor's eyes. The area is
	scaled by proximityCloseByFactor, which should provide a fairly neat box around the actor - within reasonable reach
	to indicate they can trigger actions on these entities. */
	virtual Entities& NearQuery() = 0;
};
