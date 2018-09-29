#include "core.h"


#include "polar.h"
#include <Windows.h>



// ������ ����������� ������
lpLaxePhysicsEngine *lpe;

Airplane ap;

// ��������� �������
float		rho;


void initPhysics()
{
	// ������������� ����������� ������

	lpe = new lpLaxePhysicsEngine();
	//lpe->setWorld(lpe->createWorldSimple(SOLVER_SEQUENTIAL));

	// ��������� ����������
	lpe->setGravity(lpVec3(0.0f, -9.80665f, 0.0f));

	// �������� �������� ���� ��������
	lpRigidBody *airplane = lpe->createRigidBody(true);

	// ��������� ����� ��������
	airplane->setMass(AIRPLANE_MASS);

	// ������� ������� �������
	lpMat3 inert = lpInertia::boxTensor(2.0f * AIRPLANE_MASS, 1.0f * AIRPLANE_LENGTH, AIRPLANE_HEIGHT, AIRPLANE_WIDTH);
	//inert.m_data[1][1] *= 1.5f;
	airplane->setInertiaTensor(inert);
	
	// ������������ ��������
	airplane->setAngularDamping(1.0f);
	airplane->setLinearDamping(1.0f);


	// ��������� �������
	airplane->setPosition(0.0f, START_HEIGHT, 12000.0f);
	
	// ��������
	airplane->m_linearVel.setTo(-START_VELOCITY, 0.f, 0.0f);
	//airplane->m_linearVel.setTo(0.0f, -20.0f, 0.0f);

	//airplane->rotate(-20.0f * M_PI / 180, lpVec3(LP_Z_AXIS));
	//airplane->rotate(90.0f * M_PI / 180, lpVec3(LP_X_AXIS));
	//airplane->m_angularVel.setTo(0.0f, 2.4f, 0.0f);


	ap.rb = airplane;

	polarInit();


	// ��������������� ������ ����� �����
	// ���������
	int n = WING_N;
	ap.wing_points = new lpVec3[WING_N];

	// ���
	float dz = (WING_LENGTH - WING_FUSELAGE_WIDTH) / n;
	lpVec3 w;
	float wl2 = 0.5f * WING_LENGTH;
	float dz2 = 0.5f * dz;
	float tv = (float)tan(WING_V_ANGLE * M_PI / 180.0f);
	float ts = (float)tan(WING_SWEPT_ANGLE * M_PI / 180.0f);

	int i;
	for (i = 0; i < n / 2; i++)
	{
		float zs = i * dz + dz2;
		w.setTo(WING_X - zs * ts, WING_Y + zs * tv, WING_FUSELAGE_WIDTH * 0.5f + zs);
		ap.wing_points[i] = w;
		w.m_z *= -1.0f;
		ap.wing_points[i + n / 2] = w;
	}
}



