
#pragma once

#define RADIAN  0.01745329f
#define PI      3.14159265f
#define TWO_PI  6.28318530f
#define HALF_PI 1.57079632f

#define rnd() ((float)rand()/RAND_MAX)
#define rnd_range(a, b) (a + (b - a) * (float)rand()/RAND_MAX)

#define RELEASE(p) if (p) { p->Release(); p = nullptr; }
#define FREE_PTR(p) if (p) { free(p); p = nullptr; }
#define DELETE_PTR(p) if (p) { delete p; p = nullptr; }
#define DELETE_ARRAY(p) if (p) { delete[] p; p = nullptr; }

#ifdef PLATFORM_PC
#define DEBUG_BREAK __debugbreak();
#endif

#ifdef EDITOR
#define MESSAGE_BOX(caption, text) MessageBox(nullptr, text, caption, MB_ICONERROR);
#else
#define MESSAGE_BOX(caption, text)
#endif

#define _FL_ __FILE__ , __LINE__