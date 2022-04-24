/*
 * gnome-tesserae.h - Main program function prototypes
 * Written by Nathan Thompson-Amato <ndt@wagonfixers.com>
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

#ifndef _GNOME_TESSERAE_H_
#define _GNOME_TESSERAE_H_

void       init_new_game (DIFFICULTY difficulty, BOARD_STYLE board_style);
GtkWidget *get_window (void);
gboolean   get_game_in_progress (void);
guint      get_score (void);
guint      get_moves (void);
guint      get_undos (void);
BOARD_STYLE get_mosaic (void);
DIFFICULTY  get_difficulty (void);
void       reset_game_board (void);

#endif
