# add po and intl back in after we figure
# out why gettextize keeps failing
SUBDIRS = macros src pixmaps sounds help

# We include a prebuilt .spec file so J. Random User
# can build his or her own RPMs without waiting for
# us to do it.
EXTRA_DIST = \
	gnome-tesserae.desktop \
	TODO \
	gnome-tesserae.spec

# capitalization here must match that
# of the actual directory
Gamesdir = $(datadir)/gnome/apps/Games
Games_DATA = gnome-tesserae.desktop

scoredir = $(localstatedir)/games

# work some magic to create empty high-score files and make
# them and our binary owned by the `games' group.
install-data-local:
	-$(mkinstalldirs) $(DESTDIR)$(scoredir)
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.small-circle-easy.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.small-circle-med.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.small-circle-hard.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.med-circle-easy.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.med-circle-med.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.med-circle-hard.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.small-rect-easy.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.small-rect-med.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.small-rect-hard.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.big-rect-easy.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.big-rect-med.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.big-rect-hard.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.med-rect-easy.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.med-rect-med.scores
	touch $(DESTDIR)$(scoredir)/gnome-tesserae.med-rect-hard.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.small-circle-easy.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.small-circle-med.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.small-circle-hard.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.med-circle-easy.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.med-circle-med.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.med-circle-hard.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.small-rect-easy.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.small-rect-med.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.small-rect-hard.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.big-rect-easy.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.big-rect-med.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.big-rect-hard.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.med-rect-easy.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.med-rect-med.scores
	-chown games.games $(DESTDIR)$(scoredir)/gnome-tesserae.med-rect-hard.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.small-circle-easy.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.small-circle-med.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.small-circle-hard.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.med-circle-easy.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.med-circle-med.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.med-circle-hard.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.small-rect-easy.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.small-rect-med.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.small-rect-hard.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.big-rect-easy.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.big-rect-med.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.big-rect-hard.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.med-rect-easy.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.med-rect-med.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gnome-tesserae.med-rect-hard.scores
	-chgrp games $(DESTDIR)$(bindir)/gnome-tesserae && chmod 2111 $(DESTDIR)$(bindir)/gnome-tesserae
	$(mkinstalldirs) $(DESTDIR)$(Gamesdir)