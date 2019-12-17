#pragma once

// place for stuff like signatures, bit masks, offsets, etc.
#define OFFSET_TIMESCALE		0x24
#define SIGN_BUILD_NUMBER		"\xA1\x00\x00\x00\x00\x83\xEC\x08\x00\x33\x00\x85\xC0"
#define SIGN_BUILD_NUMBER_NEW	"\x55\x8B\xEC\x83\xEC\x08\xA1\x00\x00\x00\x00\x56\x33\xF6\x85\xC0\x0F\x85\x00\x00\x00\x00\x53\x33\xDB\x8B\x04\x9D"
#define MASK_BUILD_NUMBER		"x????xxx?x?xx"
#define MASK_BUILD_NUMBER_NEW	"xxxxxxx????xxxxxxx????xxxxxx"
