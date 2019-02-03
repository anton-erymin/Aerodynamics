#pragma once

#include "export.h"
#include <vector>

#include "lpMath.h"

class lpRigidBody;


#define		PRIMITIVE_SPHERE	1
#define		PRIMITIVE_BOX		2


class DLL_EXPORT lpCollisionPrimitive
{
public:
	lpRigidBody *m_body;
	lpMat3 m_transform;
	int	m_type;
	
	lpCollisionPrimitive();
	virtual ~lpCollisionPrimitive();

	virtual float getVolume() = 0;
};


class DLL_EXPORT lpCollisionGeometry
{
public:
	std::vector<lpCollisionPrimitive*> m_geometries;


	lpCollisionGeometry(void);
	virtual ~lpCollisionGeometry(void);

	void addGeometry(lpCollisionPrimitive *geom);
};



class DLL_EXPORT lpSphere : public lpCollisionPrimitive
{
public:
	float m_radius;

	lpSphere(float radius);

	float getVolume();
};


class DLL_EXPORT lpBox : public lpCollisionPrimitive
{
public:
	lpVec3 m_radius;

	lpVec3 axis1;
	lpVec3 axis2;
	lpVec3 axis3;

	lpBox(float width, float height, float depth);
	lpBox(const lpVec3 &boxSize);

	float getVolume();
};