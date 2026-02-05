#include "rmf-structs.h"

#include "rmf-loader.h"
#include "rmf-private.h"

#include <glib.h>

/**
 * RmfVisgroup:
 * @name: Display name of the group.
 * @color: Color used for the group.
 * @visgroup_id: Internal ID number.
 * @visible: Whether the group is visible or not.
 *
 * A named group of objects which can be hidden in the editor.
 */
G_DEFINE_BOXED_TYPE(
    RmfVisgroup,
    rmf_visgroup,
    rmf_visgroup_copy,
    rmf_visgroup_free
)

void rmf_read_visgroup(RmfLoader *self, RmfVisgroup *visgroup)
{
    rmf_loader_read(self, 128, visgroup->name);
    rmf_read_color(self, &visgroup->color);
    rmf_loader_seek(self, 1);
    rmf_read_int(self, &visgroup->visgroup_id);
    rmf_byte visible = 0;
    rmf_read_byte(self, &visible);
    visgroup->visible = visible == 0;
    rmf_loader_seek(self, 3);

    rmf_loader_log_oneline(
        self,
        "visgroup",
        nullptr,
        "name",
        "%s",
        visgroup->name,
        "id",
        "%u",
        visgroup->visgroup_id,
        nullptr
    );
}

RmfVisgroup *rmf_visgroup_new(RmfLoader *loader)
{
    auto const self = g_new(RmfVisgroup, 1);
    rmf_read_visgroup(loader, self);
    return self;
}

RmfVisgroup *rmf_visgroup_copy(RmfVisgroup const *self)
{
    auto const copy = g_new(RmfVisgroup, 1);
    memcpy(copy, self, sizeof(RmfVisgroup));
    return copy;
}

void rmf_visgroup_free(RmfVisgroup *self)
{
    g_free(self);
}

/**
 * RmfFace:
 * @texture_name: Name of the texture applied to the
 * face.
 * @right_axis: Projected right axis for texture application.
 * @shift_x: Horizontal shift of the texture
 * @down_axis: Projected down axis for texture application.
 * @shift_y: Vertical shift of the texture.
 * @angle: Rotation applied to the right and down axes.
 * @scale_x: Horizontal scaling multiplier.
 * @scale_y: Vertical scaling multiplier.
 * @vertices: (element-type RmfVector): The vertices which make up the polygon.
 * @plane_points: A triple of points which define the face's 3D plane.
 *
 * A flat polygon, used to define the 3D space which makes up a
 * [class@RmfSolid].
 */
G_DEFINE_BOXED_TYPE(RmfFace, rmf_face, rmf_face_copy, rmf_face_free)

void rmf_read_face(RmfLoader *self, RmfFace *face)
{
    auto const RMF_VERSION = rmf_loader_get_version(self);

    rmf_loader_read(self, RMF_VERSION > 1.6f ? 256 : 36, face->texture_name);
    rmf_loader_seek(self, 4);
    if (RMF_VERSION >= 2.2f) {
        rmf_read_vector(self, &face->right_axis);
    } else {
        // TODO: Compute right_axis. See
        // https://github.com/id-Software/Quake-Tools/blob/master/qutils/QBSP/MAP.C
        // for details.
    }
    rmf_read_float(self, &face->shift_x);
    if (RMF_VERSION >= 2.2f) {
        rmf_read_vector(self, &face->down_axis);
    } else {
        // TODO: Compute down_axis. See
        // https://github.com/id-Software/Quake-Tools/blob/master/qutils/QBSP/MAP.C
        // for details.
    }
    rmf_read_float(self, &face->shift_y);
    rmf_read_float(self, &face->angle);
    rmf_read_float(self, &face->scale_x);
    rmf_read_float(self, &face->scale_y);
    rmf_loader_seek(self, RMF_VERSION > 1.6f ? 16 : 4);
    rmf_int n_vertices = 0;
    rmf_read_int(self, &n_vertices);

    rmf_loader_log_oneline(
        self,
        "face",
        nullptr,
        "n_vertices",
        "%u",
        n_vertices,
        nullptr
    );

    auto const vertices = g_new(RmfVector, n_vertices);
    rmf_loader_read(self, n_vertices * sizeof(RmfVector), vertices);
    face->vertices
        = g_array_new_take(vertices, n_vertices, FALSE, sizeof(RmfVector));
    rmf_loader_read(self, 3 * sizeof(RmfVector), face->plane_points);
}

