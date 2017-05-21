#pragma once

#define COND(cond) cond
#define Assert(cond, text) if (COND(cond)) {} else { printf("Assert: %s", text); __debugbreak(); }

#define _USE_MATH_DEFINES
#include <math.h> // fabs
#include <cmath>
#include <cfloat> // For FLT_EPSILON

#include <cassert>
#include <string> // For std::ostream

typedef float Real;

const Real g_degToRad = (Real)M_PI / 180.f;

#include <Vector3.h>
#include <Matrix3.h>
#include <Matrix.h>

#include <physicsObject.h>
