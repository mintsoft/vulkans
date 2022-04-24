/*
 * properties.c - `Properties'-handling code
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

#include <config.h>
#include <gnome.h>

#include "properties.h"

static void reset_properties (void);
static void animate_select (GtkWidget *widget, gpointer data);
static void sound_select (GtkWidget *widget, gpointer data);
static void save_properties (void);

static GtkWidget *properties_window = NULL;

typedef struct PROPERTIES_WINDOW_STATE
{
    gboolean animate;
    gboolean sound;
    
} PROPERTIES_WINDOW_STATE;

static PROPERTIES_WINDOW_STATE properties_window_state;

/* is the dialog visible? */
static gboolean mapped = FALSE;

void
load_properties (void)
{
    properties_window_state.animate =
        gnome_config_get_bool_with_default (
            "/gnome-tesserae/Preferences/animate=TRUE", NULL);

    properties_window_state.sound =
        gnome_config_get_bool_with_default (
            "/gnome-tesserae/Preferences/sound=TRUE", NULL);

    /* FIXME: should have options for tile set to use */
}

static void
reset_properties (void)
{
    /* FIXME: is reloading the properties equivalent to resetting? */
    load_properties ();
}

static void
animate_select (GtkWidget *widget, gpointer data)
{
	if (GTK_TOGGLE_BUTTON (widget)->active)
		properties_window_state.animate = TRUE;
	else
        properties_window_state.animate = FALSE;
    
	gnome_property_box_changed (GNOME_PROPERTY_BOX (properties_window));
}

static void
sound_select (GtkWidget *widget, gpointer data)
{
	if (GTK_TOGGLE_BUTTON (widget)->active)
		properties_window_state.sound = TRUE;
	else
        properties_window_state.sound = FALSE;
    
	gnome_property_box_changed (GNOME_PROPERTY_BOX (properties_window));
}

static void
save_properties (void)
{
    gnome_config_set_bool (
        "/gnome-tesserae/Preferences/animate",
        properties_window_state.animate);

    gnome_config_set_bool (
        "/gnome-tesserae/Preferences/sound",
        properties_window_state.sound);

	gnome_config_sync ();
}

static void
apply_cb (GtkWidget *widget, gpointer data)
{
	save_properties ();
}

static void
destroy_cb (GtkWidget *widget, gpointer data)
{
	mapped = FALSE;
}

static void
dialog_help_callback (GnomePropertyBox *box, gint page_num)
{
    GnomeHelpMenuEntry effects_entry = { "gnome-tesserae", "index.html#preferences" };

    switch (page_num) {
        case 0:
            gnome_help_display (0, &effects_entry);
            break;
        default:
            break;
    }
}

/*
 * Displays the `Properties' dialog, constructing
 * it first if necessary
 */

void
show_properties_dialog (GtkWidget *parent)
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *button;

    /* if the properties window already exists, do nothing */
    /* FIXME: why don't we need to show the window? */
	if (properties_window)
		return;
	
	reset_properties ();
	
	properties_window = gnome_property_box_new ();
    gtk_window_set_title (GTK_WINDOW (properties_window), _("Preferences"));
	gnome_dialog_set_parent (GNOME_DIALOG (properties_window), GTK_WINDOW (parent));
	gtk_signal_connect (GTK_OBJECT (properties_window),
                        "destroy", GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                        &properties_window);
    
    /* begin effects settings */
	label = gtk_label_new (_("Effects"));
	gtk_widget_show (label);
	
	table = gtk_table_new (2, 2, FALSE);
	gtk_container_border_width (GTK_CONTAINER (table), GNOME_PAD);
	gtk_table_set_row_spacings (GTK_TABLE (table), GNOME_PAD);
	gtk_table_set_col_spacings (GTK_TABLE (table), GNOME_PAD);
	gtk_widget_show (table);

    /* begin animation checkbox */
    
	button = gtk_check_button_new_with_label (_("Animate moves"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button),
                                  properties_window_state.animate);
	gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC
                        (animate_select), NULL);

    /* FIXME: add animation so we can make this checkbox sensitive again */
    gtk_widget_set_sensitive (button, FALSE);
	gtk_widget_show (button);
	gtk_table_attach (GTK_TABLE (table), button, 0, 1, 0, 1,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    /* end animation checkbox */

    /* begin sound checkbox */
	button = gtk_check_button_new_with_label (_("Sound effects"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button),
                                  properties_window_state.sound);
	gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC
                        (sound_select), NULL);
	gtk_widget_show (button);
	gtk_table_attach (GTK_TABLE (table), button, 0, 1, 1, 2,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    /* end sound checkbox */
    
	gnome_property_box_append_page (GNOME_PROPERTY_BOX (properties_window),
                                    table,
                                    label);

	gtk_signal_connect (GTK_OBJECT (properties_window), "apply", GTK_SIGNAL_FUNC
                        (apply_cb), NULL);

	gtk_signal_connect (GTK_OBJECT (properties_window), "destroy", GTK_SIGNAL_FUNC
			(destroy_cb), NULL);

    gtk_signal_connect (GTK_OBJECT (properties_window), "help",
                        GTK_SIGNAL_FUNC (dialog_help_callback), NULL);
  
	gtk_widget_show (properties_window);
	mapped = TRUE;
}

/*
 * Getters and setters
 */
    
gboolean
properties_get_animate (void)
{
    return properties_window_state.animate;
}

gboolean
properties_get_sound (void)
{
    return properties_window_state.sound;
}

