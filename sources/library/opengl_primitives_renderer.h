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
#include "primitives_renderer.h"
#include <stdint.h>

class COpenGLPrimitivesRenderer : public IPrimitivesRenderer
{
public:
	virtual void Begin() override;
	virtual void End() override;
	virtual void ToggleDepthTest(bool state) override;
	virtual void RenderPrimitives(PrimitiveType type, 
		const std::vector<vec3_t>& verticesBuffer, 
		const std::vector<Color>& colorBuffer) override;

private:
	uint32_t GetGlPrimitiveEnum(PrimitiveType pt) const;
};
