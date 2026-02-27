#pragma once

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

// WadFileType

#define WAD_TYPE_FILE_TYPE wad_file_type_get_type()

/**
 * WadFileType:
 * @WAD_FILE_TYPE_QPIC: A [struct@WadQpicFile].
 * @WAD_FILE_TYPE_MIPTEX: A [struct@WadMiptexFile].
 * @WAD_FILE_TYPE_FONT: A [struct@WadFontFile].
 * @WAD_FILE_TYPE_SPRAYDECAL: Alias for [struct@WadMiptexFile].
 *
 * File types able to be stored in the WAD.
 */
typedef enum {
    WAD_FILE_TYPE_QPIC = 0x42,
    WAD_FILE_TYPE_MIPTEX = 0x43,
    WAD_FILE_TYPE_FONT = 0x46,
    WAD_FILE_TYPE_SPRAYDECAL = 0x40,
} WadFileType;

GType wad_file_type_get_type(void);

// WadDirectoryEntry

#define WAD_TYPE_DIRECTORY_ENTRY wad_directory_entry_get_type()

/**
 * WadDirectoryEntry:
 * @entry_offset: Offset from the beginning of the WAD3 data.
 * @disk_size: The entry's size in bytes inside the archive.
 * @entry_size: The entry's uncompressed size in bytes.
 * @file_type: The file's type.
 * @compressed: Whether the file is compressed.
 * @texture_name: The name of the texture.
 *
 * Describes a file contained inside the WAD.
 */
typedef struct {
    guint32 entry_offset;
    guint32 disk_size;
    guint32 entry_size;
    WadFileType file_type;
    bool compressed;
    char texture_name[16];
} WadDirectoryEntry;

GType wad_directory_entry_get_type(void);
WadDirectoryEntry *wad_directory_entry_copy(WadDirectoryEntry *dir_entry);
void wad_directory_entry_free(WadDirectoryEntry *dir_entry);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(WadDirectoryEntry, wad_directory_entry_free)

G_END_DECLS
