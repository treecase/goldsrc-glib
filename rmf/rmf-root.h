#ifndef RMF_ROOT_H
#define RMF_ROOT_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-structs.h"
#include "rmf/rmf-worldspawn.h"

#include <glib-object.h>

G_BEGIN_DECLS

// RmfVisgroupIterator

#define RMF_TYPE_VISGROUP_ITERATOR rmf_visgroup_iterator_get_type()
G_DECLARE_FINAL_TYPE(
    RmfVisgroupIterator,
    rmf_visgroup_iterator,
    RMF,
    VISGROUP_ITERATOR,
    GObject
)

RmfVisgroup *rmf_visgroup_iterator_next(RmfVisgroupIterator *iterator);

// RmfRoot

#define RMF_TYPE_ROOT rmf_root_get_type()
G_DECLARE_FINAL_TYPE(RmfRoot, rmf_root, RMF, ROOT, GObject)

rmf_int rmf_root_get_n_visgroups(RmfRoot *root);
RmfVisgroupIterator *rmf_root_get_visgroups(RmfRoot *root);
RmfWorldspawn *rmf_root_get_worldspawn(RmfRoot *root);
RmfDocinfo *rmf_root_get_docinfo(RmfRoot *root);

G_END_DECLS

#endif
