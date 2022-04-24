/*
 * score.c - Some high-score list code
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
#include "move-utils.h"
#include "new-game.h"
#include "score.h"
#include "custom-highscore-dialog.h"

static GtkWidget* create_endgame_dialog (gboolean win);
static void end_game_highscore_record_and_display (GtkWidget *widget, gint button);
static void end_game_highscore_display (GtkWidget *widget, gint button);
static void end_game_decision (GtkWidget *widget, gint button);

/*
 * Shows the game's high score list
 */

void
scores_cb (GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    
    dialog = create_highscore_dialog (BOARD_STYLE_UNDEFINED);
    gnome_dialog_set_parent (GNOME_DIALOG (dialog), GTK_WINDOW (get_window ()));
    gnome_dialog_set_default (GNOME_DIALOG (dialog), 1);
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
    gnome_dialog_set_close (GNOME_DIALOG (dialog), TRUE);
    gtk_widget_show (dialog);
}

/*
 * If `win' is TRUE, congratulate user; else give her the bad news.
 */

void
end_game_scoring (gboolean win)
{
    GtkWidget *dialog;
    
    dialog = create_endgame_dialog (win);
    gnome_dialog_set_parent (GNOME_DIALOG (dialog), GTK_WINDOW (get_window ()));
    gnome_dialog_set_default (GNOME_DIALOG (dialog), 1);
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
    gnome_dialog_set_close (GNOME_DIALOG (dialog), TRUE);
    if (win) {
        gtk_signal_connect (GTK_OBJECT (dialog), "clicked",
                            (GtkSignalFunc)end_game_highscore_record_and_display, NULL);
    } else {
        gtk_signal_connect (GTK_OBJECT (dialog), "clicked",
                            (GtkSignalFunc)end_game_highscore_display, NULL);
    }
        
    gtk_widget_show (dialog);
}

static void
end_game_highscore_record_and_display (GtkWidget *widget, gint button)
{
    gint position;
    GtkWidget *dialog;
    
    position = gnome_score_log (get_moves (),
                                (gchar *)named_levels[get_mosaic ()][get_difficulty ()],
                                FALSE);

    reset_game_board ();

    dialog = create_highscore_dialog (get_mosaic ());
    gnome_dialog_set_parent (GNOME_DIALOG (dialog), GTK_WINDOW (get_window ()));
    gnome_dialog_set_default (GNOME_DIALOG (dialog), 1);
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
    gnome_dialog_set_close (GNOME_DIALOG (dialog), TRUE);
    gtk_signal_connect (GTK_OBJECT (dialog), "clicked",
                        (GtkSignalFunc)end_game_decision, NULL);
    gtk_widget_show (dialog);
}

static void
end_game_highscore_display (GtkWidget *widget, gint button)
{
    GtkWidget *dialog;
    
    reset_game_board ();
    
    dialog = create_highscore_dialog (get_mosaic ());
    gnome_dialog_set_parent (GNOME_DIALOG (dialog), GTK_WINDOW (get_window ()));
    gnome_dialog_set_default (GNOME_DIALOG (dialog), 1);
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
    gnome_dialog_set_close (GNOME_DIALOG (dialog), TRUE);
    gtk_signal_connect (GTK_OBJECT (dialog), "clicked",
                        (GtkSignalFunc)end_game_decision, NULL);
    gtk_widget_show (dialog);
}

static void
end_game_decision (GtkWidget *widget, gint button)
{
    /* right now, there's only one button, but we
       leave this flexible in case we want more than
       an "OK" option
    */

	if (button == 0) {
        new_game_cb (NULL, NULL);
	}
}

