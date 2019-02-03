#pragma once

#include "export.h"

#include "lpMath.h"


class DLL_EXPORT lpInertia
{
public:
	static lpMat3 boxTensor(float mass, float width, float height, float depth);
	static lpMat3 sphereSolidTensor(float mass, float radius);
	static lpMat3 sphereShellTensor(float mass, float radius);
};