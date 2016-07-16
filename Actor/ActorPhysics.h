#pragma once

#include <CryNetwork/ISerialize.h>
#include <CryMath/Cry_Vector3.h>
#include <IGameObject.h>


struct SActorPhysics
{
	enum EActorPhysicsFlags
	{
		Flying = BIT(0),
		WasFlying = BIT(1),
		Stuck = BIT(2)
	};


	SActorPhysics()
		/*: angVelocity(ZERO)
		, velocity(ZERO)
		, velocityDelta(ZERO)
		, velocityUnconstrained(ZERO)
		, velocityUnconstrainedLast(ZERO)
		, gravity(ZERO)
		, groundNormal(0, 0, 1)
		, speed(0)
		, groundHeight(0.0f)
		, mass(80.0f)
		, lastFrameUpdate(0)
		, groundMaterialIdx(-1)
		, groundColliderId(0)*/
	{}

	void Serialize(TSerialize ser, EEntityAspects aspects);

	CCryFlags<uint32> flags;

/*	Vec3 angVelocity;
	Vec3 velocity;
	Vec3 velocityDelta;
	Vec3 velocityUnconstrained;
	Vec3 velocityUnconstrainedLast;
	Vec3 gravity;
	Vec3 groundNormal;

	float speed;
	float groundHeight;
	float mass;

	int lastFrameUpdate;
	int groundMaterialIdx;
	EntityId groundColliderId;
*/
};
