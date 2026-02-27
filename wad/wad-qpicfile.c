#include "wad-qpicfile.h"

#include <gio/gio.h>

G_DEFINE_BOXED_TYPE(
    WadQpicFile,
    wad_qpic_file,
    wad_qpic_file_copy,
    wad_qpic_file_free
)

WadQpicFile *wad_qpic_file_copy(WadQpicFile const *qpic)
{
    WadQpicFile *copy = g_new(WadQpicFile, 1);
    copy->width = qpic->width;
    copy->height = qpic->height;
    copy->data = g_array_ref(qpic->data);
    copy->palette = g_array_ref(qpic->palette);
    return copy;
}

void wad_qpic_file_free(WadQpicFile *qpic)
{
    if (qpic->data) {
        g_array_unref(qpic->data);
    }
    if (qpic->palette) {
        g_array_unref(qpic->palette);
    }
    g_free(qpic);
}
