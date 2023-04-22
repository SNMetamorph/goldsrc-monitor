/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#pragma once
#include <stdint.h>
#include <random>

class Color
{
public:
    Color()
    {
        Setup(1.f, 1.f, 1.f, 1.f);
    };

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    {
        Setup(r, g, b, a);
    };

    Color(float r, float g, float b, float a = 1.0f)
    {
        Setup(r, g, b, a);
    };

    inline void Initialize(uint32_t color)
    {
        uint8_t r = (color >> 24) & 0xFF;
        uint8_t g = (color >> 16) & 0xFF;
        uint8_t b = (color >> 8) & 0xFF;
        uint8_t a = color & 0xFF;
        Setup(r, g, b, a);
    }

    inline void Initialize(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        Setup(r, g, b, a);
    }

    inline void Normalize()
    {
        float maxValue = m_Components[0];
        for (int i = 0; i < 4; ++i) 
        {
            if (m_Components[i] > maxValue) {
                maxValue = m_Components[i];
            }
        }
        m_Components[0] /= maxValue;
        m_Components[1] /= maxValue;
        m_Components[2] /= maxValue;
        m_Components[3] /= maxValue;
    }
    
    inline Color GetNormalized() const
    {
        Color result = *this;
        result.Normalize();
        return result;
    }

    inline uint32_t GetUint32() const
    {
        uint8_t r = static_cast<uint8_t>(m_Components[0] * 255);
        uint8_t g = static_cast<uint8_t>(m_Components[1] * 255);
        uint8_t b = static_cast<uint8_t>(m_Components[2] * 255);
        uint8_t a = static_cast<uint8_t>(m_Components[3] * 255);
        return (r << 24) | (g << 16) | (b << 8) | a;
    }

    inline float Red() const    { return m_Components[0]; }
    inline float Green() const  { return m_Components[1]; }
    inline float Blue() const   { return m_Components[2]; }
    inline float Alpha() const  { return m_Components[3]; }

    inline void SetRed(float v)     { m_Components[0] = v; }
    inline void SetGreen(float v)   { m_Components[1] = v; }
    inline void SetBlue(float v)    { m_Components[2] = v; }
    inline void SetAlpha(float v)   { m_Components[3] = v; }

    static inline Color GetRandom(int seed, float minBound = 0.2f, float maxBound = 1.f)
    {
        std::default_random_engine gen;
        std::uniform_real_distribution<float> dist(minBound, maxBound);
        gen.seed(seed);
        return Color(dist(gen), dist(gen), dist(gen));
    }

    Color& operator=(const Color &operand)
    {
        m_Components[0] = operand.Red();
        m_Components[1] = operand.Green();
        m_Components[2] = operand.Blue();
        m_Components[3] = operand.Alpha();
        return *this;
    }

private:
    inline void Setup(float r, float g, float b, float a)
    {
        m_Components[0] = r;
        m_Components[1] = g;
        m_Components[2] = b;
        m_Components[3] = a;
    }
    float m_Components[4];
};
