/*
 * board-utils.h - Game move utility routines
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

#ifndef _MOVE_UTILS_H_
#define _MOVE_UTILS_H_

/* there are eight potential moves from a given location */
#define MOVE_CACHE_SIZE 8

typedef struct MOVE
{
    guint x1, y1; /* start location */
    guint x2, y2; /* end location */
    gboolean valid;
} MOVE;

typedef struct MOVE_CACHE
{
    /* there are eight potential moves from a given location */
    MOVE potential_moves[MOVE_CACHE_SIZE];
} MOVE_CACHE;

gboolean is_valid_move (guint x1, guint y1,
                        guint x2, guint y2,
                        MOVE_CACHE cache);
void     cache_valid_moves (guint x, guint y, MOVE_CACHE *cache);
// void draw_valid_moves (void);
void     make_move (guint x1, guint y1, guint x2, guint y2);
gboolean valid_moves_exist (void);
guint    tiles_remaining (void);

#endif
