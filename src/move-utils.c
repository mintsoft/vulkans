/*
 * move-utils.c - Game move utilities
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
#include "move-utils.h"

static gboolean primary_color (BOARD_SQUARE color);
static gboolean contains_color (BOARD_SQUARE color1,
                                BOARD_SQUARE color2);
static gboolean is_subset_color (BOARD_SQUARE color1,
                                 BOARD_SQUARE color);
static void mark_if_valid (guint x1, guint y1, guint x2, guint y2,
                           guint cache_index,
                           MOVE_CACHE *cache);
static BOARD_SQUARE remove_common_colors (BOARD_SQUARE color1,
                                          BOARD_SQUARE color2);
static BOARD_SQUARE combine_colors (BOARD_SQUARE color1,
                                    BOARD_SQUARE color2);

/*
 * Returns TRUE if the move from (x1, y1) to (x2, y2) is a valid
 * move accordint to the move cache.  Assumes the move cache is
 * up to date
 */
gboolean
is_valid_move (guint x1, guint y1,
               guint x2, guint y2,
               MOVE_CACHE cache)
{
    guint i;

    for (i = 0; i < MOVE_CACHE_SIZE; i++) {

        if (cache.potential_moves[i].valid == TRUE) { /* valid move */

            /*
            g_print ("is_valid_move found a valid move (%i: %i,%i to %i,%i)\n",
                     i,
                     cache.potential_moves[i].x1,
                     cache.potential_moves[i].y1,
                     cache.potential_moves[i].x2,
                     cache.potential_moves[i].y2);
            */
            
            /* is this valid move the requested move? */
            if (cache.potential_moves[i].x1 == x1 &&
                cache.potential_moves[i].y1 == y1 &&
                cache.potential_moves[i].x2 == x2 &&
                cache.potential_moves[i].y2 == y2)
            {
                return TRUE;
            }
            /*
            g_print ("rejected.\n");
            */
        }
    }

    return FALSE;
}

/*
 * Caches valid moves from the start location (x, y)
 * into the given move cache.
 */
void
cache_valid_moves (guint x, guint y, MOVE_CACHE *cache)
{
    guint i;

    for (i = 0; i < MOVE_CACHE_SIZE; i++)
        cache->potential_moves[i].valid = FALSE;

    /* look through all tiles two squares away in any direction
       (up, down, left, right, diagonal)
    */
    /* FIXME:
       should be done this way:
    for (k = 0, i = -2; i <= 2; i+=2) {
        for (j = -2; j <= 2; j+=2, k++) {
    */
    
    mark_if_valid (x, y, x + 2, y + 2, 0, cache);
    mark_if_valid (x, y, x + 2, y + 0, 1, cache);
    mark_if_valid (x, y, x + 2, y - 2, 2, cache);
    mark_if_valid (x, y, x - 2, y + 2, 3, cache);
    mark_if_valid (x, y, x - 2, y + 0, 4, cache);
    mark_if_valid (x, y, x - 2, y - 2, 5, cache);
    mark_if_valid (x, y, x + 0, y + 2, 6, cache);
    mark_if_valid (x, y, x + 0, y - 2, 7, cache);
}

/*
 * Marks the given cache_index as valid
 * and assigns it the given move if the move is
 * valid
 */
static void
mark_if_valid (guint x1, guint y1,
               guint x2, guint y2,
               guint cache_index,
               MOVE_CACHE *cache)
{
    /* the intermediate tile */
    guint int_x;
    guint int_y;

    if (x1 > x2) int_x = x1 - 1;
    else if (x1 < x2) int_x = x1 + 1;
    else int_x = x1;

    if (y1 > y2) int_y = y1 - 1;
    else if (y1 < y2) int_y = y1 + 1;
    else int_y = y1;

    /*
    g_print ("examining move %i: (%i, %i) to (%i, %i) via (%i, %i)\n",
             cache_index, x1, y1, x2, y2, int_x, int_y);
    */
            
    /* are we on the board? */
    if (x2 < 0 || x2 >= BOARD_SQUARE_WIDTH) return;
    if (y2 < 0 || y2 >= BOARD_SQUARE_HEIGHT) return;

    /* is the destination a used square? */
    if (board[x2][y2] == BOARD_SQUARE_UNUSED) return;

    /* are the tiles non-identical? */
    if (board[x2][y2] != board[x1][y1]) {
        /* if so, does the destination already contain the
           source color(s)? */
                
        if (contains_color (board[x2][y2], board[x1][y1])) {
            return;
        }
    }
            
    /* check the tile between source and destination */
    if (board[int_x][int_y] == BOARD_SQUARE_EMPTY) {
        return; /* empty tiles can never be jumped over */
    }

    /* we can only jump a tile if...
       (a) it contains some of our colors and *only* our colors, or
       (b) it is a primary tile and we have a primary tile
    */
    if (!(primary_color (board[int_x][int_y]) &&
          primary_color (board[x1][y1])))
    {
        if (!(is_subset_color (board[x1][y1], board[int_x][int_y]))) {
            return;
        }
    }

    /* we're allowing the move */
    cache->potential_moves[cache_index].valid = TRUE;
    cache->potential_moves[cache_index].x1 = x1;
    cache->potential_moves[cache_index].y1 = y1;
    cache->potential_moves[cache_index].x2 = x2;
    cache->potential_moves[cache_index].y2 = y2;
}

