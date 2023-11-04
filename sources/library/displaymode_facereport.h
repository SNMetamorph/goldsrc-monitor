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
#include "display_mode.h"
#include "hlsdk.h"
#include "engine_types.h"
#include "bounding_box.h"
#include <vector>

class CModeFaceReport : public IDisplayMode
{
public: 
    CModeFaceReport();
    virtual ~CModeFaceReport() {};

    void Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override;
    bool KeyInput(bool keyDown, int keyCode, const char *bindName) override;
    void HandleChangelevel() override;
    DisplayModeType GetModeIndex() override { return DisplayModeType::FaceReport; };

private:
    int TraceEntity(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect);
    void GetSurfaceBoundingBox(Engine::msurface_t *surf, CBoundingBox &bbox);
    void DrawFaceOutline(Engine::msurface_t *surf);
    void DrawSurfaceBounds(Engine::msurface_t *surf);
    bool GetLightmapProbe(Engine::msurface_t *surf, const vec3_t &point, color24 &probe);
    Engine::mleaf_t *PointInLeaf(vec3_t point, Engine::mnode_t *node);
    Engine::msurface_t *TraceSurface(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect);
    Engine::msurface_t *SurfaceAtPoint(model_t *pModel, Engine::mnode_t *node, vec3_t start, vec3_t end, vec3_t &intersect);

    color24 m_colorProbe;
    model_t *m_currentModel;
    Engine::msurface_t *m_currentFace;
    CBoundingBox m_currentFaceBounds;
    std::vector<vec3_t> m_boundPoints;
};
