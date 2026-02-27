#include "wad-texturearchive.h"

/**
 * WadTextureArchive:
 *
 * A collection of textures.
 */
struct _WadTextureArchive {
    GObject parent_instance;
    GHashTable *textures;
};

G_DEFINE_FINAL_TYPE(WadTextureArchive, wad_texture_archive, G_TYPE_OBJECT)

// Private /////////////////////////////////////////////////////////////////////

static void free_value(GValue *value)
{
    g_value_unset(value);
    g_free(value);
}

// GObject /////////////////////////////////////////////////////////////////////

static void wad_texture_archive_finalize(GObject *object)
{
    WadTextureArchive *self = WAD_TEXTURE_ARCHIVE(object);
    if (self->textures) {
        g_hash_table_unref(self->textures);
    }
    G_OBJECT_CLASS(wad_texture_archive_parent_class)->finalize(object);
}

// WadTextureArchive ///////////////////////////////////////////////////////////

static void wad_texture_archive_class_init(WadTextureArchiveClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->finalize = wad_texture_archive_finalize;
}

static void wad_texture_archive_init(WadTextureArchive *self)
{
    self->textures = g_hash_table_new_full(
        g_str_hash,
        g_str_equal,
        g_free,
        (GDestroyNotify)free_value
    );
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * wad_texture_archive_new:
 *
 * Creates a new texture archive.
 *
 * Returns: A new [class@WadTextureArchive].
 */
WadTextureArchive *wad_texture_archive_new(void)
{
    return g_object_new(WAD_TYPE_TEXTURE_ARCHIVE, nullptr);
}

/**
 * wad_texture_archive_add_texture:
 * @archive: A [class@WadTextureArchive].
 * @texture_name: Name of the texture.
 * @texture: (transfer full): A texture.
 *
 * Adds a texture to the archive.
 */
void wad_texture_archive_add_texture(
    WadTextureArchive *self,
    char const *texture_name,
    GValue *texture
)
{
    g_return_if_fail(WAD_IS_TEXTURE_ARCHIVE(self));
    g_return_if_fail(G_IS_VALUE(texture));
    g_hash_table_insert(self->textures, g_strdup(texture_name), texture);
}

/**
 * wad_texture_archive_remove_texture:
 * @archive: A [class@WadTextureArchive].
 * @texture: Name of the texture.
 *
 * Removes a texture from the archive.
 */
void
wad_texture_archive_remove_texture(WadTextureArchive *self, char const *texture)
{
    g_return_if_fail(WAD_IS_TEXTURE_ARCHIVE(self));
    g_hash_table_remove(self->textures, texture);
}

/**
 * wad_texture_archive_get_texture:
 * @archive: A [class@WadTextureArchive].
 * @texture_name: Name of the texture.
 *
 * Gets a texture from the wad.
 * Returns: (transfer none) (nullable): The requested texture, or `NULL` if the
 * given name does not exist.
 */
GValue *wad_texture_archive_get_texture(
    WadTextureArchive *self,
    char const *texture_name
)
{
    g_return_val_if_fail(WAD_IS_TEXTURE_ARCHIVE(self), nullptr);
    return g_hash_table_lookup(self->textures, texture_name);
}
