#pragma once

#include "export.h"
#include <vector>

#define		SOLVER_SEQUENTIAL	0
#define		SOLVER_LCP			1


#include "lpRigidBody.h"
#include "lpJoint.h"
#include "lpContactJoint.h"


class DLL_EXPORT lpSolver
{
public:
	lpSolver(void);
	virtual ~lpSolver(void);

	virtual void solve(float dt, std::vector<lpRigidBody*> &bodies, std::vector<lpJoint*> &joints, std::vector<lpContactJoint*> &contacts, int numContacts, int numDynamicBodies) = 0;

};