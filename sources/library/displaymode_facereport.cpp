#include "displaymode_facereport.h"
#include "client_module.h"
#include "utils.h"
#include "local_player.h"
#include <vector>
#include <gl/GL.h>

#define PlaneDiff(point,plane) (((plane)->type < 3 ? (point)[(plane)->type] : DotProduct((point), (plane)->normal)) - (plane)->dist)

void CModeFaceReport::Render2D(int scrWidth, int scrHeight, CStringStack &screenText)
{
    const float lineLen = 11590.0f;
    vec3_t viewOrigin = g_LocalPlayer.GetViewOrigin();
    vec3_t viewDir = g_LocalPlayer.GetViewDirection();
    EngineTypes::msurface_t *surface = TraceSurface(viewOrigin, viewDir, lineLen);
    
    screenText.Clear();
    if (surface)
    {
        color24 colorProbe = { 0 };
        const mplane_t *plane = surface->plane;
        const texture_t *texture = surface->texinfo->texture;
        vec3_t planeCenter = plane->normal * plane->dist;
        vec3_t s = surface->texinfo->vecs[0];
        vec3_t t = surface->texinfo->vecs[1];
        int lightmapWidth = (surface->extents[0] >> 4) + 1;
        int lightmapHeight = (surface->extents[1] >> 4) + 1;

        if (surface->samples) {
            colorProbe = surface->samples[0];
        }

        screenText.PushPrintf("Texture Name: %s", texture->name);
        screenText.PushPrintf("Width: %d", texture->width);
        screenText.PushPrintf("Height: %d", texture->height);
        screenText.PushPrintf("Lightmap Color: %d %d %d", colorProbe.r, colorProbe.g, colorProbe.b);
        screenText.PushPrintf("Lightmap Width: %d", lightmapWidth);
        screenText.PushPrintf("Lightmap Height: %d", lightmapHeight);
        screenText.PushPrintf("Origin: (%.1f; %.1f; %.1f)", planeCenter.x, planeCenter.y, planeCenter.z);
        screenText.PushPrintf("Normal: (%.1f; %.1f; %.1f)", plane->normal.x, plane->normal.y, plane->normal.z);
        screenText.PushPrintf("Vector S: (%.1f; %.1f; %.1f)", s.x, s.y, s.z);
        screenText.PushPrintf("Vector T: (%.1f; %.1f; %.1f)", t.x, t.y, t.z);
    }
    else {
        screenText.Push("Face not found");
    }
   
    m_pCurrentFace = surface;
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
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        DrawFaceOutline(m_pCurrentFace);
        //DrawSurfaceBounds(m_pCurrentFace);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
    }
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
    std::vector<vec3_t> vertices;
    vertices.reserve(surf->numedges * 2);
    bbox = CBoundingBox();

    for (int i = 0; i < surf->numedges; ++i)
    {
        int edgeIndex = m_pCurrentModel->surfedges[surf->firstedge + i];
        const medge_t *edge = m_pCurrentModel->edges + (edgeIndex >= 0 ? edgeIndex : -edgeIndex);
        const mvertex_t *vertex = m_pCurrentModel->vertexes + (edgeIndex >= 0 ? edge->v[0] : edge->v[1]);
        vertices.push_back(vertex->position);
    }

    if (vertices.size() > 0)
    {
        bbox.SetCenterToPoint(vertices[0]);
        for (int i = 0; i < vertices.size(); ++i)
        {
            const vec3_t &vertex = vertices[i];
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
    const Color boxColor = Color(0.f, 1.f, 0.f, 1.f);

    GetSurfaceBoundingBox(surf, bounds);
    Utils::DrawCuboid(
        bounds.GetCenterPoint(), 
        nullVec, 
        nullVec, 
        bounds.GetSize(), 
        boxColor
    );
}

EngineTypes::msurface_t *CModeFaceReport::TraceSurface(vec3_t origin, vec3_t dir, float distance)
{
    vec3_t intersectPoint;
    CBoundingBox surfaceBounds;
    int entityIndex = TraceEntity(origin, dir, distance, intersectPoint);
    cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);

    if (entity->model->type != mod_brush) {
        m_pCurrentModel = g_pClientEngfuncs->hudGetModelByIndex(1);
   }
    else {
        m_pCurrentModel = entity->model;
    }

    EngineTypes::mleaf_t *leaf = PointInLeaf(intersectPoint, (EngineTypes::mnode_t*)m_pCurrentModel->nodes);
    if (leaf)
    {
        for (int i = 0; i < leaf->nummarksurfaces; ++i)
        {
            EngineTypes::msurface_t *surf = (EngineTypes::msurface_t *)leaf->firstmarksurface[i];
            GetSurfaceBoundingBox(surf, surfaceBounds);
            if (Utils::TraceBBoxLine(surfaceBounds, origin, origin + dir * distance) < 1.0f) {
                return surf;
            }
        }
    }

    m_pCurrentModel = nullptr;
    return nullptr;
}
