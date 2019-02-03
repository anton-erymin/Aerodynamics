#pragma once

#include <LaxePhysicsEngine.h>


// Параметры самолета

// Длина
#define			AIRPLANE_LENGTH			14.0f

// Ширина
#define			AIRPLANE_WIDTH			7.0f

// Высота
#define			AIRPLANE_HEIGHT			4.61f

// Диаметр миделева сечения
#define			MIDLE_DIAMETER			2.0f

// Масса
#define			AIRPLANE_MASS			5000.0f



// Угол установки крыла
#define			WING_ANGLE				0.5f

// Площадь крыла
#define			WING_AREA				26.0f


#define			WING_LENGTH				15.5f
#define			WING_X					1.0f
#define			WING_Y					-0.6f
#define			WING_FUSELAGE_WIDTH		2.0f
#define			WING_SWEPT_ANGLE		26.0f
#define			WING_V_ANGLE			2.0f


// Угол установки горизонтального стабилизатора
#define			HOR_STABILISER_ANGLE	-2.0f

// Площадь горизонтального стабилизатора
#define			HOR_STABILISER_AREA		20.0f

// Площадь вертикального стабилизатора
#define			VERT_STABILISER_AREA	15.0f

// Фокус крыла
#define			FOCUS_WING_X			-2.0f
#define			FOCUS_WING_Y			-0.5f
#define			FOCUS_WING_Z			5.5f

// Фокус оперения
#define			FOCUS_STAB_X			-6.0f
#define			FOCUS_STAB_Y			2.0f
#define			FOCUS_STAB_Z			0.0f


// Максимальный угол отклонения руля высоты
#define			ELEVATOR_MAX_ANGLE		25.0f

// Максимальный угол отклонения элеронов
#define			AILERON_MAX_ANGLE		20.0f

// Максимальный угол отклонения руля направления
#define			RUDDER_MAX_ANGLE		30.0f


// Максимальная тяга двигателя
#define			THRUST_MAX				40000.0f




// Начальная высота
#define			START_HEIGHT			3000.0f

// Начальная скорость
#define			START_VELOCITY			150.0f


#define			WING_N					50


// Органы управления
struct Controls
{
	float elevator;
	float aileron;
	float rudder;
	float thrust;
};


struct Airplane
{
	// Физическое тело самолета
	lpRigidBody *rb;

	// Управление
	Controls ctrls;

	// Локальный базис самолета
	lpVec3 localX;
	lpVec3 localY;
	lpVec3 localZ;

	// Вектор скорости в локальной системе координат
	lpVec3 V;
	float V_mag;
	// Проекция скорости на продольную плоскость OXY
	lpVec3 Vxy;
	float Vxy_mag;
	// Проекция скорости на горизонтальную плоскость OXZ
	lpVec3 Vxz;
	float Vxz_mag;

	// Угол атаки самолета
	float alpha;
	// Угол скольжения самолета
	float beta;

	// Вектор направления подъемной силы горизонтальных поверхностей
	lpVec3 Lh;
	// Вектор направления подъемной силы вертикальных поверхностей
	lpVec3 Lv;

	// Вектор направления лобового сопротивления
	lpVec3 D;


	// Сопротивление фюзеляжа
	lpVec3 Df;
	float Df_mag;

	// Угол атаки крыла
	float wingAttackAngle_l;
	float wingAttackAngle_r;

	// Силы крыла
	lpVec3 Lw_l;
	float Lw_mag_l;
	lpVec3 Lw_r;
	float Lw_mag_r;
	lpVec3 Dw_l;
	float Dw_mag_l;
	lpVec3 Dw_r;
	float Dw_mag_r;

	// Силы оперения
	lpVec3 Lhor;
	float Lhor_mag;
	lpVec3 Dhor;
	float Dhor_mag;
	lpVec3 Lvert;
	float Lvert_mag;
	lpVec3 Dvert;
	float Dvert_mag;
	
	
	// Модуль силы тяжести
	float weightMag;

	// Аэродинамический центр крыла
	lpVec3 ac1;
	lpVec3 ac2;

	// Аэродинамический центр оперения
	lpVec3 tailPoint;


	// Вектор тяги
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