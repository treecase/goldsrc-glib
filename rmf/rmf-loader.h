#ifndef RMF_LOADER_H
#define RMF_LOADER_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-types.h"

#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _RmfRoot RmfRoot;

// RmfLoaderError

/**
 * RMF_LOADER_ERROR:
 *
 * Error domain for `RmfLoader`.
 */
#define RMF_LOADER_ERROR rmf_loader_error_quark()

typedef enum {
    // TODO
    RMF_LOADER_ERROR_XYZ,
} RmfLoaderError;

// RmfLoader

#define RMF_TYPE_LOADER rmf_loader_get_type()
G_DECLARE_FINAL_TYPE(RmfLoader, rmf_loader, RMF, LOADER, GObject)

RmfLoader *rmf_loader_new(void);

void rmf_loader_load_from_file(RmfLoader *loader, GFile *file, GError **error);

RmfRoot *rmf_loader_get_root(RmfLoader *loader);

rmf_float rmf_loader_get_version(RmfLoader *loader);

G_END_DECLS

#endif
