/*
 * main.c - Driver for gnome-tesserae
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
#include <libgnomeui/gnome-window-icon.h>
#include <gdk/gdkkeysyms.h>

#include <sys/time.h>
#include <string.h>

#include "board-utils.h"
#include "gnome-tesserae.h"
#include "move-utils.h"
#include "undo-utils.h"
#include "properties.h"
#include "new-game.h"
#include "score.h"

/* FIXME: if we change these, be sure to
   handle underflow conditions
*/
static const int SCORE_INCREMENT = 1;
static const int UNDO_PENALTY = 1;

/* FIXME: tilesets and background graphics should be configurable */
static const char *TILE_SET = "classic.png";
static const char *BACKGROUND = "colorful-cloth.jpg";

static const char *SELECT_SOUND = "select.wav";
static const char *FLIP_SOUND = "flip.wav";

static char *select_sound_filename = NULL;
static char *flip_sound_filename = NULL;

static void quit_game_maybe (GtkWidget *widget, gint button);
static void quit_game_cb (GtkWidget *widget, gpointer data);
static gint save_state_cb (GnomeClient *client,
                           gint phase,
                           GnomeRestartStyle save_style,
                           gint shutdown,
                           GnomeInteractStyle interact_style,
                           gint fast, gpointer client_data);
static void about_cb (GtkWidget *widget, gpointer data);
static void undo_move_cb (GtkWidget *widget, gpointer data);
static void properties_cb (GtkWidget *widget, gpointer data);
static void create_main_window (void);
static void load_pixmaps (void);
static void load_sounds (void);

static void draw_pixmap_buffer (BOARD_SQUARE which, gint x, gint y);
static void draw_board (const MOVE_CACHE *cache);
static void draw_valid_moves_buffer (const MOVE_CACHE *cache);
static void draw_selected_square_buffer (guint x, guint y);
static void show_message (gchar *message);
static void update_score_display (void);

static void set_background_color (void);
static char *number_to_string (gint n);

/* FIXME: why do we need two?
   (one is for "ON", one is for "OFF", but what does that
   mean?)
*/
static GdkGC *board_GC[2] = { NULL, NULL };

/* these indicate whether a tile has already been selected
   and, if so, which one. */
/* FIXME: should this be a struct? */
static gboolean square_selected = FALSE;
static guint    selected_x = 0;
static guint    selected_y = 0;

static gboolean game_in_progress = FALSE;
static guint moves = 0;
static guint undos = 0;
static DIFFICULTY current_difficulty = DIFFICULTY_UNDEFINED;
static BOARD_STYLE current_board_style = BOARD_STYLE_UNDEFINED;

static UNDO_LIST_REF undo_list = NULL;

static GnomeAppBar *appbar = NULL;
static GtkWidget *window = NULL;
static GtkWidget *drawing_area = NULL;
static GtkWidget *score_display = NULL;

/* per-game score */
/* FIXME: it's probably safe to eliminate this variable
   and all references to it; scoring is now based on
   moves taken and tiles remaining for a given mosaic.
*/
static guint score = 0;

