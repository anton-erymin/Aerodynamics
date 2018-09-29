#pragma once

#include "lpSolver.h"

 
class DLL_EXPORT lpSequentialSolver : public lpSolver
{
public:
	lpSequentialSolver(void);
	~lpSequentialSolver(void);

	void solve(float dt, std::vector<lpRigidBody*> &bodies, std::vector<lpJoint*> &joints, std::vector<lpContactJoint*> &contacts, int numContacts, int numDynamicBodies);

private:
	void prepareContacts(std::vector<lpContactJoint*> &contacts, int numContacts);
	void solvePenetrations(std::vector<lpContactJoint*> &contacts, int numContacts);
	void solveVelocities(std::vector<lpContactJoint*> &contacts, int numContacts);
};
