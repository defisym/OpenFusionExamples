#pragma once

// ------------------------------------------------
// Macro
// ------------------------------------------------

// Macro that used in resource file should be defined here

// copy hardware decode result directly 
// instead of copy to memory first
#define COPY_D3D_TEXTURE

#ifdef COPY_D3D_TEXTURE
// pre compile shader instead of compile in runtime
#define PRE_COMPILE_SHADER
#endif

// ------------------------------------------------
// MUST ENDED BY EMPTY LINE
// ------------------------------------------------
