#pragma once

#include "wad/wad-texturearchive.h"

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define WAD_TYPE_ROOT wad_root_get_type()

G_DECLARE_FINAL_TYPE(WadRoot, wad_root, WAD, ROOT, GObject)

WadRoot *wad_root_new(void);

void
wad_root_load_from_stream(WadRoot *root, GInputStream *stream, GError **error);

void wad_root_load_from_file(WadRoot *root, GFile *file, GError **error);

WadTextureArchive *wad_root_get_archive(WadRoot *root);

G_END_DECLS
