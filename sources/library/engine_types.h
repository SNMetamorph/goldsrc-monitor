#pragma once
#include "hlsdk.h"

/*
* For structures mnode_t, mleaf_t, msurface_t, texture_t there is two
* incompatible versions: for software and hardware renderers.
* In this header presented only hardware renderer structures (they also present in Xash3D)
* By default in HLSDK presented only software renderer structures.
* To prevent collision with HLSDK these structures located in separate "Engine" namespace.
*/

namespace Engine
{
	struct mnode_t
	{
		// common with leaf
		int				contents;		// 0, to differentiate from leafs
		int				visframe;		// node needs to be traversed if current
		float			minmaxs[6];		// for bounding box culling. HW.
		mnode_t *parent;
		// node specific
		mplane_t *plane;
		mnode_t *children[2];
		unsigned short	firstsurface;
		unsigned short	numsurfaces;
	};

	struct mleaf_t
	{
		// common with node
		int         contents;       // wil be a negative contents number
		int         visframe;       // node needs to be traversed if current
		float       minmaxs[6];     // for bounding box culling HW
		mnode_t *parent;
		// leaf specific
		byte *compressed_vis;
		struct efrag_s *efrags;
		msurface_t **firstmarksurface;
		int         nummarksurfaces;
		int         key;            // BSP sequence number for leaf's contents
		byte        ambient_sound_level[NUM_AMBIENTS];
	};

	struct msurface_t
	{
		int         visframe;       // should be drawn when node is crossed

		mplane_t *plane;
		int         flags;

		int         firstedge;  // look up in model->surfedges[], negative numbers
		int         numedges;   // are backwards edges

		short       texturemins[2];
		short       extents[2];

		int         light_s, light_t;           // gl lightmap coordinates

		struct glpoly_t *polys;                     // multiple if warped
		struct msurface_t *texturechain;

		mtexinfo_t *texinfo;

		// lighting info
		int         dlightframe;
		int         dlightbits;

		int         lightmaptexturenum;
		byte        styles[MAXLIGHTMAPS];
		int         cached_light[MAXLIGHTMAPS]; // values currently used in lightmap
		qboolean    cached_dlight;              // true if dynamic light in cache

	//  byte		*samples;                   // [numstyles*surfsize]
		color24 *samples;                   // note: this is the actual lightmap data for this surface
		decal_t *pdecals;
	};

	struct texture_t
	{
		char        name[16];
		unsigned    width, height;
		int         gl_texturenum;
		struct msurface_s *texturechain;  // for gl_texsort drawing
		int         anim_total;             // total tenths in sequence ( 0 = no)
		int         anim_min, anim_max;     // time for this frame min <=time< max
		struct texture_s *anim_next;        // in the animation sequence
		struct texture_s *alternate_anims;  // bmodels in frmae 1 use these
		unsigned    offsets[MIPLEVELS];     // four mip maps stored
	};

	inline Engine::mnode_t *CastType(::mnode_t *origType) { return reinterpret_cast<Engine::mnode_t *>(origType); }
	inline Engine::mleaf_t *CastType(::mleaf_t *origType) { return reinterpret_cast<Engine::mleaf_t *>(origType); }
	inline Engine::msurface_t *CastType(::msurface_t *origType) { return reinterpret_cast<Engine::msurface_t *>(origType); }
	inline Engine::texture_t *CastType(::texture_t *origType) { return reinterpret_cast<Engine::texture_t *>(origType); }
};
