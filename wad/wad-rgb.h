#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * WadRgb:
 * An RGB triple.
 */
typedef struct {
    guchar rgb[3];
} WadRgb;

G_END_DECLS
