#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * WAD_LOAD_ERROR:
 *
 * Error domain for [class@WadRoot].
 */
#define WAD_LOAD_ERROR wad_load_error_quark()

/**
 * WadLoadError:
 * @WAD_LOAD_ERROR_MAGIC: Invalid WAD magic number.
 * @WAD_LOAD_ERROR_FILE_TYPE: Invalid file entry type.
 *
 * Error codes for `WAD_LOAD_ERROR`.
 */
typedef enum {
    WAD_LOAD_ERROR_MAGIC,
    WAD_LOAD_ERROR_FILE_TYPE,
} WadLoadError;

GQuark wad_load_error_quark(void);

G_END_DECLS
