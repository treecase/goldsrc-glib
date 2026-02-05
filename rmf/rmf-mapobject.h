#ifndef RMF_MAP_OBJECT_H
#define RMF_MAP_OBJECT_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-loader.h"

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _RmfMapObject RmfMapObject;

// RmfObjectType

#define RMF_TYPE_OBJECT_TYPE rmf_object_type_get_type()

typedef enum {
    RMF_OBJECT_TYPE_UNKNOWN,
    RMF_OBJECT_TYPE_WORLD,
    RMF_OBJECT_TYPE_SOLID,
    RMF_OBJECT_TYPE_ENTITY,
    RMF_OBJECT_TYPE_GROUP,
} RmfObjectType;

GType rmf_object_type_get_type(void);

// RmfMapObjectIterator

#define RMF_TYPE_MAP_OBJECT_ITERATOR rmf_map_object_iterator_get_type()
G_DECLARE_FINAL_TYPE(
    RmfMapObjectIterator,
    rmf_map_object_iterator,
    RMF,
    MAP_OBJECT_ITERATOR,
    GObject
)

RmfMapObject *rmf_map_object_iterator_next(RmfMapObjectIterator *iterator);

// RmfMapObject

#define RMF_TYPE_MAP_OBJECT rmf_map_object_get_type()
G_DECLARE_DERIVABLE_TYPE(
    RmfMapObject,
    rmf_map_object,
    RMF,
    MAP_OBJECT,
    GObject
);

struct _RmfMapObjectClass {
    GObjectClass parent_class;

    void (*load)(RmfMapObject *map_object, RmfLoader *loader);
};

void rmf_map_object_load(RmfMapObject *map_object, RmfLoader *loader);

RmfObjectType rmf_map_object_get_object_type(RmfMapObject *map_object);
rmf_int rmf_map_object_get_visgroup_id(RmfMapObject *map_object);
RmfColor rmf_map_object_get_color(RmfMapObject *map_object);
rmf_int rmf_map_object_get_n_children(RmfMapObject *map_object);
RmfMapObjectIterator *rmf_map_object_get_children(RmfMapObject *map_object);

G_END_DECLS

#endif
