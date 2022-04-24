/*
 * undo-utils.c - Move undo utilities
 * Written by Nathan Thompson-Amato <ndt@wagonfixers.com>
 * Based on code by Matt Mullen <matt@mattmullen.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * For more details see the file COPYING.
 */

#include <config.h>
#include <gnome.h>
#include <libgnomeui/gnome-window-icon.h>
#include <gdk/gdkkeysyms.h>

#include "properties.h"
#include "board-utils.h"
#include "undo-utils.h"


typedef struct UNDO_TILE
{
    guint x, y;
    BOARD_SQUARE type;
} UNDO_TILE;

typedef struct UNDO_NODE {
    UNDO_TILE tile[3];
    struct UNDO_NODE *next;
    struct UNDO_NODE *prev;
} UNDO_NODE;

typedef struct UNDO_LIST
{
    UNDO_NODE *first;
    UNDO_NODE *last;
} UNDO_LIST;

UNDO_LIST_REF
new_undo_list (void)
{
    UNDO_LIST_REF undo_list = g_malloc (sizeof (UNDO_LIST));
    undo_list->first = undo_list->last = NULL;

    return undo_list;
}

void
delete_undo_list (UNDO_LIST_REF *undo_list) {
    
    UNDO_NODE *destroyer;
    
    destroyer = (*undo_list)->first;
    
    while (destroyer != NULL) {
        UNDO_NODE *next = destroyer->next;
        g_free (destroyer);
        destroyer = next;
    }

    *undo_list = NULL;
}

/*
 * Append the given move to the end of the undo list
 */

void
save_undo_info (guint src_x, guint src_y,
                guint dest_x, guint dest_y,
                UNDO_LIST_REF undo_list) {

    UNDO_NODE *new_node = g_malloc (sizeof (UNDO_NODE));

    /* location of the intermediate tile */
    /* FIXME: is this being computed correctly? */
    guint i_x = src_x + ((dest_x - src_x) / 2);
    guint i_y = src_y + ((dest_y - src_y) / 2);

    /* source tile */
    new_node->tile[0].x = src_x;
    new_node->tile[0].y = src_y;
    new_node->tile[0].type = board[src_x][src_y];

    /* destination tile */
    new_node->tile[1].x = dest_x;
    new_node->tile[1].y = dest_y;
    new_node->tile[1].type = board[dest_x][dest_y];

    /* intermediate tile */
    new_node->tile[2].x = i_x;
    new_node->tile[2].y = i_y;
    new_node->tile[2].type = board[i_x][i_y];

    /* add the new node to the list */
    if (undo_list->first == NULL) {
        undo_list->first = new_node;
        undo_list->first->next = undo_list->first->prev = NULL;
        undo_list->last = undo_list->first;
    } else {
        undo_list->last->next = new_node;
        new_node->prev = undo_list->last;
        new_node->next = NULL;
        undo_list->last = new_node;
    }
}

/*
 * Undo the move saved at the end of the undo list & remove
 * the move from the list
 */

void
undo (UNDO_LIST_REF undo_list)
{
    /* can't do anything with empty list */
    if (undo_list->last == NULL) {
        return;
    }

    g_print ("undoing squares: 1: (%u, %u) 2: (%u, %u) intermediate: (%u, %u)\n",
             undo_list->last->tile[0].x, undo_list->last->tile[0].y,
             undo_list->last->tile[1].x, undo_list->last->tile[1].y,
             undo_list->last->tile[2].x, undo_list->last->tile[2].y);
    
    /* place old tiles back onto the board */
    board[undo_list->last->tile[0].x][undo_list->last->tile[0].y] =
        undo_list->last->tile[0].type;
    board[undo_list->last->tile[1].x][undo_list->last->tile[1].y] =
        undo_list->last->tile[1].type;
    board[undo_list->last->tile[2].x][undo_list->last->tile[2].y] =
        undo_list->last->tile[2].type;

    /* prepare to remove the last list node */
    if (undo_list->last->prev) {
        undo_list->last->prev->next = NULL;
    }

    /* if we're deleting the only node,
       make sure we mark the first node as NULL too
    */
    if (undo_list->last == undo_list->first) {
        undo_list->first = NULL;
    }
    
    {
        UNDO_NODE *old_last = undo_list->last;
        
        undo_list->last = undo_list->last->prev;
        
        g_free (old_last);
    }
}

/*
 * Return TRUE if the given list is empty; return FALSE otherwise
 */

gboolean
undo_list_is_empty (UNDO_LIST_REF undo_list)
{
    if (undo_list->last == NULL) {
        return TRUE;
    }

    return FALSE;
}
