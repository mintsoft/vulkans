/*
 * undo-utils.h - Move undo utility routines & data types
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

#ifndef _UNDO_UTILS_H_
#define _UNDO_UTILS_H_

typedef struct UNDO_LIST * UNDO_LIST_REF;

UNDO_LIST_REF new_undo_list (void);
void          delete_undo_list (UNDO_LIST_REF *undo_list);
void          save_undo_info (guint src_x, guint src_y,
                              guint dest_x, guint dest_y,
                              UNDO_LIST_REF undo_list);
void          undo (UNDO_LIST_REF undo_list);
gboolean      undo_list_is_empty (UNDO_LIST_REF undo_list);

#endif
