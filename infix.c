// ****************************************************************************
//  infix.c                                          XL - An extensible language
// ****************************************************************************
//
//   File Description:
//
//     Representation of infix nodes, like A+B or A and B
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

#include "infix.h"
#include <stdlib.h>
#include <strings.h>


tree_p infix_handler(tree_cmd_t cmd, tree_p tree, va_list va)
// ----------------------------------------------------------------------------
//   The common handler for prefix and postfix
// ----------------------------------------------------------------------------
{
    infix_p    infix = (infix_p) tree;
    tree_io_fn io;
    void *     stream;
    tree_p     left, right;
    text_p     opcode;

    switch(cmd)
    {
    case TREE_TYPENAME:
        // Return a default tree type name
        return (tree_p) "infix";

    case TREE_SIZE:
        // Return the size of the tree in bytes (is dynamic for infixs)
        return (tree_p) (sizeof(infix_t));

    case TREE_ARITY:
        // Prefix and postfix have three children
        return (tree_p) 3;

    case TREE_CHILDREN:
        // Pointer to the children is right after the tree 'header
        return tree + 1;

    case TREE_INITIALIZE:
        // Fetch pointer to data and size from varargs list (see infix_new)
        opcode = va_arg(va, text_p);
        left = va_arg(va, tree_p);
        right = va_arg(va, tree_p);

        // Create infix and copy data in it
        infix = (infix_p) malloc(sizeof(infix_t));
        infix->left = tree_refptr(left);
        infix->right = tree_refptr(right);
        infix->opcode = (text_p) tree_refptr((tree_p) opcode);
        return (tree_p) infix;

    case TREE_RENDER:
        // Render left then right
        io = va_arg(va, tree_io_fn);
        stream = va_arg(va, void *);
        tree_render(infix->left, io, stream);
        tree_render((tree_p) infix->opcode, io, stream);
        tree_render(infix->right, io, stream);
        return tree;

    default:
        // Other cases are handled correctly by the tree handler
        return tree_handler(cmd, tree, va);
    }
}