
#ifndef __TurtleBoid_h__
#define __TurtleBoid_h__
#pragma once

#include "../Flock.h"
#include "../Avian/BoidAvian.h"


//////////////////////////////////////////////////////////////////////////
class CTurtleBoid : public CBoidAvian
{
public:
	//CTurtleBoid(SBoidContext &bc) : CChickenBoid(bc) {};
	CTurtleBoid(SBoidContext &bc);
	virtual void Update(float dt, SBoidContext &bc);
	virtual void Think(float dt, SBoidContext &bc);
	virtual void Kill(const Vec3 &hitPoint, const Vec3 &force);

};

//////////////////////////////////////////////////////////////////////////
class CTurtleFlock : public CFlock
{
public:
	CTurtleFlock(IEntity *pEntity);
	virtual CBoidObject* CreateBoid() { return new CTurtleBoid(m_bc); };
};

#endif // __TurtleBoids_h__