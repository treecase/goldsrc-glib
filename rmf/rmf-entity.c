#include "rmf-entity.h"

#include "rmf-entitydata.h"
#include "rmf-loader.h"
#include "rmf-mapobject.h"
#include "rmf-private.h"

#include <glib-object.h>

/**
 * RmfEntity:
 *
 * A point or brush entity.
 */
struct _RmfEntity {
    RmfEntityData parent_instance;
    RmfVector origin;
};

enum Property {
    PROP_ORIGIN = 1,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

G_DEFINE_FINAL_TYPE(RmfEntity, rmf_entity, RMF_TYPE_ENTITY_DATA)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_entity_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    auto const self = RMF_ENTITY(object);
    switch ((enum Property)property_id) {
    case PROP_ORIGIN:
        g_value_set_boxed(value, &self->origin);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// RmfMapObject ////////////////////////////////////////////////////////////////

static void rmf_entity_load(RmfMapObject *map_object, RmfLoader *loader)
{
    rmf_loader_log_begin(loader, "entity", nullptr);
    RMF_MAP_OBJECT_CLASS(rmf_entity_parent_class)->load(map_object, loader);

    auto const self = RMF_ENTITY(map_object);
    auto object_type = rmf_map_object_get_object_type(RMF_MAP_OBJECT(self));
    g_return_if_fail(object_type == RMF_OBJECT_TYPE_ENTITY);

    rmf_loader_seek(loader, 2);
    rmf_read_vector(loader, &self->origin);
    rmf_loader_seek(loader, 4);

    g_autofree auto content = g_strdup_printf(
        "%g %g %g",
        self->origin.x,
        self->origin.y,
        self->origin.z
    );
    rmf_loader_log_oneline(loader, "origin", content, nullptr);

    rmf_loader_log_end(loader);
}

// RmfEntity ///////////////////////////////////////////////////////////////////

static void rmf_entity_class_init(RmfEntityClass *klass)
{
    RMF_MAP_OBJECT_CLASS(klass)->load = rmf_entity_load;

    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->get_property = rmf_entity_get_property;

    /**
     * RmfEntity:origin
     * A point entity's position.
     */
    obj_properties[PROP_ORIGIN] = g_param_spec_boxed(
        "origin",
        nullptr,
        nullptr,
        RMF_TYPE_VECTOR,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_entity_init(RmfEntity *)
{
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * rmf_entity_get_origin
 * @entity: The entity.
 *
 * Gets the entity's origin point.
 *
 * Returns: The entity's origin.
 */
RmfVector *rmf_entity_get_origin(RmfEntity *self)
{
    RmfVector *value = nullptr;
    g_object_get(self, "origin", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfEntity *rmf_entity_new(RmfLoader *loader)
{
    RmfEntity *self = g_object_new(RMF_TYPE_ENTITY, nullptr);
    rmf_entity_load(RMF_MAP_OBJECT(self), loader);
    return self;
}
