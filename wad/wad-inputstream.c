/*
 * https://twhl.info/wiki/page/Specification:_WAD3 has more info on the format.
 */
#include "wad-inputstream.h"

#include <gio/gio.h>

/**
 * WadInputStream:
 *
 * An input stream with functions for reading WAD structs from a binary input
 * stream.
 */
struct _WadInputStream {
    GDataInputStream parent_instance;
};

G_DEFINE_FINAL_TYPE(WadInputStream, wad_input_stream, G_TYPE_DATA_INPUT_STREAM)

// GObject /////////////////////////////////////////////////////////////////////

static void wad_input_stream_constructed(GObject *object)
{
    g_object_set(
        object,
        "byte-order",
        G_DATA_STREAM_BYTE_ORDER_LITTLE_ENDIAN,
        nullptr
    );
    G_OBJECT_CLASS(wad_input_stream_parent_class)->constructed(object);
}

// WadInputStream //////////////////////////////////////////////////////////////

static void wad_input_stream_class_init(WadInputStreamClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->constructed = wad_input_stream_constructed;
}

static void wad_input_stream_init(WadInputStream *)
{
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * wad_input_stream_new:
 * @base_stream: A [class@Gio.InputStream].
 *
 * Creates a new wad input stream for the @base_stream.
 *
 * Returns: A new [class@WadInputStream].
 */
WadInputStream *wad_input_stream_new(GInputStream *base_stream)
{
    return g_object_new(
        WAD_TYPE_INPUT_STREAM,
        "base-stream",
        base_stream,
        nullptr
    );
}

/**
 * wad_input_stream_read_directory_entry:
 * @stream: A [class@WadInputStream].
 * @error: The return location for [struct@GError].
 *
 * Reads a [struct@WadDirectoryEntry] from `stream`.
 * Returns: (transfer full): A [struct@WadDirectoryEntry] read from the stream
 * or `NULL` if an error occurred.
 */
WadDirectoryEntry *
wad_input_stream_read_directory_entry(WadInputStream *self, GError **error)
{
    g_return_val_if_fail(WAD_IS_INPUT_STREAM(self), nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    GDataInputStream *stream = G_DATA_INPUT_STREAM(self);
    GError *e = nullptr;
    g_autoptr(WadDirectoryEntry) entry = g_new(WadDirectoryEntry, 1);

    entry->entry_offset = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    entry->disk_size = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    entry->entry_size = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    entry->file_type = g_data_input_stream_read_byte(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    entry->compressed = g_data_input_stream_read_byte(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    g_data_input_stream_read_uint16(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    gsize bytes_read;
    g_input_stream_read_all(
        G_INPUT_STREAM(stream),
        entry->texture_name,
        16,
        &bytes_read,
        nullptr,
        &e
    );
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    return wad_directory_entry_copy(entry);
}

/**
 * wad_input_stream_read_qpic_file:
 * @stream: A [class@WadInputStream].
 * @error: The return location for [struct@GError].
 *
 * Reads a [struct@WadQpicFile] from `stream`.
 * Returns: (transfer full): A [struct@WadQpicFile] read from the stream or
 * `NULL` if an error occurred.
 */
WadQpicFile *
wad_input_stream_read_qpic_file(WadInputStream *self, GError **error)
{
    g_return_val_if_fail(WAD_IS_INPUT_STREAM(self), nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    GDataInputStream *stream = G_DATA_INPUT_STREAM(self);
    gsize bytes_read;
    GError *e = nullptr;
    g_autoptr(WadQpicFile) qpic = g_new(WadQpicFile, 1);

    qpic->width = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    qpic->height = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    qpic->data = g_array_sized_new(
        FALSE,
        FALSE,
        sizeof(guchar),
        qpic->width * qpic->height
    );
    g_input_stream_read_all(
        G_INPUT_STREAM(stream),
        qpic->data->data,
        sizeof(guchar) * qpic->data->len,
        &bytes_read,
        nullptr,
        &e
    );
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    guint16 colors_used = g_data_input_stream_read_uint16(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    qpic->palette
        = g_array_sized_new(FALSE, FALSE, sizeof(WadRgb), colors_used);
    g_input_stream_read_all(
        G_INPUT_STREAM(stream),
        qpic->palette->data,
        sizeof(WadRgb) * qpic->palette->len,
        &bytes_read,
        nullptr,
        &e
    );
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    return wad_qpic_file_copy(qpic);
}

/**
 * wad_input_stream_read_miptex_file:
 * @stream: A [class@WadInputStream].
 * @error: The return location for [struct@GError].
 *
 * Reads a [struct@WadMiptexFile] from `stream`.
 * Returns: (transfer full): A [struct@WadMiptexFile] read from the stream or
 * `NULL` if an error occurred.
 */
WadMiptexFile *
wad_input_stream_read_miptex_file(WadInputStream *self, GError **error)
{
    g_return_val_if_fail(WAD_IS_INPUT_STREAM(self), nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    GDataInputStream *stream = G_DATA_INPUT_STREAM(self);
    gsize bytes_read;
    GError *e = nullptr;
    g_autoptr(WadMiptexFile) miptex = g_new(WadMiptexFile, 1);

    g_input_stream_read_all(
        G_INPUT_STREAM(stream),
        miptex->texture_name,
        16,
        &bytes_read,
        nullptr,
        &e
    );
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    miptex->width = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    miptex->height = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    for (size_t i = 0; i < 4; ++i) {
        g_data_input_stream_read_uint32(stream, nullptr, &e);
        if (e) {
            g_propagate_error(error, e);
            return nullptr;
        }
    }
    constexpr guint32 divisor[4] = {1, 2, 4, 8};
    for (size_t i = 0; i < 4; ++i) {
        gsize bufsize
            = (miptex->width / divisor[i]) * (miptex->height / divisor[i]);
        miptex->mip_images[i]
            = g_array_sized_new(FALSE, FALSE, sizeof(guchar), bufsize);
        g_input_stream_read_all(
            G_INPUT_STREAM(stream),
            miptex->mip_images[i]->data,
            sizeof(guchar) * miptex->mip_images[i]->len,
            &bytes_read,
            nullptr,
            &e
        );
        if (e) {
            g_propagate_error(error, e);
            return nullptr;
        }
    }
    guint16 colors_used = g_data_input_stream_read_uint16(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    miptex->palette
        = g_array_sized_new(FALSE, FALSE, sizeof(WadRgb), colors_used);
    if (miptex->palette->len != 0) {
        g_input_stream_read_all(
            G_INPUT_STREAM(stream),
            miptex->palette->data,
            sizeof(WadRgb) * miptex->palette->len,
            &bytes_read,
            nullptr,
            &e
        );
        if (e) {
            g_propagate_error(error, e);
            return nullptr;
        }
    }
    return wad_miptex_file_copy(miptex);
}

/**
 * wad_input_stream_read_font_file:
 * @stream: A [class@WadInputStream].
 * @error: The return location for [struct@GError].
 *
 * Reads a [struct@WadFontFile] from `stream`.
 * Returns: (transfer full): A [struct@WadFontFile] read from the stream or
 * `NULL` if an error occurred.
 */
WadFontFile *
wad_input_stream_read_font_file(WadInputStream *self, GError **error)
{
    g_return_val_if_fail(WAD_IS_INPUT_STREAM(self), nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    GDataInputStream *stream = G_DATA_INPUT_STREAM(self);
    gsize bytes_read;
    GError *e = nullptr;
    g_autoptr(WadFontFile) font = g_new(WadFontFile, 1);

    g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    font->height = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    font->row_count = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    font->row_height = g_data_input_stream_read_uint32(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    font->font_info = g_array_sized_new(FALSE, FALSE, sizeof(WadCharInfo), 256);
    g_input_stream_read_all(
        G_INPUT_STREAM(stream),
        font->font_info->data,
        sizeof(WadCharInfo) * font->font_info->len,
        &bytes_read,
        nullptr,
        &e
    );
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    font->data
        = g_array_sized_new(FALSE, FALSE, sizeof(guchar), font->height * 256);
    g_input_stream_read_all(
        G_INPUT_STREAM(stream),
        font->data->data,
        sizeof(guchar) * font->data->len,
        &bytes_read,
        nullptr,
        &e
    );
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    guint16 colors_used = g_data_input_stream_read_uint16(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    font->palette
        = g_array_sized_new(FALSE, FALSE, sizeof(WadRgb), colors_used);
    g_input_stream_read_all(
        G_INPUT_STREAM(stream),
        font->palette->data,
        sizeof(WadRgb) * font->palette->len,
        &bytes_read,
        nullptr,
        &e
    );
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    return wad_font_file_copy(font);
}

/**
 * wad_input_stream_read_char_info:
 * @stream: A [class@WadInputStream].
 * @error: The return location for [struct@GError].
 *
 * Reads a [struct@WadCharInfo] from `stream`.
 * Returns: (transfer full): A [struct@WadCharInfo] read from the stream or
 * `NULL` if an error occurred.
 */
WadCharInfo *
wad_input_stream_read_char_info(WadInputStream *self, GError **error)
{
    g_return_val_if_fail(WAD_IS_INPUT_STREAM(self), nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    GDataInputStream *stream = G_DATA_INPUT_STREAM(self);
    GError *e = nullptr;
    WadCharInfo char_info = {};

    char_info.offset_x = g_data_input_stream_read_byte(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    char_info.offset_y = g_data_input_stream_read_byte(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    char_info.charwidth = g_data_input_stream_read_uint16(stream, nullptr, &e);
    if (e) {
        g_propagate_error(error, e);
        return nullptr;
    }
    return wad_char_info_copy(&char_info);
}
