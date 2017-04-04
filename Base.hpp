#pragma once

#define COND(cond) cond
#define Assert(cond, text) if (COND(cond)) {} else {__debugbreak(); printf(text);}

#define _USE_MATH_DEFINES
#include <math.h> // fabs
#include <cmath>
#include <cfloat> // For FLT_EPSILON

#include <cassert>
#include <string> // For std::ostream

typedef float Real;

const Real g_degToRad = (Real)M_PI / 180.f;

#include <Vector3.hpp>
#include <Matrix3.hpp>
#include <Matrix.hpp>

typedef Vector3 Point3;
typedef Vector3 Point3fParameter;

#include <physicsObject.hpp>
