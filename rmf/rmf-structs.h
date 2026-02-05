#ifndef RMF_STRUCTS_H
#define RMF_STRUCTS_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-types.h"

#include <glib-object.h>
#include <stddef.h>

// RmfVisgroup

#define RMF_TYPE_VISGROUP rmf_visgroup_get_type()

typedef struct {
    char name[128];
    RmfColor color;
    rmf_int visgroup_id;
    bool visible;
} RmfVisgroup;

GType rmf_visgroup_get_type(void);
RmfVisgroup *rmf_visgroup_copy(RmfVisgroup const *self);
void rmf_visgroup_free(RmfVisgroup *self);

// RmfFace

#define RMF_TYPE_FACE rmf_face_get_type()

typedef struct {
    char texture_name[256]; // Until RMF v1.8: 36 bytes long
    RmfVector right_axis;   // Since RMF v2.2
    rmf_float shift_x;
    RmfVector down_axis; // Since RMF v2.2
    rmf_float shift_y;
    rmf_float angle;
    rmf_float scale_x;
    rmf_float scale_y;
    GArray *vertices;
    RmfVector plane_points[3];
} RmfFace;

GType rmf_face_get_type(void);
RmfFace *rmf_face_copy(RmfFace const *self);
void rmf_face_free(RmfFace *self);

// RmfKeyvalue

#define RMF_TYPE_KEYVALUE rmf_keyvalue_get_type()

typedef struct {
    rmf_nstring key;
    rmf_nstring value;
} RmfKeyvalue;

GType rmf_keyvalue_get_type(void);
RmfKeyvalue *rmf_keyvalue_copy(RmfKeyvalue const *self);
void rmf_keyvalue_free(RmfKeyvalue *self);

// RmfPathNode

#define RMF_TYPE_PATH_NODE rmf_path_node_get_type()

typedef struct {
    RmfVector position;
    rmf_int index;
    char name_override[128];
    GArray *keyvalues; // Array<RmfKeyvalue>
} RmfPathNode;

GType rmf_path_node_get_type(void);
RmfPathNode *rmf_path_node_copy(RmfPathNode *self);
void rmf_path_node_free(RmfPathNode *self);

// RmfPath

#define RMF_TYPE_PATH rmf_path_get_type()

typedef struct {
    char path_name[128];
    char classname[128];
    rmf_int path_type; // 0: One-way, first to last then stop
                       // 1: Circular, first to last then teleport to first
                       // 2: Ping-pong, first to last then reverse back to first
    GPtrArray *nodes;  // PtrArray<RmfPathNode>
} RmfPath;

GType rmf_path_get_type(void);
RmfPath *rmf_path_copy(RmfPath *self);
void rmf_path_free(RmfPath *self);

// RmfCamera

#define RMF_TYPE_CAMERA rmf_camera_get_type()

typedef struct {
    RmfVector eye_position;
    RmfVector lookat_position;
} RmfCamera;

GType rmf_camera_get_type(void);
RmfCamera *rmf_camera_copy(RmfCamera *self);
void rmf_camera_free(RmfCamera *self);

// RmfDocinfo

#define RMF_TYPE_DOCINFO rmf_docinfo_get_type()

typedef struct {
    rmf_float docinfo_version;
    rmf_int active_camera;
    GArray *cameras; // Array<RmfCamera>
} RmfDocinfo;

GType rmf_docinfo_get_type(void);
RmfDocinfo *rmf_docinfo_copy(RmfDocinfo *self);
void rmf_docinfo_free(RmfDocinfo *self);

#endif
