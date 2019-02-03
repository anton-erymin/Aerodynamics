#pragma once

#include "export.h"

#include "lpMath.h"
#include "lpBroadPhase.h"
#include "lpNarrowPhase.h"


class DLL_EXPORT lpSurface
{
public:
	// Параметры поверхности тела
		
	// Коэффициент отскока
	float m_restitution;
	// Коэффициент трения
	float m_friction;


	lpSurface(float restitution = 0.8f, float friction = 0.6f);
	void	setRestitution(float restitution);
	float	getRestitution();
	void	setFriction(float friction);
	float	getFriction();
};


class DLL_EXPORT lpRigidBody
{
public:
	// Позиция в мировой системе
	lpVec3 m_pos; //0 4 8
	// Матрица ориентации в мировой системе. Содержит только вращение
	lpMat3 m_orientation; //12 16 20 24 28 32 36 40 44
		
	// Матрица трансформации из локальной системы в мировую. Комбинация позиции и ориентации
	lpMat3 m_transform; //48 - 80
		
	// Локальный бокс
	lpAxisAlignedBoundingBox m_localAABB; //84 88 92 96 100 104
	// Преобразованный бокс
	lpAxisAlignedBoundingBox m_worldAABB; //108 112 116 120 124 128
		
	// Линейная скорость
	lpVec3 m_linearVel; //132 136 140
	// Угловая скорость
	lpVec3 m_angularVel; //144 148 152

	// Энергия тела
	float m_energy;	//156
	// Флаг, показывающий активно ли тело в данный момент
	bool m_freezed; // 160


	// Линейное ускорение
	lpVec3 m_acceleration;
	// Угловое ускорение
	lpVec3 m_angularAcceleration; 

	// Флаг показывающий тип тела
	bool m_immovable;
		
	// Геометрия тела, непосредственно используется для определения коллизий в узкой фазе
	lpCollisionGeometry *m_geometry; 
		
	// Любые пользовательские данные, например меш графического движка для отображения тела на экране
	void *m_userData; 
		
	// Параметры поверхности тела
	lpSurface *m_surface;
		
	int m_id;
		
		
	// Последнее изменение скорости
	lpVec3 m_deltaVel; 
	
	// Масса тела
	float m_mass; 
	// Обратная масса тела
	float m_invMass;
	// Инвертированный тензор инерции в локальных координатах
	lpMat3 m_invI;
	// Инвертированный тензор инерции в мировых координатах
	lpMat3 m_invIWorld;	
		

	
	// Флаг показывающйи, может ли тело "заснуть"
	bool m_canFreeze;
private:
	// Порог энергии деактивации тела
	float m_freezeEps;
	// Флаг процесса заморозки тела
	bool m_freezing;
	short m_freezeFrames;
	short m_numFramesToFreeze;

public:
	// Вектор гравитации
	lpVec3 m_gravity;

	// Аккумулятор сил
	lpVec3 m_forceAccum;
	// Аккумулятор моментов сил
	lpVec3 m_torqueAccum;

	lpVec3 m_pseudoForce;
	lpVec3 m_pseudoTorque;

private:
	// Коэффициенты затухания линейной и угловой скоростей
	float m_linearDamping;
	float m_angularDamping;


public:
	lpRigidBody(void);
	~lpRigidBody(void);

	void integrate(float dt);
	void addGeometry(lpCollisionPrimitive *geom);
	void updateAABB();
	void update();

	void rotate(float angle, const lpVec3 &axis);

	void setSurfaceParams(float restitution, float friction);

	void setGravity(const lpVec3 &gravity);

	void setMass(float mass);
	void setInertiaTensor(const lpMat3 &inertiaTensor);

	void applyForce(const lpVec3 &force);
	void applyForceAtPoint(const lpVec3 &force, const lpVec3 point);
	void applyTorque(const lpVec3 &torque);

	void clearAccums();

	void multByInverseInertiaTensorWorld(lpVec3 &vec);
	void calcInverseInertiaTensorWorld();

	void freeze();
	void unfreeze();

	void makeImmovable();

	float	getLinearDamping();
	void	setLinearDamping(float linDamping);
	float	getAngularDamping();
	void	setAngularDamping(float angDamping);

	void		setPosition(float x, float y, float z);
	void		setPosition(const lpVec3 &pos);
	lpVec3		getPosition();
};