RmfFace *rmf_face_new(RmfLoader *loader)
{
    auto const self = g_new(RmfFace, 1);
    rmf_read_face(loader, self);
    return self;
}

RmfFace *rmf_face_copy(RmfFace const *self)
{
    auto const copy = g_new(RmfFace, 1);
    memcpy(copy, self, sizeof(RmfFace));
    copy->vertices = g_array_ref(self->vertices);
    return copy;
}

void rmf_face_free(RmfFace *self)
{
    g_array_unref(self->vertices);
    g_free(self);
}

/**
 * RmfKeyvalue:
 * @key: Key name.
 * @value: Value string.
 *
 * A key-value pair, stored as strings.
 */
G_DEFINE_BOXED_TYPE(
    RmfKeyvalue,
    rmf_keyvalue,
    rmf_keyvalue_copy,
    rmf_keyvalue_free
)

void rmf_read_keyvalue(RmfLoader *self, RmfKeyvalue *keyvalue)
{
    rmf_read_nstring(self, &keyvalue->key);
    rmf_read_nstring(self, &keyvalue->value);
    rmf_loader_log_oneline(
        self,
        "keyvalue",
        keyvalue->value.data,
        "key",
        "%s",
        keyvalue->key.data,
        nullptr
    );
}

RmfKeyvalue *rmf_keyvalue_new(RmfLoader *loader)
{
    auto const self = g_new(RmfKeyvalue, 1);
    rmf_read_keyvalue(loader, self);
    return self;
}

RmfKeyvalue *rmf_keyvalue_copy(RmfKeyvalue const *self)
{
    auto const copy = g_new(RmfKeyvalue, 1);
    memcpy(copy, self, sizeof(RmfKeyvalue));
    return copy;
}

void rmf_keyvalue_free(RmfKeyvalue *self)
{
    g_free(self);
}

/**
 * RmfPathNode:
 * @position: The node's position in the world.
 * @index: The node's index in the path.
 * @name_override: Display name to use instead of an auto-generated one.
 * @keyvalues: (element-type RmfKeyvalue): The node's key-value pairs.
 *
 * A node making up an [struct@RmfPath].
 */
G_DEFINE_BOXED_TYPE(
    RmfPathNode,
    rmf_path_node,
    rmf_path_node_copy,
    rmf_path_node_free
)

void rmf_read_pathnode(RmfLoader *self, RmfPathNode *pathnode)
{
    rmf_read_vector(self, &pathnode->position);
    rmf_read_int(self, &pathnode->index);
    rmf_loader_read(self, 128, pathnode->name_override);
    rmf_int n_keyvalues = 0;
    rmf_read_int(self, &n_keyvalues);
    pathnode->keyvalues
        = g_array_sized_new(FALSE, FALSE, sizeof(RmfKeyvalue), n_keyvalues);
    for (rmf_int i = 0; i < n_keyvalues; ++i) {
        RmfKeyvalue keyvalue;
        rmf_read_keyvalue(self, &keyvalue);
        g_array_append_val(pathnode->keyvalues, keyvalue);
    }
}

RmfPathNode *rmf_path_node_new(RmfLoader *loader)
{
    auto const self = g_new(RmfPathNode, 1);
    rmf_read_pathnode(loader, self);
    return self;
}

RmfPathNode *rmf_path_node_copy(RmfPathNode *self)
{
    auto const copy = g_new(RmfPathNode, 1);
    memcpy(copy, self, sizeof(RmfPathNode));
    copy->keyvalues = g_array_ref(self->keyvalues);
    return copy;
}

void rmf_path_node_free(RmfPathNode *self)
{
    g_array_unref(self->keyvalues);
    g_free(self);
}

/**
 * RmfPath:
 * @path_name: Base name of this path.
 * @classname: Path's class name (usually `path_corner` or `path_track`).
 * @path_type: The direction of the path.
 * @nodes: (element-type RmfPathNode): The constituent nodes.
 *
 * A path placed by the Path Tool.
 */
G_DEFINE_BOXED_TYPE(RmfPath, rmf_path, rmf_path_copy, rmf_path_free)

void rmf_read_path(RmfLoader *self, RmfPath *path)
{
    rmf_loader_read(self, 128, &path->path_name);
    rmf_loader_read(self, 128, &path->classname);
    rmf_read_int(self, &path->path_type);
    rmf_int n_nodes;
    rmf_read_int(self, &n_nodes);
    path->nodes
        = g_ptr_array_new_full(n_nodes, (GDestroyNotify)rmf_path_node_free);
    for (rmf_int i = 0; i < n_nodes; ++i) {
        RmfPathNode *pathnode = rmf_path_node_new(self);
        g_ptr_array_add(path->nodes, pathnode);
    }
}

