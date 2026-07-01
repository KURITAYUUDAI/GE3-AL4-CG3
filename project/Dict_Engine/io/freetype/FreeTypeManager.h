#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include <Windows.h>
#include <string>
#include <iostream>

#include <unordered_map>
#include <vector>
#include <cstdint>

class FreeTypeManager
{
public: 
    struct GlyphInfo
    {
        int width = 0;
        int height = 0;

        int bearingX = 0;
        int bearingY = 0;

        int advance = 0;

        std::vector<unsigned char> bitmap;
    };

	void TestFreeType();

private:

    std::unordered_map<char32_t, GlyphInfo> glyphs;
};
