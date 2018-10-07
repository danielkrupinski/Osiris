#include <cmath>

#include "QAngle.h"

void QAngle::Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
{
    pitch = ix;
    yaw = iy;
    roll = iz;
}


QAngle::QAngle(void)
{
    Init();
}

QAngle::QAngle(float X, float Y, float Z)
{
    Init(X, Y, Z);
}

QAngle::QAngle(const float* clr)
{
    Init(clr[0], clr[1], clr[2]);
}

float QAngle::operator[](int i) const
{
    return ((float*)this)[i];
}
float& QAngle::operator[](int i)
{
    return ((float*)this)[i];
}

QAngle& QAngle::operator+=(const QAngle& v)
{
    pitch += v.pitch; yaw += v.yaw; roll += v.roll;
    return *this;
}

QAngle& QAngle::operator-=(const QAngle& v)
{
    pitch -= v.pitch; yaw -= v.yaw; roll -= v.roll;
    return *this;
}

QAngle& QAngle::operator*=(float fl)
{
    pitch *= fl;
    yaw *= fl;
    roll *= fl;
    return *this;
}

QAngle& QAngle::operator*=(const QAngle& v)
{
    pitch *= v.pitch;
    yaw *= v.yaw;
    roll *= v.roll;
    return *this;
}

QAngle& QAngle::operator/=(const QAngle& v)
{
    pitch /= v.pitch;
    yaw /= v.yaw;
    roll /= v.roll;
    return *this;
}

QAngle& QAngle::operator+=(float fl)
{
    pitch += fl;
    yaw += fl;
    roll += fl;
    return *this;
}

QAngle& QAngle::operator/=(float fl)
{
    pitch /= fl;
    yaw /= fl;
    roll /= fl;
    return *this;
}

QAngle& QAngle::operator-=(float fl)
{
    pitch -= fl;
    yaw -= fl;
    roll -= fl;
    return *this;
}

QAngle& QAngle::operator=(const QAngle &vOther)
{
    pitch = vOther.pitch; yaw = vOther.yaw; roll = vOther.roll;
    return *this;
}

QAngle QAngle::operator-(void) const
{
    return QAngle(-pitch, -yaw, -roll);
}
QAngle QAngle::operator+(const QAngle& v) const
{
    return QAngle(pitch + v.pitch, yaw + v.yaw, roll + v.roll);
}
QAngle QAngle::operator-(const QAngle& v) const
{
    return QAngle(pitch - v.pitch, yaw - v.yaw, roll - v.roll);
}
QAngle QAngle::operator*(float fl) const
{
    return QAngle(pitch * fl, yaw * fl, roll * fl);
}
QAngle QAngle::operator*(const QAngle& v) const
{
    return QAngle(pitch * v.pitch, yaw * v.yaw, roll * v.roll);
}
QAngle QAngle::operator/(float fl) const
{
    return QAngle(pitch / fl, yaw / fl, roll / fl);
}
QAngle QAngle::operator/(const QAngle& v) const
{
    return QAngle(pitch / v.pitch, yaw / v.yaw, roll / v.roll);
}

float QAngle::Length() const
{
    return std::sqrt(pitch*pitch + yaw * yaw + roll * roll);
}
float QAngle::LengthSqr(void) const
{
    return (pitch*pitch + yaw * yaw + roll * roll);
}
bool QAngle::IsZero(float tolerance = 0.01f) const
{
    return (pitch > -tolerance && pitch < tolerance &&
        yaw > -tolerance && yaw < tolerance &&
        roll > -tolerance && roll < tolerance);
}

float QAngle::Normalize() const
{
    QAngle res = *this;
    float l = res.Length();
    if (l != 0.0f)
    {
        res /= l;
    }
    else
    {
        res[0] = res[1] = res[2] = 0.0f;
    }
    return l;
}
