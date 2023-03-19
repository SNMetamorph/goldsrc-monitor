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
    DisplayModeIndex GetModeIndex() override { return DISPLAYMODE_FACEREPORT; };

private:
    int TraceEntity(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect);
    void GetSurfaceBoundingBox(Engine::msurface_t *surf, CBoundingBox &bbox);
    void DrawFaceOutline(Engine::msurface_t *surf);
    void DrawSurfaceBounds(Engine::msurface_t *surf);
    bool GetLightmapProbe(Engine::msurface_t *surf, const vec3_t &point, color24 &probe);
    Engine::mleaf_t *PointInLeaf(vec3_t point, Engine::mnode_t *node);
    Engine::msurface_t *TraceSurface(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect);
    Engine::msurface_t *SurfaceAtPoint(model_t *pModel, Engine::mnode_t *node, vec3_t start, vec3_t end, vec3_t &intersect);

    color24 m_ColorProbe;
    model_t *m_pCurrentModel;
    Engine::msurface_t *m_pCurrentFace;
    CBoundingBox m_CurrentFaceBounds;
    std::vector<vec3_t> m_BoundPoints;
};
