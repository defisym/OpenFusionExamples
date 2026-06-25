#pragma once

#ifndef ENABLE_FREETYPE
//#define ENABLE_FREETYPE
#endif // ENABLE_FREETYPE

#ifdef ENABLE_FREETYPE

#include <ft2build.h>
#include FT_FREETYPE_H

#pragma comment(lib, "freetype.lib")

#endif // ENABLE_FREETYPE