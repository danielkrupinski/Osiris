#pragma once

class Vector {
public:
    Vector(void);
    Vector(float X, float Y, float Z);
    Vector(const float* clr);
    void Init(float, float, float);
    bool IsValid() const;
    void Invalidate();
    float& operator[](int i);
    float operator[](int i) const;
    void Zero();
    bool operator==(const Vector& src) const;
    bool operator!=(const Vector& src) const;
    Vector& operator+=(const Vector& v);
    Vector& operator-=(const Vector& v);
    Vector& operator*=(float fl);
    Vector& operator*=(const Vector& v);
    Vector& operator/=(const Vector& v);
    Vector& operator+=(float fl);
    Vector& operator/=(float fl);
    Vector& operator-=(float fl);
    void NormalizeInPlace();
    Vector Normalized() const;
    float DistTo(const Vector &vOther) const;
    float DistToSqr(const Vector &vOther) const;
    float Dot(const Vector& vOther) const;
    float Length() const;
    float LengthSqr(void) const;
    float Length2D() const;
    Vector& operator=(const Vector &vOther);
    Vector operator-(void) const;
    Vector operator+(const Vector& v) const;
    Vector operator-(const Vector& v) const;
    Vector operator*(float fl) const;
    Vector operator*(const Vector& v) const;
    Vector operator/(float fl) const;
    Vector operator/(const Vector& v) const;

    float x, y, z;
};

inline Vector operator*(float lhs, const Vector& rhs)
{
    return rhs * lhs;
}
inline Vector operator/(float lhs, const Vector& rhs)
{
    return rhs / lhs;
}

class __declspec(align(16)) VectorAligned : public Vector {
public:
    inline VectorAligned(void) { };
    inline VectorAligned(float X, float Y, float Z)
    {
        Init(X, Y, Z);
    }

public:
    explicit VectorAligned(const Vector &vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
    }

    VectorAligned& operator=(const Vector &vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    VectorAligned& operator=(const VectorAligned &vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    float w;
};