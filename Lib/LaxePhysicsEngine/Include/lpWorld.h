#pragma once

#include "export.h"
#include <vector>

#include "lpRigidBody.h"
#include "lpJoint.h"
#include "lpContactJoint.h"
#include "lpSolver.h"
#include "lpSequentialSolver.h"
#include "lpLCPSolver.h"
#include "lpCollisionTest.h"



class DLL_EXPORT lpWorld
{
public:
	// Список контактов, найденных системой определения столкновений
	std::vector<lpContactJoint*> m_contacts;
	// Количество найденных контактов
	unsigned int m_numContacts;

	// Список джойнтов в мире
	std::vector<lpJoint*> m_joints;

protected:
	// Список тел
	std::vector<lpRigidBody*> m_bodies;
	// Количество динамичных тел
	int m_numDynamicBodies;

	// Солвер, используемый для решения контактов
	lpSolver *m_solver;

	// Вектор гравитации мира
	lpVec3 m_gravity;
	

public:
	lpWorld(int solverType);
	virtual ~lpWorld();

	void addBody(lpRigidBody *body);
	void removeBody(const lpRigidBody *body);

	void setGravity(const lpVec3 &gravity);

	void step(float dt);

	void updateBodies();

protected:
	virtual void collide() = 0;

	void solve(float dt);
};



class DLL_EXPORT lpWorldSimple : public lpWorld
{
public:
	lpWorldSimple(int solverType);

private:
	void collide();
};