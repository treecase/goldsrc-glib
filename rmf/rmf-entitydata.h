#ifndef RMF_ENTITY_DATA_H
#define RMF_ENTITY_DATA_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-mapobject.h"
#include "rmf/rmf-structs.h"
#include "rmf/rmf-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

// RmfKeyvalueIterator

#define RMF_TYPE_KEYVALUE_ITERATOR rmf_keyvalue_iterator_get_type()
G_DECLARE_FINAL_TYPE(
    RmfKeyvalueIterator,
    rmf_keyvalue_iterator,
    RMF,
    KEYVALUE_ITERATOR,
    GObject
)

RmfKeyvalue *rmf_keyvalue_iterator_next(RmfKeyvalueIterator *iterator);

// RmfEntityData

#define RMF_TYPE_ENTITY_DATA rmf_entity_data_get_type()
G_DECLARE_DERIVABLE_TYPE(
    RmfEntityData,
    rmf_entity_data,
    RMF,
    ENTITY_DATA,
    RmfMapObject
);

struct _RmfEntityDataClass {
    RmfMapObjectClass parent_class;
};

char *rmf_entity_data_get_classname(RmfEntityData *entity_data);
rmf_int rmf_entity_data_get_spawnflags(RmfEntityData *entity_data);
rmf_int rmf_entity_data_get_n_keyvalues(RmfEntityData *entity_data);
RmfKeyvalueIterator *rmf_entity_data_get_keyvalues(RmfEntityData *entity_data);

G_END_DECLS

#endif
