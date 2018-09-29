#pragma once

#include "export.h"

#include "lpSolver.h"


class DLL_EXPORT lpLCPSolver : public lpSolver
{
	int curDofs;
	int curS;
	
	float *V;
	float *pV;
	float *a;
	float **J;
	int	  **Jmap;
	float *xi;
	float *pseudoxi;
	float *lo;
	float *hi;
	float *A[12];
	float *lambda;
	float *diag;
	float **At;

public:

	lpLCPSolver();

	void solve(float dt, std::vector<lpRigidBody*> &bodies, std::vector<lpJoint*> &joints, std::vector<lpContactJoint*> &contacts, int numContacts, int numDynamicBodies);
};