void calcAerodynamics()
{
	// ������ ���������������� ���

	// ��������� ������� ���������
	ap.localX.setTo(-ap.rb->m_orientation.m_data[0][0], -ap.rb->m_orientation.m_data[1][0], -ap.rb->m_orientation.m_data[2][0]);
	ap.localY.setTo(ap.rb->m_orientation.m_data[0][1], ap.rb->m_orientation.m_data[1][1], ap.rb->m_orientation.m_data[2][1]);
	ap.localZ.setTo(ap.rb->m_orientation.m_data[0][2], ap.rb->m_orientation.m_data[1][2], ap.rb->m_orientation.m_data[2][2]);

	calcAeroCenters();


	// ������ �������� � ��������� ������� ���������
	ap.V.setTo(ap.rb->m_linearVel.dot(ap.localX), ap.rb->m_linearVel.dot(ap.localY), ap.rb->m_linearVel.dot(ap.localZ));
	ap.V_mag = ap.V.norm();

	// �������� �������� �� ���������� ��������� OXY
	ap.Vxy = ap.V;
	ap.Vxy.m_z = 0.0f;
	ap.Vxy_mag = ap.Vxy.norm();

	// �������� �������� �� �������������� ��������� OXZ
	ap.Vxz = ap.V;
	ap.Vxz.m_y = 0.0f;
	ap.Vxz_mag = ap.Vxz.norm();

	
	// ���� ����� ��������
	ap.alpha = 0.0f;
	if (ap.Vxy_mag > 0.0f)
	{
		float cosa = ap.Vxy.m_x / ap.Vxy_mag;
		if (cosa > 1.0f) cosa = 1.0f;
		if (cosa < -1.0f) cosa = -1.0f;
		ap.alpha = float(acosf(cosa)) * 180.0f / float(M_PI);

		// ��� ���������� ����� ����
		if (ap.Vxy.m_y > 0.0f)
		{
			ap.alpha *= -1.0f;
		}
	}

	// ���� ���������� ��������
	ap.beta = 0.0f;
	if (ap.Vxz_mag > 0.0f)
	{
		float cosa = ap.Vxz.m_x / ap.Vxz_mag;
		if (cosa > 1.0f) cosa = 1.0f;
		if (cosa < -1.0f) cosa = -1.0f;
		ap.beta = float(acosf(cosa)) * 180.0f / float(M_PI);

		// ��� ���������� ����� ����
		if (ap.Vxz.m_z > 0.0f)
		{
			ap.beta *= -1.0f;
		}
	}


	// ����������� ��������� ���� �������������� ���������������� ������������
	lpVec3 vz(LP_Z_AXIS);
	vz = vz % ap.Vxy;
	ap.Lh = ap.localX * vz.m_x + ap.localY * vz.m_y + ap.localZ * vz.m_z;
	ap.Lh.normalize();

	// ����������� ��������� ���� ������������ ���������������� ������������
	lpVec3 vy(LP_Y_AXIS);
	vy = vy % ap.Vxz;
	ap.Lv = ap.localX * vy.m_x + ap.localY * vy.m_y + ap.localZ * vy.m_z;
	ap.Lv.normalize();

	// ����������� ����������������� �������������
	ap.D = ap.rb->m_linearVel * -1.0f;
	ap.D.normalize();


	// ������ ��������
	calcFuselage();

	// ������ �����

	ap.wing_forces = new lpVec3[WING_N * 3];

	integrateWing();
	calcWing();

	// ������ ��������������� ��������
	calcHorizontalStabiliser();

	// ������ ������������� ��������
	calcVerticalStabiliser();
}


void step(float dt, float &accTime)
{
	// ������������� ��� ������
	while (accTime > dt)
	{
		lpe->step(dt);
		accTime -= dt;
	}
}



void calcEngine()
{
	// ������������ ���� ���� ���������

	float max = THRUST_MAX;// * (rho - 0.225);
	ap.T_mag = ap.ctrls.thrust * max;
	ap.T = ap.localX * ap.T_mag;
	ap.rb->applyForce(ap.T);
}

void calcPhysics()
{
	
	// ������ ���� �������
	ap.weightMag = fabsf(ap.rb->m_mass * ap.rb->m_gravity.m_y);

	calcAirDensity();

	calcAerodynamics();
	calcEngine();
	ap.rb->calcInverseInertiaTensorWorld();
}


void calcFuselage()
{
	// ������ �������� ������������� ��������
	// ������������, ��� ������� ��������� ���� �� �������

	// ������������ ������� ������������ �������������

	float s = float(M_PI) * MIDLE_DIAMETER * MIDLE_DIAMETER * 0.25f;
	float Cx = 0.1f + (ap.alpha / 100.0f) * (ap.alpha / 100.0f);
	ap.Df_mag = 0.5f * Cx * rho * s * ap.V_mag * ap.V_mag;
	ap.Df = ap.D * ap.Df_mag;
	
	ap.rb->applyForce(ap.Df);
}


