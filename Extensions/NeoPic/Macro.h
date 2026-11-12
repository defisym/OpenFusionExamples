#pragma once

// ------------------------------------------------
// Macro
// ------------------------------------------------

// Macro that used in resource file should be defined here

// only enable when needed to speed up build
#define PERSPECTIVE_TRANSFORMATION

// if you need to compatible with windows 7 (which doesn't support DX12)
// you need to comment both macro below, which will disable VRAM check

// #define QUERY_VRAM
#ifdef QUERY_VRAM
// #define QUERY_NON_LOCAL
// #define DYNAMIC_LINK_DXGI
#endif

// ------------------------------------------------
// MUST ENDED BY EMPTY LINE
// ------------------------------------------------
