#include "displaymode_facereport.h"
#include "client_module.h"
#include "utils.h"
#include "local_player.h"
#include <gl/GL.h>

#define PlaneDiff(point,plane) (((plane)->type < 3 ? (point)[(plane)->type] : DotProduct((point), (plane)->normal)) - (plane)->dist)
#define	SURF_PLANEBACK		2
#define	SURF_DRAWSKY		4
#define SURF_DRAWSPRITE		8
#define SURF_DRAWTURB		0x10
#define SURF_DRAWTILED		0x20
#define SURF_DRAWBACKGROUND	0x40

void CModeFaceReport::Render2D(int scrWidth, int scrHeight, CStringStack &screenText)
{
    const float lineLen = 11590.0f;
    vec3_t intersectPoint;
    vec3_t viewOrigin = g_LocalPlayer.GetViewOrigin();
    vec3_t viewDir = g_LocalPlayer.GetViewDirection();
    m_pCurrentFace = TraceSurface(viewOrigin, viewDir, lineLen, intersectPoint);
    
    screenText.Clear();
    if (m_pCurrentFace)
    {
        const mplane_t *plane = m_pCurrentFace->plane;
        const texture_t *texture = m_pCurrentFace->texinfo->texture;
        vec3_t planeCenter = plane->normal * plane->dist;
        m_ColorProbe = { 0 };

        screenText.PushPrintf("Texture Name: %s", texture->name);
        screenText.PushPrintf("Width: %d", texture->width);
        screenText.PushPrintf("Height: %d", texture->height);
        screenText.PushPrintf("Edges: %d", m_pCurrentFace->numedges);
        screenText.PushPrintf("Normal: (%.1f; %.1f; %.1f)", plane->normal.x, plane->normal.y, plane->normal.z);

        if (GetLightmapProbe(m_pCurrentFace, intersectPoint, m_ColorProbe)) 
        {
            screenText.PushPrintf("Lightmap Color: %d %d %d", 
                m_ColorProbe.r, m_ColorProbe.g, m_ColorProbe.b);
            screenText.Push("Press V to print lightmap info");
        } 
    }
    else {
        screenText.Push("Surface not found");
    }
   
    Utils::DrawStringStack(
        static_cast<int>(ConVars::gsm_margin_right->value),
        static_cast<int>(ConVars::gsm_margin_up->value),
        screenText
    );
}

