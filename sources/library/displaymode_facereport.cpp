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

CModeFaceReport::CModeFaceReport()
{
    m_colorProbe = { 0 };
    m_currentModel = nullptr;
    m_currentFace = nullptr;
    m_boundPoints = {};
}

void CModeFaceReport::Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText)
{
    const float lineLen = 11590.0f;
    vec3_t intersectPoint;
    vec3_t viewOrigin = g_LocalPlayer.GetViewOrigin();
    vec3_t viewDir = g_LocalPlayer.GetViewDirection();
    m_currentFace = TraceSurface(viewOrigin, viewDir, lineLen, intersectPoint);

    screenText.Clear();
    if (m_currentFace)
    {
        const mplane_t *plane = m_currentFace->plane;
        const Engine::texture_t *texture = Engine::CastType(m_currentFace->texinfo->texture);
        vec3_t planeCenter = plane->normal * plane->dist;
        int32_t lightmapWidth = (m_currentFace->extents[0] >> 4) + 1;
        int32_t lightmapHeight = (m_currentFace->extents[1] >> 4) + 1;

        m_colorProbe = { 0 };
        GetSurfaceBoundingBox(m_currentFace, m_currentFaceBounds);
        const vec3_t &surfSize = m_currentFaceBounds.GetSize();

        screenText.PushPrintf("Model Name: %s", m_currentModel->name);
        screenText.PushPrintf("Texture Name: %s", texture->name);
        screenText.PushPrintf("Texture Size: %d x %d", texture->width, texture->height);
        screenText.PushPrintf("Lightmap Size: %d x %d", lightmapWidth, lightmapHeight);
        screenText.PushPrintf("Edges: %d", m_currentFace->numedges);
        screenText.PushPrintf("Surfaces: %d", m_currentModel->nummodelsurfaces);
        screenText.PushPrintf("Bounds: (%.1f; %.1f; %.1f)", surfSize.x, surfSize.y, surfSize.z);
        screenText.PushPrintf("Normal: (%.1f; %.1f; %.1f)", plane->normal.x, plane->normal.y, plane->normal.z);
        screenText.PushPrintf("Intersect point: (%.1f; %.1f; %.1f)", intersectPoint.x, intersectPoint.y, intersectPoint.z);

        if (GetLightmapProbe(m_currentFace, intersectPoint, m_colorProbe))
        {
            screenText.PushPrintf("Lightmap Color: %d %d %d",
                m_colorProbe.r, m_colorProbe.g, m_colorProbe.b);
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
    if (m_currentFace && m_currentModel)
    {
        g_pClientEngfuncs->pTriAPI->RenderMode(kRenderTransColor);
        g_pClientEngfuncs->pTriAPI->CullFace(TRI_NONE);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        DrawFaceOutline(m_currentFace);
        //DrawSurfaceBounds(m_pCurrentFace);
        g_pClientEngfuncs->pTriAPI->RenderMode(kRenderNormal);
        g_pClientEngfuncs->pTriAPI->CullFace(TRI_FRONT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
    }
}

bool CModeFaceReport::KeyInput(bool keyDown, int keyNum, const char *bindName)
{
    if (keyDown && keyNum == 'v')
    {
        if (m_currentFace)
        {
            const texture_t *texture = m_currentFace->texinfo->texture;
            g_pClientEngfuncs->Con_Printf("%s %d %d %d\n", 
                texture->name, m_colorProbe.r, m_colorProbe.g, m_colorProbe.b);
            g_pClientEngfuncs->pfnPlaySoundByName("buttons/blip1.wav", 0.8f);
            return false;
        }
    }
    return true;
}

void CModeFaceReport::HandleChangelevel()
{
    m_currentModel = nullptr;
    m_currentFace = nullptr;
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

void CModeFaceReport::GetSurfaceBoundingBox(Engine::msurface_t *surf, CBoundingBox &bbox)
{
    bbox = CBoundingBox();
    m_boundPoints.reserve(surf->numedges * 2);
    m_boundPoints.clear();
 
    for (int i = 0; i < surf->numedges; ++i)
    {
        int edgeIndex = m_currentModel->surfedges[surf->firstedge + i];
        const medge_t *edge = m_currentModel->edges + (edgeIndex >= 0 ? edgeIndex : -edgeIndex);
        const mvertex_t *vertex = m_currentModel->vertexes + (edgeIndex >= 0 ? edge->v[0] : edge->v[1]);
        m_boundPoints.push_back(vertex->position);
    }

    if (m_boundPoints.size() > 0)
    {
        bbox.SetCenterToPoint(m_boundPoints[0]);
        for (size_t i = 0; i < m_boundPoints.size(); ++i)
        {
            const vec3_t &vertex = m_boundPoints[i];
            bbox.ExpandToPoint(vertex);
        }
    }
}

Engine::mleaf_t *CModeFaceReport::PointInLeaf(vec3_t point, Engine::mnode_t *node)
{
    for (;;)
    {
        if (node->contents < 0)
            return reinterpret_cast<Engine::mleaf_t *>(node);
        node = node->children[PlaneDiff(point, node->plane) <= 0];
    }
    return NULL;
}

void CModeFaceReport::DrawFaceOutline(Engine::msurface_t *surf)
{
    const Color lineColor = Color(0.f, 1.f, 1.f, 1.f);
    g_pClientEngfuncs->pTriAPI->Begin(TRI_LINES);
    g_pClientEngfuncs->pTriAPI->Color4f(lineColor.Red(), lineColor.Green(), lineColor.Blue(), lineColor.Alpha());
    for (int i = 0; i < surf->numedges; ++i)
    {
        int edgeIndex = m_currentModel->surfedges[surf->firstedge + i];
        const medge_t *edge = m_currentModel->edges + (edgeIndex >= 0 ? edgeIndex : -edgeIndex);
        vec3_t &vertex1 = m_currentModel->vertexes[edge->v[0]].position;
        vec3_t &vertex2 = m_currentModel->vertexes[edge->v[1]].position;
        g_pClientEngfuncs->pTriAPI->Vertex3fv(vertex1);
        g_pClientEngfuncs->pTriAPI->Vertex3fv(vertex2);
    }
    g_pClientEngfuncs->pTriAPI->End();
}

void CModeFaceReport::DrawSurfaceBounds(Engine::msurface_t *surf)
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

bool CModeFaceReport::GetLightmapProbe(Engine::msurface_t *surf, const vec3_t &point, color24 &probe)
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

Engine::msurface_t *CModeFaceReport::TraceSurface(vec3_t origin, vec3_t dir, float distance, vec3_t &intersect)
{
    int entityIndex = TraceEntity(origin, dir, distance, intersect);
    cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);
    model_t *worldModel = g_pClientEngfuncs->hudGetModelByIndex(1);
    Engine::mnode_t *firstNode = nullptr;
    Engine::msurface_t *surf = nullptr;

    if (entity)
    {
        if (entity->model->type != mod_brush) {
            m_currentModel = worldModel;
        }
        else {
            m_currentModel = entity->model;
        }

        firstNode = Engine::CastType(m_currentModel->nodes);
        firstNode = m_currentModel->hulls[0].firstclipnode + firstNode;

        vec3_t endPoint = origin + (dir * distance);
        surf = SurfaceAtPoint(m_currentModel, firstNode, origin, endPoint, intersect);
        if (surf) {
            return surf;
        }
    }

    m_currentModel = nullptr;
    return nullptr;
}

Engine::msurface_t *CModeFaceReport::SurfaceAtPoint(model_t *pModel, Engine::mnode_t *node, vec3_t start, vec3_t end, vec3_t &intersect)
{
    mplane_t    *plane;
    vec3_t		mid;
    mtexinfo_t  *tex;
    Engine::msurface_t *surf;
    
    if (node->contents < 0) {
        return nullptr;
    }

    plane = node->plane;
    float front = DotProduct(start, plane->normal) - plane->dist;
    float back = DotProduct(end, plane->normal) - plane->dist;
    int side = front < 0;

    if ((back < 0) == side) {
        return SurfaceAtPoint(pModel, node->children[side], start, end, intersect);
    }

    float frac = front / (front - back);
    mid[0] = start[0] + (end[0] - start[0]) * frac;
    mid[1] = start[1] + (end[1] - start[1]) * frac;
    mid[2] = start[2] + (end[2] - start[2]) * frac;

    surf = SurfaceAtPoint(pModel, node->children[side], start, mid, intersect);
    if (surf) {
        return surf;
    }

    if ((back < 0) == side) {
        return nullptr;
    }

    surf = Engine::CastType(pModel->surfaces) + node->firstsurface;
    for (int i = 0; i < node->numsurfaces; i++, surf++)
    {
        tex = surf->texinfo;

        int s = DotProduct(mid, tex->vecs[0]) + tex->vecs[0][3];
        int t = DotProduct(mid, tex->vecs[1]) + tex->vecs[1][3];

        if (s < surf->texturemins[0] || t < surf->texturemins[1]) {
            continue;
        }

        int ds = s - surf->texturemins[0];
        int dt = t - surf->texturemins[1];

        if (ds > surf->extents[0] || dt > surf->extents[1]) {
            continue;
        }

        intersect = mid;
        return surf;
    }
    return SurfaceAtPoint(pModel, node->children[!side], mid, end, intersect);
}
