#pragma once

#include <gio/gio.h>
#include <glib-object.h>
#include <wad/wad-rgb.h>

G_BEGIN_DECLS

#define WAD_TYPE_QPIC_FILE wad_qpic_file_get_type()

/**
 * WadQpicFile:
 * @width: Width of the image, in pixels.
 * @height: Height of the image, in pixels.
 * @data: (element-type guchar): Pixel data in indexed format. Must have `width
 * * height` elements.
 * @palette: (element-type WadRgb): Image color palette.
 *
 * A simple image.
 */
typedef struct {
    guint32 width, height;
    GArray *data;
    GArray *palette;
} WadQpicFile;

GType wad_qpic_file_get_type(void);
WadQpicFile *wad_qpic_file_copy(WadQpicFile const *qpic);
void wad_qpic_file_free(WadQpicFile *qpic);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(WadQpicFile, wad_qpic_file_free)

G_END_DECLS
