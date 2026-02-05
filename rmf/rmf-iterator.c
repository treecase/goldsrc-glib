#include "rmf-iterator.h"

#include <glib-object.h>
#include <glib.h>

/**
 * RmfIterator:
 *
 * A generic iterator interface.
 */

G_DEFINE_INTERFACE(RmfIterator, rmf_iterator, G_TYPE_OBJECT);

static void rmf_iterator_default_init(RmfIteratorInterface *)
{
}

// Public //////////////////////////////////////////////////////////////////////

/**
 * rmf_iterator_next: (virtual next):
 * @iterator: The [iface@RmfIterator].
 *
 * Advance the iterator by one, returning the next object.
 *
 * Returns: (transfer none): The next object in the iterator, or `NULL` on error
 * or if the iterator is exhausted.
 */
void *rmf_iterator_next(RmfIterator *iterator)
{
    g_return_val_if_fail(RMF_IS_ITERATOR(iterator), nullptr);
    auto iface = RMF_ITERATOR_GET_IFACE(iterator);
    g_return_val_if_fail(iface->next != nullptr, nullptr);
    return iface->next(iterator);
}
