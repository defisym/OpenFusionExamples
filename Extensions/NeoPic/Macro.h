#pragma once

// ------------------------------------------------
// Macro
// ------------------------------------------------

// Macro that used in resource file should be defined here

// Experimental capture backends: 0 = existing DC, 1 = Frame secondary, 2 = WGC.
// Set consistently for the entire project and Rebuild after changing it.
#ifndef NEOPIC_CAPTURE_BACKEND
#define NEOPIC_CAPTURE_BACKEND 0
#endif
#if NEOPIC_CAPTURE_BACKEND < 0 || NEOPIC_CAPTURE_BACKEND > 2
#error Invalid NEOPIC_CAPTURE_BACKEND
#endif

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
