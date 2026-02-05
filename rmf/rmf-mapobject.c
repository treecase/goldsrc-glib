#include "rmf-mapobject.h"

#include "rmf-entity.h"
#include "rmf-group.h"
#include "rmf-iterator.h"
#include "rmf-loader.h"
#include "rmf-private.h"
#include "rmf-solid.h"
#include "rmf-types.h"
#include "rmf-worldspawn.h"

#include <glib-object.h>

/**
 * RmfObjectType:
 * @RMF_OBJECT_TYPE_UNKNOWN: The object has an unknown type.
 * @RMF_OBJECT_TYPE_WORLD:   The object is a [class@RmfWorldspawn].
 * @RMF_OBJECT_TYPE_SOLID:   The object is a [class@RmfSolid].
 * @RMF_OBJECT_TYPE_ENTITY:  The object is an [class@RmfEntity].
 * @RMF_OBJECT_TYPE_GROUP:   The object is a [class@RmfGroup].
 *
 * Possible types of [class@RmfMapObject]s.
 */
G_DEFINE_ENUM_TYPE(
    RmfObjectType,
    rmf_object_type,
    G_DEFINE_ENUM_VALUE(RMF_OBJECT_TYPE_UNKNOWN, "unknown"),
    G_DEFINE_ENUM_VALUE(RMF_OBJECT_TYPE_WORLD, "world"),
    G_DEFINE_ENUM_VALUE(RMF_OBJECT_TYPE_SOLID, "solid"),
    G_DEFINE_ENUM_VALUE(RMF_OBJECT_TYPE_ENTITY, "entity"),
    G_DEFINE_ENUM_VALUE(RMF_OBJECT_TYPE_GROUP, "group")
)

/**
 * RmfMapObjectIterator:
 *
 * [iface@RmfIterator] which outputs [class@RmfMapObject]s.
 */

/**
 * rmf_map_object_iterator_next:
 * @iterator: The iterator.
 *
 * Advance the iterator by one, returning the next object.
 *
 * Returns: (transfer none): The next object in the iterator, or `NULL` if the
 * iterator is exhausted.
 */
RMF_DEFINE_ITERATOR_TYPE(
    RmfMapObjectIterator,
    rmf_map_object_iterator,
    RMF,
    MAP_OBJECT_ITERATOR,
    RmfMapObject
)

/**
 * RmfMapObject:
 *
 * Base class for map objects.
 *
 * All map object types ([class@RmfWorldspawn], [class@RmfSolid],
 * [class@RmfEntity], and [class@RmfGroup]) are derived from this type.
 */
typedef struct {
    RmfObjectType object_type;
    rmf_int visgroup_id;
    RmfColor color;
    GPtrArray *children;
} RmfMapObjectPrivate;

