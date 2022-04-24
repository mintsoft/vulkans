INCLUDES = -I$(top_srcdir) -I$(includedir) $(GNOME_INCLUDEDIR) \
		-DG_LOG_DOMAIN=\"gnome-tesserae\" \
		-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
		-I../intl -I$(top_srcdir)/intl

# add $(INTLLIBS) to this after we get gettextize working again
gnome_tesserae_LDADD = $(GNOMEUI_LIBS) $(GNOME_LIBDIR)

bin_PROGRAMS = gnome-tesserae

# add in this one at some point
gnome_tesserae_SOURCES = \
	main.c \
	gnome-tesserae.h \
	board-utils.c \
	board-utils.h \
	move-utils.c \
	move-utils.h \
	undo-utils.c \
	undo-utils.h \
	new-game.c \
	new-game.h \
	score.c \
	score.h \
	custom-highscore-dialog.c \
	custom-highscore-dialog.h \
	properties.c \
	properties.h
