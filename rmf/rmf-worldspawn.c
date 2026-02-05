#include "rmf/rmf-worldspawn.h"

#include "rmf/rmf-iterator.h"
#include "rmf/rmf-private.h"
#include "rmf/rmf-types.h"

#include <glib-object.h>
#include <glib.h>

/**
 * RmfPathIterator
 *
 * An [iface@RmfIterator] which outputs [struct@RmfPath]s.
 */

/**
 * rmf_path_iterator_next
 * @iterator: The iterator.
 *
 * Advance the iterator by one, returning the next path.
 *
 * Returns: (transfer none): The next path in the iterator, or `NULL` if the
 * iterator is exhausted.
 */
RMF_DEFINE_ITERATOR_TYPE(
    RmfPathIterator,
    rmf_path_iterator,
    RMF,
    PATH_ITERATOR,
    RmfPath
)

/**
 * RmfWorldspawn
 *
 * The root object of the map's object tree.
 */
struct _RmfWorldspawn {
    RmfEntityData parent_instance;
    GPtrArray *paths;
};

enum Property {
    PROP_N_PATHS = 1,
    PROP_PATHS,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

G_DEFINE_FINAL_TYPE(RmfWorldspawn, rmf_worldspawn, RMF_TYPE_ENTITY_DATA)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_worldspawn_dispose(GObject *object)
{
    auto self = RMF_WORLDSPAWN(object);
    if (self->paths) {
        g_ptr_array_unref(self->paths);
        self->paths = nullptr;
    }
    G_OBJECT_CLASS(rmf_worldspawn_parent_class)->dispose(object);
}

static void rmf_worldspawn_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    auto const self = RMF_WORLDSPAWN(object);
    switch ((enum Property)property_id) {
    case PROP_N_PATHS:
        g_value_set_uint(value, self->paths->len);
        break;
    case PROP_PATHS:
        g_value_take_object(value, rmf_path_iterator_new(self->paths));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// RmfMapObject ////////////////////////////////////////////////////////////////

static void rmf_worldspawn_load(RmfMapObject *map_object, RmfLoader *loader)
{
    rmf_loader_log_begin(loader, "worldspawn", nullptr);
    auto const self = RMF_WORLDSPAWN(map_object);

    RMF_MAP_OBJECT_CLASS(rmf_worldspawn_parent_class)->load(map_object, loader);

    auto object_type = rmf_map_object_get_object_type(map_object);
    g_return_if_fail(object_type == RMF_OBJECT_TYPE_WORLD);

    rmf_int n_paths;
    rmf_read_int(loader, &n_paths);
    rmf_loader_log_begin(loader, "paths", "count", "%u", n_paths, nullptr);

    self->paths = g_ptr_array_new_full(n_paths, (GDestroyNotify)rmf_path_free);
    for (rmf_int i = 0; i < n_paths; ++i) {
        RmfPath *path = rmf_path_new(loader);
        g_ptr_array_add(self->paths, path);
    }
    rmf_loader_log_end(loader);
    rmf_loader_log_end(loader);
}

// RmfWorldspawn ///////////////////////////////////////////////////////////////

static void rmf_worldspawn_class_init(RmfWorldspawnClass *klass)
{
    auto moclass = RMF_MAP_OBJECT_CLASS(klass);
    moclass->load = rmf_worldspawn_load;

    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = rmf_worldspawn_dispose;
    oclass->get_property = rmf_worldspawn_get_property;

    /**
     * RmfWorldspawn:n-paths
     *
     * Number of paths associated with the world.
     */
    obj_properties[PROP_N_PATHS] = g_param_spec_uint(
        "n-paths",
        "Path count",
        "Number of paths.",
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE
    );

    /**
     * RmfWorldspawn:paths
     *
     * A collection of paths associated with the world.
     */
    obj_properties[PROP_PATHS] = g_param_spec_object(
        "paths",
        nullptr,
        nullptr,
        RMF_TYPE_PATH_ITERATOR,
        G_PARAM_READABLE
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_worldspawn_init(RmfWorldspawn *)
{
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * rmf_worldspawn_get_n_paths
 * @worldspawn: The worldspawn.
 *
 * Gets the number of paths in the world.
 *
 * Returns: The number of paths.
 */
rmf_int rmf_worldspawn_get_n_paths(RmfWorldspawn *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-paths", &value, nullptr);
    return value;
}

/**
 * rmf_worldspawn_get_paths
 * @worldspawn: The worldspawn.
 *
 * Gets the [struct@RmfPath]s associated with the object.
 *
 * Returns: (transfer full): An iterator over the world's paths.
 */
RmfPathIterator *rmf_worldspawn_get_paths(RmfWorldspawn *self)
{
    RmfPathIterator *value = nullptr;
    g_object_get(self, "paths", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfWorldspawn *rmf_worldspawn_new(RmfLoader *loader)
{
    RmfWorldspawn *self = g_object_new(RMF_TYPE_WORLDSPAWN, nullptr);
    rmf_worldspawn_load(RMF_MAP_OBJECT(self), loader);
    return self;
}
