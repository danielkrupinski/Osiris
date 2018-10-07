#include "../Interfaces.h"
#include "Math.h"

namespace Math
{
    //--------------------------------------------------------------------------------
    float VectorDistance(const Vector& v1, const Vector& v2)
    {
        return FASTSQRT(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
    }
    //--------------------------------------------------------------------------------
    QAngle CalcAngle(const Vector& src, const Vector& dst)
    {
        QAngle vAngle;
        Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
        double hyp = sqrt(delta.x*delta.x + delta.y*delta.y);

        vAngle.pitch = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
        vAngle.yaw = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
        vAngle.roll = 0.0f;

        if (delta.x >= 0.0)
            vAngle.yaw += 180.0f;

        return vAngle;
    }
    //--------------------------------------------------------------------------------
    float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle)
    {
        Vector ang, aim;

        AngleVectors(viewAngle, aim);
        AngleVectors(aimAngle, ang);

        return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
    }
    //--------------------------------------------------------------------------------
    void ClampAngles(QAngle& angles)
    {
        if (angles.pitch > 89.0f) angles.pitch = 89.0f;
        else if (angles.pitch < -89.0f) angles.pitch = -89.0f;

        if (angles.yaw > 180.0f) angles.yaw = 180.0f;
        else if (angles.yaw < -180.0f) angles.yaw = -180.0f;

        angles.roll = 0;
    }
    //--------------------------------------------------------------------------------
    void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
    {
        out[0] = in1.Dot(in2[0]) + in2[0][3];
        out[1] = in1.Dot(in2[1]) + in2[1][3];
        out[2] = in1.Dot(in2[2]) + in2[2][3];
    }
    //--------------------------------------------------------------------------------
    void AngleVectors(const QAngle &angles, Vector& forward)
    {
        float	sp, sy, cp, cy;

        DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
        DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

        forward.x = cp * cy;
        forward.y = cp * sy;
        forward.z = -sp;
    }
    //--------------------------------------------------------------------------------
    void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up)
    {
        float sr, sp, sy, cr, cp, cy;

        DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
        DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
        DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

        forward.x = (cp * cy);
        forward.y = (cp * sy);
        forward.z = (-sp);
        right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
        right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
        right.z = (-1 * sr * cp);
        up.x = (cr * sp * cy + -sr * -sy);
        up.y = (cr * sp * sy + -sr * cy);
        up.z = (cr * cp);
    }
    //--------------------------------------------------------------------------------
    void VectorAngles(const Vector& forward, QAngle& angles)
    {
        float	tmp, yaw, pitch;

        if (forward[1] == 0 && forward[0] == 0) {
            yaw = 0;
            if (forward[2] > 0)
                pitch = 270;
            else
                pitch = 90;
        }
        else {
            yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
            if (yaw < 0)
                yaw += 360;

            tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
            pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
            if (pitch < 0)
                pitch += 360;
        }

        angles[0] = pitch;
        angles[1] = yaw;
        angles[2] = 0;
    }
    //--------------------------------------------------------------------------------
    static bool screen_transform(const Vector& in, Vector& out)
    {
        static auto& w2sMatrix = interfaces.engine->WorldToScreenMatrix();

        out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
        out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
        out.z = 0.0f;

        float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

        if (w < 0.001f) {
            out.x *= 100000;
            out.y *= 100000;
            return false;
        }

        out.x /= w;
        out.y /= w;

        return true;
    }
    //--------------------------------------------------------------------------------
    bool WorldToScreen(const Vector& in, Vector& out)
    {
        if (screen_transform(in, out)) {
            int w, h;
            interfaces.engine->GetScreenSize(w, h);

            out.x = (w / 2.0f) + (out.x * w) / 2.0f;
            out.y = (h / 2.0f) - (out.y * h) / 2.0f;

            return true;
        }
        return false;
    }
    //--------------------------------------------------------------------------------
}
