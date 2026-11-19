#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <fstream>

bool saveBitmap(const FT_Bitmap& bitmap, const char* filename) {
    if (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
        std::cerr << "Unsupported bitmap pixel mode: "
                  << static_cast<int>(bitmap.pixel_mode)
                  << std::endl;
        return false;
    }

    std::ofstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to create bitmap file\n";
        return false;
    }

    // PGM grayscale image
    file << "P5\n"
         << bitmap.width << " "
         << bitmap.rows << "\n255\n";

    for (unsigned int y = 0; y < bitmap.rows; ++y) {
        const unsigned char* row;

        if (bitmap.pitch >= 0) {
            row = bitmap.buffer + y * bitmap.pitch;
        } else {
            row = bitmap.buffer +
                  (bitmap.rows - 1 - y) * (-bitmap.pitch);
        }

        file.write(
            reinterpret_cast<const char*>(row),
            bitmap.width
        );
    }

    return file.good();
}


int TestFreeTypeLib(){
    std::cout << "Test FreeType..." << std::endl;

    FT_Library library = nullptr;
    FT_Face face = nullptr;

    FT_Error error = FT_Init_FreeType(&library);

    if (error) {
        std::cerr << "FT_Init_FreeType failed: "
                  << error << "\n";
        return -1;
    }
    std::cout << "[OK] FreeType initialized\n";

    // 2. Load font
    error = FT_New_Face(
        library,
        "../assets/fonts/arial.ttf",
        0,
        &face
    );

    if (error) {
        std::cerr << "FT_New_Face failed: "
                  << error << "\n";

        FT_Done_FreeType(library);
        return -1;
    }

    std::cout << "[OK] Font loaded\n";

    // 3. Print font information
    std::cout << "Family: "
              << (face->family_name ?
                  face->family_name : "Unknown")
              << "\n";

    std::cout << "Style: "
              << (face->style_name ?
                  face->style_name : "Unknown")
              << "\n";

    std::cout << "Glyph count: "
              << face->num_glyphs
              << "\n";
    
    std::cout << "ascender :" << face->ascender << "  descender :" << face->descender << std::endl;

    // auto chMap = face->charmap;

    // 4. Set font size
    error = FT_Set_Pixel_Sizes(face, 0, 48);

    if (error) {
        std::cerr << "FT_Set_Pixel_Sizes failed: "
                  << error << "\n";

        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return -1;
    }

     // 5. Load character A and render bitmap
    error = FT_Load_Char(
        face,
        'A',
        FT_LOAD_RENDER
    );


    if (error) {
        std::cerr << "FT_Load_Char failed: "
                  << error << "\n";

        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return -1;
    }

    FT_GlyphSlot slot = face->glyph;

    std::cout << "\nGlyph information:\n";

    std::cout << "Glyph index: "
              << FT_Get_Char_Index(face, 'A')
              << "\n";

    std::cout << "Bitmap width: "
              << slot->bitmap.width
              << "\n";

    std::cout << "Bitmap height: "
              << slot->bitmap.rows
              << "\n";

    std::cout << "Bitmap left: "
              << slot->bitmap_left
              << "\n";

    std::cout << "Bitmap top: "
              << slot->bitmap_top
              << "\n";

    std::cout << "Advance X: "
              << slot->advance.x / 64.0f
              << "\n";

     // 6. Save bitmap
    if (saveBitmap(slot->bitmap, "glyph_A.pgm")) {
        std::cout << "[OK] Bitmap saved: glyph_A.pgm\n";
    } else {
        std::cerr << "[FAIL] Save bitmap failed\n";
    }


    error = FT_Load_Char(
        face,
        'A',
        FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING
    );

    if (!error && face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {

        FT_Outline& outline = face->glyph->outline;

        std::cout << "Outline points: "
                << outline.n_points << "\n";

        std::cout << "Outline contours: "
                << outline.n_contours << "\n";
    }

    // 7. Release resources
    FT_Done_Face(face);
    FT_Done_FreeType(library);

    std::cout << "\n[OK] FreeType test completed\n";
    return 0;
}


