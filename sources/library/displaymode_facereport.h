#pragma once
#include "display_mode.h"
#include "hlsdk.h"
#include "engine_types.h"
#include "bounding_box.h"

class CModeFaceReport : public IDisplayMode
{
public: 
    CModeFaceReport() {};
    virtual ~CModeFaceReport() {};

    void Render2D(int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override;
    bool KeyInput(int, int, const char *) override { return true; };
    void HandleChangelevel() override {};
    DisplayModeIndex GetModeIndex() override { return DISPLAYMODE_FACEREPORT; };

private:
    int TraceEntity(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect);
    void GetSurfaceBoundingBox(EngineTypes::msurface_t *surf, CBoundingBox &bbox);
    void DrawFaceOutline(EngineTypes::msurface_t *surf);
    void DrawSurfaceBounds(EngineTypes::msurface_t *surf);
    EngineTypes::mleaf_t *PointInLeaf(vec3_t point, EngineTypes::mnode_t *node);
    EngineTypes::msurface_t *TraceSurface(vec3_t origin, vec3_t dir, float distance);

    model_t *m_pCurrentModel = nullptr;
    EngineTypes::msurface_t *m_pCurrentFace = nullptr;
};
