/*
 * new-game.c - `New-game'-handling code
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

#include <config.h>
#include <gnome.h>
#include <libgnomeui/gnome-window-icon.h>

#include "board-utils.h"
#include "gnome-tesserae.h"
#include "new-game.h"

/* make sure these match the GUI settings */
static DIFFICULTY selected_difficulty = DIFFICULTY_EASY;
static BOARD_STYLE selected_board_style = BOARD_STYLE_SMALL_RECTANGLE;

static void new_game_preferences (void);
static void new_game_maybe (GtkWidget *widget, gint button);
static void new_game_preferences_maybe (GtkWidget *widget, gint button);
static GtkWidget* create_dialog (void);

static void easy_radiobutton_toggle (GtkWidget *widget, gpointer data);
static void medium_radiobutton_toggle (GtkWidget *widget, gpointer data);
static void hard_radiobutton_toggle (GtkWidget *widget, gpointer data);
static void small_rect_radiobutton_toggle (
    GtkWidget *widget, gpointer data);
static void medium_rect_radiobutton_toggle (
    GtkWidget *widget, gpointer data);
static void large_rect_radiobutton_toggle (
    GtkWidget *widget, gpointer data);
static void medium_circle_radiobutton_toggle (
    GtkWidget *widget, gpointer data);
static void small_circle_radiobutton_toggle (
    GtkWidget *widget, gpointer data);

void
new_game_cb (GtkWidget *widget, gpointer data)
{
	GtkWidget *box;
    gboolean game_in_progress = get_game_in_progress ();
    GtkWidget *window = get_window ();

	if (game_in_progress) {
		box = gnome_message_box_new (_("End this game?"),
                                     GNOME_MESSAGE_BOX_QUESTION,
                                     GNOME_STOCK_BUTTON_YES,
                                     GNOME_STOCK_BUTTON_NO, NULL);
		gnome_dialog_set_parent (GNOME_DIALOG (box), GTK_WINDOW (window));
		gnome_dialog_set_default (GNOME_DIALOG (box), 1);
		gtk_window_set_modal (GTK_WINDOW (box), TRUE);
		gtk_signal_connect (GTK_OBJECT (box), "clicked",
                            (GtkSignalFunc)new_game_maybe, NULL);
		gtk_widget_show (box);
	} else {
		new_game_maybe (NULL, 0);
	}
}

static void
new_game_preferences (void)
{
	GtkWidget *dialog = create_dialog ();
    gnome_dialog_set_parent (GNOME_DIALOG (dialog), GTK_WINDOW (get_window ()));
    gnome_dialog_set_default (GNOME_DIALOG (dialog), 1);
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
    gnome_dialog_set_close (GNOME_DIALOG (dialog), TRUE);
    gtk_signal_connect (GTK_OBJECT (dialog), "clicked",
                        (GtkSignalFunc)new_game_preferences_maybe, NULL);
    gtk_widget_show (dialog);
}

static void
new_game_maybe (GtkWidget *widget, gint button)
{
	if (button == 0) {
        new_game_preferences ();
	}
}

static void
new_game_preferences_maybe (GtkWidget *widget, gint button)
{
	if (button == 0) {
        init_new_game (selected_difficulty, selected_board_style);
        /* reset to match GUI settings */
        selected_difficulty = DIFFICULTY_EASY;
        selected_board_style = BOARD_STYLE_SMALL_RECTANGLE;
	}
}

static void
easy_radiobutton_toggle (GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
        selected_difficulty = DIFFICULTY_EASY;
    }
}

static void
medium_radiobutton_toggle (GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
        selected_difficulty = DIFFICULTY_MEDIUM;
    }
}

static void
hard_radiobutton_toggle (GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
        selected_difficulty = DIFFICULTY_HARD;
    }
}

static void
small_rect_radiobutton_toggle (GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
        selected_board_style = BOARD_STYLE_SMALL_RECTANGLE;
    }
}

static void
medium_rect_radiobutton_toggle (GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
        selected_board_style = BOARD_STYLE_MEDIUM_RECTANGLE;
    }
}

