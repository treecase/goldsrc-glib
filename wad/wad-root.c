#include "wad-root.h"

#include "wad-inputstream.h"
#include "wad-loaderror.h"

/**
 * WadRoot:
 *
 * Top-level object for loading WAD data.
 */
struct _WadRoot {
    GObject parent_instance;
    WadTextureArchive *archive;
};

G_DEFINE_FINAL_TYPE(WadRoot, wad_root, G_TYPE_OBJECT)

// Private /////////////////////////////////////////////////////////////////////

void load_from_stream(WadRoot *self, GInputStream *base_stream, GError **error)
{
    g_autoptr(WadInputStream) stream = wad_input_stream_new(base_stream);
    GDataInputStream *dstream = G_DATA_INPUT_STREAM(stream);
    GError *e = nullptr;

    // Header
    char buffer[4];
    gsize bytes_read;
    g_input_stream_read_all(
        G_INPUT_STREAM(stream),
        buffer,
        4,
        &bytes_read,
        nullptr,
        &e
    );
    if (e) {
        g_propagate_error(error, e);
        return;
    }
    if (strncmp(buffer, "WAD3", 4) != 0) {
        g_set_error(
            error,
            WAD_LOAD_ERROR,
            WAD_LOAD_ERROR_MAGIC,
            "Bad magic number %.4s",
            buffer
        );
        return;
    }
    guint32 num_dirs = g_data_input_stream_read_uint32(dstream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return;
    }
    guint32 dir_offset = g_data_input_stream_read_uint32(dstream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return;
    }
    g_info(
        "\nHEADER\n  num_dirs = %" G_GUINT32_FORMAT "\n  dir_offset = %#x",
        num_dirs,
        dir_offset
    );

    // Directory Entries
    g_autoptr(GPtrArray) directory = g_ptr_array_new_full(
        num_dirs,
        (GDestroyNotify)wad_directory_entry_free
    );
    g_seekable_seek(G_SEEKABLE(stream), dir_offset, G_SEEK_SET, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return;
    }
    for (guint32 i = 0; i < num_dirs; ++i) {
        WadDirectoryEntry *entry
            = wad_input_stream_read_directory_entry(stream, &e);
        if (e) {
            g_propagate_error(error, e);
            return;
        }
        g_ptr_array_add(directory, entry);
        g_info(
            "\nentry %" G_GUINT32_FORMAT
            " {\n" "  entry_offset = %#x\n" "  disk_size = %" G_GUINT32_FORMAT
            "\n" "  entry_size = %" G_GUINT32_FORMAT
            "\n" "  file_type = %hhx\n" "  compressed = %hhu\n" "  texture_name = '%.16s'\n" "}",
            i,
            entry->entry_offset,
            entry->disk_size,
            entry->entry_size,
            entry->file_type,
            entry->compressed,
            entry->texture_name
        );
    }

    // File Entries
    g_autoptr(WadTextureArchive) archive = wad_texture_archive_new();
    for (guint i = 0; i < directory->len; ++i) {
        WadDirectoryEntry *dir_entry = directory->pdata[i];
        g_seekable_seek(
            G_SEEKABLE(stream),
            dir_entry->entry_offset,
            G_SEEK_SET,
            nullptr,
            &e
        );
        if (e) {
            g_propagate_error(error, e);
            return;
        }
        switch (dir_entry->file_type) {
        case WAD_FILE_TYPE_QPIC: {
            WadQpicFile *qpic = wad_input_stream_read_qpic_file(stream, &e);
            if (e) {
                g_propagate_error(error, e);
                return;
            }
            g_info(
                "%.16s -- Qpic (%" G_GUINT32_FORMAT " x %" G_GUINT32_FORMAT ")",
                dir_entry->texture_name,
                qpic->width,
                qpic->height
            );
            GValue *v = g_new0(GValue, 1);
            g_value_init(v, WAD_TYPE_QPIC_FILE);
            g_value_take_boxed(v, qpic);
            wad_texture_archive_add_texture(
                archive,
                dir_entry->texture_name,
                v
            );
        } break;
        case WAD_FILE_TYPE_SPRAYDECAL:
        case WAD_FILE_TYPE_MIPTEX: {
            WadMiptexFile *miptex
                = wad_input_stream_read_miptex_file(stream, &e);
            if (e) {
                g_propagate_error(error, e);
                return;
            }
            g_info("%.16s -- miptex/spraydecal", dir_entry->texture_name);
            GValue *v = g_new0(GValue, 1);
            g_value_init(v, WAD_TYPE_MIPTEX_FILE);
            g_value_take_boxed(v, miptex);
            wad_texture_archive_add_texture(
                archive,
                dir_entry->texture_name,
                v
            );
        } break;
        case WAD_FILE_TYPE_FONT: {
            WadFontFile *font = wad_input_stream_read_font_file(stream, &e);
            if (e) {
                g_propagate_error(error, e);
                return;
            }
            g_info(
                "%.16s -- font (%" G_GUINT32_FORMAT " rows)",
                dir_entry->texture_name,
                font->row_count
            );
            GValue *v = g_new0(GValue, 1);
            g_value_init(v, WAD_TYPE_FONT_FILE);
            g_value_take_boxed(v, font);
            wad_texture_archive_add_texture(
                archive,
                dir_entry->texture_name,
                v
            );
        } break;
        default:
            g_set_error(
                error,
                WAD_LOAD_ERROR,
                WAD_LOAD_ERROR_FILE_TYPE,
                "unknown file type %#hhx",
                dir_entry->file_type
            );
            return;
            break;
        }
    }
    self->archive = g_object_ref(archive);
}

