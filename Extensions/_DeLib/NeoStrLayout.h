#pragma once

struct CharSize {
    long width = 0;
    long height = 0;
};

struct CharPos {
    long x = 0;
    long y = 0;
    long maxWidth = 0;
    long totalHeight = 0;
};

struct StrPos {
    size_t start;
    size_t end;
    size_t length;
    long width;
    long height;
    long x;
    long y;
};