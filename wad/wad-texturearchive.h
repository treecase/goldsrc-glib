#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define WAD_TYPE_TEXTURE_ARCHIVE wad_texture_archive_get_type()

G_DECLARE_FINAL_TYPE(
    WadTextureArchive,
    wad_texture_archive,
    WAD,
    TEXTURE_ARCHIVE,
    GObject
)

WadTextureArchive *wad_texture_archive_new(void);

void wad_texture_archive_add_texture(
    WadTextureArchive *archive,
    char const *texture_name,
    GValue *texture
);

void wad_texture_archive_remove_texture(
    WadTextureArchive *archive,
    char const *texture
);

GValue *wad_texture_archive_get_texture(
    WadTextureArchive *archive,
    char const *texture_name
);

G_END_DECLS
