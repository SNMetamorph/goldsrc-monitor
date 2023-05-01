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
#include "hlsdk.h"
#include "color.h"
#include <vector>

class IPrimitivesRenderer
{
public:
	enum class PrimitiveType {
		Quads,
		LineLoop
	};

	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void ToggleDepthTest(bool state) = 0;
	virtual void RenderPrimitives(PrimitiveType type, 
		const std::vector<vec3_t> &verticesBuffer, 
		const std::vector<Color> &colorBuffer) = 0;
};
