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

#include "opengl_primitives_renderer.h"
#include "client_module.h"
#include <cassert>
#ifdef _WIN32
#include <gl/GL.h>
#endif

void COpenGLPrimitivesRenderer::Begin()
{
	// disable textures & culling, enable alpha blending
	g_pClientEngfuncs->pTriAPI->RenderMode(kRenderTransColor);
    g_pClientEngfuncs->pTriAPI->CullFace(TRI_NONE);
    glDisable(GL_TEXTURE_2D);
}

void COpenGLPrimitivesRenderer::End()
{
	// restore original OpenGL state, now doing it wrong, but :)
	g_pClientEngfuncs->pTriAPI->RenderMode(kRenderNormal);
    g_pClientEngfuncs->pTriAPI->CullFace(TRI_FRONT);
    glEnable(GL_TEXTURE_2D);
}

void COpenGLPrimitivesRenderer::ToggleDepthTest(bool state)
{
	if (state) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}

void COpenGLPrimitivesRenderer::RenderPrimitives(PrimitiveType type, 
	const std::vector<vec3_t> &verticesBuffer, 
	const std::vector<Color> &colorBuffer)
{
	assert(!verticesBuffer.empty() && !verticesBuffer.empty());
	glBegin(GetGlPrimitiveEnum(type));
	for (size_t i = 0; i < verticesBuffer.size(); ++i) 
	{
		// TODO optimize it, avoid using immediate mode
		const Color &vertexColor = colorBuffer[std::min(i, colorBuffer.size() - 1)];
		glColor4f(vertexColor.Red(), vertexColor.Green(), vertexColor.Blue(), vertexColor.Alpha());
		glVertex3fv(verticesBuffer[i]);
	}
	glEnd();
}

uint32_t COpenGLPrimitivesRenderer::GetGlPrimitiveEnum(PrimitiveType pt) const
{
	switch (pt) {
		case PrimitiveType::Quads: return GL_QUADS;
		case PrimitiveType::LineLoop: return GL_LINE_LOOP;
		default: return GL_NONE;
	}
}
