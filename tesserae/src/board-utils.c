/*
 * board-utils.c - Game board utilities
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

const gchar *named_levels[BOARD_STYLES][DIFFICULTY_LEVELS] =
{
    { "small-rect-easy",   "small-rect-med",   "small-rect-hard" },
    { "med-rect-easy",     "med-rect-med",     "med-rect-hard" }, 
    { "big-rect-easy",     "big-rect-med",     "big-rect-hard" },
    { "small-circle-easy", "small-circle-med", "small-circle-hard" },
    { "med-circle-easy",   "med-circle-med",   "med-circle-hard" }
};

BOARD_SQUARE board[BOARD_SQUARE_WIDTH][BOARD_SQUARE_HEIGHT];

/* FIXME: depends on BOARD_SQUARE_WIDTH being 14.  this is wrong.  */
void
init_board (BOARD_STYLE style)
{

   int i,j;

   for (i = 0; i < BOARD_SQUARE_HEIGHT; i++)
       for (j = 0; j < BOARD_SQUARE_WIDTH; j++)
           board[i][j] = BOARD_SQUARE_UNUSED;

   switch (style) {

       default:  /* if something goes wrong, make it a small rectangle */
           
       case BOARD_STYLE_SMALL_RECTANGLE:
           
           for (i = 2; i < BOARD_SQUARE_HEIGHT - 2; i++) {
               for (j = 3;j < BOARD_SQUARE_WIDTH - 3; j++) {
                   board[i][j] = BOARD_SQUARE_EMPTY;
               }
           }
           break;
           
       case BOARD_STYLE_MEDIUM_RECTANGLE:
           
           for (i = 1; i < BOARD_SQUARE_HEIGHT - 1; i++) {
               for (j = 2; j < BOARD_SQUARE_WIDTH - 2; j++) {
                   board[i][j] = BOARD_SQUARE_EMPTY;
               }
           }
           break;
           
       case BOARD_STYLE_LARGE_RECTANGLE:
           
           for (i = 0; i < BOARD_SQUARE_HEIGHT; i++) {
               for(j = 0; j < BOARD_SQUARE_WIDTH; j++) {
                   board[i][j] = BOARD_SQUARE_EMPTY;
               }
           }
           break;
           
       case BOARD_STYLE_SMALL_CIRCLE:

           /* FIXME: there's way too much weird math &
              magic numerology going on in these circle
              cases
           */
           for (i = 1; i < BOARD_SQUARE_HEIGHT; i++) {
               for (j = abs (5 - i) + 4;
                    j < BOARD_SQUARE_WIDTH - abs(5 - i) - 4; j++)
               {
                   board[i + 1][j] = BOARD_SQUARE_EMPTY;
               }
           }
           break;
           
       case BOARD_STYLE_MEDIUM_CIRCLE:
           for (i = 1; i < BOARD_SQUARE_HEIGHT - 1; i++) {
               for (j = abs (4 - i) + 3;
                    j < BOARD_SQUARE_WIDTH - abs (4 - i) - 3; j++)
               {
                   board[i + 2][j] = BOARD_SQUARE_EMPTY;
               }
           }
           break;
   }

}

void
generate_squares (DIFFICULTY difficulty)
{
   int i, j;
   int mod;
   int simple_weight;

   /* primary tiles only */
   if (difficulty == DIFFICULTY_EASY) mod = 3;
   /* primary & secondary */
   else if (difficulty == DIFFICULTY_MEDIUM) mod = 6;
   /* primary, secondary, and tertiary */
   else mod = 7;

   /* prefer `simple' tiles even when
      we're on a high-difficulty level
   */

   if (difficulty == DIFFICULTY_MEDIUM) {
       simple_weight = 3; /* 2/3 chance of getting a primary tile
                             even when a secondary would
                             normally show up
                          */
   } else if (difficulty == DIFFICULTY_HARD) {
       simple_weight = 2; /* 1/2 chance of getting a primary instead
                             of a secondary or tertiary
                          */
   } else {
       simple_weight = 1; /* on easy level, we don't need to make
                             any adjustments
                          */
   }

   for (i = 0; i < BOARD_SQUARE_WIDTH; i++) {
       for (j = 0; j < BOARD_SQUARE_HEIGHT; j++) {
           if (board[i][j] != BOARD_SQUARE_UNUSED) {

               int saved_mod = mod;
               
               if ((random () % simple_weight) != 0) {
                   mod = 3;
               }
               
               switch (random () % mod) {
                   case 0:  board[i][j] = BOARD_SQUARE_RED;     break;
                   case 1:  board[i][j] = BOARD_SQUARE_YELLOW;  break;
                   case 2:  board[i][j] = BOARD_SQUARE_BLUE;    break;
                   case 3:  board[i][j] = BOARD_SQUARE_ORANGE;  break;
                   case 4:  board[i][j] = BOARD_SQUARE_PURPLE;  break;
                   case 5:  board[i][j] = BOARD_SQUARE_GREEN;   break;
                   case 6:  board[i][j] = BOARD_SQUARE_SILVER;  break;
                       
                   default:
                       /* this shouldn't ever happen */
                       board[i][j] = BOARD_SQUARE_EMPTY;
                       break;
               }
               
               mod = saved_mod;
           }
       }
   }
}