RmfPath *rmf_path_new(RmfLoader *loader)
{
    auto const self = g_new(RmfPath, 1);
    rmf_read_path(loader, self);
    return self;
}

RmfPath *rmf_path_copy(RmfPath *self)
{
    auto const copy = g_new(RmfPath, 1);
    memcpy(copy, self, sizeof(RmfPath));
    copy->nodes = g_ptr_array_ref(self->nodes);
    return copy;
}

void rmf_path_free(RmfPath *self)
{
    g_ptr_array_unref(self->nodes);
    g_free(self);
}

/**
 * RmfCamera:
 * @eye_position: The position of the camera in the world.
 * @lookat_position: Target position that the camera is looking at.
 *
 * A saved viewport position.
 */
G_DEFINE_BOXED_TYPE(RmfCamera, rmf_camera, rmf_camera_copy, rmf_camera_free)

void rmf_read_camera(RmfLoader *self, RmfCamera *camera)
{
    rmf_loader_read(self, sizeof(RmfCamera), camera);
    g_autofree auto eyestr = g_strdup_printf(
        "%g %g %g",
        camera->eye_position.x,
        camera->eye_position.y,
        camera->eye_position.z
    );
    g_autofree auto lookstr = g_strdup_printf(
        "%g %g %g",
        camera->lookat_position.x,
        camera->lookat_position.y,
        camera->lookat_position.z
    );
    rmf_loader_log_oneline(
        self,
        "camera",
        nullptr,
        "eye",
        "%s",
        eyestr,
        "lookat",
        "%s",
        lookstr,
        nullptr
    );
}

RmfCamera *rmf_camera_new(RmfLoader *loader)
{
    auto const self = g_new(RmfCamera, 1);
    rmf_read_camera(loader, self);
    return self;
}

RmfCamera *rmf_camera_copy(RmfCamera *self)
{
    auto const copy = g_new(RmfCamera, 1);
    memcpy(copy, self, sizeof(RmfCamera));
    return copy;
}

void rmf_camera_free(RmfCamera *self)
{
    g_free(self);
}

/**
 * RmfDocinfo:
 * @docinfo_version: Docinfo version number -- always `0.2`.
 * @active_camera: Index of the active camera.
 * @cameras: (element-type RmfCamera): List of saved cameras.
 *
 * Stores camera data.
 */
G_DEFINE_BOXED_TYPE(RmfDocinfo, rmf_docinfo, rmf_docinfo_copy, rmf_docinfo_free)

void rmf_read_docinfo(RmfLoader *self, RmfDocinfo *docinfo)
{
    char docinfo_header[8];
    rmf_loader_read(self, 8, docinfo_header);
    g_warn_if_fail(memcmp(docinfo_header, "DOCINFO", 8) == 0);

    rmf_read_float(self, &docinfo->docinfo_version);
    rmf_read_int(self, &docinfo->active_camera);

    rmf_loader_log_begin(
        self,
        "docinfo",
        "version",
        "%g",
        docinfo->docinfo_version,
        "active_camera",
        "%u",
        docinfo->active_camera,
        nullptr
    );

    rmf_int n_cameras = 0;
    rmf_read_int(self, &n_cameras);

    rmf_loader_log_begin(self, "cameras", "count", "%u", n_cameras, nullptr);

    docinfo->cameras
        = g_array_sized_new(FALSE, FALSE, sizeof(RmfCamera), n_cameras);
    for (rmf_int i = 0; i < n_cameras; ++i) {
        RmfCamera camera;
        rmf_read_camera(self, &camera);
        g_array_append_val(docinfo->cameras, camera);
    }

    rmf_loader_log_end(self);
    rmf_loader_log_end(self);
}

RmfDocinfo *rmf_docinfo_new(RmfLoader *loader)
{
    auto const self = g_new(RmfDocinfo, 1);
    rmf_read_docinfo(loader, self);
    return self;
}

RmfDocinfo *rmf_docinfo_copy(RmfDocinfo *self)
{
    auto const copy = g_new(RmfDocinfo, 1);
    memcpy(copy, self, sizeof(RmfDocinfo));
    copy->cameras = g_array_ref(self->cameras);
    return copy;
}

void rmf_docinfo_free(RmfDocinfo *self)
{
    g_array_unref(self->cameras);
    g_free(self);
}
