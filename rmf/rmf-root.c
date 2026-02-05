#include "rmf/rmf-root.h"

#include "glib.h"
#include "rmf/rmf-iterator.h"
#include "rmf/rmf-private.h"

#include <float.h>
#include <glib-object.h>

/**
 * RmfVisgroupIterator:
 *
 * An [iface@RmfIterator] which outputs [struct@RmfVisgroup]s.
 */

/**
 * rmf_visgroup_iterator_next:
 * @iterator: The iterator.
 *
 * Advance the iterator by one, returning the next visgroup.
 *
 * Returns: (transfer none): The next visgroup in the iterator, or `NULL` if the
 * iterator is exhausted.
 */
RMF_DEFINE_ITERATOR_TYPE(
    RmfVisgroupIterator,
    rmf_visgroup_iterator,
    RMF,
    VISGROUP_ITERATOR,
    RmfVisgroup
)

/**
 * RmfRoot:
 *
 * The top-level RMF object.
 */
struct _RmfRoot {
    GObject parent_instance;
    GPtrArray *visgroups; // PtrArray<RmfVisgroup>
    RmfWorldspawn *worldspawn;
    RmfDocinfo *docinfo;
};

G_DEFINE_FINAL_TYPE(RmfRoot, rmf_root, G_TYPE_OBJECT)

enum Property {
    PROP_N_VISGROUPS = 1,
    PROP_VISGROUPS,
    PROP_WORLDSPAWN,
    PROP_DOCINFO,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_root_dispose(GObject *object)
{
    auto self = RMF_ROOT(object);
    if (self->visgroups) {
        g_ptr_array_unref(self->visgroups);
        self->visgroups = nullptr;
    }
    g_clear_object(&self->worldspawn);
    if (self->docinfo) {
        rmf_docinfo_free(self->docinfo);
        self->docinfo = nullptr;
    }
    G_OBJECT_CLASS(rmf_root_parent_class)->dispose(object);
}

static void rmf_root_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    auto self = RMF_ROOT(object);
    switch ((enum Property)property_id) {
    case PROP_N_VISGROUPS:
        g_value_set_uint(value, self->visgroups->len);
        break;
    case PROP_VISGROUPS:
        g_value_take_object(value, rmf_visgroup_iterator_new(self->visgroups));
        break;
    case PROP_WORLDSPAWN:
        g_value_set_object(value, self->worldspawn);
        break;
    case PROP_DOCINFO:
        g_value_set_boxed(value, self->docinfo);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

// RmfRoot /////////////////////////////////////////////////////////////////////

static void rmf_root_class_init(RmfRootClass *klass)
{
    auto oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = rmf_root_dispose;
    oclass->get_property = rmf_root_get_property;

    /**
     * RmfRoot:n-visgroups
     * The number of visgroups in the RMF data.
     */
    obj_properties[PROP_N_VISGROUPS] = g_param_spec_uint(
        "n-visgroups",
        nullptr,
        nullptr,
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE
    );

    /**
     * RmfRoot:visgroups
     * List of visgroups in the RMF data.
     */
    obj_properties[PROP_VISGROUPS] = g_param_spec_object(
        "visgroups",
        nullptr,
        nullptr,
        RMF_TYPE_VISGROUP_ITERATOR,
        G_PARAM_READABLE
    );

    /**
     * RmfRoot:worldspawn
     * The root object of the RMF data's object tree.
     */
    obj_properties[PROP_WORLDSPAWN] = g_param_spec_object(
        "worldspawn",
        nullptr,
        nullptr,
        RMF_TYPE_WORLDSPAWN,
        G_PARAM_READABLE
    );

    /**
     * RmfRoot:docinfo
     * The RMF data's DOCINFO block.
     */
    obj_properties[PROP_DOCINFO] = g_param_spec_boxed(
        "docinfo",
        nullptr,
        nullptr,
        RMF_TYPE_VISGROUP,
        G_PARAM_READABLE
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_root_init(RmfRoot *)
{
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * rmf_root_get_n_visgroups
 * @root: The root.
 *
 * Gets the number of visgroups in the RMF.
 *
 * Returns: The number of visgroups.
 */
rmf_int rmf_root_get_n_visgroups(RmfRoot *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-visgroups", &value, nullptr);
    return value;
}

/**
 * rmf_root_get_visgroups
 * @root: The root.
 *
 * Gets the list of visgroups in the RMF.
 *
 * Returns: (transfer full): The list of visgroups.
 */
RmfVisgroupIterator *rmf_root_get_visgroups(RmfRoot *self)
{
    RmfVisgroupIterator *value = nullptr;
    g_object_get(self, "visgroups", &value, nullptr);
    return value;
}

/**
 * rmf_root_get_worldspawn
 * @root: The root.
 *
 * Gets the worldspawn of the RMF.
 *
 * Returns: (transfer none): The RMF's worldspawn.
 */
RmfWorldspawn *rmf_root_get_worldspawn(RmfRoot *self)
{
    RmfWorldspawn *value = nullptr;
    g_object_get(self, "worldspawn", &value, nullptr);
    return value;
}

/**
 * rmf_root_get_docinfo
 * @root: The root.
 *
 * Gets the RMF's DOCINFO block.
 *
 * Returns: (transfer none): The RMF's DOCINFO.
 */
RmfDocinfo *rmf_root_get_docinfo(RmfRoot *self)
{
    RmfDocinfo *value = nullptr;
    g_object_get(self, "docinfo", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfRoot *rmf_root_new(RmfLoader *loader)
{
    RmfRoot *self = g_object_new(RMF_TYPE_ROOT, nullptr);
    rmf_read_root(loader, self);
    return self;
}

void rmf_read_root(RmfLoader *loader, RmfRoot *self)
{
    rmf_int n_visgroups = 0;
    rmf_read_int(loader, &n_visgroups);
    self->visgroups
        = g_ptr_array_new_full(n_visgroups, (GDestroyNotify)rmf_visgroup_free);

    rmf_loader_log_begin(
        loader,
        "visgroups",
        "count",
        "%u",
        n_visgroups,
        nullptr
    );
    for (rmf_int i = 0; i < n_visgroups; ++i) {
        auto visgroup = rmf_visgroup_new(loader);
        g_ptr_array_add(self->visgroups, visgroup);
    }
    rmf_loader_log_end(loader);

    self->worldspawn = rmf_worldspawn_new(loader);
    self->docinfo = rmf_docinfo_new(loader);
}
