#include "wad-fontfile.h"

#include <gio/gio.h>

// WadCharInfo

G_DEFINE_BOXED_TYPE(
    WadCharInfo,
    wad_char_info,
    wad_char_info_copy,
    wad_char_info_free
)

WadCharInfo *wad_char_info_copy(WadCharInfo const *char_info)
{
    WadCharInfo *copy = g_new(WadCharInfo, 1);
    copy->offset_x = char_info->offset_x;
    copy->offset_y = char_info->offset_y;
    copy->charwidth = char_info->charwidth;
    return copy;
}

void wad_char_info_free(WadCharInfo *char_info)
{
    g_free(char_info);
}

// WadFontFile

G_DEFINE_BOXED_TYPE(
    WadFontFile,
    wad_font_file,
    wad_font_file_copy,
    wad_font_file_free
)

WadFontFile *wad_font_file_copy(WadFontFile const *font)
{
    WadFontFile *copy = g_new(WadFontFile, 1);
    copy->height = font->height;
    copy->row_count = font->row_count;
    copy->row_height = font->row_height;
    copy->font_info = g_array_ref(font->font_info);
    copy->data = g_array_ref(font->data);
    copy->palette = g_array_ref(font->palette);
    return copy;
}

void wad_font_file_free(WadFontFile *font)
{
    if (font->font_info) {
        g_array_unref(font->font_info);
    }
    if (font->data) {
        g_array_unref(font->data);
    }
    if (font->palette) {
        g_array_unref(font->palette);
    }
    g_free(font);
}
