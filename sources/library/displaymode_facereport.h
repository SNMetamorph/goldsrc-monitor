#pragma once
#include "display_mode.h"
#include "hlsdk.h"
#include "engine_types.h"
#include "bounding_box.h"
#include <vector>

class CModeFaceReport : public IDisplayMode
{
public: 
    CModeFaceReport() {};
    virtual ~CModeFaceReport() {};

    void Render2D(int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override;
    bool KeyInput(int isKeyDown, int keyNum, const char *bindName) override;
    void HandleChangelevel() override;
    DisplayModeIndex GetModeIndex() override { return DISPLAYMODE_FACEREPORT; };

private:
    int TraceEntity(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect);
    void GetSurfaceBoundingBox(EngineTypes::msurface_t *surf, CBoundingBox &bbox);
    void DrawFaceOutline(EngineTypes::msurface_t *surf);
    void DrawSurfaceBounds(EngineTypes::msurface_t *surf);
    bool IsSurfaceIntersected(EngineTypes::msurface_t *surf, vec3_t p1, vec3_t p2);
    bool GetLightmapProbe(EngineTypes::msurface_t *surf, const vec3_t &point, color24 &probe);
    EngineTypes::mleaf_t *PointInLeaf(vec3_t point, EngineTypes::mnode_t *node);
    EngineTypes::msurface_t *TraceSurface(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect);

    color24 m_ColorProbe = { 0 };
    std::vector<vec3_t> m_BoundPoints;
    model_t *m_pCurrentModel = nullptr;
    EngineTypes::msurface_t *m_pCurrentFace = nullptr;
};
