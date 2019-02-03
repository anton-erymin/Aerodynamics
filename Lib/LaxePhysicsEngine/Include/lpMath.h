#pragma once

#include "export.h"

#define _USE_MATH_DEFINES
#include <math.h>

class lpMat3;

// Вектор в трехмерном пространстве
class DLL_EXPORT lpVec3
{
public:

	// x-компонента вектора
	float m_x;

	// y-компонента вектора
	float m_y;

	// z-компонента вектора
	float m_z;


	lpVec3();
	lpVec3(float x, float y, float z);
	lpVec3(float s);
	lpVec3(float *v);

	void		clear();
	void		setTo(float x, float y, float z);

	void		add(const lpVec3 &other);
	lpVec3		addR(const lpVec3 &other);
	void		sub(const lpVec3 &other);
	lpVec3		subR(const lpVec3 &other);
	void 		scale(float s);
	lpVec3		scaleR(float s);
	void		addScaled(const lpVec3 &other, float s);
	lpVec3		addScaledR(const lpVec3 &other, float s);

	float		dot(const lpVec3 &other);
	void		cross(const lpVec3 &other);
	lpVec3		crossR(const lpVec3 &other);

	float		norm();
	float		normSq();
	void		normalize();

	lpVec3	operator+(const lpVec3 &other);
	void	operator+=(const lpVec3 &other);

	lpVec3	operator-(const lpVec3 &other);
	void	operator-=(const lpVec3 &other);

	float   operator*(const lpVec3 &other);
	lpVec3  operator*(float s);
	void	operator*=(float s);
	void	operator*=(const lpMat3 &mat);

	lpVec3	operator%(const lpVec3 &other);
	void	operator%=(const lpVec3 &other);

};



// Матрица 3x3
class DLL_EXPORT lpMat3
{
public:

	// Элементы матрицы
	float m_data[3][3];


	lpMat3();
	lpMat3(float s);
	lpMat3(float **v);

	void		clear();
	void		identity();

	void		add(const lpMat3 &other);
	lpMat3		addR(const lpMat3 &other);
	void		sub(const lpMat3 &other);
	lpMat3		subR(const lpMat3 &other);
	void 		scale(float s);
	lpMat3		scaleR(float s);
	void		addScaled(const lpMat3 &other, float s);
	lpMat3		addScaledR(const lpMat3 &other, float s);

	void		transpose();

	lpVec3		multTransposed(const lpVec3 &vec);
	void		multLeft(const lpMat3 &other);

	void		rotate(float angle, const lpVec3 &axis);
	
	void		invert();

	lpMat3	operator+(const lpMat3 &other);
	void	operator+=(const lpMat3 &other);

	lpMat3	operator-(const lpMat3 &other);
	void	operator-=(const lpMat3 &other);

	lpVec3  operator*(const lpVec3 &vec);
	lpMat3  operator*(const lpMat3 &other);
	void	operator*=(const lpMat3 &other);

	lpMat3  operator~();
};