void calcWing()
{
	// ������ ������������ �������� ����������� (����� ������� ��������� ������ ��� X ���� ����� ������� ���������� ������)

	// �������� ������ �������� � ��������� �����������
	lpVec3 omegax(ap.rb->m_angularVel.dot(ap.localX), ap.rb->m_angularVel.dot(ap.localY), ap.rb->m_angularVel.dot(ap.localZ));
	// ����� ������ �� �����
	lpVec3 d(FOCUS_WING_X, FOCUS_WING_Y, FOCUS_WING_Z);

	// ������������ ��������
	lpVec3 u_r = omegax % d;
	lpVec3 u_l = u_r * -1.0f;
	u_r.m_z = 0.0f;
	u_l.m_z = 0.0f;


	// ���������� � �������������� ���������
	lpVec3 v_l = ap.Vxy + u_l;
	lpVec3 v_r = ap.Vxy + u_r;

	// ������
	float v_l_mag = v_l.norm();
	float v_r_mag = v_r.norm();


	lpVec3 vz(LP_Z_AXIS);
	lpVec3 l_l = vz % v_l;
	l_l = ap.localX * l_l.m_x + ap.localY * l_l.m_y + ap.localZ * l_l.m_z;
	l_l.normalize();
	lpVec3 l_r = vz % v_r;
	l_r = ap.localX * l_r.m_x + ap.localY * l_r.m_y + ap.localZ * l_r.m_z;
	l_r.normalize();

	// ����������� ����������������� �������������
	lpVec3 d_l = v_l * -1.0f;
	d_l = ap.localX * d_l.m_x + ap.localY * d_l.m_y + ap.localZ * d_l.m_z;
	d_l.normalize();
	lpVec3 d_r = v_r * -1.0f;
	d_r = ap.localX * d_r.m_x + ap.localY * d_r.m_y + ap.localZ * d_r.m_z;
	d_r.normalize();



	// ���� ����� �����������
	// ������
	ap.wingAttackAngle_l = 0.0f;
	if (v_l_mag > 0.0f)
	{
		float cosa = v_l.m_x / v_l_mag;
		if (cosa > 1.0f) cosa = 1.0f;
		if (cosa < -1.0f) cosa = -1.0f;
		ap.wingAttackAngle_l = float(acosf(cosa)) * 180.0f / float(M_PI);

		// ��� ���������� ����� ����
		if (v_l.m_y > 0.0f)
		{
			ap.wingAttackAngle_l *= -1.0f;
		}
	}

	// �������
	ap.wingAttackAngle_r = 0.0f;
	if (v_r_mag > 0.0f)
	{
		float cosa = v_r.m_x / v_r_mag;
		if (cosa > 1.0f) cosa = 1.0f;
		if (cosa < -1.0f) cosa = -1.0f;
		ap.wingAttackAngle_r = float(acosf(cosa)) * 180.0f / float(M_PI);

		// ��� ���������� ����� ����
		if (v_r.m_y > 0.0f)
		{
			ap.wingAttackAngle_r *= -1.0f;
		}
	}




	// ������� ��������������� ������������ ��������� ���� � ������������� �� ������ ��� ������ � ������� �����������
	float Cx_l, Cy_l;
	float Cx_r, Cy_r;

	ap.wingAttackAngle_l += WING_ANGLE;
	ap.wingAttackAngle_r += WING_ANGLE;

	getAeroCoeffs(ap.wingAttackAngle_l, Cx_l, Cy_l);
	getAeroCoeffs(ap.wingAttackAngle_r, Cx_r, Cy_r);

	ap.Lw_mag_l = 0;// 0.5f * 0.5f *	Cy_l * rho * WING_AREA * ap.Vxy_mag * ap.Vxy_mag;
	ap.Dw_mag_l = 0;// 0.5f * 0.5f * Cx_l * rho * WING_AREA * ap.Vxy_mag * ap.Vxy_mag;

	ap.Lw_mag_r = 0;// 0.5f * 0.5f * Cy_r * rho * WING_AREA * ap.Vxy_mag * ap.Vxy_mag;
	ap.Dw_mag_r = 0;// 0.5f * 0.5f * Cx_r * rho * WING_AREA * ap.Vxy_mag * ap.Vxy_mag;

	if (ap.ctrls.aileron != 0.0f)
	{
		// �������

		float aileron_angle_l = ap.wingAttackAngle_l - AILERON_MAX_ANGLE * ap.ctrls.aileron;
		float aileron_angle_r = ap.wingAttackAngle_r + AILERON_MAX_ANGLE * ap.ctrls.aileron;

		getAeroCoeffs(aileron_angle_l, Cx_l, Cy_l);
		getAeroCoeffs(aileron_angle_r, Cx_r, Cy_r);


		float lm = 0.5f * Cy_l * rho * 1.5f * v_l_mag * v_l_mag;
		float dm = 0.5f * Cx_l * rho * 1.5f * v_l_mag * v_l_mag;
		//lm = dm = 0;
		ap.Lw_mag_l = lm;
		ap.Dw_mag_l = dm;

		lm = 0.5f * Cy_r * rho * 1.5f * v_r_mag * v_r_mag;
		dm = 0.5f * Cx_r * rho * 1.5f * v_r_mag * v_r_mag;

		ap.Lw_mag_r = lm;
		ap.Dw_mag_r = dm;
	}


	ap.Lw_l = l_l * ap.Lw_mag_l;
	ap.Lw_r = l_r * ap.Lw_mag_r;

	ap.Dw_l = d_l * ap.Dw_mag_l;
	ap.Dw_r = d_r * ap.Dw_mag_r;


	lpVec3 f_l = ap.rb->m_pos + ap.localX * FOCUS_WING_X + ap.localY * FOCUS_WING_Y + ap.localZ * FOCUS_WING_Z;
	lpVec3 f_r = ap.rb->m_pos + ap.localX * FOCUS_WING_X + ap.localY * FOCUS_WING_Y - ap.localZ * FOCUS_WING_Z;

	ap.rb->applyForceAtPoint(ap.Lw_l, f_l);
	ap.rb->applyForceAtPoint(ap.Lw_r, f_r);

	ap.rb->applyForceAtPoint(ap.Dw_l, f_l);
	ap.rb->applyForceAtPoint(ap.Dw_r, f_r);
}



