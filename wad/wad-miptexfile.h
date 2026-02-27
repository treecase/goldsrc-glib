#pragma once

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define WAD_TYPE_MIPTEX_FILE wad_miptex_file_get_type()

/**
 * WadMiptexFile:
 * @texture_name: Name of the texture.
 * @width: Width of the image, in pixels.
 * @height: Height of the image, in pixels.
 * @mip_images: An array of mipmaps. The first is full size (ie. width*height
 * pixels), the second is half size (width/2 * height/2), the third is quarter
 * size, and the fourth is eighth size.
 * @palette: (element-type WadRgb): Image color palette.
 *
 * A mipmapped texture.
 */
typedef struct {
    char texture_name[16];
    guint32 width, height;
    GArray *mip_images[4];
    GArray *palette;
} WadMiptexFile;

GType wad_miptex_file_get_type(void);
WadMiptexFile *wad_miptex_file_copy(WadMiptexFile const *miptex);
void wad_miptex_file_free(WadMiptexFile *miptex);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(WadMiptexFile, wad_miptex_file_free)

G_END_DECLS
