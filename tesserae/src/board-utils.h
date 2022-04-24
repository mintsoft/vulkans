/*
 * board-utils.h - Gameboard utility routines
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

#ifndef _BOARD_UTILS_H_
#define _BOARD_UTILS_H_

/* in pixels */
#define TILE_WIDTH   30
#define TILE_HEIGHT  30

/* in squares */
#define BOARD_SQUARE_WIDTH  14
#define BOARD_SQUARE_HEIGHT 14

/* in pixels */
#define BOARD_PIXEL_WIDTH  (TILE_WIDTH * BOARD_SQUARE_WIDTH)
#define BOARD_PIXEL_HEIGHT (TILE_HEIGHT * BOARD_SQUARE_HEIGHT)

/* these #defines MUST match the corresponding enums */
#define DIFFICULTY_LEVELS 3
#define BOARD_STYLES 5


/* FIXME: the high-score code depends on DIFFICULTY and
   BOARD_STYLE having certain numeric values.  this is bad
   for maintenance, but it appears to be necessary in order
   to allow things like indexing the named_levels[] array
   with named_levels[BOARD_STYLE][DIFFICULTY]
*/

typedef enum DIFFICULTY { DIFFICULTY_EASY = 0,
                          DIFFICULTY_MEDIUM,
                          DIFFICULTY_HARD,

                          /* the "default" value */
                          DIFFICULTY_UNDEFINED } DIFFICULTY;

/* FIXME: parts of the code depend on BOARD_STYLE and
   BOARD_SQUARE having certain numeric values.  this is bad
   from a maintenance point of view, but good in that
   it lets sets of squares be "and"ed and "or"ed to yield
   other sets. */

typedef enum BOARD_STYLE { BOARD_STYLE_SMALL_RECTANGLE = 0,
                           BOARD_STYLE_MEDIUM_RECTANGLE,
                           BOARD_STYLE_LARGE_RECTANGLE,
                           BOARD_STYLE_SMALL_CIRCLE,
                           BOARD_STYLE_MEDIUM_CIRCLE,

                           /* the "default" value */
                           BOARD_STYLE_UNDEFINED } BOARD_STYLE;

typedef enum BOARD_SQUARE { BOARD_SQUARE_RED    = 1,
                            BOARD_SQUARE_YELLOW = 2,
                            BOARD_SQUARE_BLUE   = 4,
                            BOARD_SQUARE_ORANGE = 3,
                            BOARD_SQUARE_PURPLE = 5,
                            BOARD_SQUARE_GREEN  = 6,
                            BOARD_SQUARE_SILVER = 7,
                            
                            /* EMPTY: valid space but unoccupied */
                            BOARD_SQUARE_EMPTY  = 0,

                            /* UNUSED: invalid space on the board */
                            BOARD_SQUARE_UNUSED = 8 } BOARD_SQUARE;

/* You must keep this character array up to date with both the mosaics
   (BOARD_STYLE) and difficulty levels (DIFFICULTY)
*/
extern const gchar *named_levels[BOARD_STYLES][DIFFICULTY_LEVELS];

void init_board (BOARD_STYLE style);
void generate_squares (DIFFICULTY difficulty);

extern BOARD_SQUARE board[BOARD_SQUARE_WIDTH][BOARD_SQUARE_HEIGHT];

#endif
