#pragma once

#include <LaxePhysicsEngine.h>


// ��������� ��������

// �����
#define			AIRPLANE_LENGTH			14.0f

// ������
#define			AIRPLANE_WIDTH			7.0f

// ������
#define			AIRPLANE_HEIGHT			4.61f

// ������� �������� �������
#define			MIDLE_DIAMETER			2.0f

// �����
#define			AIRPLANE_MASS			5000.0f



// ���� ��������� �����
#define			WING_ANGLE				0.5f

// ������� �����
#define			WING_AREA				26.0f


#define			WING_LENGTH				15.5f
#define			WING_X					1.0f
#define			WING_Y					-0.6f
#define			WING_FUSELAGE_WIDTH		2.0f
#define			WING_SWEPT_ANGLE		26.0f
#define			WING_V_ANGLE			2.0f


// ���� ��������� ��������������� �������������
#define			HOR_STABILISER_ANGLE	-2.0f

// ������� ��������������� �������������
#define			HOR_STABILISER_AREA		20.0f

// ������� ������������� �������������
#define			VERT_STABILISER_AREA	15.0f

// ����� �����
#define			FOCUS_WING_X			-2.0f
#define			FOCUS_WING_Y			-0.5f
#define			FOCUS_WING_Z			5.5f

// ����� ��������
#define			FOCUS_STAB_X			-6.0f
#define			FOCUS_STAB_Y			2.0f
#define			FOCUS_STAB_Z			0.0f


// ������������ ���� ���������� ���� ������
#define			ELEVATOR_MAX_ANGLE		25.0f

// ������������ ���� ���������� ��������
#define			AILERON_MAX_ANGLE		20.0f

// ������������ ���� ���������� ���� �����������
#define			RUDDER_MAX_ANGLE		30.0f


// ������������ ���� ���������
#define			THRUST_MAX				40000.0f




// ��������� ������
#define			START_HEIGHT			3000.0f

// ��������� ��������
#define			START_VELOCITY			150.0f


#define			WING_N					50


// ������ ����������
struct Controls
{
	float elevator;
	float aileron;
	float rudder;
	float thrust;
};


struct Airplane
{
	// ���������� ���� ��������
	lpRigidBody *rb;

	// ����������
	Controls ctrls;

	// ��������� ����� ��������
	lpVec3 localX;
	lpVec3 localY;
	lpVec3 localZ;

	// ������ �������� � ��������� ������� ���������
	lpVec3 V;
	float V_mag;
	// �������� �������� �� ���������� ��������� OXY
	lpVec3 Vxy;
	float Vxy_mag;
	// �������� �������� �� �������������� ��������� OXZ
	lpVec3 Vxz;
	float Vxz_mag;

	// ���� ����� ��������
	float alpha;
	// ���� ���������� ��������
	float beta;

	// ������ ����������� ��������� ���� �������������� ������������
	lpVec3 Lh;
	// ������ ����������� ��������� ���� ������������ ������������
	lpVec3 Lv;

	// ������ ����������� �������� �������������
	lpVec3 D;


	// ������������� ��������
	lpVec3 Df;
	float Df_mag;

	// ���� ����� �����
	float wingAttackAngle_l;
	float wingAttackAngle_r;

	// ���� �����
	lpVec3 Lw_l;
	float Lw_mag_l;
	lpVec3 Lw_r;
	float Lw_mag_r;
	lpVec3 Dw_l;
	float Dw_mag_l;
	lpVec3 Dw_r;
	float Dw_mag_r;

	// ���� ��������
	lpVec3 Lhor;
	float Lhor_mag;
	lpVec3 Dhor;
	float Dhor_mag;
	lpVec3 Lvert;
	float Lvert_mag;
	lpVec3 Dvert;
	float Dvert_mag;
	
	
	// ������ ���� �������
	float weightMag;

	// ���������������� ����� �����
	lpVec3 ac1;
	lpVec3 ac2;

	// ���������������� ����� ��������
	lpVec3 tailPoint;


	// ������ ����
	lpVec3 T;
	float T_mag;

	lpVec3 *wing_forces;

	lpVec3 *wing_points;
};



void calcAerodynamics();
void initPhysics();
void calcPhysics();
void step(float dt, float &accTime);
void calcAeroCenters();

void calcFuselage();
void calcWing(); 
void integrateWing();
void calcHorizontalStabiliser();
void calcVerticalStabiliser();
void calcControlAeroSurfaces();

void calcEngine();

void calcAirDensity();