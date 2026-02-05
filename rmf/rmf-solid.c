#include "rmf-solid.h"

#include "rmf/rmf-iterator.h"
#include "rmf/rmf-private.h"
#include "rmf/rmf-structs.h"

#include <glib-object.h>
#include <glib.h>

/**
 * RmfFaceIterator:
 *
 * An [iface@RmfIterator] which outputs [struct@RmfFace]s.
 */

/**
 * rmf_face_iterator_next:
 * @iterator: The iterator.
 *
 * Advance the iterator by one, returning the next object.
 *
 * Returns: (transfer none): The next face in the iterator, or `NULL` if the
 * iterator is exhausted.
 */
RMF_DEFINE_ITERATOR_TYPE(
    RmfFaceIterator,
    rmf_face_iterator,
    RMF,
    FACE_ITERATOR,
    RmfFace
)

/**
 * RmfSolid:
 *
 * An enclosed solid 3D space.
 */
struct _RmfSolid {
    RmfMapObject parent_instance;
    GPtrArray *faces;
};

enum Property {
    PROP_N_FACES = 1,
    PROP_FACES,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

G_DEFINE_FINAL_TYPE(RmfSolid, rmf_solid, RMF_TYPE_MAP_OBJECT)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_solid_dispose(GObject *object)
{
    auto self = RMF_SOLID(object);
    if (self->faces) {
        g_ptr_array_unref(self->faces);
        self->faces = nullptr;
    }
    G_OBJECT_CLASS(rmf_solid_parent_class)->dispose(object);
}

static void rmf_solid_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    auto const self = RMF_SOLID(object);
    switch ((enum Property)property_id) {
    case PROP_N_FACES:
        g_value_set_uint(value, self->faces->len);
        break;
    case PROP_FACES:
        g_value_take_object(value, rmf_face_iterator_new(self->faces));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// RmfMapObject ////////////////////////////////////////////////////////////////

static void rmf_solid_load(RmfMapObject *map_object, RmfLoader *loader)
{
    rmf_loader_log_begin(loader, "solid", nullptr);
    RMF_MAP_OBJECT_CLASS(rmf_solid_parent_class)->load(map_object, loader);

    auto const self = RMF_SOLID(map_object);
    auto const object_type = rmf_map_object_get_object_type(map_object);
    g_return_if_fail(object_type == RMF_OBJECT_TYPE_SOLID);

    rmf_int n_faces;
    rmf_read_int(loader, &n_faces);
    rmf_loader_log_begin(loader, "faces", "count", "%u", n_faces, nullptr);

    self->faces = g_ptr_array_new_full(n_faces, (GDestroyNotify)rmf_face_free);
    for (rmf_int i = 0; i < n_faces; ++i) {
        RmfFace *face = rmf_face_new(loader);
        g_ptr_array_add(self->faces, face);
    }
    rmf_loader_log_end(loader);
    rmf_loader_log_end(loader);
}

// RmfSolid ////////////////////////////////////////////////////////////////////

static void rmf_solid_class_init(RmfSolidClass *klass)
{
    auto moclass = RMF_MAP_OBJECT_CLASS(klass);
    moclass->load = rmf_solid_load;

    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = rmf_solid_dispose;
    oclass->get_property = rmf_solid_get_property;

    /**
     * RmfSolid:n-faces:
     *
     * The number of faces making up the brush.
     */
    obj_properties[PROP_N_FACES] = g_param_spec_uint(
        "n-faces",
        nullptr,
        "Number of faces.",
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    /**
     * RmfSolid:faces:
     *
     * A collection of faces which make up the brush.
     */
    obj_properties[PROP_FACES] = g_param_spec_object(
        "faces",
        nullptr,
        nullptr,
        RMF_TYPE_FACE_ITERATOR,
        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_solid_init(RmfSolid *)
{
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * rmf_solid_get_n_faces:
 * @solid: The solid.
 *
 * Gets the number of faces making up the solid.
 *
 * Returns: Number of faces in the solid.
 */
rmf_int rmf_solid_get_n_faces(RmfSolid *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-faces", &value, nullptr);
    return value;
}

/**
 * rmf_solid_get_faces:
 * @solid: The solid
 *
 * Gets the faces which make up the solid.
 *
 * Returns: (transfer full): Iterator over the [struct@RmfFace]s of the solid.
 */
RmfFaceIterator *rmf_solid_get_faces(RmfSolid *self)
{
    RmfFaceIterator *value = nullptr;
    g_object_get(self, "faces", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfSolid *rmf_solid_new(RmfLoader *loader)
{
    RmfSolid *self = g_object_new(RMF_TYPE_SOLID, nullptr);
    rmf_solid_load(RMF_MAP_OBJECT(self), loader);
    return self;
}
