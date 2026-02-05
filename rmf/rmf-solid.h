#ifndef RMF_SOLID_H
#define RMF_SOLID_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-mapobject.h"
#include "rmf/rmf-structs.h"

#include <glib-object.h>

G_BEGIN_DECLS

// RmfFaceIterator

#define RMF_TYPE_FACE_ITERATOR rmf_face_iterator_get_type()
G_DECLARE_FINAL_TYPE(
    RmfFaceIterator,
    rmf_face_iterator,
    RMF,
    FACE_ITERATOR,
    GObject
)

RmfFace *rmf_face_iterator_next(RmfFaceIterator *iterator);

// RmfSolid

#define RMF_TYPE_SOLID rmf_solid_get_type()
G_DECLARE_FINAL_TYPE(RmfSolid, rmf_solid, RMF, SOLID, RmfMapObject);

rmf_int rmf_solid_get_n_faces(RmfSolid *solid);
RmfFaceIterator *rmf_solid_get_faces(RmfSolid *solid);

G_END_DECLS

#endif
