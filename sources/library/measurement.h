#pragma once
#include "stdafx.h"

enum
{
    SNAPMODE_FREE,
    SNAPMODE_AXIS_X,
    SNAPMODE_AXIS_Y,
    SNAPMODE_AXIS_Z,
    SNAPMODE_MAX,
};

vec3_t          GetPointOriginA();
vec3_t          GetPointOriginB();
float           GetPointsDistance();
void            MeasurementHandleInput(int keyCode);
void            MeasurementVisualize(int screenWidth, int screenHeight);
float           GetLineElevationAngle();
const char *    GetSnapModeName();
