#include "rmf-types.h"

#include "rmf-loader.h"
#include "rmf-private.h"

#include <glib-object.h>
#include <glib.h>

void rmf_read_byte(RmfLoader *self, rmf_byte *b)
{
    rmf_loader_read(self, sizeof(rmf_byte), b);
}

void rmf_read_int(RmfLoader *self, rmf_int *i)
{
    rmf_loader_read(self, sizeof(rmf_int), i);
    *i = GUINT32_FROM_LE(*i);
}

void rmf_read_float(RmfLoader *self, rmf_float *f)
{
    rmf_loader_read(self, sizeof(rmf_float), f);
}

void rmf_read_nstring(RmfLoader *rmf, rmf_nstring *nstring)
{
    rmf_read_byte(rmf, &nstring->length);
    g_assert(nstring->length > 0);
    rmf_loader_read(rmf, nstring->length, nstring->data);
    bool const is_null_terminated = nstring->data[nstring->length - 1] == '\0';
    g_assert(is_null_terminated);
}

/**
 * RmfColor:
 *
 * An RGB color.
 */
G_DEFINE_BOXED_TYPE(RmfColor, rmf_color, rmf_color_copy, rmf_color_free)

void rmf_read_color(RmfLoader *rmf, RmfColor *color)
{
    rmf_loader_read(rmf, sizeof(RmfColor), color);
}

RmfColor *rmf_color_copy(RmfColor *color)
{
    RmfColor *out = g_new(RmfColor, 1);
    memcpy(out, color, sizeof(RmfColor));
    return out;
}

void rmf_color_free(RmfColor *color)
{
    g_free(color);
}

/**
 * RmfVector:
 *
 * A 3D vector.
 */
G_DEFINE_BOXED_TYPE(RmfVector, rmf_vector, rmf_vector_copy, rmf_vector_free)

void rmf_read_vector(RmfLoader *rmf, RmfVector *color)
{
    rmf_loader_read(rmf, sizeof(RmfVector), color);
}

RmfVector *rmf_vector_copy(RmfVector *vector)
{
    RmfVector *out = g_new(RmfVector, 1);
    memcpy(out, vector, sizeof(RmfVector));
    return out;
}

void rmf_vector_free(RmfVector *vector)
{
    g_free(vector);
}
