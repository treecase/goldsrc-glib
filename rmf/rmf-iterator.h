#ifndef RMF_ITERATOR_H
#define RMF_ITERATOR_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RMF_TYPE_ITERATOR rmf_iterator_get_type()
G_DECLARE_INTERFACE(RmfIterator, rmf_iterator, RMF, ITERATOR, GObject)

struct _RmfIteratorInterface {
    GTypeInterface parent;

    void *(*next)(RmfIterator *iterator);
};

void *rmf_iterator_next(RmfIterator *iterator);

/**
 * RMF_ITERATOR_FOREACH:
 * @Type: Type returned by the iterator. (eg. [struct@RmfKeyvalue] for
 * [class@RmfKeyvalueIterator])
 * @name: Name to use for the iterated value.
 * @iterator: Name of the iterator to iterate through.
 *
 * Helper macro for iterating through [iface@RmfIterator]s.
 *
 * Example usage:
 *
 * ```c
 * RmfKeyvalueIterator *keyvalues = ...;
 * RMF_ITERATOR_FOREACH(RmfKeyvalue, kv, keyvalues) {
 *   g_print("key:%s value:%s\n", kv->key.data, kv->value.data);
 * }
 * ```
 */
#define RMF_ITERATOR_FOREACH(Type, name, iterator)               \
    for (Type *name = rmf_iterator_next(RMF_ITERATOR(iterator)); \
         name != nullptr;                                        \
         name = rmf_iterator_next(RMF_ITERATOR(iterator)))

G_END_DECLS

#endif
