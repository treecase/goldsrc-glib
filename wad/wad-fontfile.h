#pragma once

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

// WadCharInfo

#define WAD_TYPE_CHAR_INFO wad_char_info_get_type()

/**
 * WadCharInfo:
 * @offset_x: Offset from the left of the fontsheet image.
 * @offset_y: Offset from the top of the fontsheet image.
 * @charwidth: Width of the glyph.
 *
 * Font glyph data.
 */
typedef struct {
    char offset_x, offset_y;
    guint16 charwidth;
} WadCharInfo;

GType wad_char_info_get_type(void);
WadCharInfo *wad_char_info_copy(WadCharInfo const *char_info);
void wad_char_info_free(WadCharInfo *char_info);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(WadCharInfo, wad_char_info_free)

// WadFontFile

#define WAD_TYPE_FONT_FILE wad_font_file_get_type()

/**
 * WadFontFile:
 * @height: Height of the fontsheet.
 * @row_count: Number of rows of glyphs.
 * @row_height: Height of the glyph rows.
 * @font_info: (array fixed-size=256) (element-type WadCharInfo): Glyph data for
 * codepoints 0-255.
 * @data: (element-type guchar): Image data in indexed format.
 * @palette: (element-type WadRgb): Image color palette.
 *
 * A font sheet texture.
 */
typedef struct {
    guint32 height;
    guint32 row_count;
    guint32 row_height;
    GArray *font_info;
    GArray *data;
    GArray *palette;
} WadFontFile;

GType wad_font_file_get_type(void);
WadFontFile *wad_font_file_copy(WadFontFile const *font);
void wad_font_file_free(WadFontFile *font);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(WadFontFile, wad_font_file_free)

G_END_DECLS
