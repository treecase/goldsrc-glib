#include "rmf/rmf-group.h"

#include "rmf/rmf-loader.h"
#include "rmf/rmf-mapobject.h"

#include <glib-object.h>
#include <glib.h>

/**
 * RmfGroup
 *
 * A collection of child objects.
 */
struct _RmfGroup {
    RmfMapObject parent_instance;
};

G_DEFINE_FINAL_TYPE(RmfGroup, rmf_group, RMF_TYPE_MAP_OBJECT)

// RmfMapObject ////////////////////////////////////////////////////////////////

static void rmf_group_load(RmfMapObject *map_object, RmfLoader *loader)
{
    RMF_MAP_OBJECT_CLASS(rmf_group_parent_class)->load(map_object, loader);

    auto object_type = rmf_map_object_get_object_type(map_object);
    g_return_if_fail(object_type == RMF_OBJECT_TYPE_GROUP);
}

// RmfGroup ////////////////////////////////////////////////////////////////////

static void rmf_group_class_init(RmfGroupClass *klass)
{
    auto moclass = RMF_MAP_OBJECT_CLASS(klass);
    moclass->load = rmf_group_load;
}

static void rmf_group_init(RmfGroup *)
{
}

// Internal ////////////////////////////////////////////////////////////////////

RmfGroup *rmf_group_new(RmfLoader *loader)
{
    RmfGroup *self = g_object_new(RMF_TYPE_GROUP, nullptr);
    rmf_group_load(RMF_MAP_OBJECT(self), loader);
    return self;
}
