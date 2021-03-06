#ifndef BLOB_H
#define BLOB_H
// ****************************************************************************
//  blob.h                                          XL - An extensible language
// ****************************************************************************
//
//   File Description:
//
//     Blobs (short for "binary lumped object") are used to store
//     binary data that is not otherwise interpreted by the XL language
//     in any way. The most common example is text. A blob is a sized
//     sequence of bytes in memory, and can contain anything. What the data
//     actually means is left to the handler.
//
//
//
//
// ****************************************************************************
//  (C) 2017 Christophe de Dinechin <christophe@dinechin.org>
//   This software is licensed under the GNU General Public License v3
//   See LICENSE file for details.
// ****************************************************************************

#include "tree.h"
#include <stdint.h>


typedef struct blob
// ----------------------------------------------------------------------------
//   A blob internal representation is little more than a stream of bytes
// ----------------------------------------------------------------------------
//   The bytes are allocated immediately after the blob
{
    tree_t      tree;           // The base tree
    size_t      length;         // Size in bytes of the data that follows
} blob_t;

#ifdef BLOB_C
#define inline extern inline
#endif

tree_type(blob);
inline blob_p   blob_new(srcpos_t position, size_t sz, const char *data);
extern void     blob_append_data(blob_p *blob, size_t sz, const char *data);
inline void     blob_append(blob_p *blob, blob_p other);
extern void     blob_range(blob_p *blob, size_t start, size_t len);
inline char   * blob_data(blob_p blob);
inline size_t   blob_length(blob_p blob);
extern int      blob_compare(blob_p blob1, blob_p blob2);

// Private blob handler, should not be called directly in general
inline blob_p   blob_make(tree_handler_fn h, srcpos_t, size_t, const char *);
extern tree_p   blob_handler(tree_cmd_t cmd, tree_p tree, va_list va);

#undef inline



// ============================================================================
//
//   Inline implementations
//
// ============================================================================

inline blob_p blob_make(tree_handler_fn h, srcpos_t pos,
                        size_t sz, const char *data)
// ----------------------------------------------------------------------------
//   Create a blob with the given parameters
// ----------------------------------------------------------------------------
{
    return (blob_p) tree_make(h, pos, sz, data);
}


inline blob_p blob_new(srcpos_t position, size_t sz, const char *data)
// ----------------------------------------------------------------------------
//    Allocate a blob with the given data
// ----------------------------------------------------------------------------
{
    return blob_make(blob_handler, position, sz, data);
}


inline void blob_append(blob_p *blob, blob_p blob2)
// ----------------------------------------------------------------------------
//   Append one blob to another
// ----------------------------------------------------------------------------
{
    blob_append_data(blob, blob_length(blob2), blob_data(blob2));
}


inline char *blob_data(blob_p blob)
// ----------------------------------------------------------------------------
//   Return the data for the blob
// ----------------------------------------------------------------------------
{
    return (char *) (blob + 1);
}


inline size_t blob_length(blob_p blob)
// ----------------------------------------------------------------------------
//   Return the data for the blob
// ----------------------------------------------------------------------------
{
    return blob->length;
}



// ============================================================================
//
//    Blobs for a given item type
//
// ============================================================================

#define blob_type(item, type)                                           \
                                                                        \
    tree_type(type);                                                    \
                                                                        \
    inline text_p type##_make(tree_handler_fn h, srcpos_t pos,          \
                              size_t sz, const item *data)              \
    {                                                                   \
        sz *= sizeof(item);                                             \
        return (text_p) tree_make(h, pos, sz, data);                    \
    }                                                                   \
                                                                        \
    inline type##_p type##_new(srcpos_t pos,                            \
                               size_t sz, const item *data)             \
    {                                                                   \
        return (type##_p) type##_make(type##_handler, pos,sz, data);    \
    }                                                                   \
                                                                        \
    inline void type##_append(type##_p *type, type##_p type2)           \
    {                                                                   \
        blob_append((blob_p *) type, (blob_p) type2);                   \
    }                                                                   \
                                                                        \
                                                                        \
    inline void type##_append_data(type##_p *type,                      \
                                       size_t sz, const item *data)     \
    {                                                                   \
        sz *= sizeof(item);                                             \
        const char *chardata = (const char *) data;                     \
        blob_append_data((blob_p *) type, sz, chardata);                \
    }                                                                   \
                                                                        \
                                                                        \
    inline void type##_range(type##_p *type,                            \
                             size_t start, size_t len)                  \
    {                                                                   \
        start *= sizeof(item);                                          \
        len *= sizeof(item);                                            \
        blob_range((blob_p *) type, start, len);                        \
    }                                                                   \
                                                                        \
    inline item *type##_data(type##_p type)                             \
    {                                                                   \
        return (item *) blob_data((blob_p) type);                       \
    }                                                                   \
                                                                        \
    inline size_t type##_length(type##_p type)                          \
    {                                                                   \
        return blob_length((blob_p) type) / sizeof(item);               \
    }                                                                   \
                                                                        \
    inline int type##_compare(type##_p t1, type##_p t2)                 \
    {                                                                   \
        return blob_compare((blob_p) t1, (blob_p) t2);                  \
    }                                                                   \
                                                                        \
    inline void type##_push(type##_p *type, item value)                 \
    {                                                                   \
        blob_append_data((blob_p *) type,                               \
                         sizeof(value), (const char *) &value);         \
    }                                                                   \
                                                                        \
    inline item type##_top(type##_p type)                               \
    {                                                                   \
        return type##_data(type)[type##_length(type)-1];                \
    }                                                                   \
                                                                        \
    inline void type##_pop(type##_p *type)                              \
    {                                                                   \
        assert(type##_length(*type) && "Can only pop if non-empty");    \
        size_t new_size = (type##_length(*type) - 1);                   \
        return type##_range(type, 0, new_size);                         \
    }


#define blob_type_handler(type)                                         \
                                                                        \
    tree_p type##_handler(tree_cmd_t cmd,tree_p tree,va_list va)        \
    {                                                                   \
        switch(cmd)                                                     \
        {                                                               \
        case TREE_TYPENAME:                                             \
            return (tree_p) #type;                                      \
                                                                        \
        case TREE_CAST:                                                 \
            if (tree_cast_handler(va) == type##_handler)                \
                return tree;                                            \
            break;                                                      \
        default:                                                        \
            break;                                                      \
        }                                                               \
        return blob_handler(cmd, tree, va);                             \
    }


#endif // BLOB_H
