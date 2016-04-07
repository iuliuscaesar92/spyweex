//#pragma once
//#ifdef NOMINMAX
//#include <windows.h>
//#else
//#define NOMINMAX
//#include <windows.h>
//#undef NOMINMAX
//#endif
#define NOMINMAX
#include <algorithm>
#include <windows.h>
#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif;
#include <gdiplus.h>
#undef min
#undef max
