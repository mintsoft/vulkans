/*
 * properties.h - Prototypes for `properties' functions.
 * Written by Nathan Thompson-Amato <ndt@wagonfixers.com>
 * Based on code by Ian Peters <itp@gnu.org>
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

#ifndef _PROPERTIES_H_
#define _PROPERTIES_H_

void load_properties (void);
void show_properties_dialog (GtkWidget *parent);
gboolean properties_get_animate (void);
gboolean properties_get_sound (void);

#endif
