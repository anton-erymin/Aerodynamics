#pragma once

#include "export.h"
#include "lpJoint.h"
#include "lpMath.h"


class lpRigidBody;


class DLL_EXPORT lpContactJoint : public lpJoint
{
public:
	// Коэффициент упругости удара
	float m_restitution;
		
	// Коэффициент трения контакта
	float m_friction;
		
	// Координаты точки контакта
	lpVec3 m_point;
		
	// Нормаль контакта в мировых координатах
	lpVec3 m_normal;
		
	// Глубина проникновения в контакте
	float m_depth;
		
		
	lpVec3 m_posChange[2];
	lpVec3 m_orChange[2];
	lpVec3 m_velChange[2];
	lpVec3 m_angVelChange[2];
		
	// Внутренние промежуточные данные
private:	
		// Матрица преобразования для системы координат связанной с точкой контакта
	lpMat3 m_transform;
		
	// Тангенциальные оси трения
	lpVec3 m_frictionDir1;
	lpVec3 m_frictionDir2;
		
			
public:
	lpVec3 m_r1;
	lpVec3 m_r2;

	lpVec3 m_relVel;
	lpVec3 m_relVelLocal;
		
	float m_desiredDeltaVelocity;


	lpContactJoint(void);
	~lpContactJoint(void);

	void prepare();
	void calcDesiredDeltaVelocity();
	void calcRelativeVelocity();
	float calcLambda(const lpVec3 &normal, float dv);
	void solveVelocity();
	void solvePenetration();
	void calcContactTransform();

	float** jacobian(float idt);
};
