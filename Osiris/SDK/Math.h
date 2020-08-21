#pragma once
#include <cassert>

#include "Vector.h"

constexpr float M_PIF = static_cast<float>(M_PI);

namespace Math {

	inline Vector RotatePoint(Vector EntityPos, Vector LocalPlayerPos, float posX, float posY, float sizeX, float sizeY, float angle, float zoom, bool* viewCheck, bool angleInRadians = false) noexcept
	{
		float r_1, r_2;
		float x_1, y_1;
		r_1 = -(EntityPos.y - LocalPlayerPos.y);
		r_2 = EntityPos.x - LocalPlayerPos.x;
		float Yaw = angle - 90.0f;
		float yawToRadian = Yaw * (float)(M_PIF / 180.0F);
		x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
		y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;
		*viewCheck = y_1 < 0;
		x_1 *= zoom;
		y_1 *= zoom;
		float sizX = sizeX / 2.0f;
		float sizY = sizeY / 2.0f;
		x_1 += sizX;
		y_1 += sizY;
		if (x_1 < 5)
			x_1 = 5;
		if (x_1 > sizeX - 5)
			x_1 = sizeX - 5;
		if (y_1 < 5)
			y_1 = 5;
		if (y_1 > sizeY - 5)
			y_1 = sizeY - 5;
		x_1 += posX;
		y_1 += posY;
		return Vector{ x_1, y_1 ,0.0f };
		return Vector{ x_1, y_1 ,0.0f };
	}

	inline float mpicthis(float t_this)
	{
		return static_cast<float>(M_PIF / 180.0f) * t_this;
	}

	inline void sinCos2(float radians, PFLOAT sine, PFLOAT cosine)
	{
		__asm
		{
			fld dword ptr[radians]
			fsincos
			mov edx, dword ptr[cosine]
			mov eax, dword ptr[sine]
			fstp dword ptr[edx]
			fstp dword ptr[eax]
		}
	}

	inline double deg2rad2 (double degrees) {
		return degrees * 4.0 * atan (1.0) / 180.0;
	}
	
	inline void AngleVectorsFast(const Vector& angles, Vector* f)
	{
		float sp, sy, sr, cp, cy, cr;

		sinCos2(deg2rad2(angles.x), &sp, &cp);
		sinCos2(deg2rad2(angles.y), &sy, &cy);
		sinCos2(deg2rad2(angles.z), &sr, &cr);

		f->x = cp * cy;
		f->y = cp * sy;
		f->z = -sp;
	}


	inline void AngleVectors(const Vector& angles, Vector* forward) noexcept
	{
		//assert(s_bMathlibInitialized);
		assert(forward);

		float	sp, sy, cp, cy;

		sy = sin(Math::mpicthis(angles.y));
		cy = cos(Math::mpicthis(angles.y));

		sp = sin(Math::mpicthis(angles.x));
		cp = cos(Math::mpicthis(angles.x));

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	inline	Vector CalcAngle(const Vector& vecSource, const Vector& vecDestination)
	{
		Vector qAngles;
		Vector delta;
		delta.x = vecSource.x - vecDestination.x;
		delta.y = vecSource.y - vecDestination.y;
		delta.z = vecSource.z - vecDestination.z;
		double hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);
		qAngles.x = (float)(atan(delta.z / hyp) * (180.0 / static_cast<float>(M_PIF)));
		qAngles.y = (float)(atan(delta.y / delta.x) * (180.0 / static_cast<float>(M_PIF)));
		qAngles.z = 0.f;
		if (delta.x >= 0.f)
			qAngles.y += 180.f;

		return qAngles;
	}

	typedef float Vector_t;
	inline Vector_t VectorNormalize(Vector& v)
	{
		Vector_t l = v.length();

		if (l != 0.0f)
		{
			v /= l;
		}
		else
		{
			v.x = v.y = 0.0f; v.z = 1.0f;
		}

		return l;
	}

	inline	void VectorAngles(const Vector& vecForward, Vector& vecAngles)
	{
		Vector vecView;
		if (vecForward.y == 0.f && vecForward.x == 0.f)
		{
			vecView.x = 0.f;
			vecView.y = 0.f;
		}
		else
		{
			vecView.y = atan2(vecForward.y, vecForward.x) * 180.f / 3.14159265358979323846f;

			if (vecView.y < 0.f)
				vecView.y += 360.f;

			vecView.z = sqrt(vecForward.x * vecForward.x + vecForward.y * vecForward.y);

			vecView.x = atan2(vecForward.z, vecView.z) * 180.f / 3.14159265358979323846f;
		}

		vecAngles.x = -vecView.x;
		vecAngles.y = vecView.y;
		vecAngles.z = 0.f;
	}

	inline	void SetRandomSeed(int seed)
	{
		static auto random_seed = reinterpret_cast<void(*)(int)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"));

		random_seed(seed);
	}

	inline float RandomFloat(float min, float max)
	{
		static auto random_float = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));

		return random_float(min, max);
	}

	inline void SinCos(float radian, float* sin, float* cos)
	{
		*sin = std::sin(radian);
		*cos = std::cos(radian);
	}

	inline void AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
	{
		float sp, sy, sr, cp, cy, cr;

		SinCos(Math::mpicthis(angles.x), &sp, &cp);
		SinCos(Math::mpicthis(angles.y), &sy, &cy);
		SinCos(Math::mpicthis(angles.z), &sr, &cr);

		if (forward != nullptr)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right != nullptr)
		{
			right->x = -1 * sr * sp * cy + -1 * cr * -sy;
			right->y = -1 * sr * sp * sy + -1 * cr * cy;
			right->z = -1 * sr * cp;
		}

		if (up != nullptr)
		{
			up->x = cr * sp * cy + -sr * -sy;
			up->y = cr * sp * sy + -sr * cy;
			up->z = cr * cp;
		}
	}

}
