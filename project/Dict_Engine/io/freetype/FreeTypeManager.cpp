#include "FreeTypeManager.h"

void FreeTypeManager::TestFreeType()
{
    FT_Library library = nullptr;

    FT_Error error = FT_Init_FreeType(&library);
    if (error)
    {
        OutputDebugStringA("FT_Init_FreeType failed\n");
        return;
    }

    FT_Face face = nullptr;

    error = FT_New_Face(
        library,
        "resources/fonts/x8y12pxTheStrongGamer.ttf",
        0,
        &face
    );

    if (error)
    {
        char buffer[256]{};
        sprintf_s(buffer, "FT_New_Face failed. error = 0x%02X\n", error);
        OutputDebugStringA(buffer);

        FT_Done_FreeType(library);
        return;
    }

    error = FT_Set_Pixel_Sizes(face, 0, 32);
    if (error)
    {
        OutputDebugStringA("FT_Set_Pixel_Sizes failed\n");
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return;
    }

    error = FT_Load_Char(face, 'A', FT_LOAD_RENDER);
    if (error)
    {
        OutputDebugStringA("FT_Load_Char failed\n");
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return;
    }

    FT_GlyphSlot glyph = face->glyph;

    char buffer[256]{};
    sprintf_s(
        buffer,
        "A bitmap: width=%d height=%d bearingX=%d bearingY=%d advance=%ld\n",
        glyph->bitmap.width,
        glyph->bitmap.rows,
        glyph->bitmap_left,
        glyph->bitmap_top,
        glyph->advance.x >> 6
    );

    OutputDebugStringA(buffer);





    for (char32_t c = U' '; c <= U'~'; ++c)
    {
        FT_Error error = FT_Load_Char(face, static_cast<FT_ULong>(c), FT_LOAD_RENDER);
        if (error)
        {
            continue;
        }

        FT_GlyphSlot glyph = face->glyph;
        const FT_Bitmap& bitmap = glyph->bitmap;

        GlyphInfo info;
        info.width = bitmap.width;
        info.height = bitmap.rows;
        info.bearingX = glyph->bitmap_left;
        info.bearingY = glyph->bitmap_top;
        info.advance = static_cast<int>(glyph->advance.x >> 6);

        const size_t bufferSize =
            static_cast<size_t>(bitmap.width) *
            static_cast<size_t>(bitmap.rows);

        info.bitmap.resize(bufferSize);

        if (bitmap.buffer && bufferSize > 0)
        {
            std::memcpy(info.bitmap.data(), bitmap.buffer, bufferSize);
        }

        glyphs[c] = std::move(info);
    }

    for (char32_t c = U' '; c <= U'~'; ++c)
    {
        const GlyphInfo& g = glyphs[c];

        char buffer[256]{};
        sprintf_s(
            buffer,
            "%c: width=%d height=%d bearingX=%d bearingY=%d advance=%d\n",
            static_cast<char>(c),
            g.width,
            g.height,
            g.bearingX,
            g.bearingY,
            g.advance
        );

        OutputDebugStringA(buffer);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

