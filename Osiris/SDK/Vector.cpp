#include <sstream>

#include "Vector.h"

Vector::Vector(void)
{
    Invalidate();
}

Vector::Vector(float X, float Y, float Z)
{
    x = X;
    y = Y;
    z = Z;
}

Vector::Vector(const float* clr)
{
    x = clr[0];
    y = clr[1];
    z = clr[2];
}

void Vector::Init(float ix, float iy, float iz)
{
    x = ix; y = iy; z = iz;
}

bool Vector::IsValid() const
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

void Vector::Invalidate()
{
    x = y = z = std::numeric_limits<float>::infinity();
}

float& Vector::operator[](int i)
{
    return ((float*)this)[i];
}

float Vector::operator[](int i) const
{
    return ((float*)this)[i];
}

void Vector::Zero()
{
    x = y = z = 0.0f;
}

bool Vector::operator==(const Vector& src) const
{
    return (src.x == x) && (src.y == y) && (src.z == z);
}

bool Vector::operator!=(const Vector& src) const
{
    return (src.x != x) || (src.y != y) || (src.z != z);
}

Vector& Vector::operator+=(const Vector& v)
{
    x += v.x; y += v.y; z += v.z;
    return *this;
}

Vector& Vector::operator-=(const Vector& v)
{
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}

Vector& Vector::operator*=(float fl)
{
    x *= fl;
    y *= fl;
    z *= fl;
    return *this;
}

Vector& Vector::operator*=(const Vector& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

Vector& Vector::operator/=(const Vector& v)
{
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
}

Vector& Vector::operator+=(float fl)
{
    x += fl;
    y += fl;
    z += fl;
    return *this;
}

Vector& Vector::operator/=(float fl)
{
    x /= fl;
    y /= fl;
    z /= fl;
    return *this;
}

Vector& Vector::operator-=(float fl)
{
    x -= fl;
    y -= fl;
    z -= fl;
    return *this;
}

void Vector::NormalizeInPlace()
{
    *this = Normalized();
}

Vector Vector::Normalized() const
{
    Vector res = *this;
    float l = res.Length();
    if (l != 0.0f) {
        res /= l;
    }
    else {
        res.x = res.y = res.z = 0.0f;
    }
    return res;
}

float Vector::DistTo(const Vector &vOther) const
{
    Vector delta;

    delta.x = x - vOther.x;
    delta.y = y - vOther.y;
    delta.z = z - vOther.z;

    return delta.Length();
}

float Vector::DistToSqr(const Vector &vOther) const
{
    Vector delta;

    delta.x = x - vOther.x;
    delta.y = y - vOther.y;
    delta.z = z - vOther.z;

    return delta.LengthSqr();
}

float Vector::Dot(const Vector& vOther) const
{
    return (x*vOther.x + y * vOther.y + z * vOther.z);
}

float Vector::Length() const
{
    return sqrt(x*x + y * y + z * z);
}

float Vector::LengthSqr(void) const
{
    return (x*x + y * y + z * z);
}

float Vector::Length2D() const
{
    return sqrt(x*x + y * y);
}

Vector& Vector::operator=(const Vector &vOther)
{
    x = vOther.x; y = vOther.y; z = vOther.z;
    return *this;
}

Vector Vector::operator-(void) const
{
    return Vector(-x, -y, -z);
}

Vector Vector::operator+(const Vector& v) const
{
    return Vector(x + v.x, y + v.y, z + v.z);
}

Vector Vector::operator-(const Vector& v) const
{
    return Vector(x - v.x, y - v.y, z - v.z);
}

Vector Vector::operator*(float fl) const
{
    return Vector(x * fl, y * fl, z * fl);
}

Vector Vector::operator*(const Vector& v) const
{
    return Vector(x * v.x, y * v.y, z * v.z);
}

Vector Vector::operator/(float fl) const
{
    return Vector(x / fl, y / fl, z / fl);
}

Vector Vector::operator/(const Vector& v) const
{
    return Vector(x / v.x, y / v.y, z / v.z);
}

