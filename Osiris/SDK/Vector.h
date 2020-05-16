#pragma once

#include <cmath>

#include "matrix3x4.h"
#include <cmath>
#include <xmmintrin.h>
#include <pmmintrin.h>

static const float invtwopi = 0.1591549f;
static const float twopi = 6.283185f;
static const float threehalfpi = 4.7123889f;
static const float pi = 3.141593f;
static const float halfpi = 1.570796f;
static const __m128 signmask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

static const __declspec(align(16)) float null[4] = { 0.f, 0.f, 0.f, 0.f };
static const __declspec(align(16)) float _pi2[4] = { 1.5707963267948966192f, 1.5707963267948966192f, 1.5707963267948966192f, 1.5707963267948966192f };
static const __declspec(align(16)) float _pi[4] = { 3.141592653589793238f, 3.141592653589793238f, 3.141592653589793238f, 3.141592653589793238f };

typedef __declspec(align(16)) union
{
	float f[4];
	__m128 v;
} m128;

__forceinline __m128 sqrt_ps(const __m128 squared)
{
	return _mm_sqrt_ps(squared);
}

__forceinline __m128 cos_52s_ps(const __m128 x)
{
	const auto c1 = _mm_set1_ps(0.9999932946f);
	const auto c2 = _mm_set1_ps(-0.4999124376f);
	const auto c3 = _mm_set1_ps(0.0414877472f);
	const auto c4 = _mm_set1_ps(-0.0012712095f);
	const auto x2 = _mm_mul_ps(x, x);
	return _mm_add_ps(c1, _mm_mul_ps(x2, _mm_add_ps(c2, _mm_mul_ps(x2, _mm_add_ps(c3, _mm_mul_ps(c4, x2))))));
}

__forceinline __m128 cos_ps(__m128 angle)
{
	angle = _mm_andnot_ps(signmask, angle);
	angle = _mm_sub_ps(angle, _mm_mul_ps(_mm_cvtepi32_ps(_mm_cvttps_epi32(_mm_mul_ps(angle, _mm_set1_ps(invtwopi)))), _mm_set1_ps(twopi)));

	auto cosangle = angle;
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(pi), angle))));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(pi)), signmask));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(threehalfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(twopi), angle))));

	auto result = cos_52s_ps(cosangle);
	result = _mm_xor_ps(result, _mm_and_ps(_mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_cmplt_ps(angle, _mm_set1_ps(threehalfpi))), signmask));
	return result;
}

__forceinline __m128 sin_ps(const __m128 angle)
{
	return cos_ps(_mm_sub_ps(_mm_set1_ps(halfpi), angle));
}

__forceinline void sincos_ps(__m128 angle, __m128* sin, __m128* cos) {
	const auto anglesign = _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(signmask, angle));
	angle = _mm_andnot_ps(signmask, angle);
	angle = _mm_sub_ps(angle, _mm_mul_ps(_mm_cvtepi32_ps(_mm_cvttps_epi32(_mm_mul_ps(angle, _mm_set1_ps(invtwopi)))), _mm_set1_ps(twopi)));

	auto cosangle = angle;
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(pi), angle))));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(pi)), signmask));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(threehalfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(twopi), angle))));

	auto result = cos_52s_ps(cosangle);
	result = _mm_xor_ps(result, _mm_and_ps(_mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_cmplt_ps(angle, _mm_set1_ps(threehalfpi))), signmask));
	*cos = result;

	const auto sinmultiplier = _mm_mul_ps(anglesign, _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(_mm_cmpgt_ps(angle, _mm_set1_ps(pi)), signmask)));
	*sin = _mm_mul_ps(sinmultiplier, sqrt_ps(_mm_sub_ps(_mm_set1_ps(1.f), _mm_mul_ps(result, result))));
}


#ifndef maxs
#define maxs(a,b)            (((a) > (b)) ? (a) : (b))
#endif


#ifndef mins
#define mins(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef float vec_t;


template<class T, class U>
inline T clamp2(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}
#define CHECK_VALID( _v ) 0

struct Vector {
	constexpr operator bool() const noexcept
	{
		return x || y || z;
	}

	constexpr Vector& operator=(const float array[3]) noexcept
	{
		x = array[0];
		y = array[1];
		z = array[2];
		return *this;
	}

	auto distance(const Vector& v) const noexcept
	{
		return std::hypot(x - v.x, y - v.y, z - v.z);
	}

