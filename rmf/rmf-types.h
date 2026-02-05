#ifndef RMF_TYPES_H
#define RMF_TYPES_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include <glib-object.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t rmf_byte;
typedef uint32_t rmf_int;
typedef float rmf_float;

typedef struct {
    rmf_byte length;
    char data[256];
} rmf_nstring;

// RmfColor

#define RMF_TYPE_COLOR rmf_color_get_type()

typedef struct {
    rmf_byte r, g, b;
} RmfColor;

GType rmf_color_get_type(void);
RmfColor *rmf_color_copy(RmfColor *color);
void rmf_color_free(RmfColor *color);

// RmfVector

#define RMF_TYPE_VECTOR rmf_vector_get_type()

typedef struct {
    rmf_float x, y, z;
} RmfVector;

GType rmf_vector_get_type(void);
RmfVector *rmf_vector_copy(RmfVector *vector);
void rmf_vector_free(RmfVector *vector);

#endif