static GnomeUIInfo game_menu[] = {

    GNOMEUIINFO_MENU_NEW_GAME_ITEM (new_game_cb, NULL),
    GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_UNDO_MOVE_ITEM (undo_move_cb, NULL),
	GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_MENU_SCORES_ITEM (scores_cb, NULL),
    GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_MENU_EXIT_ITEM (quit_game_cb, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo settings_menu[] = {
    GNOMEUIINFO_MENU_PREFERENCES_ITEM (properties_cb, NULL),
    GNOMEUIINFO_END
};

static GnomeUIInfo help_menu[] = {
    GNOMEUIINFO_HELP ("gnome-tesserae"),
	GNOMEUIINFO_MENU_ABOUT_ITEM (about_cb, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo main_menu[] = {
    GNOMEUIINFO_MENU_GAME_TREE (game_menu),
    GNOMEUIINFO_MENU_SETTINGS_TREE (settings_menu),
    GNOMEUIINFO_MENU_HELP_TREE (help_menu),
	GNOMEUIINFO_END
};

/* so we can toggle the sensitivity of the `Undo Move'
   command later on
*/
static GtkWidget *undo_widget = NULL;

GdkPixmap *buffer_pixmap = NULL;
GdkPixmap *tiles_pixmap = NULL;
GdkBitmap *tiles_mask = NULL;
GdkPixmap *background_pixmap = NULL;

static guint session_xpos = -1;
static guint session_ypos = -1;

/* struct for commandline options */
static const struct poptOption options[] = {
    { NULL, 'x', POPT_ARG_INT, &session_xpos, 0, NULL, NULL},
    { NULL, 'y', POPT_ARG_INT, &session_ypos, 0, NULL, NULL},
    { NULL, '\0', 0, NULL, 0}
};

static gint
expose_event (GtkWidget *widget, GdkEventExpose *event)
{
	gdk_draw_pixmap (widget->window,
                     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                     buffer_pixmap,
                     event->area.x, event->area.y,
                     event->area.x, event->area.y,
                     event->area.width, event->area.height);

	return FALSE;
}

static gint
configure_event (GtkWidget *widget, GdkEventConfigure *event)
{
    draw_board (NULL);
	
	return TRUE;
}

/*
 * Handles the mouse-button presses inside the game board.
 *
 * FIXME: we're redrawing here *way* more than should be
 * necessary.  I think we need at most one call to draw_board if
 * we do this correctly...
 *
 */

static gint
button_press_event (GtkWidget *widget, GdkEventButton *event)
{
	guint x, y;
    MOVE_CACHE cache;

	if (event->button == 1) {

		x = event->x / TILE_WIDTH;
		y = event->y / TILE_HEIGHT;

        /*
        g_print ("now looking at click at %i, %i\n",
                 x, y);
        */

        /* do we need to move the tile or display possible moves? */
        if (square_selected) {
            /*
            g_print ("a square is already selected\n");
            */

            /* unselected the selected piece so the user can try again */
            if ((x == selected_x && y == selected_y) ||
                board[x][y] == BOARD_SQUARE_UNUSED)
            {
                square_selected = FALSE;
                draw_board (NULL);
                return TRUE; /* click handled */
            }
            
            cache_valid_moves (selected_x, selected_y, &cache);
            
            if (is_valid_move (selected_x, selected_y, x, y, cache)) {

                square_selected = FALSE;
                
                save_undo_info (selected_x, selected_y, x, y,
                                undo_list);

                gtk_widget_set_sensitive (undo_widget, TRUE);

                moves++;
                score += SCORE_INCREMENT;
                update_score_display ();

                /*
                g_print ("move from %i %i to %i %i is valid.\n",
                         selected_x, selected_y, x, y);
                */

                if (properties_get_sound ()) {
                    gnome_sound_play (flip_sound_filename);
                }
                
                make_move (selected_x, selected_y, x, y);
                draw_board (NULL);
                
                if (!valid_moves_exist ()) {

                    if (tiles_remaining () == 1) {

                        end_game_scoring (TRUE);

                    } else {

                        end_game_scoring (FALSE);

                    }

                }
            } else {

                /*
                g_print ("move from %i %i to %i %i is NOT valid.\n",
                         selected_x, selected_y, x, y);
                */
                
                /* unselected the selected piece so the user can try again */
                square_selected = FALSE;

                draw_board (NULL);
            }
        } else {
            /*
            g_print ("selecting the square\n");
            */
            if (board[x][y] == BOARD_SQUARE_EMPTY) return TRUE;
            if (board[x][y] == BOARD_SQUARE_UNUSED) return TRUE;
            square_selected = TRUE;
            selected_x = x; selected_y = y;
            if (properties_get_sound ()) {
                gnome_sound_play (select_sound_filename);
            }

            cache_valid_moves (selected_x, selected_y, &cache);
            
            draw_board (&cache);

            /*
            g_print ("valid moves drawn\n");
            */
        }
    }
    
    /* event was handled */
    return TRUE;
}

static void
undo_move_cb (GtkWidget *widget, gpointer data)
{
	if (game_in_progress) {

        if (!undo_list_is_empty (undo_list)) {
            undos++;
            score -= UNDO_PENALTY;
            update_score_display ();
        }
        
		undo (undo_list);
        square_selected = FALSE;
        draw_board (NULL);

        if (undo_list_is_empty (undo_list)) {
            gtk_widget_set_sensitive (undo_widget, FALSE);
        }
	}
}

static void
properties_cb (GtkWidget *widget, gpointer data)
{
	show_properties_dialog (window);
}

static gint
save_state_cb (GnomeClient *client,
               gint phase,
               GnomeRestartStyle save_style,
               gint shutdown,
               GnomeInteractStyle interact_style,
               gint fast, gpointer client_data)
{
	char *argv[20];
	gint i;
	gint xpos, ypos;

	gdk_window_get_origin (window->window, &xpos, &ypos);

	i = 0;
	argv[i++] = (char *)client_data;
	argv[i++] = "-x";
	argv[i++] = number_to_string (xpos);
	argv[i++] = "-y";
	argv[i++] = number_to_string (ypos);

	gnome_client_set_restart_command (client, i, argv);
	gnome_client_set_clone_command (client, 0, NULL);

	free (argv[2]);
	free (argv[4]);

	return TRUE;
}

static void
about_cb (GtkWidget *widget, gpointer data)
{
	static GtkWidget *about;

	const gchar *authors[] = {"Nathan Thompson-Amato <ndt@wagonfixers.com>",
                              "Matt Mullen <matt@mattmullen.net>",
                              NULL};

	if (about != NULL) {
		gdk_window_raise (about->window);
		gdk_window_show (about->window);
		return;
	}

	about =
        gnome_about_new (
            _("Gnome Tesserae"), VERSION,
            _("(C) 2001 Nathan Thompson-Amato and Matt Mullen"),
            (const char **)authors,
            _("Send comments and bug reports to <ndt@wagonfixers.com> or <matt@mattmullen.net>"),
            NULL);

	gtk_signal_connect (GTK_OBJECT (about), "destroy", GTK_SIGNAL_FUNC
                        (gtk_widget_destroyed), &about);
    
	gnome_dialog_set_parent (GNOME_DIALOG (about), GTK_WINDOW (window));

	gtk_widget_show (about);
}

static void
quit_game_cb (GtkWidget *widget, gpointer data)
{
	GtkWidget *box;

    box = gnome_message_box_new (_("Really quit?"),
                                 GNOME_MESSAGE_BOX_QUESTION,
                                 GNOME_STOCK_BUTTON_YES,
                                 GNOME_STOCK_BUTTON_NO, NULL);
    
    gnome_dialog_set_parent (GNOME_DIALOG (box), GTK_WINDOW (window));
    gnome_dialog_set_default (GNOME_DIALOG (box), 1);
    gtk_window_set_modal (GTK_WINDOW (box), TRUE);
    gtk_signal_connect (GTK_OBJECT (box), "clicked",
                        (GtkSignalFunc)quit_game_maybe, NULL);
    gtk_widget_show(box);
}

static void
quit_game_maybe (GtkWidget *widget, gint button)
{
    /* make sure the user actually pressed "yes" */
    if (button == 0) {
		gtk_main_quit();
	}
}

/*
 * Returns a string containing `n'.
 *
 * Note that the caller is responsible for calling
 * g_free () on the returned string.
 */

static char *
number_to_string (gint n)
{
	return g_strdup_printf ("%d", n);
}

static void
create_main_window (void)
{
	GtkWidget *table;

	window = gnome_app_new ("gnome-tesserae", _("Gnome Tesserae"));

	gtk_widget_realize (window);
	gtk_window_set_policy (GTK_WINDOW(window), FALSE, FALSE, TRUE);
	gtk_signal_connect (GTK_OBJECT(window), "delete_event",
                        GTK_SIGNAL_FUNC(quit_game_cb), NULL);

	gnome_app_create_menus (GNOME_APP (window), main_menu);

	gtk_widget_push_visual (gdk_imlib_get_visual ());
	gtk_widget_push_colormap (gdk_imlib_get_colormap ());

	drawing_area = gtk_drawing_area_new();

	gtk_widget_pop_colormap ();
	gtk_widget_pop_visual ();

	gnome_app_set_contents (GNOME_APP (window), drawing_area);

	gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area),
                           BOARD_PIXEL_WIDTH, BOARD_PIXEL_HEIGHT);
    
	gtk_signal_connect (GTK_OBJECT (drawing_area),
                        "expose_event",
                        GTK_SIGNAL_FUNC (expose_event), NULL);
	gtk_signal_connect (GTK_OBJECT (window),
                        "configure_event",
                        GTK_SIGNAL_FUNC (configure_event), NULL);
	gtk_signal_connect (GTK_OBJECT (drawing_area),
                        "button_press_event",
                        GTK_SIGNAL_FUNC (button_press_event), NULL);
    
	gtk_widget_set_events (drawing_area,
                           GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
	gtk_widget_show (drawing_area);

	appbar = GNOME_APPBAR (gnome_appbar_new (FALSE, TRUE, FALSE));
	gnome_app_set_statusbar (GNOME_APP (window), GTK_WIDGET (appbar));
	gnome_app_install_menu_hints (GNOME_APP (window), main_menu);

	table = gtk_table_new (1, 8, FALSE);

	score_display = gtk_label_new (NULL);
	gtk_widget_show (score_display);

	gtk_table_attach (GTK_TABLE(table), score_display, 7, 8, 0, 1, 0, 0, 3, 1);

	gtk_widget_show (table);

	gtk_box_pack_start (GTK_BOX (appbar), table, FALSE, TRUE, 0);

	gnome_appbar_set_status (GNOME_APPBAR (appbar),
				_("Welcome to Gnome Tesserae!"));
}

static void
load_sounds (void)
{
    /* we don't really load sounds, we just setup the filenames
       so that we can call gnome_sound_play later
    */

    char *tmp;

    tmp = g_strconcat ("gnome-tesserae/", SELECT_SOUND, NULL);
	select_sound_filename = gnome_unconditional_sound_file (tmp);
	g_free (tmp);
    
	if (!g_file_exists (select_sound_filename)) {
		g_print (_("Could not find \'%s\' sound file for Gnome Tesserae\n"),
                 select_sound_filename);
		exit (1);
	}

    tmp = g_strconcat ("gnome-tesserae/", FLIP_SOUND, NULL);
	flip_sound_filename = gnome_unconditional_sound_file (tmp);
	g_free (tmp);
    
	if (!g_file_exists (flip_sound_filename)) {
		g_print (_("Could not find \'%s\' sound file for Gnome Tesserae\n"),
                 flip_sound_filename);
		exit (1);
	}
}

static void
load_pixmaps (void)
{
    char *tmp;
	char *fname;

	GdkImlibImage *image;
	GdkVisual *visual;

    /* load the tile graphics */
    
	tmp = g_strconcat ("gnome-tesserae/", TILE_SET, NULL);
	fname = gnome_unconditional_pixmap_file (tmp);
	g_free (tmp);
    
	if (!g_file_exists (fname)) {
		g_print (_("Could not find \'%s\' pixmap file for Gnome Tesserae\n"),
                 fname);
		exit (1);
	}

	image = gdk_imlib_load_image (fname);
	visual = gdk_imlib_get_visual ();
	if (visual->type != GDK_VISUAL_TRUE_COLOR) {
		gdk_imlib_set_render_type (RT_PLAIN_PALETTE);
	}
	gdk_imlib_render (image, image->rgb_width, image->rgb_height);
	tiles_pixmap = gdk_imlib_move_image (image);
    tiles_mask = gdk_imlib_move_mask (image);

	gdk_imlib_destroy_image (image);
	g_free (fname);

    /* load the background image */
       
	tmp = g_strconcat ("gnome-tesserae/", BACKGROUND, NULL);
	fname = gnome_unconditional_pixmap_file (tmp);
	g_free (tmp);
    
	if (!g_file_exists (fname)) {
		g_print (_("Could not find \'%s\' pixmap file for Gnome Tesserae\n"),
                 fname);
		exit (1);
	}

	image = gdk_imlib_load_image (fname);
	visual = gdk_imlib_get_visual ();
	if (visual->type != GDK_VISUAL_TRUE_COLOR) {
		gdk_imlib_set_render_type (RT_PLAIN_PALETTE);
	}
	gdk_imlib_render (image, image->rgb_width, image->rgb_height);
	background_pixmap = gdk_imlib_move_image (image);

	gdk_imlib_destroy_image (image);
	g_free (fname);
}

/* FIXME: eliminate this function if possible */
static void
set_background_color ()
{
	GdkImage *tmpimage;
	GdkColor bgcolor;

	tmpimage = gdk_image_get (tiles_pixmap, 0, 0, 1, 1);
	bgcolor.pixel = gdk_image_get_pixel (tmpimage, 0, 0);
	gdk_window_set_background (drawing_area->window, &bgcolor);

	if (board_GC[0])
        gdk_gc_destroy (board_GC[0]);
    
	board_GC[0] = gdk_gc_new (drawing_area->window);
    
	if (board_GC[1])
        gdk_gc_destroy (board_GC[1]);
    
	board_GC[1] = gdk_gc_new (drawing_area->window);

	gdk_gc_copy (board_GC [0], drawing_area->style->bg_gc[0]);
    gdk_gc_copy (board_GC [1], drawing_area->style->bg_gc[1]);
	
	gdk_gc_set_background (board_GC[0], &bgcolor);
	gdk_gc_set_foreground (board_GC[0], &bgcolor);
	
	/* Create a complementary color to use for the ON state */
	bgcolor.pixel = 0xFFFFFF - bgcolor.pixel;
	gdk_gc_set_background (board_GC[1], &bgcolor);
	gdk_gc_set_foreground (board_GC[1], &bgcolor);
	
	gdk_image_destroy (tmpimage);
}

/*
 * Draws a box around reachable tiles.  Draws to buffer_pixmap.
 */
static void
draw_valid_moves_buffer (const MOVE_CACHE *cache)
{
    guint i;
    guint src_x = 0;
    guint src_y = TILE_HEIGHT * 2;
    
    for (i = 0; i < MOVE_CACHE_SIZE; i++) {
        
        if (cache->potential_moves[i].valid == TRUE) {
            gint x = cache->potential_moves[i].x2;
            gint y = cache->potential_moves[i].y2;

            /*
              FIXME: this is the Right Way to overlay a
              blue box on a tile, but gdk_draw_pixmap ()
              doesn't seem to support transparent blits.
              For now, we do this the Wrong Way.
            */
            /*
            gdk_draw_pixmap (buffer_pixmap,
                             board_GC[0],
                             tiles_pixmap,
                             src_x, src_y,
                             x * TILE_WIDTH,
                             y * TILE_HEIGHT,
                             TILE_WIDTH, TILE_HEIGHT);
            */

            /* begin Wrong Way */

            /* in the Wrong Way, we've pre-drawn the overlay on each
               possible tile, so this is just a blit from the appropriate
               location in the tile graphics file
            */

            src_x = TILE_WIDTH * board[x][y];

            gdk_draw_pixmap (buffer_pixmap,
                             board_GC[0],
                             tiles_pixmap,
                             src_x, src_y,
                             x * TILE_WIDTH,
                             y * TILE_HEIGHT,
                             TILE_WIDTH, TILE_HEIGHT);
            
            /* end Wrong Way */
        }
    }
}

/*
 * Draws a box around the given tile. Draws to buffer_pixmap.
 */
static void
draw_selected_square_buffer (guint x, guint y)
{
    guint src_x;
    guint src_y = TILE_HEIGHT;
    
    /*
      FIXME: this is the Right Way to overlay a
      box on a tile, but gdk_draw_pixmap ()
      doesn't seem to support transparent blits.
      For now, we do this the Wrong Way.
    */
    /*
    gdk_draw_pixmap (buffer_pixmap,
                     board_GC[0],
                     tiles_pixmap,
                     src_x, src_y,
                     x * TILE_WIDTH,
                     y * TILE_HEIGHT,
                     TILE_WIDTH, TILE_HEIGHT);
    */

    /* begin Wrong Way */

    /* in the Wrong Way, we've pre-drawn the overlay on each
       possible tile, so this is just a blit from the appropriate
       location in the tile graphics file
    */

    src_x = TILE_WIDTH * board[x][y];

    gdk_draw_pixmap (buffer_pixmap,
                     board_GC[0],
                     tiles_pixmap,
                     src_x, src_y,
                     x * TILE_WIDTH,
                     y * TILE_HEIGHT,
                     TILE_WIDTH, TILE_HEIGHT);
            
    /* end Wrong Way */
}

/*
 * draws the given BOARD_SQUARE into buffer_pixmap.
 */
static void
draw_pixmap_buffer (BOARD_SQUARE which,
                    /* in pixels relative to buffer_pixmap */
                    gint x, gint y)
{
    if (which != BOARD_SQUARE_UNUSED) {

        gint src_x = TILE_WIDTH * which;
        gint src_y = 0;

        /* draw the tile */
        gdk_draw_pixmap (buffer_pixmap,
                         board_GC[0],
                         tiles_pixmap,
                         src_x, src_y,
                         x * TILE_WIDTH,
                         y * TILE_HEIGHT,
                         TILE_WIDTH, TILE_HEIGHT);
    } else {

        gint src_x = x * TILE_WIDTH;
        gint src_y = y * TILE_HEIGHT;
        gint dest_x = src_x;
        gint dest_y = src_y;
        
        /* draw part of the background image */
        gdk_draw_pixmap (buffer_pixmap,
                         board_GC[0],
                         background_pixmap,
                         src_x, src_y,
                         dest_x, dest_y,
                         TILE_WIDTH, TILE_HEIGHT);
    }
}

/*
 * Draws the squares on the game board onto the buffer_pixmap,
 * then draws that onto the window's drawing area
 */

static void
draw_board (const MOVE_CACHE *cache)
{
    guint i, j;

	if (board_GC[0] != 0) {
		gdk_draw_rectangle (buffer_pixmap,
                            board_GC[0],
                            1, 0, 0,
                            BOARD_PIXEL_WIDTH, BOARD_PIXEL_HEIGHT);
        
		for (i = 0; i < BOARD_SQUARE_WIDTH; i++)
			for (j = 0; j < BOARD_SQUARE_HEIGHT; j++)
                draw_pixmap_buffer (board[i][j], i, j);

        if (cache) {
            draw_valid_moves_buffer (cache);
        }

        if (square_selected) {
            draw_selected_square_buffer (selected_x, selected_y);
        }
        
        gdk_draw_pixmap (drawing_area->window, board_GC[0],
                         buffer_pixmap, 0, 0, 0, 0,
                         BOARD_PIXEL_WIDTH, BOARD_PIXEL_HEIGHT);
	}

    /* FIXME: why does "grid" index the board_GC[] array? */
    /*
	for(i = 1; i < 8; i++) {

		gdk_draw_line (buffer_pixmap, board_GC[grid],
                       i * BOARD_WIDTH / 8-1, 0, i * BOARD_WIDTH / 8-1, BOARD_HEIGHT);
		gdk_draw_line (buffer_pixmap, board_GC[grid],
                       0, i * BOARD_HEIGHT / 8-1, BOARD_WIDTH, i * BOARD_HEIGHT / 8-1);
	}
    */
}

static void
update_score_display (void)
{
    /* FIXME: rename all the score_display functions
       and remove the `score' variable
    */
    gchar *text = g_strdup_printf (_("Moves: %i"), moves);
    gtk_label_set_text (GTK_LABEL (score_display), text);
    g_free (text);
}

static void
show_message (gchar *message)
{
	gnome_appbar_pop (GNOME_APPBAR (appbar));
    gnome_appbar_push (GNOME_APPBAR (appbar), message);
}

/*
 * Resets the game board to its initial blank state.
 * Ends any games that're in progress.
 */
void
reset_game_board (void)
{
    /* clear the play area */
    int i, j;
    
    for (i = 0; i < BOARD_SQUARE_HEIGHT; i++)
        for (j = 0; j < BOARD_SQUARE_WIDTH; j++)
            board[i][j] = BOARD_SQUARE_UNUSED;

    /* end all games */
    game_in_progress = FALSE;
    square_selected = FALSE;
    moves = 0;
    score = 0;
    undos = 0;
    update_score_display ();

    if (undo_list) {
        delete_undo_list (&undo_list);
    }

    /* gray out undo option since the undo list is empty */
    gtk_widget_set_sensitive (undo_widget, FALSE);

    /* redraw the now-blank board */
    draw_board (NULL);
}

/*
 * Returns the current score
 */
guint
get_score (void)
{
    return score;
}

/*
 * Returns the current move count
 */
guint
get_moves (void)
{
    return moves;
}

/*
 * Returns the current undo count
 */
guint
get_undos (void)
{
    return undos;
}

/*
 * Return the current mosaic
 */
BOARD_STYLE
get_mosaic (void)
{
    return current_board_style;
}

/*
 * Return the current difficulty level
 */
DIFFICULTY
get_difficulty (void)
{
    return current_difficulty;
}

/*
 * Returns the value of `game_in_progress'
 */
gboolean
get_game_in_progress (void)
{
    return game_in_progress;
}

/*
 * Returns the value of `window'
 */
GtkWidget *
get_window (void)
{
    return window;
}

/*
 * Resets the game state for a new game based on the
 * given difficulty and game board style.
 * 
 */
void
init_new_game (DIFFICULTY difficulty, BOARD_STYLE board_style)
{
	game_in_progress = TRUE;
    square_selected = FALSE;
	moves = 0;
    score = 0;
    undos = 0;
    update_score_display ();


    if (undo_list) {
        delete_undo_list (&undo_list);
    }
    undo_list = new_undo_list ();

    /* gray out undo option since the undo list is empty */
    gtk_widget_set_sensitive (undo_widget, FALSE);

    current_board_style = board_style;
    current_difficulty = difficulty;
    init_board (board_style);
    generate_squares (difficulty);

    draw_board (NULL);
}

int
main (int argc, char **argv)
{
	GnomeClient *client;
	struct timeval tv;

	gnome_score_init ("gnome-tesserae");

    /* FIXME: i18n/L10N support is busted and I don't know why */
#ifdef _SOMETHING_    
	bindtextdomain (PACKAGE, GNOMELOCALEDIR);
	textdomain (PACKAGE);
#endif

	gettimeofday (&tv, NULL);
	srand (tv.tv_usec);

	gnome_init_with_popt_table("gnome-tesserae", VERSION, argc, argv,
                               options, 0, NULL);

    {
        char *fname = gnome_unconditional_pixmap_file ("gnome-tesserae.png");

        if (!g_file_exists (fname)) {
            g_print(_("Could not find \'%s\' pixmap file for Gnome Tesserae\n"),
                    fname);
            exit (1);
        }

        gnome_window_icon_set_default_from_file (fname);

        g_free (fname);
    }

	client = gnome_master_client ();

	gtk_object_ref (GTK_OBJECT (client));
	gtk_object_sink (GTK_OBJECT (client));

	gtk_signal_connect (GTK_OBJECT (client),
                        "save_yourself",
                        GTK_SIGNAL_FUNC (save_state_cb), argv[0]);
    
    gtk_signal_connect (GTK_OBJECT (client),
                        "die", GTK_SIGNAL_FUNC (quit_game_cb), argv[0]);

    create_main_window ();

	load_properties ();

	load_pixmaps ();
    load_sounds ();

	if (session_xpos >= 0 && session_ypos >= 0) {
		gtk_widget_set_uposition (window, session_xpos, session_ypos);
 	}

    undo_widget = game_menu[2].widget;
    gtk_widget_set_sensitive (undo_widget, FALSE);

    reset_game_board ();
    
    gtk_widget_show (window);

	buffer_pixmap = gdk_pixmap_new (drawing_area->window,
                                    BOARD_PIXEL_WIDTH,
                                    BOARD_PIXEL_HEIGHT, -1);

    /* FIXME: we draw a nice background image instead, so
       is it necessary to do this anyway?
    */
	set_background_color ();
    
	gtk_main ();

    /* FIXME: why does this whine about client not being a
       valid object?  does this have to do with the Iagno source
       being CORBA-centric?
    */
    gtk_object_unref (GTK_OBJECT (client));

	return 0;
}