/*
 * Moves the given (x1, y1) tile to the (x2, y2) destination,
 * combining & subtracting colors as appropriate.  Doesn't actually
 * draw the modified board to the screen.
 */

void
make_move (guint x1, guint y1, guint x2, guint y2)
{
    /* intermediate tile */
    guint int_x;
    guint int_y;

    if (x1 > x2) int_x = x1 - 1;
    else if (x1 < x2) int_x = x1 + 1;
    else int_x = x1;

    if (y1 > y2) int_y = y1 - 1;
    else if (y1 < y2) int_y = y1 + 1;
    else int_y = y1;
    
    /* if intermediate is primary, delete it, else strip src from it */
    if (primary_color (board[int_x][int_y])) {
        board[int_x][int_y] = BOARD_SQUARE_EMPTY;
    } else {
        board[int_x][int_y] = remove_common_colors (board[x1][y1], board[int_x][int_y]);
    }

    /* combine the src & dest squares */
    board[x2][y2] = combine_colors (board[x1][y1], board[x2][y2]);

    /* clear the source square */
    board[x1][y1] = BOARD_SQUARE_EMPTY;

    /* FIXME: do some special effect like this */
    /*
    play_sample(tile_click, vol, 127, 1000, FALSE);
    rest(10);
    play_sample(tile_click, vol, 127, 1000, FALSE);
    */

    return;
}

/*
 * Returns TRUE if any valid moves exist; FALSE otherwise.
 */

gboolean
valid_moves_exist (void)
{
   int i, j, k;

   for (i = 0; i < BOARD_SQUARE_HEIGHT; i++) {
       for (j = 0; j < BOARD_SQUARE_WIDTH; j++) {

           MOVE_CACHE cache;
           
           if (board[j][i] == BOARD_SQUARE_EMPTY ||
               board[j][i] == BOARD_SQUARE_UNUSED) continue;

           cache_valid_moves(j, i, &cache);
           
           for (k = 0; k < MOVE_CACHE_SIZE; k++) {
               if (cache.potential_moves[k].valid) {
                   /*
                   g_print ("found a valid move: %i, %i to %i, %i\n",
                            cache.potential_moves[k].x1,
                            cache.potential_moves[k].y1,
                            cache.potential_moves[k].x2,
                            cache.potential_moves[k].y2);
                   */
                   return TRUE;
               }
           }
       }
   }
   return FALSE;
}

/*
 * Returns the number of tiles still on the board
 */
guint
tiles_remaining (void)
{
   guint i, j;
   guint k = 0;

   for (i = 0; i < BOARD_SQUARE_HEIGHT; i++) {
       for (j = 0; j < BOARD_SQUARE_WIDTH; j++) {
           if (board[j][i] == BOARD_SQUARE_EMPTY ||
               board[j][i] == BOARD_SQUARE_UNUSED) continue;
           k++;
      }
   }

   return k;
}

/*
 * Returns true if color1 is a subset of color2
 */
static gboolean
is_subset_color (BOARD_SQUARE color1,
                 BOARD_SQUARE color2)
{
    if ((color1 & color2) == color1) {
        return TRUE;
    }

    return FALSE;
}

/*
 * Checks to see if the two colors share any component colors
 */
static gboolean
contains_color (BOARD_SQUARE color1,
                BOARD_SQUARE color2)
{
    if ((color1 & color2) != 0) return TRUE;

    return FALSE;
}

/*
 * Returns TRUE if the color is primary, returns
 * FALSE otherwise.
 */
static gboolean
primary_color (BOARD_SQUARE color)
{
    switch (color) {
        case BOARD_SQUARE_YELLOW:
        case BOARD_SQUARE_BLUE:
        case BOARD_SQUARE_RED:

            return TRUE;

        default:
            /* nope, not primary */
    }

    return FALSE;
}

/*
 * Returns the result of removing any common colors from
 * color1 and color2
 */
static BOARD_SQUARE
remove_common_colors (BOARD_SQUARE color1,
                      BOARD_SQUARE color2)
{
    return ((BOARD_SQUARE_SILVER - color1) & color2);
}

/*
 * Combines color1 and color2 into a third color that has
 * all the component colors of 1 and 2.
 */
static BOARD_SQUARE
combine_colors (BOARD_SQUARE color1,
                BOARD_SQUARE color2)
{
    return color1 | color2;
}
