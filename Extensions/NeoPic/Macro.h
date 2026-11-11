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

// Master switch: 0 compiles out benchmark instrumentation and captures once.
// Set to 1 for a loop inside ONE action invocation. Rebuild the whole project.
#ifndef NEOPIC_CAPTURE_BENCHMARK
#define NEOPIC_CAPTURE_BENCHMARK 0
#endif
#if NEOPIC_CAPTURE_BENCHMARK != 0 && NEOPIC_CAPTURE_BENCHMARK != 1
#error Invalid NEOPIC_CAPTURE_BENCHMARK
#endif
// Counts are retained while the master switch is off.
#ifndef NEOPIC_CAPTURE_BENCHMARK_ITERATIONS
#define NEOPIC_CAPTURE_BENCHMARK_ITERATIONS 100
#endif
#ifndef NEOPIC_CAPTURE_BENCHMARK_WARMUP
#define NEOPIC_CAPTURE_BENCHMARK_WARMUP 10
#endif
#if NEOPIC_CAPTURE_BENCHMARK_ITERATIONS < 0 || NEOPIC_CAPTURE_BENCHMARK_ITERATIONS > 100000
#error Invalid NEOPIC_CAPTURE_BENCHMARK_ITERATIONS
#endif
#if NEOPIC_CAPTURE_BENCHMARK_WARMUP < 0 || NEOPIC_CAPTURE_BENCHMARK_WARMUP > 10000
#error Invalid NEOPIC_CAPTURE_BENCHMARK_WARMUP
#endif
// Derived condition; a zero iteration count also keeps the ordinary action.
#define NEOPIC_CAPTURE_BENCHMARK_ENABLED (NEOPIC_CAPTURE_BENCHMARK && NEOPIC_CAPTURE_BENCHMARK_ITERATIONS > 0)

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