enum Property {
    PROP_OBJECT_TYPE = 1,
    PROP_VISGROUP_ID,
    PROP_COLOR,
    PROP_N_CHILDREN,
    PROP_CHILDREN,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

G_DEFINE_TYPE_WITH_PRIVATE(RmfMapObject, rmf_map_object, G_TYPE_OBJECT)

// Private /////////////////////////////////////////////////////////////////////

static RmfObjectType object_type_from_nstring(rmf_nstring const *nstring)
{
    if (strncmp(nstring->data, "CMapWorld", nstring->length) == 0) {
        return RMF_OBJECT_TYPE_WORLD;
    } else if (strncmp(nstring->data, "CMapSolid", nstring->length) == 0) {
        return RMF_OBJECT_TYPE_SOLID;
    } else if (strncmp(nstring->data, "CMapEntity", nstring->length) == 0) {
        return RMF_OBJECT_TYPE_ENTITY;
    } else if (strncmp(nstring->data, "CMapGroup", nstring->length) == 0) {
        return RMF_OBJECT_TYPE_GROUP;
    } else {
        g_warning("unknown object type '%.*s'", nstring->length, nstring->data);
        return RMF_OBJECT_TYPE_UNKNOWN;
    }
}

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_map_object_dispose(GObject *object)
{
    auto const self = RMF_MAP_OBJECT(object);
    RmfMapObjectPrivate *const priv = rmf_map_object_get_instance_private(self);
    if (priv->children) {
        g_ptr_array_unref(priv->children);
        priv->children = nullptr;
    }
    G_OBJECT_CLASS(rmf_map_object_parent_class)->dispose(object);
}

static void rmf_map_object_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    auto const self = RMF_MAP_OBJECT(object);
    RmfMapObjectPrivate *const priv = rmf_map_object_get_instance_private(self);
    switch (property_id) {
    case PROP_OBJECT_TYPE:
        g_value_set_enum(value, priv->object_type);
        break;
    case PROP_VISGROUP_ID:
        g_value_set_uint(value, priv->visgroup_id);
        break;
    case PROP_COLOR:
        g_value_set_pointer(value, &priv->color);
        break;
    case PROP_N_CHILDREN:
        g_value_set_uint(value, priv->children->len);
        break;
    case PROP_CHILDREN:
        g_value_take_object(value, rmf_map_object_iterator_new(priv->children));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// RmfMapObject ////////////////////////////////////////////////////////////////

static void rmf_map_object_load_impl(RmfMapObject *self, RmfLoader *loader)
{
    RmfMapObjectPrivate *priv = rmf_map_object_get_instance_private(self);

    rmf_nstring type;
    rmf_read_nstring(loader, &type);
    priv->object_type = object_type_from_nstring(&type);

    rmf_read_int(loader, &priv->visgroup_id);
    rmf_read_color(loader, &priv->color);

    rmf_int n_children;
    rmf_read_int(loader, &n_children);

    if (n_children > 0) {
        priv->children = g_ptr_array_new_full(n_children, g_object_unref);

        rmf_loader_log_begin(
            loader,
            "children",
            "count",
            "%u",
            n_children,
            nullptr
        );
        for (rmf_int i = 0; i < n_children; ++i) {
            RmfMapObject *child = rmf_map_object_new(loader);
            g_ptr_array_add(priv->children, child);
        }
        g_assert(priv->children->len == n_children);
        rmf_loader_log_end(loader);
    }
}

static void rmf_map_object_class_init(RmfMapObjectClass *klass)
{
    klass->load = rmf_map_object_load_impl;

    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = rmf_map_object_dispose;
    oclass->get_property = rmf_map_object_get_property;

    /**
     * RmfMapObject:object-type
     * The object's type.
     */
    obj_properties[PROP_OBJECT_TYPE] = g_param_spec_enum(
        "object-type",
        nullptr,
        nullptr,
        RMF_TYPE_OBJECT_TYPE,
        RMF_OBJECT_TYPE_UNKNOWN,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    /**
     * RmfMapObject:visgroup-id
     * ID of the visgroup this object belongs to.
     */
    obj_properties[PROP_VISGROUP_ID] = g_param_spec_uint(
        "visgroup-id",
        nullptr,
        nullptr,
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    /**
     * RmfMapObject:color
     * Editor color of the object.
     */
    obj_properties[PROP_COLOR] = g_param_spec_boxed(
        "color",
        nullptr,
        nullptr,
        RMF_TYPE_COLOR,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    /**
     * RmfMapObject:n-children
     * Number of child objects.
     */
    obj_properties[PROP_N_CHILDREN] = g_param_spec_uint(
        "n-children",
        nullptr,
        nullptr,
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    /**
     * RmfMapObject:children
     * List of child objects.
     */
    obj_properties[PROP_CHILDREN] = g_param_spec_object(
        "children",
        nullptr,
        nullptr,
        RMF_TYPE_MAP_OBJECT_ITERATOR,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_map_object_init(RmfMapObject *)
{
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * rmf_map_object_load:
 * @map_object: The object.
 * @loader: The loader to read from.
 *
 * Reads the object's data from the loader.
 */
void rmf_map_object_load(RmfMapObject *self, RmfLoader *loader)
{
    g_return_if_fail(RMF_IS_MAP_OBJECT(self));
    g_return_if_fail(RMF_IS_LOADER(loader));
    RmfMapObjectClass *klass = RMF_MAP_OBJECT_GET_CLASS(self);
    g_return_if_fail(klass->load != nullptr);
    klass->load(self, loader);
}

/**
 * rmf_map_object_get_object_type:
 * @map_object: The object.
 *
 * Gets the object's type.
 *
 * Returns: The object's type.
 */
RmfObjectType rmf_map_object_get_object_type(RmfMapObject *self)
{
    RmfObjectType value = RMF_OBJECT_TYPE_UNKNOWN;
    g_object_get(self, "object-type", &value, nullptr);
    return value;
}

/**
 * rmf_map_object_get_visgroup_id:
 * @map_object: The object.
 *
 * Gets the ID of the visgroup this object belongs to.
 *
 * Returns: The visgroup ID.
 */
rmf_int rmf_map_object_get_visgroup_id(RmfMapObject *self)
{
    rmf_int value = 0;
    g_object_get(self, "visgroup-id", &value, nullptr);
    return value;
}

/**
 * rmf_map_object_get_color:
 * @map_object: The object.
 *
 * Gets the object's editor color.
 *
 * Returns: The object's color.
 */
RmfColor rmf_map_object_get_color(RmfMapObject *self)
{
    RmfColor *value = nullptr;
    g_object_get(self, "color", &value, nullptr);
    return *value;
}

/**
 * rmf_map_object_get_n_children:
 * @map_object: The object.
 *
 * Gets the number of children belonging to the object.
 *
 * Returns: The number of children.
 */
rmf_int rmf_map_object_get_n_children(RmfMapObject *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-children", &value, nullptr);
    return value;
}

/**
 * rmf_map_object_get_children:
 * @map_object: The object.
 *
 * Gets the children of the object.
 *
 * Returns: (transfer full): An iterator over the children of this object.
 */
RmfMapObjectIterator *rmf_map_object_get_children(RmfMapObject *self)
{
    RmfMapObjectIterator *value = nullptr;
    g_object_get(self, "children", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfMapObject *rmf_map_object_new(RmfLoader *loader)
{
    // Peek the object type.
    rmf_nstring type_str;
    rmf_read_nstring(loader, &type_str);
    rmf_loader_seek(loader, -(1 + type_str.length));
    auto const object_type = object_type_from_nstring(&type_str);

    // Construct the proper subclass according to the object type.
    switch (object_type) {
    case RMF_OBJECT_TYPE_WORLD:
        return RMF_MAP_OBJECT(rmf_worldspawn_new(loader));
    case RMF_OBJECT_TYPE_SOLID:
        return RMF_MAP_OBJECT(rmf_solid_new(loader));
    case RMF_OBJECT_TYPE_ENTITY:
        return RMF_MAP_OBJECT(rmf_entity_new(loader));
    case RMF_OBJECT_TYPE_GROUP:
        return RMF_MAP_OBJECT(rmf_group_new(loader));
    case RMF_OBJECT_TYPE_UNKNOWN:
        break;
    }
    g_return_val_if_reached(nullptr);
}
