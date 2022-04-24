/*
 * custom-highscore-dialog.c - Custom dialog for viewing a high-score list
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
#include "custom-highscore-dialog.h"

static void
create_high_scores_for_mosaic (BOARD_STYLE mosaic,
                               DIFFICULTY difficulty,
                               GtkObject *dialog,
                               GtkBox *container_vbox)
{
    GtkWidget *label;
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    
    gchar **names;
    gfloat *scores;
    time_t *scoretimes;
    gint elements, i;

    if (difficulty == DIFFICULTY_EASY) {
        frame = gtk_frame_new (_("Easy"));
    } else if (difficulty == DIFFICULTY_MEDIUM) {
        frame = gtk_frame_new (_("Medium"));
    } else {
        frame = gtk_frame_new (_("Hard"));
    }
    gtk_widget_ref (frame);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "frame", frame,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (container_vbox), frame, TRUE, TRUE, 0);
    
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "vbox", vbox,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (frame), vbox);
    
    hbox = gtk_hbox_new (TRUE, 0);
    gtk_widget_ref (hbox);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "hbox", hbox,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
    
    label = gtk_label_new (_("User"));
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "label", label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    
    label = gtk_label_new (_("Moves"));
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "label", label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    
    label = gtk_label_new (_("Date"));
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "label", label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);


    elements = gnome_score_get_notable ("gnome-tesserae",
                                        /* FIXME: do we have to trust
                                           gnome_score_get_notable with
                                           this cast?
                                        */
                                        (gchar *)named_levels[mosaic][difficulty],
                                        &names, &scores, &scoretimes);

    if (elements == 0) {
        
        hbox = gtk_hbox_new (TRUE, 0);
        gtk_widget_ref (hbox);
        gtk_object_set_data_full (GTK_OBJECT (dialog), "hbox", hbox,
                                  (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (hbox);
        gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

        label = gtk_label_new (_("-- No scores yet -- "));
        gtk_widget_ref (label);
        gtk_object_set_data_full (GTK_OBJECT (dialog), "label", label,
                                  (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (label);
        gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
        
    } else {
        for (i = 0; i < elements; i++) {
            
            hbox = gtk_hbox_new (TRUE, 0);
            gtk_widget_ref (hbox);
            gtk_object_set_data_full (GTK_OBJECT (dialog), "hbox", hbox,
                                      (GtkDestroyNotify) gtk_widget_unref);
            gtk_widget_show (hbox);
            gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
            
            label = gtk_label_new (names[i]);
            gtk_widget_ref (label);
            gtk_object_set_data_full (GTK_OBJECT (dialog), "label", label,
                                      (GtkDestroyNotify) gtk_widget_unref);
            gtk_widget_show (label);
            gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
            
            {
                gchar *moves = g_strdup_printf ("%i", (int)scores[i]);
                label = gtk_label_new (moves);
                gtk_widget_ref (label);
                gtk_object_set_data_full (GTK_OBJECT (dialog), "label", label,
                                          (GtkDestroyNotify) gtk_widget_unref);
                gtk_widget_show (label);
                gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
                
                g_free (moves);
            }
            
            {
                gchar *win_date = g_strdup (ctime (&scoretimes[i]));

                /* get rid of the trailing \n from the ctime call.
                   (why does ctime append a newline at all?  how does that make
                   sense for a general-purpose, standard-C-library function?
                   grrrr....)
                */
                win_date[strlen (win_date) - 1] = '\0';
                
                label = gtk_label_new (win_date);
                gtk_widget_ref (label);
                gtk_object_set_data_full (GTK_OBJECT (dialog), "label", label,
                                          (GtkDestroyNotify) gtk_widget_unref);
                gtk_widget_show (label);
                gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

                g_free (win_date);
            }
        }
    }
}

GtkWidget*
create_highscore_dialog (BOARD_STYLE highlight_mosaic)
{
    GtkWidget *dialog;
    GtkWidget *dialog_vbox1;
    GtkWidget *vbox1;
    GtkWidget *gnome_tesserae_label;
    GtkWidget *notebook1;
    GtkWidget *empty_notebook_page;
    GtkWidget *small_rect_label;
    GtkWidget *med_rect_label;
    GtkWidget *large_rect_label;
    GtkWidget *small_circle_label;
    GtkWidget *medium_circle_label;
    GtkWidget *dialog_action_area1;
    GtkWidget *button1;

    gint i;

    dialog = gnome_dialog_new (_("High Scores"), NULL);
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
    
    gnome_tesserae_label = gtk_label_new (_("Gnome Tesserae"));
    gtk_widget_ref (gnome_tesserae_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "gnome_tesserae_label", gnome_tesserae_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (gnome_tesserae_label);
    gtk_box_pack_start (GTK_BOX (vbox1), gnome_tesserae_label, FALSE, FALSE, 0);
    
    notebook1 = gtk_notebook_new ();
    gtk_widget_ref (notebook1);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "notebook1", notebook1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notebook1);
    gtk_box_pack_start (GTK_BOX (vbox1), notebook1, TRUE, TRUE, 0);
    
    empty_notebook_page = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (empty_notebook_page);
    gtk_container_add (GTK_CONTAINER (notebook1), empty_notebook_page);

    for (i = 0; i < 3; i++) {
        create_high_scores_for_mosaic (BOARD_STYLE_SMALL_RECTANGLE, i,
                                       GTK_OBJECT (dialog),
                                       GTK_BOX (empty_notebook_page));
    }
    
    small_rect_label = gtk_label_new (_("Small Rectangle"));
    gtk_widget_ref (small_rect_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "small_rect_label", small_rect_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (small_rect_label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0),
                                small_rect_label);
    
    empty_notebook_page = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (empty_notebook_page);
    gtk_container_add (GTK_CONTAINER (notebook1), empty_notebook_page);

    for (i = 0; i < 3; i++) {
        create_high_scores_for_mosaic (BOARD_STYLE_MEDIUM_RECTANGLE, i,
                                       GTK_OBJECT (dialog),
                                       GTK_BOX (empty_notebook_page));
    }
    
    med_rect_label = gtk_label_new (_("Medium Rectangle"));
    gtk_widget_ref (med_rect_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "med_rect_label", med_rect_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (med_rect_label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1),
                                med_rect_label);

    empty_notebook_page = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (empty_notebook_page);
    gtk_container_add (GTK_CONTAINER (notebook1), empty_notebook_page);
    
    for (i = 0; i < 3; i++) {
        create_high_scores_for_mosaic (BOARD_STYLE_LARGE_RECTANGLE, i,
                                       GTK_OBJECT (dialog),
                                       GTK_BOX (empty_notebook_page));
    }
    
    large_rect_label = gtk_label_new (_("Large Rectangle"));
    gtk_widget_ref (large_rect_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "large_rect_label", large_rect_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (large_rect_label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2),
                                large_rect_label);
    
    empty_notebook_page = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (empty_notebook_page);
    gtk_container_add (GTK_CONTAINER (notebook1), empty_notebook_page);

    for (i = 0; i < 3; i++) {
        create_high_scores_for_mosaic (BOARD_STYLE_SMALL_CIRCLE, i,
                                       GTK_OBJECT (dialog),
                                       GTK_BOX (empty_notebook_page));
    }
    
    small_circle_label = gtk_label_new (_("Small Circle"));
    gtk_widget_ref (small_circle_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "small_circle_label", small_circle_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (small_circle_label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3),
                                small_circle_label);
    
    empty_notebook_page = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (empty_notebook_page);
    gtk_container_add (GTK_CONTAINER (notebook1), empty_notebook_page);

    for (i = 0; i < 3; i++) {
        create_high_scores_for_mosaic (BOARD_STYLE_MEDIUM_CIRCLE, i,
                                       GTK_OBJECT (dialog),
                                       GTK_BOX (empty_notebook_page));
    }
    
    medium_circle_label = gtk_label_new (_("Medium Circle"));
    gtk_widget_ref (medium_circle_label);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "medium_circle_label", medium_circle_label,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (medium_circle_label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 4),
                                medium_circle_label);
    
    dialog_action_area1 = GNOME_DIALOG (dialog)->action_area;
    gtk_object_set_data (GTK_OBJECT (dialog), "dialog_action_area1", dialog_action_area1);
    gtk_widget_show (dialog_action_area1);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_SPREAD);
    gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area1), 8);
    
    gnome_dialog_append_button (GNOME_DIALOG (dialog), GNOME_STOCK_BUTTON_OK);
    button1 = GTK_WIDGET (g_list_last (GNOME_DIALOG (dialog)->buttons)->data);
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (dialog), "button1", button1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);
    
    gtk_widget_grab_focus (button1);
    gtk_widget_grab_default (button1);


    if (highlight_mosaic != BOARD_STYLE_UNDEFINED) {
        /* this depends on tabs being added to the
           notebook in the same order the BOARD_STYLEs
           are listed in board-utils.h
        */
        gtk_notebook_set_page (GTK_NOTEBOOK (notebook1),
                               highlight_mosaic);
    }
    
    return dialog;
}
