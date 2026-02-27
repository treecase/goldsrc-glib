#include <gio/gio.h>
#include <wad/wad.h>

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        g_printerr("Usage: %s WADFILE\n", argv[0]);
        return EXIT_FAILURE;
    }
    g_autoptr(GFile) file = g_file_new_for_path(argv[1]);
    g_autoptr(GError) error = nullptr;

    g_autoptr(WadRoot) root = wad_root_new();
    wad_root_load_from_file(root, file, &error);
    WadTextureArchive *archive = wad_root_get_archive(root);
    if (error) {
        g_printerr(
            "%s: wad_root_load_from_file -- %s\n",
            g_file_get_basename(file),
            error->message
        );
        return EXIT_FAILURE;
    }
    g_print("Load successful.\n");

    // TODO: list archive contents

    return EXIT_SUCCESS;
}