void integrateWing()
{
	// ������� �������� � ��������� �������
	lpVec3 omega(ap.rb->m_angularVel.dot(ap.localX), ap.rb->m_angularVel.dot(ap.localY), ap.rb->m_angularVel.dot(ap.localZ));

	// ������� �������
	float ds = WING_AREA / WING_N;
	lpVec3 w;

	// ����������� ��������� ���� ����� � ������ ���� ����������� V
	float vangle = WING_V_ANGLE * float(M_PI) / 180;
	lpVec3 l_l(0.0f, sin(vangle), cos(vangle));
	lpVec3 l_r = l_l;
	l_r.m_z *= -1.0f;


	// �������� ���� � �������
	lpVec3 l_sum, d_sum, Ml_sum, Md_sum;
	l_sum.clear();
	d_sum.clear();
	Ml_sum.clear();
	Md_sum.clear();


	for (int i = 0; i < WING_N; i++)
	{
		// ����� ����� �� ����� ����� �� ������� ������������
		w = ap.wing_points[i];

		// �������� �������� ������� �� ����� ��������� ���������
		lpVec3 u = w % omega;
		
		// ���������� � �������������� ��������� � ��������
		lpVec3 v = ap.Vxy + u;
		v.m_z = 0.0f;

		// ����������� ����������������� �������������
		lpVec3 d = (ap.V + u) * -1.0f;
		d = ap.localX * d.m_x + ap.localY * d.m_y + ap.localZ * d.m_z;
		d.normalize();

		

		// ������
		float v_mag = v.norm();

		lpVec3 l;
		if (w.m_z > 0.0f) l = l_l % v;
		else l = v % l_r;
		l = ap.localX * l.m_x + ap.localY * l.m_y + ap.localZ * l.m_z;
		l.normalize();

		

		// ���� ����� ������� �����
		ap.wingAttackAngle_l = 0.0f;
		if (v_mag > 0.0f)
		{
			float cosa = v.m_x / v_mag;
			if (cosa > 1.0f) cosa = 1.0f;
			if (cosa < -1.0f) cosa = -1.0f;
			ap.wingAttackAngle_l = acosf(cosa) * 180.0f / float(M_PI);

			// ��� ���������� ����� ����
			if (v.m_y > 0.0f)
			{
				ap.wingAttackAngle_l *= -1.0f;
			}
		}

		float Cx, Cy;

		ap.wingAttackAngle_l += WING_ANGLE;

		getAeroCoeffs(ap.wingAttackAngle_l, Cx, Cy);

		float l_mag = 0.5f * rho * Cy * ds * v_mag * v_mag;
		float d_mag = 0.5f * rho * Cx * ds * v_mag * v_mag;

		lpVec3 lp = l * l_mag;
		lpVec3 dp = d * d_mag;

		l_sum += lp;
		d_sum += dp;

		w = ap.localX * w.m_x + ap.localY * w.m_y + ap.localZ * w.m_z;

		Ml_sum += w % lp;
		Md_sum += w % dp;

		ap.wing_forces[3 * i]	  = w + ap.rb->m_pos;
		ap.wing_forces[3 * i + 1] = lp * 0.001f + ap.wing_forces[3 * i];
		ap.wing_forces[3 * i + 2] = dp * 0.001f + ap.wing_forces[3 * i];
	}
	

	ap.rb->applyForce(l_sum);
	ap.rb->applyForce(d_sum);
	ap.rb->applyTorque(Ml_sum);
	ap.rb->applyTorque(Md_sum);

}

