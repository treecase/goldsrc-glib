#pragma once

#include "wad/wad-directoryentry.h"
#include "wad/wad-fontfile.h"
#include "wad/wad-miptexfile.h"
#include "wad/wad-qpicfile.h"

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define WAD_TYPE_INPUT_STREAM wad_input_stream_get_type()

G_DECLARE_FINAL_TYPE(
    WadInputStream,
    wad_input_stream,
    WAD,
    INPUT_STREAM,
    GDataInputStream
)

WadInputStream *wad_input_stream_new(GInputStream *base_stream);

WadDirectoryEntry *
wad_input_stream_read_directory_entry(WadInputStream *stream, GError **error);

WadQpicFile *
wad_input_stream_read_qpic_file(WadInputStream *stream, GError **error);

WadMiptexFile *
wad_input_stream_read_miptex_file(WadInputStream *stream, GError **error);

WadFontFile *
wad_input_stream_read_font_file(WadInputStream *stream, GError **error);

WadCharInfo *
wad_input_stream_read_char_info(WadInputStream *stream, GError **error);

G_END_DECLS