	constexpr Vector& operator+=(const Vector& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	constexpr Vector& operator-=(const Vector& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	constexpr auto operator-(const Vector& v) const noexcept
	{
		return Vector{ x - v.x, y - v.y, z - v.z };
	}

	constexpr auto operator+(const Vector& v) const noexcept
	{
		return Vector{ x + v.x, y + v.y, z + v.z };
	}

	constexpr Vector& operator/=(float div) noexcept
	{
		x /= div;
		y /= div;
		z /= div;
		return *this;
	}

	constexpr auto operator*(float mul) const noexcept
	{
		return Vector{ x * mul, y * mul, z * mul };
	}

	constexpr void normalize() noexcept
	{
		x = std::isfinite(x) ? std::remainder(x, 360.0f) : 0.0f;
		y = std::isfinite(y) ? std::remainder(y, 360.0f) : 0.0f;
		z = 0.0f;
	}

	auto length() const noexcept
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	auto length2D() const noexcept
	{
		return std::sqrt(x * x + y * y);
	}

	constexpr auto squareLength() const noexcept
	{
		return x * x + y * y + z * z;
	}

	constexpr auto dotProduct(const Vector& v) const noexcept
	{
		return x * v.x + y * v.y + z * v.z;
	}

	constexpr auto transform(const matrix3x4& mat) const noexcept
	{
		return Vector{ dotProduct({ mat[0][0], mat[0][1], mat[0][2] }) + mat[0][3],
					   dotProduct({ mat[1][0], mat[1][1], mat[1][2] }) + mat[1][3],
					   dotProduct({ mat[2][0], mat[2][1], mat[2][2] }) + mat[2][3] };
	}

	float x, y, z;
};


#define M_PI 3.14159265358979323846
#define deg(a) a * 57.295779513082

#define Assert( _exp ) ((void)0)


class Vector2
{
public:
	float x, y, z;




	//Vector2(void);
	//Vector2(float X, float Y, float Z);
//	Vector2	operator*(const Vector2& v);
//	Vector2	operator/(const Vector& v) const;
	//Vector2	operator*(float fl);
	constexpr operator bool() const noexcept
	{
		return x || y || z;
	}
	Vector2& operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}
	Vector2 operator+(float fl) const
	{
		return Vector2(x + fl, y + fl, z + fl);
	}

	Vector2(const float* clr)
	{
		x = clr[0];
		y = clr[1];
		z = clr[2];
	}

	__forceinline Vector2 operator*(float fl) const
	{
		Vector2 res;
		res.x = x * fl;
		res.y = y * fl;
		res.z = z * fl;
		return res;
	}


	__forceinline Vector2 operator8(float fl) const
	{
		Vector2 res;
		res.x = x * fl;
		res.y = y * fl;
		res.z = z * fl;
		return res;
	}
	__forceinline Vector2 operator*(const Vector2& v) const
	{
		Vector2 res;
		res.x = x * v.x;
		res.y = y * v.y;
		res.z = z * v.z;
		return res;
	}

	__forceinline float LengthSqr(void) const
	{
		CHECK_VALID(*this);
		return (this->x * this->x + this->y * this->y + this->z * this->z);
	}
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance);
	}
	constexpr auto squareLength() const noexcept
	{
		return x * x + y * y + z * z;
	}

	inline float sqrt2(float sqr)
	{
		float root = 0;

		__asm
		{
			sqrtss xmm0, sqr
			movss root, xmm0
		}

		return root;
	}

	Vector2 Vector2CrossProduct(const Vector2& a, const Vector2& b) const
	{
		return Vector2(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	//===============================================
	inline void Init2(float ix, float iy, float iz)
	{
		x = ix; y = iy; z = iz;
		CHECK_VALID(*this);
	}

	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
	{
		x = ix;
		y = iy;
		z = iz;
		CHECK_VALID(*this);
	}
	//===============================================
	inline Vector2(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
		CHECK_VALID(*this);
	}
	//===============================================
	inline Vector2(void) {}
	//===============================================
	inline void Zero()
	{
		x = y = z = 0.0f;
	}
	//===============================================
	inline void Rotate2D(const float& f)
	{
		float _x, _y;

		float s, c;

		float r = f * M_PI / 180.0;
		s = sin(r);
		c = cos(r);

		_x = x;
		_y = y;

		x = (_x * c) - (_y * s);
		y = (_x * s) + (_y * c);
	}

	inline Vector2 NormalizeVec()
	{
		Vector2& vecIn = *this;
		for (int axis = 0; axis < 3; ++axis)
		{
			while (vecIn[axis] > 180.f)
				vecIn[axis] -= 360.f;

			while (vecIn[axis] < -180.f)
				vecIn[axis] += 360.f;

		}

		vecIn[2] = 0.f;
		return vecIn;
	}

	//===============================================
	inline void Vector2Clear(Vector2& a)
	{
		a.x = a.y = a.z = 0.0f;
	}
	//===============================================
	inline Vector2& operator=(const Vector2& vOther)
	{
		CHECK_VALID(vOther);
		x = vOther.x; y = vOther.y; z = vOther.z;
		return *this;
	}
	//===============================================
	inline float& operator[](int i)
	{
		Assert((i >= 0) && (i < 3));
		return ((float*)this)[i];
	}
	//===============================================
	inline float operator[](int i) const
	{
		Assert((i >= 0) && (i < 3));
		return ((float*)this)[i];
	}
	//===============================================
	inline bool operator==(const Vector2& src) const
	{
		CHECK_VALID(src);
		CHECK_VALID(*this);
		return (src.x == x) && (src.y == y) && (src.z == z);
	}
	//===============================================
	inline bool operator!=(const Vector2& src) const
	{
		CHECK_VALID(src);
		CHECK_VALID(*this);
		return (src.x != x) || (src.y != y) || (src.z != z);
	}
	//===============================================
	__forceinline void Vector2Copy(const Vector2& src, Vector2& dst)
	{
		CHECK_VALID(src);
		dst.x = src.x;
		dst.y = src.y;
		dst.z = src.z;
	}


	//===============================================
	__forceinline  Vector2& operator+=(const Vector2& v)
	{
		CHECK_VALID(*this);
		CHECK_VALID(v);
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	//===============================================
	__forceinline  Vector2& operator-=(const Vector2& v)
	{
		CHECK_VALID(*this);
		CHECK_VALID(v);
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	//===============================================

	//===============================================
	__forceinline  Vector2& operator*=(const Vector2& v)
	{
		CHECK_VALID(v);
		x *= v.x;
		y *= v.y;
		z *= v.z;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	__forceinline Vector2& operator+=(float fl)
	{
		x += fl;
		y += fl;
		z += fl;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	__forceinline Vector2& operator-=(float fl)
	{
		x -= fl;
		y -= fl;
		z -= fl;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	__forceinline  Vector2& operator/=(float fl)
	{
		Assert(fl != 0.0f);
		float oofl = 1.0f / fl;
		x *= oofl;
		y *= oofl;
		z *= oofl;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	__forceinline  Vector2& operator/=(const Vector2& v)
	{
		CHECK_VALID(v);
		Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
		x /= v.x;
		y /= v.y;
		z /= v.z;
		CHECK_VALID(*this);
		return *this;
	}
	//===============================================
	inline float Length(void) const
	{
		CHECK_VALID(*this);

		float root = 0.0f;

		float sqsr = x * x + y * y + z * z;

		__asm sqrtss xmm0, sqsr
		__asm movss root, xmm0

		return root;
	}
	//===============================================
	inline float Length2D(void) const
	{
		CHECK_VALID(*this);

		float root = 0.0f;

		float sqst = x * x + y * y;

		__asm
		{
			sqrtss xmm0, sqst
			movss root, xmm0
		}

		return root;
	}
	//===============================================
	inline float Length2DSqr(void) const
	{
		return (x * x + y * y);
	}
	//===============================================
	inline Vector2 Angle(Vector2* up)
	{
		if (!x && !y)
			return Vector2(0, 0, 0);

		float roll = 0;

		if (up)
		{
			Vector2 left = (*up).Cross(*this);
			roll = atan2f(left.z, (left.y * x) - (left.x * y)) * 180.0f / M_PI;
		}

		return Vector2(atan2f(-z, sqrt2(x * x + y * y)) * 180.0f / M_PI, atan2f(y, x) * 180.0f / M_PI, roll);
	}
	inline Vector2 Angle2(Vector2* up = 0)
	{
		if (!x && !y)
			return Vector2(0, 0, 0);

		float roll = 0;

		if (up)
		{
			Vector2 left = (*up).Cross(*this);

			roll = deg(atan2f(left.z, (left.y * x) - (left.x * y)));
		}

		return Vector2(deg(atan2f(-z, sqrtf(x * x + y * y))), deg(atan2f(y, x)), roll);
	}
	//===============================================
	inline Vector2 CrossProduct(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}
	//===============================================
	float DistToSqr(const Vector2& vOther) const
	{
		Vector2 delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}
	float DistTo(const Vector2& vOther) const
	{
		Vector2 delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}
	float Dist(const Vector2& vOther) const
	{
		Vector2 delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}

	inline Vector2 Cross(const Vector2& vOther) const
	{
		Vector2 res;
		return Vector2CrossProduct(*this, vOther);
	}

	inline Vector2 Normalize()
	{
		Vector2 Vector2;
		float length = this->Length();

		if (length != 0)
		{
			Vector2.x = x / length;
			Vector2.y = y / length;
			Vector2.z = z / length;
		}
		else
			Vector2.x = Vector2.y = 0.0f; Vector2.z = 1.0f;

		return Vector2;
	}


	__inline bool IsValid() const
	{
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
	}
	//===============================================
	inline float NormalizeInPlace()
	{
		Vector2& v = *this;

		float iradius = 1.f / (this->Length() + 1.192092896e-07F); //FLT_EPSILON

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;

		return v.Length();
	}
	//===============================================
	inline float Vector2Normalize(Vector2& v)
	{
		Assert(v.IsValid());
		float l = v.Length();
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
	//===============================================
	/*FORCEINLINE float Vector2Normalize(float* v)
	{
		return Vector2Normalize(*(reinterpret_cast<Vector2*>(v)));
	}
	*/
	//===============================================
	/*inline Vector2 Normalized() const
	{
		Vector2 norm = *this;
		Vector2Normalize(norm);
		return norm;
	}*/
	Vector2 Normalized() const
	{
		Vector2 res = *this;
		float l = res.Length();
		if (l != 0.0f) {
			res /= l;
		}
		else {
			res.x = res.y = res.z = 0.0f;
		}
		return res;
	}
	//===============================================
	inline Vector2 operator+(const Vector2& v) const
	{
		Vector2 res;
		res.x = x + v.x;
		res.y = y + v.y;
		res.z = z + v.z;
		return res;
	}

	//===============================================
	inline Vector2 operator-(const Vector2& v) const
	{
		Vector2 res;
		res.x = x - v.x;
		res.y = y - v.y;
		res.z = z - v.z;
		return res;
	}
	//===============================================

	//===============================================

	//===============================================
	inline Vector2 operator/(float fl) const
	{
		Vector2 res;
		res.x = x / fl;
		res.y = y / fl;
		res.z = z / fl;
		return res;
	}
	//===============================================
	inline Vector2 operator/(const Vector2& v) const
	{
		Vector2 res;
		res.x = x / v.x;
		res.y = y / v.y;
		res.z = z / v.z;
		return res;
	}
	inline float Dot(const Vector2& vOther) const
	{
		const Vector2& a = *this;

		return(a.x * vOther.x + a.y * vOther.y + a.z * vOther.z);
	}

	inline float Dot(const float* fOther) const
	{
		const Vector2& a = *this;

		return(a.x * fOther[0] + a.y * fOther[1] + a.z * fOther[2]);
	}

	inline float* Base()
	{
		return (float*)this;
	}

	inline float const* Base() const
	{
		return (float const*)this;
	}
	void normalize_in_place()
	{
		*this = normalized();
	}
	Vector2 normalized() const
	{
		auto res = *this;
		auto l = res.Length();

		if (l != 0.0f)
			res /= l;

		else
			res.x = res.y = res.z = 0.0f;

		return res;
	}
};


#define NOCOLOR CColor(0, 0, 0, 0)
#define WHITE CColor(255, 255, 255, 255)
#define BLACK CColor(0, 0, 0, 255)
#define RED CColor(255, 0, 0, 255)
#define LIGHTRED CColor(255, 100, 100, 255)
#define WHITERED CColor(255, 65, 65, 255);
#define DARKRED CColor(120, 0, 0, 255)
#define GREEN CColor(0, 255, 0, 255)
#define BLUE CColor(0, 0, 255, 255)
#define LIGHTBLUE CColor(0, 140, 255, 255)
#define DARKGREY CColor(55, 55, 55, 255)
#define GREY CColor(70, 70, 70, 255)
#define LIGHTGREY CColor(150, 150, 150, 255)
#define HOTPINK CColor(255, 20, 147, 255)
#define CYAN CColor(0, 255, 255, 255)
#define YELLOW CColor(255, 255, 0, 255)


class CColor
{
public:
	unsigned char RGBA[4];


	CColor()
	{
		RGBA[0] = 255;
		RGBA[1] = 255;
		RGBA[2] = 255;
		RGBA[3] = 255;

	}
	CColor(int r, int g, int b, int a = 255)
	{
		RGBA[0] = r;
		RGBA[1] = g;
		RGBA[2] = b;
		RGBA[3] = a;
	}

	bool operator!=(CColor color) { return RGBA[0] != color.RGBA[0] || RGBA[1] != color.RGBA[1] || RGBA[2] != color.RGBA[2] || RGBA[3] != color.RGBA[3]; }
	bool operator==(CColor color) { return RGBA[0] == color.RGBA[0] && RGBA[1] == color.RGBA[1] && RGBA[2] == color.RGBA[2] && RGBA[3] == color.RGBA[3]; }

	inline int r() const
	{
		return RGBA[0];
	}

	inline int g() const
	{
		return RGBA[1];
	}

	inline int b() const
	{
		return RGBA[2];
	}

	inline int a() const
	{
		return RGBA[3];
	}

	// returns the color from 0.f - 1.f
	static float Base(const unsigned char col)
	{
		return col / 255.f;
	}
	static CColor Inverse(const CColor color)
	{
		return CColor(255 - color.RGBA[0], 255 - color.RGBA[1], 255 - color.RGBA[2]);
	}

	static CColor Black()
	{
		return CColor(0, 0, 0);
	}

	int GetD3DColor() const
	{
		return ((int)((((RGBA[3]) & 0xff) << 24) | (((RGBA[0]) & 0xff) << 16) | (((RGBA[1]) & 0xff) << 8) | ((RGBA[2]) & 0xff)));
	}

	static CColor Red()
	{
		return CColor(255, 0, 0);
	}
	static CColor Green()
	{
		return CColor(0, 255, 26);
	}

	static CColor LightBlue()
	{
		return CColor(100, 100, 255);
	}


	static CColor White()
	{
		return CColor(255, 255, 255);
	}


	float Difference(const CColor color) const // from 0.f - 1.f with 1.f being the most different
	{
		float red_diff = fabs(Base(RGBA[0]) - Base(color.RGBA[0]));
		float green_diff = fabs(Base(RGBA[1]) - Base(color.RGBA[1]));
		float blue_diff = fabs(Base(RGBA[2]) - Base(color.RGBA[2]));
		float alpha_diff = fabs(Base(RGBA[3]) - Base(color.RGBA[3]));

		return (red_diff + green_diff + blue_diff + alpha_diff) * 0.25f;
	}

	// RGB -> HSB
	static float Hue(const CColor color)
	{
		float R = Base(color.RGBA[0]);
		float G = Base(color.RGBA[1]);
		float B = Base(color.RGBA[2]);

		float mx = maxs(R, maxs(G, B));
		float mn = mins(R, mins(G, B));
		if (mx == mn)
			return 0.f;

		float delta = mx - mn;

		float hue = 0.f;
		if (mx == R)
			hue = (G - B) / delta;
		else if (mx == G)
			hue = 2.f + (B - R) / delta;
		else
			hue = 4.f + (R - G) / delta;

		hue *= 60.f;
		if (hue < 0.f)
			hue += 360.f;

		return hue / 360.f;
	}
	static float Saturation(const CColor color)
	{
		float R = Base(color.RGBA[0]);
		float G = Base(color.RGBA[1]);
		float B = Base(color.RGBA[2]);

		float mx = maxs(R, maxs(G, B));
		float mn = mins(R, mins(G, B));

		float delta = mx - mn;

		if (mx == 0.f)
			return delta;

		return delta / mx;
	}
	static float Brightness(const CColor color)
	{
		float R = Base(color.RGBA[0]);
		float G = Base(color.RGBA[1]);
		float B = Base(color.RGBA[2]);

		return maxs(R, maxs(G, B));
	}

	float Hue() const
	{
		return Hue(*this);
	}
	float Saturation() const
	{
		return Saturation(*this);
	}
	float Brightness() const
	{
		return Brightness(*this);
	}

	static CColor FromHSB(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1)
		{
			return CColor(
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 2)
		{
			return CColor(
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 3)
		{
			return CColor(
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255)
			);
		}
		else if (h < 4)
		{
			return CColor(
				(unsigned char)(p * 255),
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else if (h < 5)
		{
			return CColor(
				(unsigned char)(t * 255),
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else
		{
			return CColor(
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255),
				(unsigned char)(q * 255)
			);
		}
	}

	// HSB -> RGB
	static CColor HSBtoRGB(float hue /* 0.f - 1.f*/,
		float saturation /* 0.f - 1.f */,
		float brightness /* 0.f - 1.f */,
		int alpha = 255)
	{
		hue = clamp2(hue, 0.f, 1.f);
		saturation = clamp2(saturation, 0.f, 1.f);
		brightness = clamp2(brightness, 0.f, 1.f);

		float h = (hue == 1.f) ? 0.f : (hue * 6.f);
		float f = h - static_cast<int>(h);
		float p = brightness * (1.f - saturation);
		float q = brightness * (1.f - saturation * f);
		float t = brightness * (1.f - (saturation * (1.f - f)));

		if (h < 1.f)
			return CColor(brightness * 255, t * 255, p * 255, alpha);
		else if (h < 2.f)
			return CColor(q * 255, brightness * 255, p * 255, alpha);
		else if (h < 3.f)
			return CColor(p * 255, brightness * 255, t * 255, alpha);
		else if (h < 4)
			return CColor(p * 255, q * 255, brightness * 255, alpha);
		else if (h < 5)
			return CColor(t * 255, p * 255, brightness * 255, alpha);
		else
			return CColor(brightness * 255, p * 255, q * 255, alpha);

	}


};


#define deg(a) a * 57.295779513082

class c_vector3d
{
public:
	float x, y, z;

	__forceinline c_vector3d() : c_vector3d(0.f, 0.f, 0.f) { }

	__forceinline c_vector3d(const float x, const float y, const float z) : x(x), y(y), z(z) { }

	__forceinline c_vector3d operator+(const c_vector3d& v) const
	{
		return c_vector3d(x + v.x, y + v.y, z + v.z);
	}

	__forceinline c_vector3d operator-(const c_vector3d& v) const
	{
		return c_vector3d(x - v.x, y - v.y, z - v.z);
	}

	__forceinline c_vector3d operator*(const c_vector3d& v) const
	{
		return c_vector3d(x * v.x, y * v.y, z * v.z);
	}

	__forceinline c_vector3d operator/(const c_vector3d& v) const
	{
		return c_vector3d(x / v.x, y / v.y, z / v.z);
	}

	__forceinline c_vector3d operator*(const float v) const
	{
		return c_vector3d(x * v, y * v, z * v);
	}

	__forceinline c_vector3d operator/(const float v) const
	{
		return c_vector3d(x / v, y / v, y / v);
	}
	__forceinline float& operator[](int i) {
		return ((float*)this)[i];
	}

	__forceinline float operator[](int i) const {
		return ((float*)this)[i];
	}

	__forceinline c_vector3d operator+=(const c_vector3d& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	__forceinline c_vector3d operator-=(const c_vector3d& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	__forceinline c_vector3d operator*=(const c_vector3d& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	__forceinline c_vector3d operator/=(const c_vector3d& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	__forceinline c_vector3d operator*=(const float other)
	{
		x *= other;
		y *= other;
		z *= other;
		return *this;
	}

	__forceinline c_vector3d operator/=(const float other)
	{
		x /= other;
		y /= other;
		z /= other;
		return *this;
	}

	__forceinline float length() const
	{
		m128 tmp;
		tmp.f[0] = x * x + y * y + z * z;
		const auto calc = sqrt_ps(tmp.v);
		return reinterpret_cast<const m128*>(&calc)->f[0];
	}

	__forceinline void Init(float ix, float iy, float iz)
	{
		x = ix; y = iy; z = iz;
		CHECK_VALID(*this);
	}

	__forceinline c_vector3d Angle(c_vector3d* up = 0)
	{
		if (!x && !y)
			return c_vector3d(0, 0, 0);

		float roll = 0;

		if (up)
		{
			c_vector3d left = (*up).cross(*this);
			roll = deg(atan2f(left.z, (left.y * x) - (left.x * y)));
		}
		return c_vector3d(deg(atan2f(-z, sqrtf(x * x + y * y))), deg(atan2f(y, x)), roll);
	}
	__forceinline float length2d() const
	{
		m128 tmp;
		tmp.f[0] = x * x + y * y;
		const auto calc = sqrt_ps(tmp.v);
		return reinterpret_cast<const m128*>(&calc)->f[0];
	}

	__forceinline float length2dsqr() const
	{
		return x * x + y * y;
	}

	__forceinline float dot(const c_vector3d& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	__forceinline float dist_to(const c_vector3d& vOther) const
	{
		c_vector3d delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.length();
	}

	__forceinline float dot(const float* other) const
	{
		return x * other[0] + y * other[1] + z * other[2];
	}

	__forceinline c_vector3d cross(const c_vector3d& other) const
	{
		return c_vector3d(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	__forceinline c_vector3d normalize()
	{
		const auto l = length();

		if (l > 0)
		{
			x /= l;
			y /= l;
			z /= l;
		}

		return *this;
	}
	auto Normalize2() {
		(*this) /= length();
	}

	c_vector3d Normalized() const
	{
		c_vector3d res = *this;
		float l = res.length();
		if (l != 0.0f) {
			res /= l;
		}
		else {
			res.x = res.y = res.z = 0.0f;
		}
		return res;
	}
	auto NormalizeInPlace() {
		*this = Normalized();
	}
	__forceinline bool is_valid() const
	{
		return std::isfinite(this->x) && std::isfinite(this->y) && std::isfinite(this->z);
	}
};

class c_vector3d_aligned : public c_vector3d
{
	float w{};
};

using c_qangle = c_vector3d;


#define ALIGN16 __declspec(align(16))
#define VALVE_RAND_MAX 0x7fff
#define VectorExpand(v) (v).x, (v).y, (v).z


class Vector2D
{
public:
	// Members
	float x, y;

	// Construction/destruction
	Vector2D(void);
	Vector2D(float X, float Y);
	Vector2D(const float* pFloat);

	// Initialization
	void Init(float ix = 0.0f, float iy = 0.0f);

	// Got any nasty NAN's?
	bool IsValid() const;

	// array access...
	float  operator[](int i) const;
	float& operator[](int i);

	// Base address...
	float* Base();
	float const* Base() const;

	// Initialization methods
	void Random(float minVal, float maxVal);

	// equality
	bool operator==(const Vector2D& v) const;
	bool operator!=(const Vector2D& v) const;

	// arithmetic operations
	Vector2D& operator+=(const Vector2D& v);
	Vector2D& operator-=(const Vector2D& v);
	Vector2D& operator*=(const Vector2D& v);
	Vector2D& operator*=(float           s);
	Vector2D& operator/=(const Vector2D& v);
	Vector2D& operator/=(float           s);

	// negate the Vector2D components
	void Negate();

	// Get the Vector2D's magnitude.
	float Length() const;

	// Get the Vector2D's magnitude squared.
	float LengthSqr(void) const;

	// return true if this vector is (0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance);
	}

	float Normalize();

	// Normalize in place and return the old length.
	float NormalizeInPlace();

	// Compare length.
	bool IsLengthGreaterThan(float val) const;
	bool IsLengthLessThan(float    val) const;

	// Get the distance from this Vector2D to the other one.
	float DistTo(const Vector2D& vOther) const;

	// Get the distance from this Vector2D to the other one squared.
	float DistToSqr(const Vector2D& vOther) const;

	// Copy
	void CopyToArray(float* rgfl) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual Vector2D equation (because it's done per-component
	// rather than per-Vector2D).
	void MulAdd(const Vector2D& a, const Vector2D& b, float scalar);

	// Dot product.
	float Dot(const Vector2D& vOther) const;

	// assignment
	Vector2D& operator=(const Vector2D& vOther);

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors
	Vector2D(const Vector2D& vOther);

	// arithmetic operations
	Vector2D operator-(void) const;

	Vector2D operator+(const Vector2D& v) const;
	Vector2D operator-(const Vector2D& v) const;
	Vector2D operator*(const Vector2D& v) const;
	Vector2D operator/(const Vector2D& v) const;
	Vector2D operator+(const int       i1) const;
	Vector2D operator+(const float     fl) const;
	Vector2D operator*(const float     fl) const;
	Vector2D operator/(const float     fl) const;

	// Cross product between two vectors.
	Vector2D Cross(const Vector2D& vOther) const;

	// Returns a Vector2D with the min or max in X, Y, and Z.
	Vector2D Min(const Vector2D& vOther) const;
	Vector2D Max(const Vector2D& vOther) const;

#else

private:
	// No copy constructors allowed if we're in optimal mode
	Vector2D(const Vector2D& vOther);
#endif
};


const Vector2D vec2_origin(0, 0);
//const Vector2D vec2_invalid(3.40282347E+38F, 3.40282347E+38F);

//-----------------------------------------------------------------------------
// Vector2D related operations
//-----------------------------------------------------------------------------

// Vector2D clear
void Vector2DClear(Vector2D& a);

// Copy
void Vector2DCopy(const Vector2D& src, Vector2D& dst);

// Vector2D arithmetic
void Vector2DAdd(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DSubtract(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DMultiply(const Vector2D& a, float           b, Vector2D& result);
void Vector2DMultiply(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DDivide(const Vector2D& a, float           b, Vector2D& result);
void Vector2DDivide(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DMA(const Vector2D& start, float       s, const Vector2D& dir, Vector2D& result);

// Store the min or max of each of x, y, and z into the result.
void Vector2DMin(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DMax(const Vector2D& a, const Vector2D& b, Vector2D& result);

#define Vector2DExpand( v ) (v).x, (v).y

// Normalization
float Vector2DNormalize(Vector2D& v);

// Length
float Vector2DLength(const Vector2D& v);

// Dot Product
float DotProduct2D(const Vector2D& a, const Vector2D& b);

// Linearly interpolate between two vectors
void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, float t, Vector2D& dest);


//-----------------------------------------------------------------------------
//
// Inlined Vector2D methods
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------

inline Vector2D::Vector2D(void)
{
#ifdef _DEBUG
	// Initialize to NAN to catch errors
	//x = y = float_NAN;
#endif
}

inline Vector2D::Vector2D(float X, float Y)
{
	x = X; y = Y;
	Assert(IsValid());
}

inline Vector2D::Vector2D(const float* pFloat)
{
	Assert(pFloat);
	x = pFloat[0]; y = pFloat[1];
	Assert(IsValid());
}
inline unsigned long& FloatBits(float& f)
{
	return *reinterpret_cast<unsigned long*>(&f);
}

inline bool IsFinite(float f)
{
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}

//-----------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------

inline Vector2D::Vector2D(const Vector2D& vOther)
{
	Assert(vOther.IsValid());
	x = vOther.x; y = vOther.y;
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

inline void Vector2D::Init(float ix, float iy)
{
	x = ix; y = iy;
	Assert(IsValid());
}

inline void Vector2D::Random(float minVal, float maxVal)
{
	x = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	y = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
}

inline void Vector2DClear(Vector2D& a)
{
	a.x = a.y = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

inline Vector2D& Vector2D::operator=(const Vector2D& vOther)
{
	Assert(vOther.IsValid());
	x = vOther.x; y = vOther.y;
	return *this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------

inline float& Vector2D::operator[](int i)
{
	Assert((i >= 0) && (i < 2));
	return ((float*)this)[i];
}

inline float Vector2D::operator[](int i) const
{
	Assert((i >= 0) && (i < 2));
	return ((float*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------

inline float* Vector2D::Base()
{
	return (float*)this;
}

inline float const* Vector2D::Base() const
{
	return (float const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

inline bool Vector2D::IsValid() const
{
	return IsFinite(x) && IsFinite(y);
}



//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool Vector2D::operator==(const Vector2D& src) const
{
	Assert(src.IsValid() && IsValid());
	return (src.x == x) && (src.y == y);
}

inline bool Vector2D::operator!=(const Vector2D& src) const
{
	Assert(src.IsValid() && IsValid());
	return (src.x != x) || (src.y != y);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

inline void Vector2DCopy(const Vector2D& src, Vector2D& dst)
{
	Assert(src.IsValid());
	dst.x = src.x;
	dst.y = src.y;
}

inline void	Vector2D::CopyToArray(float* rgfl) const
{
	Assert(IsValid());
	Assert(rgfl);
	rgfl[0] = x; rgfl[1] = y;
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------

inline void Vector2D::Negate()
{
	Assert(IsValid());
	x = -x; y = -y;
}

inline Vector2D& Vector2D::operator+=(const Vector2D& v)
{
	Assert(IsValid() && v.IsValid());
	x += v.x; y += v.y;
	return *this;
}

inline Vector2D& Vector2D::operator-=(const Vector2D& v)
{
	Assert(IsValid() && v.IsValid());
	x -= v.x; y -= v.y;
	return *this;
}

inline Vector2D& Vector2D::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	Assert(IsValid());
	return *this;
}

inline Vector2D& Vector2D::operator*=(const Vector2D& v)
{
	x *= v.x;
	y *= v.y;
	Assert(IsValid());
	return *this;
}

inline Vector2D& Vector2D::operator/=(float fl)
{
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	Assert(IsValid());
	return *this;
}

inline Vector2D& Vector2D::operator/=(const Vector2D& v)
{
	Assert(v.x != 0.0f && v.y != 0.0f);
	x /= v.x;
	y /= v.y;
	Assert(IsValid());
	return *this;
}

inline void Vector2DAdd(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	Assert(a.IsValid() && b.IsValid());
	c.x = a.x + b.x;
	c.y = a.y + b.y;
}

inline void Vector2DAdd(const Vector2D& a, const int b, Vector2D& c)
{
	Assert(a.IsValid());
	c.x = a.x + b;
	c.y = a.y + b;
}

inline void Vector2DAdd(const Vector2D& a, const float b, Vector2D& c)
{
	Assert(a.IsValid());
	c.x = a.x + b;
	c.y = a.y + b;
}

inline void Vector2DSubtract(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	Assert(a.IsValid() && b.IsValid());
	c.x = a.x - b.x;
	c.y = a.y - b.y;
}

inline void Vector2DMultiply(const Vector2D& a, const float b, Vector2D& c)
{
	Assert(a.IsValid() && IsFinite(b));
	c.x = a.x * b;
	c.y = a.y * b;
}

inline void Vector2DMultiply(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	Assert(a.IsValid() && b.IsValid());
	c.x = a.x * b.x;
	c.y = a.y * b.y;
}


inline void Vector2DDivide(const Vector2D& a, const float b, Vector2D& c)
{
	Assert(a.IsValid());
	Assert(b != 0.0f);
	float oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
}

inline void Vector2DDivide(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	Assert(a.IsValid());
	Assert((b.x != 0.0f) && (b.y != 0.0f));
	c.x = a.x / b.x;
	c.y = a.y / b.y;
}

inline void Vector2DMA(const Vector2D& start, float s, const Vector2D& dir, Vector2D& result)
{
	Assert(start.IsValid() && IsFinite(s) && dir.IsValid());
	result.x = start.x + s * dir.x;
	result.y = start.y + s * dir.y;
}

// FIXME: Remove
// For backwards compatability
inline void	Vector2D::MulAdd(const Vector2D& a, const Vector2D& b, float scalar)
{
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
}

inline void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, float t, Vector2D& dest)
{
	dest[0] = src1[0] + (src2[0] - src1[0]) * t;
	dest[1] = src1[1] + (src2[1] - src1[1]) * t;
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
inline float DotProduct2D(const Vector2D& a, const Vector2D& b)
{
	Assert(a.IsValid() && b.IsValid());
	return(a.x * b.x + a.y * b.y);
}

// for backwards compatability
inline float Vector2D::Dot(const Vector2D& vOther) const
{
	return DotProduct2D(*this, vOther);
}

inline float FastSqrt(float x)
{
	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;
	return *(float*)&i;
}
//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float Vector2DLength(const Vector2D& v)
{
	Assert(v.IsValid());
	return (float)FastSqrt(v.x * v.x + v.y * v.y);
}

inline float Vector2D::LengthSqr(void) const
{
	Assert(IsValid());
	return (x * x + y * y);
}

inline float Vector2D::NormalizeInPlace()
{
	return Vector2DNormalize(*this);
}

inline bool Vector2D::IsLengthGreaterThan(float val) const
{
	return LengthSqr() > val* val;
}

inline bool Vector2D::IsLengthLessThan(float val) const
{
	return LengthSqr() < val * val;
}

inline float Vector2D::Length(void) const
{
	return Vector2DLength(*this);
}


inline void Vector2DMin(const Vector2D& a, const Vector2D& b, Vector2D& result)
{
	result.x = (a.x < b.x) ? a.x : b.x;
	result.y = (a.y < b.y) ? a.y : b.y;
}


inline void Vector2DMax(const Vector2D& a, const Vector2D& b, Vector2D& result)
{
	result.x = (a.x > b.x) ? a.x : b.x;
	result.y = (a.y > b.y) ? a.y : b.y;
}


//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------
inline float Vector2DNormalize(Vector2D& v)
{
	Assert(v.IsValid());
	float l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f;
	}
	return l;
}


//-----------------------------------------------------------------------------
// Get the distance from this Vector2D to the other one
//-----------------------------------------------------------------------------
inline float Vector2D::DistTo(const Vector2D& vOther) const
{
	Vector2D delta;
	Vector2DSubtract(*this, vOther, delta);
	return delta.Length();
}

inline float Vector2D::DistToSqr(const Vector2D& vOther) const
{
	Vector2D delta;
	Vector2DSubtract(*this, vOther, delta);
	return delta.LengthSqr();
}


//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
inline void ComputeClosestPoint2D(const Vector2D& vecStart, float flMaxDist, const Vector2D& vecTarget, Vector2D* pResult)
{
	Vector2D vecDelta;
	Vector2DSubtract(vecTarget, vecStart, vecDelta);
	float flDistSqr = vecDelta.LengthSqr();
	if (flDistSqr <= flMaxDist * flMaxDist)
	{
		*pResult = vecTarget;
	}
	else
	{
		vecDelta /= FastSqrt(flDistSqr);
		Vector2DMA(vecStart, flMaxDist, vecDelta, *pResult);
	}
}



//-----------------------------------------------------------------------------
//
// Slow methods
//
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS
#endif
//-----------------------------------------------------------------------------
// Returns a Vector2D with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------

inline Vector2D Vector2D::Min(const Vector2D& vOther) const
{
	return Vector2D(x < vOther.x ? x : vOther.x,
		y < vOther.y ? y : vOther.y);
}

inline Vector2D Vector2D::Max(const Vector2D& vOther) const
{
	return Vector2D(x > vOther.x ? x : vOther.x,
		y > vOther.y ? y : vOther.y);
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

inline Vector2D Vector2D::operator-(void) const
{
	return Vector2D(-x, -y);
}

inline Vector2D Vector2D::operator+(const Vector2D& v) const
{
	Vector2D res;
	Vector2DAdd(*this, v, res);
	return res;
}

inline Vector2D Vector2D::operator-(const Vector2D& v) const
{
	Vector2D res;
	Vector2DSubtract(*this, v, res);
	return res;
}

inline Vector2D Vector2D::operator+(const int i1) const
{
	Vector2D res;
	Vector2DAdd(*this, i1, res);
	return res;
}

inline Vector2D Vector2D::operator+(const float fl) const
{
	Vector2D res;
	Vector2DAdd(*this, fl, res);
	return res;
}

inline Vector2D Vector2D::operator*(const float fl) const
{
	Vector2D res;
	Vector2DMultiply(*this, fl, res);
	return res;
}

inline Vector2D Vector2D::operator*(const Vector2D& v) const
{
	Vector2D res;
	Vector2DMultiply(*this, v, res);
	return res;
}

inline Vector2D Vector2D::operator/(const float fl) const
{
	Vector2D res;
	Vector2DDivide(*this, fl, res);
	return res;
}

inline Vector2D Vector2D::operator/(const Vector2D& v) const
{
	Vector2D res;
	Vector2DDivide(*this, v, res);
	return res;
}

inline Vector2D operator*(const float fl, const Vector2D& v)
{
	return v * fl;
}




