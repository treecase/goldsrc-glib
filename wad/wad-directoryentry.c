#include "wad-directoryentry.h"

// WadFileType

G_DEFINE_ENUM_TYPE(
    WadFileType,
    wad_file_type,
    G_DEFINE_ENUM_VALUE(WAD_FILE_TYPE_QPIC, "qpic"),
    G_DEFINE_ENUM_VALUE(WAD_FILE_TYPE_MIPTEX, "miptex"),
    G_DEFINE_ENUM_VALUE(WAD_FILE_TYPE_FONT, "font"),
    G_DEFINE_ENUM_VALUE(WAD_FILE_TYPE_SPRAYDECAL, "spraydecal")
)

// WadDirectoryEntry

G_DEFINE_BOXED_TYPE(
    WadDirectoryEntry,
    wad_directory_entry,
    wad_directory_entry_copy,
    wad_directory_entry_free
)

WadDirectoryEntry *wad_directory_entry_copy(WadDirectoryEntry *dir_entry)
{
    WadDirectoryEntry *copy = g_new(WadDirectoryEntry, 1);
    memcpy(copy, dir_entry, sizeof(WadDirectoryEntry));
    return copy;
}

void wad_directory_entry_free(WadDirectoryEntry *dir_entry)
{
    g_free(dir_entry);
}
