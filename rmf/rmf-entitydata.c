#include "rmf/rmf-entitydata.h"

#include "rmf/rmf-iterator.h"
#include "rmf/rmf-mapobject.h"
#include "rmf/rmf-private.h"
#include "rmf/rmf-types.h"

#include <glib-object.h>
#include <glib.h>

/**
 * RmfKeyvalueIterator:
 *
 * An [iface@RmfIterator] which outputs [struct@RmfKeyvalue]s.
 */

/**
 * rmf_keyvalue_iterator_next:
 * @iterator: The iterator.
 *
 * Advance the iterator by one, returning the next keyvalue.
 *
 * Returns: (transfer none): The next keyvalue in the iterator, or `NULL` if the
 * iterator is exhausted.
 */
RMF_DEFINE_ITERATOR_TYPE(
    RmfKeyvalueIterator,
    rmf_keyvalue_iterator,
    RMF,
    KEYVALUE_ITERATOR,
    RmfKeyvalue
)

/**
 * RmfEntityData:
 *
 * Common data shared by [class@RmfEntity] and [class@RmfWorldspawn].
 */
typedef struct {
    rmf_nstring classname;
    rmf_int spawnflags;
    GPtrArray *keyvalues;
} RmfEntityDataPrivate;

enum Property {
    PROP_CLASSNAME = 1,
    PROP_SPAWNFLAGS,
    PROP_N_KEYVALUES,
    PROP_KEYVALUES,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

G_DEFINE_TYPE_WITH_PRIVATE(RmfEntityData, rmf_entity_data, RMF_TYPE_MAP_OBJECT)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_entity_data_dispose(GObject *object)
{
    auto self = RMF_ENTITY_DATA(object);
    RmfEntityDataPrivate *priv = rmf_entity_data_get_instance_private(self);
    if (priv->keyvalues) {
        g_ptr_array_unref(priv->keyvalues);
        priv->keyvalues = nullptr;
    }
    G_OBJECT_CLASS(rmf_entity_data_parent_class)->dispose(object);
}

static void rmf_entity_data_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    auto const self = RMF_ENTITY_DATA(object);
    RmfEntityDataPrivate *const priv
        = rmf_entity_data_get_instance_private(self);
    switch ((enum Property)property_id) {
    case PROP_CLASSNAME:
        g_value_set_string(value, priv->classname.data);
        break;
    case PROP_SPAWNFLAGS:
        g_value_set_uint(value, priv->spawnflags);
        break;
    case PROP_N_KEYVALUES:
        g_value_set_uint(value, priv->keyvalues->len);
        break;
    case PROP_KEYVALUES:
        g_value_take_object(value, rmf_keyvalue_iterator_new(priv->keyvalues));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// RmfMapObject ////////////////////////////////////////////////////////////////

static void rmf_entity_data_load(RmfMapObject *map_object, RmfLoader *loader)
{
    auto self = RMF_ENTITY_DATA(map_object);
    RmfEntityDataPrivate *priv = rmf_entity_data_get_instance_private(self);

    RMF_MAP_OBJECT_CLASS(rmf_entity_data_parent_class)
        ->load(map_object, loader);

    rmf_read_nstring(loader, &priv->classname);
    rmf_loader_seek(loader, 4);
    rmf_read_int(loader, &priv->spawnflags);

    rmf_loader_log_begin(
        loader,
        priv->classname.data,
        "spawnflags",
        "%u",
        priv->spawnflags,
        nullptr
    );

    rmf_int n_keyvalues;
    rmf_read_int(loader, &n_keyvalues);

    rmf_loader_log_begin(
        loader,
        "keyvalues",
        "count",
        "%u",
        n_keyvalues,
        nullptr
    );

    priv->keyvalues
        = g_ptr_array_new_full(n_keyvalues, (GDestroyNotify)rmf_keyvalue_free);
    for (rmf_int i = 0; i < n_keyvalues; ++i) {
        RmfKeyvalue *keyvalue = rmf_keyvalue_new(loader);
        g_ptr_array_add(priv->keyvalues, keyvalue);
    }
    rmf_loader_seek(loader, 12);

    rmf_loader_log_end(loader);
    rmf_loader_log_end(loader);
}

// RmfEntityData ///////////////////////////////////////////////////////////////

static void rmf_entity_data_class_init(RmfEntityDataClass *klass)
{
    auto moclass = RMF_MAP_OBJECT_CLASS(klass);
    moclass->load = rmf_entity_data_load;

    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = rmf_entity_data_dispose;
    oclass->get_property = rmf_entity_data_get_property;

    /**
     * RmfEntityData:classname
     * Name of the entity's class.
     */
    obj_properties[PROP_CLASSNAME] = g_param_spec_string(
        "classname",
        nullptr,
        nullptr,
        "",
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    /**
     * RmfEntityData:spawnflags
     * The entity's spawn flags.
     */
    obj_properties[PROP_SPAWNFLAGS] = g_param_spec_uint(
        "spawnflags",
        nullptr,
        nullptr,
        0,
        UINT_MAX,
        0,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    /**
     * RmfEntityData:n-keyvalues
     * The number of key-value pairs associated with the entity.
     */
    obj_properties[PROP_N_KEYVALUES] = g_param_spec_uint(
        "n-keyvalues",
        "Keyvalue count",
        "Number of keyvalues.",
        0,
        UINT_MAX,
        0,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    /**
     * RmfEntityData:keyvalues
     * The key-value pairs associated with the entity.
     */
    obj_properties[PROP_KEYVALUES] = g_param_spec_object(
        "keyvalues",
        nullptr,
        nullptr,
        RMF_TYPE_KEYVALUE_ITERATOR,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_entity_data_init(RmfEntityData *)
{
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * rmf_entity_data_get_classname:
 * @entity_data: The entity.
 *
 * Gets the entity's classname.
 *
 * Returns: The entity's classname.
 */
char *rmf_entity_data_get_classname(RmfEntityData *self)
{
    char *value = nullptr;
    g_object_get(self, "classname", &value, nullptr);
    return value;
}

/**
 * rmf_entity_data_get_spawnflags:
 * @entity_data: The entity.
 *
 * Gets the entity's spawn flags.
 *
 * Returns: The entity's spawn flags.
 */
rmf_int rmf_entity_data_get_spawnflags(RmfEntityData *self)
{
    rmf_int value = 0;
    g_object_get(self, "spawnflags", &value, nullptr);
    return value;
}

/**
 * rmf_entity_data_get_n_keyvalues:
 * @entity_data: The entity.
 *
 * Gets the number of keyvalues associated with the entity.
 *
 * Returns: The number of keyvalues.
 */
rmf_int rmf_entity_data_get_n_keyvalues(RmfEntityData *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-keyvalues", &value, nullptr);
    return value;
}

/**
 * rmf_entity_data_get_keyvalues:
 * @entity_data: The entity.
 *
 * Gets the key-values associated with the object.
 *
 * Returns: (transfer full): An iterator over the object's key-values.
 */
RmfKeyvalueIterator *rmf_entity_data_get_keyvalues(RmfEntityData *self)
{
    RmfKeyvalueIterator *value = nullptr;
    g_object_get(self, "keyvalues", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfEntityData *rmf_entity_data_new(RmfLoader *loader)
{
    RmfEntityData *self = g_object_new(RMF_TYPE_ENTITY_DATA, nullptr);
    rmf_entity_data_load(RMF_MAP_OBJECT(self), loader);
    return self;
}
