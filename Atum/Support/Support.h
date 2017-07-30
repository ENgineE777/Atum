
#pragma once

#include <string>
#include <string.h>
#include <vector>

using namespace std;

#define RADIAN  0.01745329f
#define PI      3.14159265f
#define TWO_PI  6.28318530f
#define HALF_PI 1.57079632f

#define rnd() ((float)rand()/RAND_MAX)
#define rnd_range(a, b) (a + (b - a) * (float)rand()/RAND_MAX)

#define RELEASE(p) if (p) { p->Release(); p = NULL; }
#define DELETE(p) if (p) { delete p; p = NULL; }
#define DELETE_ARRAY(p) if (p) { delete[] p; p = NULL; }

#define _FL_ __FILE__ , __LINE__

typedef unsigned char	byte;
typedef signed short	int16;
typedef unsigned short	word;

#ifdef __LP64__
typedef unsigned int  dword;
#else
typedef unsigned long dword;
#endif

typedef signed long long int	int64;
typedef unsigned long long int	uint64;

#include "Color.h"
#include "Vector2.h"
#include "Vector.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Buffer.h"
#include "Delegate.h"
#include "Quaternion.h"
#include "StringUtils.h"