void calcHorizontalStabiliser()
{
	// �������������� ������������

	// ������� ��������������� ������������ ��������� ���� � ������������� �� ������
	float Cx, Cy;
	float angle = ap.alpha + HOR_STABILISER_ANGLE;
	getAeroCoeffs(angle, Cx, Cy);

	ap.Lhor_mag = 0.5f * Cy * rho * HOR_STABILISER_AREA * ap.Vxy_mag * ap.Vxy_mag;
	ap.Dhor_mag = 0.5f * Cx * rho * HOR_STABILISER_AREA * ap.Vxy_mag * ap.Vxy_mag;

	if (ap.ctrls.elevator != 0.0f)
	{
		// ���� ������
		float elev_angle = angle + ELEVATOR_MAX_ANGLE * ap.ctrls.elevator;
		getAeroCoeffs(elev_angle, Cx, Cy);

		ap.Lhor_mag += 0.5f * Cy * rho * 30.0f * ap.Vxy_mag * ap.Vxy_mag;
		ap.Dhor_mag += 0.5f * Cx * rho * 30.0f * ap.Vxy_mag * ap.Vxy_mag;
	}

	ap.Lhor = ap.Lh * ap.Lhor_mag;
	ap.Dhor = ap.D * ap.Dhor_mag;

	lpVec3 f = ap.rb->m_pos + ap.localX * FOCUS_STAB_X + ap.localY * FOCUS_STAB_Y + ap.localZ * FOCUS_STAB_Z;
	ap.rb->applyForceAtPoint(ap.Lhor, f);
	ap.rb->applyForceAtPoint(ap.Dhor, f);
}


void calcVerticalStabiliser()
{
	// ������������ ������������

	// ������� ��������������� ������������ ��������� ���� � ������������� �� ������
	float Cx, Cy;

	// ���������� � ���� ���������� ���� ������� ��������� ���� �������, ��� ��� 0 ���������� ���� ������� ��������� ����
	// ��� ����� � ������ ��������������� �������
	float angle = ap.beta + polar.a0;

	getAeroCoeffs(angle, Cx, Cy);

	ap.Lvert_mag = 0.5f * Cy * rho * VERT_STABILISER_AREA * ap.Vxz_mag * ap.Vxz_mag;
	ap.Dvert_mag = 0.5f * Cx * rho * VERT_STABILISER_AREA * ap.Vxz_mag * ap.Vxz_mag;
	
	//ap.ctrls.rudder = 0.5f;
	if (ap.ctrls.rudder != 0.0f)
	{
		// ���� �����������
		float rdr_angle = angle + RUDDER_MAX_ANGLE * ap.ctrls.rudder;
		getAeroCoeffs(rdr_angle, Cx, Cy);

		ap.Lvert_mag += 0.5f * Cy * rho * 3.0f * ap.Vxz_mag * ap.Vxz_mag;
		ap.Dvert_mag += 0.5f * Cx * rho * 3.0f * ap.Vxz_mag * ap.Vxz_mag;
	}

	ap.Lvert = ap.Lv * -ap.Lvert_mag;
	ap.Dvert = ap.D	 * ap.Dvert_mag;

	lpVec3 f = ap.rb->m_pos + ap.localX * FOCUS_STAB_X + ap.localY * FOCUS_STAB_Y + ap.localZ * FOCUS_STAB_Z;

	ap.rb->applyForceAtPoint(ap.Lvert, f);
	ap.rb->applyForceAtPoint(ap.Dvert, f);
}


void calcControlAeroSurfaces()
{
	// ����������� �����������
	
}



void calcAeroCenters()
{
	// ���������������� ����� �����
	ap.ac1 = ap.rb->m_pos + ap.localX * FOCUS_WING_X + ap.localY * FOCUS_WING_Y + ap.localZ * FOCUS_WING_Z;
	ap.ac2 = ap.rb->m_pos + ap.localX * FOCUS_WING_X + ap.localY * FOCUS_WING_Y - ap.localZ * FOCUS_WING_Z;

	// ���������������� ����� ��������
	ap.tailPoint = ap.rb->m_pos + ap.localX * FOCUS_STAB_X + ap.localY * FOCUS_STAB_Y + ap.localZ * FOCUS_STAB_Z;
}


void calcAirDensity()
{
	// ����������� ����������� �� ������ ���� T0
	float T0 = 288.15f;
	// �������� ������� ����������� � �������, � �������� ����������
	float L = 0.0065f;
	// ��������� ���������� ������� ��� ������������ �����
	float g = -ap.rb->m_gravity.m_y;
	// ����������� ����������� �������� �� ������ ����
	float p0 = 101325; 
	// ������������� ������� ����������
	float R = 8.31447f;
	// �������� ����� ������ �������
	float M = 0.0289644f;
	// ������
	float h = fabsf(ap.rb->m_pos.m_y);

	float T = T0 - L * h;
	float p = p0 * pow(1 - (L * h / T0), g * M / (R * L));
	rho = p * M / (R * T);
}