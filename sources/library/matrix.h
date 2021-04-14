#pragma once
#include "hlsdk.h"
#include <assert.h>

template<class T> class Matrix4x4
{
public:
    Matrix4x4() {};

    void Assign(Matrix4x4 &src)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
                m_Elements[i][j] = src[i][j];
        }
    }

    Matrix4x4 Add(Matrix4x4 &operand) const
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
                result[i][j] = m_Elements[i][j] + operand[i][j];
        }
        return result;
    }

    Matrix4x4 Sub(Matrix4x4 &operand) const
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
                result[i][j] = m_Elements[i][j] - operand[i][j];
        }
        return result;
    }

    Matrix4x4 Multiply(Matrix4x4 &operand) const
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                for (int k = 0; k < 4; ++k)
                    result[i][j] += m_Elements[i][k] * operand[k][j];
            }
        }
        return result;
    }

    Matrix4x4 Product(T x) const
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
                result[i][j] *= x;
        }
        return result;
    }

    Matrix4x4 Power(int power) const
    {
        assert(power > 1);
        Matrix4x4 thisMatrix = *this;
        Matrix4x4 result = thisMatrix;
        for (int i = 1; i < power; ++i) {
            result.Assign(result.Multiply(thisMatrix));
        }
        return result;
    }

    static Matrix4x4 CreateIdentity()
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (i == j)
                    result[i][j] = 1;
                else
                    result[i][j] = 0;
            }
        }
        return result;
    }

    static Matrix4x4 CreateTranslate(float x, float y, float z)
    {
        Matrix4x4 result = CreateIdentity();
        result[0][3] = x;
        result[1][3] = y;
        result[2][3] = z;
        return result;
    }

    static Matrix4x4 CreateRotateX(float angle)
    {
        Matrix4x4 result = CreateIdentity();
        const float pi = 3.141592f;
        const float radAngle = angle * (pi * 2 / 360);
        result[1][1] = cosf(radAngle);
        result[2][1] = sinf(radAngle);
        result[1][2] = -sinf(radAngle);
        result[2][2] = cosf(radAngle);
        return result;
    }

    static Matrix4x4 CreateRotateY(float angle)
    {
        Matrix4x4 result = CreateIdentity();
        const float pi = 3.141592f;
        const float radAngle = angle * (pi * 2 / 360);
        result[0][0] = cosf(radAngle);
        result[0][2] = sinf(radAngle);
        result[2][0] = -sinf(radAngle);
        result[2][2] = cosf(radAngle);
        return result;
    }

    static Matrix4x4 CreateRotateZ(float angle)
    {
        Matrix4x4 result = CreateIdentity();
        const float pi = 3.141592f;
        const float radAngle = angle * (pi * 2 / 360);
        result[0][0] = cosf(radAngle);
        result[0][1] = -sinf(radAngle);
        result[1][0] = sinf(radAngle);
        result[1][1] = cosf(radAngle);
        return result;
    }

    // HLSDK stuff
    vec3_t MultiplyVector(const vec3_t &operand, float w = 1.f)
    {
        Matrix4x4 &mat = *this;
        return vec3_t(
            mat[0][0] * operand[0] + mat[0][1] * operand[1] + mat[0][2] * operand[2] + mat[0][3] * w,
            mat[1][0] * operand[0] + mat[1][1] * operand[1] + mat[1][2] * operand[2] + mat[1][3] * w,
            mat[2][0] * operand[0] + mat[2][1] * operand[1] + mat[2][2] * operand[2] + mat[2][3] * w
        );
    }

    // operators
    Matrix4x4 operator+(Matrix4x4 &operand) const	{ return Add(operand); }
    Matrix4x4 operator-(Matrix4x4 &operand) const	{ return Sub(operand); }
    Matrix4x4 operator*(Matrix4x4 &operand) const	{ return Multiply(operand); }
    Matrix4x4 operator*(const T &operand) const		{ return Product(operand); }
    T *operator[](const int index)					{ return m_Elements[index]; }
    void operator=(Matrix4x4 &operand)				{ Assign(operand); }

private:
    T m_Elements[4][4] = { 0 };
};
