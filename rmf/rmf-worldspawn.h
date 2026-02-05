#ifndef RMF_WORLDSPAWN_H
#define RMF_WORLDSPAWN_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-entitydata.h"
#include "rmf/rmf-structs.h"
#include "rmf/rmf-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

// RmfPathIterator

#define RMF_TYPE_PATH_ITERATOR rmf_path_iterator_get_type()
G_DECLARE_FINAL_TYPE(
    RmfPathIterator,
    rmf_path_iterator,
    RMF,
    PATH_ITERATOR,
    GObject
)

RmfPath *rmf_path_iterator_next(RmfPathIterator *iterator);

// RmfWorldspawn

#define RMF_TYPE_WORLDSPAWN rmf_worldspawn_get_type()
G_DECLARE_FINAL_TYPE(
    RmfWorldspawn,
    rmf_worldspawn,
    RMF,
    WORLDSPAWN,
    RmfEntityData
);

rmf_int rmf_worldspawn_get_n_paths(RmfWorldspawn *worldspawn);
RmfPathIterator *rmf_worldspawn_get_paths(RmfWorldspawn *worldspawn);

G_END_DECLS

#endif
