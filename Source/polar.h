#pragma once

int polar_len = 16;

// Углы атаки
float polar_angles[] = {-3.0f, -2.0f,   0.0f,   2.0f,   4.0f,   6.0f,  8.0f,  10.0f,   12.0f, 14.0f,  16.0f,  18.0f, 20.0f,  22.0f,  23.0f,  24.0f};

// Коэффициент подемной силы
float polar_cy[] = {    0.0f,  0.05f,  0.2f,   0.35f,  0.5f,   0.65f, 0.78f,  0.93f,  1.08f, 1.2f,   1.35f,  1.43f, 1.46f,  1.4f,   1.35f,  1.2f};

// Коэффициент сопротивления
float polar_cx[] = {    0.01f, 0.01f,  0.012f, 0.02f,  0.028f, 0.04f, 0.048f, 0.073f, 0.1f,   0.12f,  0.15f, 0.17f, 0.2f,   0.23f,  0.27f,  0.4f};





struct Polar
{
	float a0;
	float acr;
	float ast;

	float cx0;

	float cymax;
	float cyst;
};

Polar polar;

int polar_len2 = 16;
float polar2_angles[] = {15.0f,		16.0f,		17.0f,		18.0f,		19.0f,		20.0f,		21.0f,		22.0f,		23.0f,		24.0f,		25.0f,		26.0f,		27.0f,		28.0f,		 29.0f,			30.0f};

// Коэффициент подемной силы
float polar2_cy[] = {   1.29999f,   1.37f,      1.425f,		1.44f,		1.425f,		1.37f,		1.25f,		1.12f,		1.0f,		0.9f,		0.82f,		0.76f,		0.7f,		0.65f,		0.62f,			0.6f };


void polarInit()
{
	polar.a0 = -2.0f;
	polar.ast = 15.0f;
	polar.acr = 18.0f;
	polar.cx0 = 0.024f;
	polar.cymax = 1.44f;
	polar.cyst = 1.3f;
}


void getAeroCoeffs(float a, float &cx, float &cy)
{
	bool symmetricHalf = false;
	if (a < polar.a0)
	{
		a = polar.a0 + (polar.a0 - a);
		symmetricHalf = true;
	}


	cx = (a - polar.a0) / 55.0f;
	cx = cx * cx + polar.cx0;
	if (cx > 1.1f) cx = 1.1f;

	float k;
	if (a <= polar.ast)
	{
		k = polar.cyst / (polar.ast - polar.a0);
		cy = k * (a - polar.a0);

	}
	else if (a > 30)
	{
		cy = 0.6f - (a - 30) * 0.01f;
	}
	else
	{
		int i;
		for (i = 0; i < polar_len2; i++)
		{
			if (a >= polar2_angles[i] && a < polar2_angles[i + 1]) break; 
		}
	 
		k = (a - polar2_angles[i]) / (polar2_angles[i + 1] - polar2_angles[i]);
		cy = polar2_cy[i] + (polar2_cy[i + 1] - polar2_cy[i]) * k;
	}




	if (symmetricHalf) cy *= -1.0f;
}




void getAeroCoeffs2(float a, float &cx, float &cy)
{
	bool symmetricHalf = false;
	if (a < polar_angles[0])
	{
		a = polar_angles[0] + (polar_angles[0] - a);
		symmetricHalf = true;
	}

	if (a > 24.0f) 
	{
		cx = polar_cx[polar_len - 1] + 0.13f * (a - polar_angles[polar_len - 1]);
		cy = polar_cy[polar_len - 1] - 0.01f * (a - polar_angles[polar_len - 1]);
		if (cy < 0.0f) cy = 0.0f;
	}
	else
	{
		int i;
		for (i = 0; i < polar_len; i++)
		{
			if (a >= polar_angles[i] && a < polar_angles[i + 1]) break; 
		}
	 
		float s = (a - polar_angles[i]) / (polar_angles[i + 1] - polar_angles[i]);
		cx = polar_cx[i] + (polar_cx[i + 1] - polar_cx[i]) * s;
		cy = polar_cy[i] + (polar_cy[i + 1] - polar_cy[i]) * s;
	}

	if (symmetricHalf) cy *= -1.0f;
}