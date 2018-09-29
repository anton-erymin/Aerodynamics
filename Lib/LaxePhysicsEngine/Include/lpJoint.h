#pragma once

#include "export.h"


class lpRigidBody;

#define		J_XI			12
#define		J_PSEUDOXI		13
#define		J_LO			14
#define		J_HI			15


class DLL_EXPORT lpJoint
{
public:
	// ѕервое тело в джойнте
	lpRigidBody *m_body1;
	// ¬торой взаимодействующее тело в джойнте
	lpRigidBody *m_body2;

protected:
	// якобиан
	float **m_J;

	// Error Reduction Parameter
	float ERP;

public:
	lpJoint(void);
	virtual ~lpJoint(void);

	virtual float** jacobian(float idt) = 0;
};
