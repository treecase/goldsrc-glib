#include "wad-miptexfile.h"

G_DEFINE_BOXED_TYPE(
    WadMiptexFile,
    wad_miptex_file,
    wad_miptex_file_copy,
    wad_miptex_file_free
)

WadMiptexFile *wad_miptex_file_copy(WadMiptexFile const *miptex)
{
    WadMiptexFile *copy = g_new(WadMiptexFile, 1);
    memcpy(copy->texture_name, miptex->texture_name, 16);
    copy->width = miptex->width;
    copy->height = miptex->height;
    for (size_t i = 0; i < 4; ++i) {
        copy->mip_images[i] = g_array_ref(miptex->mip_images[i]);
    }
    copy->palette = g_array_ref(miptex->palette);
    return copy;
}

void wad_miptex_file_free(WadMiptexFile *miptex)
{
    for (size_t i = 0; i < 4; ++i) {
        if (miptex->mip_images[i]) {
            g_array_unref(miptex->mip_images[i]);
        }
    }
    if (miptex->palette) {
        g_array_unref(miptex->palette);
    }
    g_free(miptex);
}
