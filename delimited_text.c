// ****************************************************************************
//  delimited_text.c                                XL - An extensible language
// ****************************************************************************
//
//   File Description:
//
//
//
//
//
//
//
//
//
//
// ****************************************************************************
//  (C) 2017 Christophe de Dinechin <christophe@dinechin.org>
//   This software is licensed under the GNU General Public License v3
//   See LICENSE file for details.
// ****************************************************************************

#define DELIMITED_TEXT_C
#include "delimited_text.h"
#include "renderer.h"

tree_p delimited_text_handler(tree_cmd_t cmd, tree_p tree, va_list va)
// ----------------------------------------------------------------------------
//   The default type handler for base trees
// ----------------------------------------------------------------------------
{
    size_t           size;
    renderer_p       renderer;
    tree_handler_fn  cast_type;
    delimited_text_p dt;
    text_p           value;
    name_p           opening, closing;

    switch(cmd)
    {
    case TREE_TYPENAME:
        // Return a default tree type name
        return (tree_p) "delimited_text";

    case TREE_SIZE:
        // Return the size of the tree in bytes (may be dynamic for subtypes)
        return (tree_p) sizeof(delimited_text_t);

    case TREE_ARITY:
        // This has three children
        return (tree_p) 3;

    case TREE_CHILDREN:
        // The children are right after the tree_t part
        return (tree_p) (tree + 1);

    case TREE_CAST:
        // Return the tree if we cast to tree
        cast_type = tree_cast_handler(va);
        if (cast_type == delimited_text_handler)
            return tree;
        break;                  // Fall through to regular text handling

    case TREE_INITIALIZE:
        value = va_arg(va, text_p);
        opening = va_arg(va, name_p);
        closing = va_arg(va, name_p);

        dt = (delimited_text_p) tree_malloc(sizeof(delimited_text_t));
        dt->value = text_use(value);
        dt->opening = name_use(opening);
        dt->closing = name_use(closing);

    case TREE_DELETE:
    case TREE_COPY:
    case TREE_CLONE:
        // These cases are handled directly by the tree handler
        return tree_handler(cmd, tree, va);

    case TREE_RENDER:
        // Default rendering simply shows the tree address and handler address
        renderer = va_arg(va, renderer_p);
        dt = (delimited_text_p) tree;
        render(renderer, (tree_p) dt->opening);
        size = text_length(dt->value);
        render_text(renderer, size, text_data(dt->value));
        render(renderer, (tree_p) dt->closing);
        return tree;

    case TREE_FREEZE:
        assert(!"Not yet implemented");
        return tree;
    case TREE_THAW:
        assert(!"Not yet implemented");
        return tree;

    default:
        break;
    }

    // Other cases are dealt with by considering the text part
    dt = (delimited_text_p) tree;
    return text_handler(cmd, (tree_p) dt->value, va);
}
