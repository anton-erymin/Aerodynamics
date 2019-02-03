#pragma once

#include "export.h"

#include "lpMath.h"
#include "lpWorld.h"
#include "lpRigidBody.h"



// TODO: QuadTree - for the good coarse collision detection
// TODO: BVH - for the same reason
// TODO: Continuous Collision Test - for the fast velocities
// TODO: Force-based method(LCP, Jacobian) - needed to be embedded into engine in the future
// TODO: More primitives tests - more importantly convex hull
// TODO: Caching contacts
// TODO: Joints - for ragdolls and so on
// TODO: grouping contacts


const lpVec3 LP_X_AXIS(1.0f, 0.0f, 0.0f);
const lpVec3 LP_Y_AXIS(0.0f, 1.0f, 0.0f);
const lpVec3 LP_Z_AXIS(0.0f, 0.0f, 1.0f);



class DLL_EXPORT lpLaxePhysicsEngine
{
private:
	lpWorld *m_world;

public:
	lpLaxePhysicsEngine();
	~lpLaxePhysicsEngine();

	void			setGravity(const lpVec3& gravity);

	void			step(float dt);

	void			addBody(lpRigidBody *body);
	void			removeBody(const lpRigidBody *body);

	lpWorld*		createWorldSimple(int solverType);
	lpWorld*		createWorldQuadtree(int solverType);
	lpWorld*		createWorldBVH(int solverType);
	void			setWorld(lpWorld *world);
	lpWorld*		getWorld();

	lpRigidBody*	createRigidBody(bool addFlag);
	lpRigidBody*	createStaticBody(bool addFlag);
};
