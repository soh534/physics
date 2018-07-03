#pragma once

#define COND(cond) cond

#if defined _DEBUG
#define Assert(cond, text) if (COND(cond)) {} else { printf("Assert: %s", text); __debugbreak(); }
#else
#define Assert(cond, text)
#endif

#define _USE_MATH_DEFINES
#include <math.h> // fabs
#include <cmath>

#include <string> // For std::ostream

typedef float Real;

const Real g_degToRad = (Real)M_PI / 180.f; // TODO: get rid of this awfulness

// Math
#include <Common/Vector4.h>
#include <Common/Transform.h>
#include <Common/Matrix.h>

// Container
#include <Common/ArrayFreeList.h>