static void
large_rect_radiobutton_toggle (GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
        selected_board_style = BOARD_STYLE_LARGE_RECTANGLE;
    }
}       

static void
small_circle_radiobutton_toggle (GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
        selected_board_style = BOARD_STYLE_SMALL_CIRCLE;
    }
}       

static void
medium_circle_radiobutton_toggle (GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
        selected_board_style = BOARD_STYLE_MEDIUM_CIRCLE;
    }
}

/*
 * Creates a new `new game' dialog and returns a pointer to it.
 *
 * Note that most of this function is automatically generated with
 * Glade; if you have major changes to make to the dialog, I recommend
 * redoing the dialog in Glade, having Glade generate source code,
 * and then re-adding the radiobutton_toggle functions by hand.
 */
static GtkWidget*
create_dialog (void)
{
  GtkWidget *dialog;
  GtkWidget *dialog_vbox;
  GtkWidget *preference_vbox;
  GtkWidget *difficulty_frame;
  GtkWidget *difficulty_vbox;
  GSList *difficulty_vbox_group = NULL;
  guint easy_radiobutton_key;
  GtkWidget *easy_radiobutton;
  guint medium_radiobutton_key;
  GtkWidget *medium_radiobutton;
  guint hard_radiobutton_key;
  GtkWidget *hard_radiobutton;
  GtkWidget *board_style_frame;
  GtkWidget *vbox3;
  GSList *vbox3_group = NULL;
  guint small_rect_radiobutton_key;
  GtkWidget *small_rect_radiobutton;
  guint medium_rect_radiobutton_key;
  GtkWidget *medium_rect_radiobutton;
  guint large_rect_radiobutton_key;
  GtkWidget *large_rect_radiobutton;
  GtkWidget *hseparator;
  guint small_circle_radiobutton_key;
  GtkWidget *small_circle_radiobutton;
  guint medium_circle_radiobutton_key;
  GtkWidget *medium_circle_radiobutton;
  GtkWidget *button_action_area;
  GtkWidget *ok_button;
  GtkWidget *cancel_button;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  dialog = gnome_dialog_new (_("New Game"), NULL);
  gtk_object_set_data (GTK_OBJECT (dialog), "dialog", dialog);
  GTK_WINDOW (dialog)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_policy (GTK_WINDOW (dialog), FALSE, FALSE, FALSE);

  dialog_vbox = GNOME_DIALOG (dialog)->vbox;
  gtk_object_set_data (GTK_OBJECT (dialog), "dialog_vbox", dialog_vbox);
  gtk_widget_show (dialog_vbox);

  preference_vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (preference_vbox);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "preference_vbox", preference_vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (preference_vbox);
  gtk_box_pack_start (GTK_BOX (dialog_vbox), preference_vbox, TRUE, TRUE, 0);

  difficulty_frame = gtk_frame_new (_("Difficulty"));
  gtk_widget_ref (difficulty_frame);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "difficulty_frame", difficulty_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (difficulty_frame);
  gtk_box_pack_start (GTK_BOX (preference_vbox), difficulty_frame, TRUE, TRUE, 0);

  difficulty_vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (difficulty_vbox);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "difficulty_vbox", difficulty_vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (difficulty_vbox);
  gtk_container_add (GTK_CONTAINER (difficulty_frame), difficulty_vbox);

  easy_radiobutton = gtk_radio_button_new_with_label (difficulty_vbox_group, "");
  easy_radiobutton_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (easy_radiobutton)->child),
                                   _("_Easy"));
  gtk_widget_add_accelerator (easy_radiobutton, "clicked", accel_group,
                              easy_radiobutton_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  difficulty_vbox_group = gtk_radio_button_group (GTK_RADIO_BUTTON (easy_radiobutton));
  gtk_widget_ref (easy_radiobutton);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "easy_radiobutton", easy_radiobutton,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect (GTK_OBJECT (easy_radiobutton), "toggled",
                      GTK_SIGNAL_FUNC (easy_radiobutton_toggle), NULL);
  gtk_widget_show (easy_radiobutton);
  gtk_box_pack_start (GTK_BOX (difficulty_vbox), easy_radiobutton, FALSE, FALSE, 0);

  medium_radiobutton = gtk_radio_button_new_with_label (difficulty_vbox_group, "");
  medium_radiobutton_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (medium_radiobutton)->child),
                                   _("_Medium"));
  gtk_widget_add_accelerator (medium_radiobutton, "clicked", accel_group,
                              medium_radiobutton_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  difficulty_vbox_group = gtk_radio_button_group (GTK_RADIO_BUTTON (medium_radiobutton));
  gtk_widget_ref (medium_radiobutton);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "medium_radiobutton", medium_radiobutton,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect (GTK_OBJECT (medium_radiobutton), "toggled",
                      GTK_SIGNAL_FUNC (medium_radiobutton_toggle), NULL);
  gtk_widget_show (medium_radiobutton);
  gtk_box_pack_start (GTK_BOX (difficulty_vbox), medium_radiobutton, FALSE, FALSE, 0);

  hard_radiobutton = gtk_radio_button_new_with_label (difficulty_vbox_group, "");
  hard_radiobutton_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (hard_radiobutton)->child),
                                   _("_Hard"));
  gtk_widget_add_accelerator (hard_radiobutton, "clicked", accel_group,
                              hard_radiobutton_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  difficulty_vbox_group = gtk_radio_button_group (GTK_RADIO_BUTTON (hard_radiobutton));
  gtk_widget_ref (hard_radiobutton);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "hard_radiobutton", hard_radiobutton,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect (GTK_OBJECT (hard_radiobutton), "toggled",
                      GTK_SIGNAL_FUNC (hard_radiobutton_toggle), NULL);
  gtk_widget_show (hard_radiobutton);
  gtk_box_pack_start (GTK_BOX (difficulty_vbox), hard_radiobutton, FALSE, FALSE, 0);

  board_style_frame = gtk_frame_new (_("Mosaic Style"));
  gtk_widget_ref (board_style_frame);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "board_style_frame", board_style_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (board_style_frame);
  gtk_box_pack_start (GTK_BOX (preference_vbox), board_style_frame, TRUE, TRUE, 0);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox3);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "vbox3", vbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (board_style_frame), vbox3);

  small_rect_radiobutton = gtk_radio_button_new_with_label (vbox3_group, "");
  small_rect_radiobutton_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (small_rect_radiobutton)->child),
                                   _("Small _Rectangle"));
  gtk_widget_add_accelerator (small_rect_radiobutton, "clicked", accel_group,
                              small_rect_radiobutton_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  vbox3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (small_rect_radiobutton));
  gtk_widget_ref (small_rect_radiobutton);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "small_rect_radiobutton", small_rect_radiobutton,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect (GTK_OBJECT (small_rect_radiobutton), "toggled",
                      GTK_SIGNAL_FUNC (small_rect_radiobutton_toggle), NULL);
  gtk_widget_show (small_rect_radiobutton);
  gtk_box_pack_start (GTK_BOX (vbox3), small_rect_radiobutton, FALSE, FALSE, 0);

  medium_rect_radiobutton = gtk_radio_button_new_with_label (vbox3_group, "");
  medium_rect_radiobutton_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (medium_rect_radiobutton)->child),
                                   _("Medium Rec_tangle"));
  gtk_widget_add_accelerator (medium_rect_radiobutton, "clicked", accel_group,
                              medium_rect_radiobutton_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  vbox3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (medium_rect_radiobutton));
  gtk_widget_ref (medium_rect_radiobutton);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "medium_rect_radiobutton", medium_rect_radiobutton,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect (GTK_OBJECT (medium_rect_radiobutton), "toggled",
                      GTK_SIGNAL_FUNC (medium_rect_radiobutton_toggle), NULL);
  gtk_widget_show (medium_rect_radiobutton);
  gtk_box_pack_start (GTK_BOX (vbox3), medium_rect_radiobutton, FALSE, FALSE, 0);

  large_rect_radiobutton = gtk_radio_button_new_with_label (vbox3_group, "");
  large_rect_radiobutton_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (large_rect_radiobutton)->child),
                                   _("Large Rectan_gle"));
  gtk_widget_add_accelerator (large_rect_radiobutton, "clicked", accel_group,
                              large_rect_radiobutton_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  vbox3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (large_rect_radiobutton));
  gtk_widget_ref (large_rect_radiobutton);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "large_rect_radiobutton", large_rect_radiobutton,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect (GTK_OBJECT (large_rect_radiobutton), "toggled",
                      GTK_SIGNAL_FUNC (large_rect_radiobutton_toggle), NULL);
  gtk_widget_show (large_rect_radiobutton);
  gtk_box_pack_start (GTK_BOX (vbox3), large_rect_radiobutton, FALSE, FALSE, 0);

  hseparator = gtk_hseparator_new ();
  gtk_widget_ref (hseparator);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "hseparator", hseparator,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator);
  gtk_box_pack_start (GTK_BOX (vbox3), hseparator, TRUE, TRUE, 0);

  small_circle_radiobutton = gtk_radio_button_new_with_label (vbox3_group, "");
  small_circle_radiobutton_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (small_circle_radiobutton)->child),
                                   _("Small _Circle"));
  gtk_widget_add_accelerator (small_circle_radiobutton, "clicked", accel_group,
                              small_circle_radiobutton_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  vbox3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (small_circle_radiobutton));
  gtk_widget_ref (small_circle_radiobutton);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "small_circle_radiobutton", small_circle_radiobutton,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect (GTK_OBJECT (small_circle_radiobutton), "toggled",
                      GTK_SIGNAL_FUNC (small_circle_radiobutton_toggle), NULL);
  gtk_widget_show (small_circle_radiobutton);
  gtk_box_pack_start (GTK_BOX (vbox3), small_circle_radiobutton, FALSE, FALSE, 0);

  medium_circle_radiobutton = gtk_radio_button_new_with_label (vbox3_group, "");
  medium_circle_radiobutton_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (medium_circle_radiobutton)->child),
                                   _("Medium Circ_le"));
  gtk_widget_add_accelerator (medium_circle_radiobutton, "clicked", accel_group,
                              medium_circle_radiobutton_key, GDK_MOD1_MASK, (GtkAccelFlags) 0);
  vbox3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (medium_circle_radiobutton));
  gtk_widget_ref (medium_circle_radiobutton);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "medium_circle_radiobutton",
                            medium_circle_radiobutton,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect (GTK_OBJECT (medium_circle_radiobutton), "toggled",
                      GTK_SIGNAL_FUNC (medium_circle_radiobutton_toggle), NULL);
  gtk_widget_show (medium_circle_radiobutton);
  gtk_box_pack_start (GTK_BOX (vbox3), medium_circle_radiobutton, FALSE, FALSE, 0);

  button_action_area = GNOME_DIALOG (dialog)->action_area;
  gtk_object_set_data (GTK_OBJECT (dialog), "button_action_area", button_action_area);
  gtk_widget_show (button_action_area);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (button_action_area), GTK_BUTTONBOX_SPREAD);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (button_action_area), 8);

  gnome_dialog_append_button (GNOME_DIALOG (dialog), GNOME_STOCK_BUTTON_OK);
  ok_button = GTK_WIDGET (g_list_last (GNOME_DIALOG (dialog)->buttons)->data);
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "ok_button", ok_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

  gnome_dialog_append_button (GNOME_DIALOG (dialog), GNOME_STOCK_BUTTON_CANCEL);
  cancel_button = GTK_WIDGET (g_list_last (GNOME_DIALOG (dialog)->buttons)->data);
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dialog), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  gtk_window_add_accel_group (GTK_WINDOW (dialog), accel_group);

  return dialog;
}