static GtkWidget*
create_endgame_dialog (gboolean win)
{
    GtkWidget *dialog;
    GtkWidget *dialog_vbox1;
    GtkWidget *vbox1;
    GtkWidget *game_over_label;
    GtkWidget *stats_frame;
    GtkWidget *table1;
    GtkWidget *tiles_left_label;
    GtkWidget *tiles_left_value_label;
    GtkWidget *moves_label;
    GtkWidget *moves_value_label;
    GtkWidget *undo_label;
    GtkWidget *undone_value_label;
    GtkWidget *dialog_action_area1;
    GtkWidget *new_game_button;

    gchar *tmpstr;

    if (win) {
        dialog = gnome_dialog_new (_("You Win!"), NULL);
    } else {
        dialog = gnome_dialog_new (_("Game Over"), NULL);
    }
    
    gtk_object_set_data (GTK_OBJECT (dialog), "dialog", dialog);
    gtk_window_set_policy (GTK_WINDOW (dialog), FALSE, FALSE, FALSE);

    dialog_vbox1 = GNOME_DIALOG (dialog)->vbox;
    gtk_object_set_data (GTK_OBJECT (dialog), "dialog_vbox1", dialog_vbox1);
    gtk_widget_show (dialog_vbox1);
    
    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "vbox1", vbox1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

    if (win) {
        game_over_label = gtk_label_new (_("You win!"));
    } else {
        game_over_label = gtk_label_new (_("Sorry, you lose."));
    }
    
    gtk_widget_ref (game_over_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "game_over_label", game_over_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (game_over_label);
    gtk_box_pack_start (GTK_BOX (vbox1), game_over_label, FALSE, FALSE, 0);
    
    stats_frame = gtk_frame_new (_("Statistics"));
    gtk_widget_ref (stats_frame);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "stats_frame", stats_frame,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (stats_frame);
    gtk_box_pack_start (GTK_BOX (vbox1), stats_frame, TRUE, TRUE, 0);
    
    table1 = gtk_table_new (3, 2, FALSE);
    gtk_widget_ref (table1);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "table1", table1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (table1);
    gtk_container_add (GTK_CONTAINER (stats_frame), table1);
    gtk_container_set_border_width (GTK_CONTAINER (table1), 10);
    gtk_table_set_row_spacings (GTK_TABLE (table1), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table1), 5);

    tiles_left_label = gtk_label_new (_("Tiles remaning: "));
    gtk_widget_ref (tiles_left_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "tiles_left_label", tiles_left_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tiles_left_label);
    gtk_table_attach (GTK_TABLE (table1), tiles_left_label, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (tiles_left_label), 0, 0.5);
    
    tmpstr = g_strdup_printf (_("%i"), tiles_remaining ());
    tiles_left_value_label = gtk_label_new (tmpstr);
    g_free (tmpstr);
    gtk_widget_ref (tiles_left_value_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "tiles_left_value_label", tiles_left_value_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tiles_left_value_label);
    gtk_table_attach (GTK_TABLE (table1), tiles_left_value_label, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (tiles_left_value_label), 0, 0.5);
    
    moves_label = gtk_label_new (_("Moves taken:"));
    gtk_widget_ref (moves_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "moves_label", moves_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (moves_label);
    gtk_table_attach (GTK_TABLE (table1), moves_label, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (moves_label), 0, 0.5);
    
    tmpstr = g_strdup_printf (_("%i"), get_moves ());
    moves_value_label = gtk_label_new (tmpstr);
    g_free (tmpstr);
    gtk_widget_ref (moves_value_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "moves_value_label", moves_value_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (moves_value_label);
    gtk_table_attach (GTK_TABLE (table1), moves_value_label, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (moves_value_label), 0, 0.5);
    
    undo_label = gtk_label_new (_("Undone moves:"));
    gtk_widget_ref (undo_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "undo_label", undo_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (undo_label);
    gtk_table_attach (GTK_TABLE (table1), undo_label, 0, 1, 2, 3,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (undo_label), 0, 0.5);
    
    tmpstr = g_strdup_printf (_("%i"), get_undos ());
    undone_value_label = gtk_label_new (tmpstr);
    g_free (tmpstr);
    gtk_widget_ref (undone_value_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "undone_value_label", undone_value_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (undone_value_label);
    gtk_table_attach (GTK_TABLE (table1), undone_value_label, 1, 2, 2, 3,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (undone_value_label), 0, 0.5);
    
    dialog_action_area1 = GNOME_DIALOG (dialog)->action_area;
    gtk_object_set_data (GTK_OBJECT (dialog), "dialog_action_area1", dialog_action_area1);
    gtk_widget_show (dialog_action_area1);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_SPREAD);
    gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area1), 8);


    gnome_dialog_append_button (GNOME_DIALOG (dialog), GNOME_STOCK_BUTTON_OK);
    new_game_button = GTK_WIDGET (g_list_last (GNOME_DIALOG (dialog)->buttons)->data);
    gtk_widget_ref (new_game_button);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "new_game_button", new_game_button,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (new_game_button);
    GTK_WIDGET_SET_FLAGS (new_game_button, GTK_CAN_DEFAULT);
    
    gtk_widget_grab_focus (new_game_button);
    gtk_widget_grab_default (new_game_button);
    return dialog;
}