void CModeFaceReport::Render3D()
{
    if (m_pCurrentFace && m_pCurrentModel)
    {
        g_pClientEngfuncs->pTriAPI->RenderMode(kRenderTransColor);
        g_pClientEngfuncs->pTriAPI->CullFace(TRI_NONE);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        DrawFaceOutline(m_pCurrentFace);
        //DrawSurfaceBounds(m_pCurrentFace);
        g_pClientEngfuncs->pTriAPI->RenderMode(kRenderNormal);
        g_pClientEngfuncs->pTriAPI->CullFace(TRI_FRONT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
    }
}

bool CModeFaceReport::KeyInput(int isKeyDown, int keyNum, const char *bindName)
{
    if (keyNum == 'v')
    {
        if (m_pCurrentFace)
        {
            const texture_t *texture = m_pCurrentFace->texinfo->texture;
            g_pClientEngfuncs->Con_Printf("%s %d %d %d\n", 
                texture->name, m_ColorProbe.r, m_ColorProbe.g, m_ColorProbe.b);
            g_pClientEngfuncs->pfnPlaySoundByName("buttons/blip1.wav", 0.8f);
            return false;
        }
    }
    return true;
}

void CModeFaceReport::HandleChangelevel()
{
    m_pCurrentModel = nullptr;
    m_pCurrentFace = nullptr;
}

int CModeFaceReport::TraceEntity(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect)
{
    pmtrace_t traceData;
    int ignoredEnt = -1;

    if (g_LocalPlayer.IsSpectate()) {
        ignoredEnt = g_LocalPlayer.GetSpectateTargetIndex();
    }

    Utils::TraceLine(origin, dir, distance, &traceData, ignoredEnt);
    intersect = origin + dir * distance * traceData.fraction;
    if (traceData.fraction < 1.f)
    {
        if (traceData.ent > 0)
            return g_pClientEngfuncs->pEventAPI->EV_IndexFromTrace(&traceData);
    }
    return 0;
}

void CModeFaceReport::GetSurfaceBoundingBox(EngineTypes::msurface_t *surf, CBoundingBox &bbox)
{
    bbox = CBoundingBox();
    m_BoundPoints.reserve(surf->numedges * 2);
    m_BoundPoints.clear();
 
    for (int i = 0; i < surf->numedges; ++i)
    {
        int edgeIndex = m_pCurrentModel->surfedges[surf->firstedge + i];
        const medge_t *edge = m_pCurrentModel->edges + (edgeIndex >= 0 ? edgeIndex : -edgeIndex);
        const mvertex_t *vertex = m_pCurrentModel->vertexes + (edgeIndex >= 0 ? edge->v[0] : edge->v[1]);
        m_BoundPoints.push_back(vertex->position);
    }

    if (m_BoundPoints.size() > 0)
    {
        bbox.SetCenterToPoint(m_BoundPoints[0]);
        for (size_t i = 0; i < m_BoundPoints.size(); ++i)
        {
            const vec3_t &vertex = m_BoundPoints[i];
            bbox.ExpandToPoint(vertex);
        }
    }
}

EngineTypes::mleaf_t *CModeFaceReport::PointInLeaf(vec3_t point, EngineTypes::mnode_t *node)
{
    for (;;)
    {
        if (node->contents < 0)
            return (EngineTypes::mleaf_t *)node;
        node = node->children[PlaneDiff(point, node->plane) <= 0];
    }
    return NULL;
}

void CModeFaceReport::DrawFaceOutline(EngineTypes::msurface_t *surf)
{
    const Color lineColor = Color(0.f, 1.f, 1.f, 1.f);
    g_pClientEngfuncs->pTriAPI->Begin(TRI_LINES);
    g_pClientEngfuncs->pTriAPI->Color4f(lineColor.Red(), lineColor.Green(), lineColor.Blue(), lineColor.Alpha());
    for (int i = 0; i < surf->numedges; ++i)
    {
        int edgeIndex = m_pCurrentModel->surfedges[surf->firstedge + i];
        const medge_t *edge = m_pCurrentModel->edges + (edgeIndex >= 0 ? edgeIndex : -edgeIndex);
        vec3_t &vertex1 = m_pCurrentModel->vertexes[edge->v[0]].position;
        vec3_t &vertex2 = m_pCurrentModel->vertexes[edge->v[1]].position;
        g_pClientEngfuncs->pTriAPI->Vertex3fv(vertex1);
        g_pClientEngfuncs->pTriAPI->Vertex3fv(vertex2);
    }
    g_pClientEngfuncs->pTriAPI->End();
}

void CModeFaceReport::DrawSurfaceBounds(EngineTypes::msurface_t *surf)
{
    CBoundingBox bounds;
    const vec3_t nullVec = vec3_t(0, 0, 0);
    const Color boxColor = Color(1.f, 0.f, 0.f, 1.f);

    GetSurfaceBoundingBox(surf, bounds);
    Utils::DrawCuboid(
        bounds.GetCenterPoint(), 
        nullVec, 
        nullVec, 
        bounds.GetSize(), 
        boxColor
    );
}

bool CModeFaceReport::IsSurfaceIntersected(EngineTypes::msurface_t *surf, vec3_t p1, vec3_t p2)
{
    CBoundingBox surfaceBounds;
    GetSurfaceBoundingBox(surf, surfaceBounds);
    if (Utils::TraceBBoxLine(surfaceBounds, p1, p2) < 1.0f) {
        return true;
    }
    return false;
}

bool CModeFaceReport::GetLightmapProbe(EngineTypes::msurface_t *surf, const vec3_t &point, color24 &probe)
{
    if (surf->flags & SURF_DRAWTILED)
        return false; // no lightmaps

    mtexinfo_t *tex = surf->texinfo;
    int s = DotProduct(point, tex->vecs[0]) + tex->vecs[0][3];
    int t = DotProduct(point, tex->vecs[1]) + tex->vecs[1][3];

    if (s < surf->texturemins[0] || t < surf->texturemins[1])
        return false;

    int ds = s - surf->texturemins[0];
    int dt = t - surf->texturemins[1];

    if (ds > surf->extents[0] || dt > surf->extents[1])
        return false;

    if (!surf->samples)
        return false;

    ds >>= 4;
    dt >>= 4;

    color24 *lightmap = surf->samples;
    int lw = (surf->extents[0] >> 4) + 1;
    int lh = (surf->extents[1] >> 4) + 1;

    if (lightmap)
    {
        // this code also should assume lightstyles but this info not available here
        lightmap += dt * lw + ds;
        probe.r = lightmap->r;
        probe.g = lightmap->g;
        probe.b = lightmap->b;
        return true;
    }
    return false;
}

EngineTypes::msurface_t *CModeFaceReport::TraceSurface(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect)
{
    int entityIndex = TraceEntity(origin, dir, distance, intersect);
    cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);
    model_t *worldModel = g_pClientEngfuncs->hudGetModelByIndex(1);

    if (entity->model->type != mod_brush) {
        m_pCurrentModel = worldModel;
    }
    else {
        m_pCurrentModel = entity->model;
    }

    if (m_pCurrentModel == worldModel)
    {
        EngineTypes::mleaf_t *leaf = PointInLeaf(intersect, (EngineTypes::mnode_t *)m_pCurrentModel->nodes);
        if (leaf)
        {
            for (int i = 0; i < leaf->nummarksurfaces; ++i)
            {
                EngineTypes::msurface_t *surf = (EngineTypes::msurface_t *)leaf->firstmarksurface[i];
                if (IsSurfaceIntersected(surf, origin, origin + dir * distance)) {
                    return surf;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < m_pCurrentModel->nummodelsurfaces; ++i)
        {
            int surfIndex = m_pCurrentModel->firstmodelsurface + i;
            EngineTypes::msurface_t *surf = (EngineTypes::msurface_t *)m_pCurrentModel->surfaces + surfIndex;
            if (IsSurfaceIntersected(surf, origin, origin + dir * distance)) {
                return surf;
            }
        }
    }

    m_pCurrentModel = nullptr;
    return nullptr;
}