// GObject /////////////////////////////////////////////////////////////////////

static void wad_root_dispose(GObject *object)
{
    WadRoot *self = WAD_ROOT(object);
    g_clear_object(&self->archive);
    G_OBJECT_CLASS(wad_root_parent_class)->dispose(object);
}

// WadRoot /////////////////////////////////////////////////////////////////////

static void wad_root_class_init(WadRootClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = wad_root_dispose;
}

static void wad_root_init(WadRoot *)
{
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * wad_root_new:
 *
 * Creates a new empty [class@WadRoot] object.
 *
 * Use wad_root_load_from_file() or wad_root_load_from_stream() to read an
 * existing WAD file.
 *
 * Returns: An empty [class@WadRoot].
 */
WadRoot *wad_root_new(void)
{
    return g_object_new(WAD_TYPE_ROOT, nullptr);
}

/**
 * wad_root_load_from_stream:
 * @root: A [class@WadRoot].
 * @stream: The stream to load from.
 * @error: The return location for [struct@GError].
 *
 * Loads a WAD texture archive from `stream`.
 */
void
wad_root_load_from_stream(WadRoot *self, GInputStream *stream, GError **error)
{
    g_return_if_fail(WAD_IS_ROOT(self));
    g_return_if_fail(G_IS_INPUT_STREAM(stream));
    g_return_if_fail(error == nullptr || *error == nullptr);
    load_from_stream(self, stream, error);
}

/**
 * wad_root_load_from_file:
 * @root: A [class@WadRoot].
 * @file: The file to load from.
 * @error: The return location for [struct@GError].
 *
 * Loads a WAD texture archive from `file`.
 */
void wad_root_load_from_file(WadRoot *self, GFile *file, GError **error)
{
    g_return_if_fail(WAD_IS_ROOT(self));
    g_return_if_fail(G_IS_FILE(file));
    g_return_if_fail(error == nullptr || *error == nullptr);

    GError *e = nullptr;
    g_autoptr(GFileInputStream) stream = g_file_read(file, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return;
    }
    load_from_stream(self, G_INPUT_STREAM(stream), error);
}

/**
 * wad_root_get_archive:
 * @root: A [class@WadRoot].
 *
 * Gets the texture archive from the loaded WAD data.
 *
 * If loading failed, the archive will be `NULL`.
 *
 * Returns: (transfer none) (nullable): The [class@WadTextureArchive].
 */
WadTextureArchive *wad_root_get_archive(WadRoot *self)
{
    g_return_val_if_fail(WAD_IS_ROOT(self), nullptr);
    return self->archive;
}
