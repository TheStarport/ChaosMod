//////////////////////////////////////////////////////////////////////
//	Project FLCoreSDK v1.1, modified for use in FLHook Plugin version
//--------------------------
//
//	File:			FLCoreDefs.h
//	Module:			
//	Description:	Common declarations
//
//	Web: www.skif.be/flcoresdk.php
//  
//
//////////////////////////////////////////////////////////////////////
#ifndef _FLCOREDEFS_H_
#define _FLCOREDEFS_H_
#pragma warning(disable: 4251 4002 4099)
#define IMPORT _declspec(dllimport)
#define EXPORT _declspec(dllexport)
#pragma warning(push)
#pragma warning(disable:4244)

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <vector>
#include <map>
#include <list>
#include <string>

#define M_PI 3.14159265358979323846

#define OBJECT_DATA_SIZE	2048
#define POPUPDIALOG_BUTTONS_LEFT_YES 1
#define POPUPDIALOG_BUTTONS_CENTER_NO 2
#define POPUPDIALOG_BUTTONS_LEFT_YES_CENTER_NO 3
#define POPUPDIALOG_BUTTONS_RIGHT_LATER 4
#define POPUPDIALOG_BUTTONS_RIGHT_LATER_LEFT_YES 5
#define POPUPDIALOG_BUTTONS_RIGHT_LATER_CENTER_NO 6
#define POPUPDIALOG_BUTTONS_RIGHT_LATER_LEFT_YES_CENTER_NO 7
#define POPUPDIALOG_BUTTONS_CENTER_OK 8
#define POPUPDIALOG_BUTTONS_CENTER_OK_LEFT_YES 9
#define POPUPDIALOG_BUTTONS_CENTER_OK_RIGHT_LATER 12
#define POPUPDIALOG_BUTTONS_CENTER_OK_LEFT_YES_RIGHT_LATER 13

#undef min
#undef max

template <int size> struct TString
{
	int len;
	char data[size + 1];

	TString() :len(0) { data[0] = 0; }
};

class Quaternion;
class Vector
{
public:
	float x, y, z;
};

class Matrix
{
public:
	union
	{
        float data[3][3];
		struct
		{
            float y1, y2, y3, x1, x2, x3, z1, z2, z3;
		};

		struct
		{
			float
			xx, yx, zx,
			xy, yy, zy,
			xz, yz, zz;
		};

		struct
		{
			float pitch1, pitch2, pitch3, yaw1, yaw2, yaw3, roll1, roll2, roll3;
		};
	};
};

class Quaternion
{
public:
	float w, x, y, z;
};

using uint = unsigned int;
using ushort = unsigned short;
using ulong = unsigned long;
using uchar = unsigned char;

#pragma warning(pop)
#endif // _FLCOREDEFS_H_
