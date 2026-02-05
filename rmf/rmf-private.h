#ifndef RMF_PRIVATE_H
#define RMF_PRIVATE_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-entity.h"
#include "rmf/rmf-entitydata.h"
#include "rmf/rmf-group.h"
#include "rmf/rmf-loader.h"
#include "rmf/rmf-mapobject.h"
#include "rmf/rmf-solid.h"
#include "rmf/rmf-structs.h"
#include "rmf/rmf-types.h"
#include "rmf/rmf-worldspawn.h"

#include <glib.h>
#include <stddef.h>

// rmf-loader
void rmf_loader_set_offset(RmfLoader *self, size_t offset);
void rmf_loader_seek(RmfLoader *self, goffset n);
void rmf_loader_read(RmfLoader *restrict self, size_t n, void *restrict dest);
void rmf_loader_log_begin(
    RmfLoader *loader,
    char const *tag,
    ...
) G_GNUC_NULL_TERMINATED;
void rmf_loader_log_oneline(
    RmfLoader *loader,
    char const *tag,
    char const *content,
    ...
) G_GNUC_NULL_TERMINATED;
void rmf_loader_log_end(RmfLoader *loader);

// rmf-types
void rmf_read_byte(RmfLoader *restrict self, rmf_byte *restrict b);
void rmf_read_int(RmfLoader *restrict self, rmf_int *restrict i);
void rmf_read_float(RmfLoader *restrict self, rmf_float *restrict f);
void rmf_read_nstring(RmfLoader *restrict self, rmf_nstring *restrict nstring);
void rmf_read_color(RmfLoader *restrict self, RmfColor *restrict color);
void rmf_read_vector(RmfLoader *restrict self, RmfVector *restrict vector);

// rmf-structs
void
rmf_read_visgroup(RmfLoader *restrict self, RmfVisgroup *restrict visgroup);
RmfVisgroup *rmf_visgroup_new(RmfLoader *loader);

void rmf_read_face(RmfLoader *restrict self, RmfFace *restrict face);
RmfFace *rmf_face_new(RmfLoader *self);

void
rmf_read_keyvalue(RmfLoader *restrict self, RmfKeyvalue *restrict keyvalue);
RmfKeyvalue *rmf_keyvalue_new(RmfLoader *self);

void
rmf_read_pathnode(RmfLoader *restrict self, RmfPathNode *restrict pathnode);
RmfPathNode *rmf_pathnode_new(RmfLoader *self);

void rmf_read_path(RmfLoader *restrict self, RmfPath *restrict path);
RmfPath *rmf_path_new(RmfLoader *self);

void rmf_read_camera(RmfLoader *restrict self, RmfCamera *restrict camera);
RmfCamera *rmf_camera_new(RmfLoader *self);

void rmf_read_docinfo(RmfLoader *restrict self, RmfDocinfo *restrict docinfo);
RmfDocinfo *rmf_docinfo_new(RmfLoader *self);

// rmf-root
void rmf_read_root(RmfLoader *restrict loader, RmfRoot *restrict root);
RmfRoot *rmf_root_new(RmfLoader *loader);

// rmf-mapobject
RmfMapObject *rmf_map_object_new(RmfLoader *loader);
RmfLoader *rmf_map_object_get_loader(RmfMapObject *self);

// rmf-entitydata
RmfEntityData *rmf_entity_data_new(RmfLoader *loader);

// rmf-worldspawn
RmfWorldspawn *rmf_worldspawn_new(RmfLoader *loader);

// rmf-solid
RmfSolid *rmf_solid_new(RmfLoader *loader);

// rmf-entity
RmfEntity *rmf_entity_new(RmfLoader *loader);

// rmf-group
RmfGroup *rmf_group_new(RmfLoader *loader);

// Convenience macro to define iterators sourced from a GPtrArray.
#define RMF_DEFINE_ITERATOR_TYPE(IT, i_t, MODULE, OBJ_NAME, RT)            \
    struct _##IT {                                                         \
        GObject parent_instance;                                           \
        size_t index;                                                      \
        GPtrArray *items;                                                  \
    };                                                                     \
                                                                           \
    static void i_t##_iterator_interface_init(RmfIteratorInterface *iface) \
    {                                                                      \
        iface->next = (void *(*)(RmfIterator *))i_t##_next;                \
    }                                                                      \
                                                                           \
    G_DEFINE_FINAL_TYPE_WITH_CODE(                                         \
        IT,                                                                \
        i_t,                                                               \
        G_TYPE_OBJECT,                                                     \
        G_IMPLEMENT_INTERFACE(                                             \
            RMF_TYPE_ITERATOR,                                             \
            i_t##_iterator_interface_init                                  \
        )                                                                  \
    )                                                                      \
                                                                           \
    static void i_t##_dispose(GObject *object)                             \
    {                                                                      \
        IT *self = MODULE##_##OBJ_NAME(object);                            \
        if (self->items) {                                                 \
            g_ptr_array_unref(self->items);                                \
            self->items = nullptr;                                         \
        }                                                                  \
        G_OBJECT_CLASS(i_t##_parent_class)->dispose(object);               \
    }                                                                      \
                                                                           \
    static void i_t##_class_init(IT##Class *klass)                         \
    {                                                                      \
        G_OBJECT_CLASS(klass)->dispose = i_t##_dispose;                    \
    }                                                                      \
                                                                           \
    static void i_t##_init(IT *self)                                       \
    {                                                                      \
        self->index = 0;                                                   \
        self->items = nullptr;                                             \
    }                                                                      \
                                                                           \
    static IT *i_t##_new(GPtrArray *items)                                 \
    {                                                                      \
        IT *self = g_object_new(MODULE##_TYPE_##OBJ_NAME, nullptr);        \
        if (items) {                                                       \
            self->items = g_ptr_array_ref(items);                          \
        }                                                                  \
        return self;                                                       \
    }                                                                      \
                                                                           \
    RT *i_t##_next(IT *self)                                               \
    {                                                                      \
        if (self->items && self->index < self->items->len) {               \
            return (RT *)self->items->pdata[self->index++];                \
        }                                                                  \
        return nullptr;                                                    \
    }

#endif
