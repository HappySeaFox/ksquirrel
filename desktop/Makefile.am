####### kdevelop will overwrite this part!!! (begin)##########


####### kdevelop will overwrite this part!!! (end)############

EXTRA_DIST = desktops.tar.gz

#EXTRA_DIST = Open with GIMP.desktop \
#	Open with GQview.desktop \
#	Open with Konqueror.desktop \
#	Open with KuickShow.desktop \
#	Open with KView.desktop \
#	Open with KWrite.desktop \
#	Open with Opera.desktop \
#	Open with ShowImg.desktop \
#	Print file.desktop \
#	Run within KDE.desktop \
#	Set as tiled wallpaper on desktop.desktop \
#	Set as wallpaper on desktop.desktop \
#	View in hexadecimal mode.desktop

install-data-local:
	$(mkinstalldirs)  $(DESTDIR)/$(kde_datadir)/ksquirrel/desktop
	$(INSTALL_DATA)   "Open with GIMP.desktop"                     "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Open with GIMP.desktop"
	$(INSTALL_DATA)   "Open with GQview.desktop"                   "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Open with GQview.desktop"
	$(INSTALL_DATA)   "Open with Konqueror.desktop"                "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Open with Konqueror.desktop"
	$(INSTALL_DATA)   "Open with KuickShow.desktop"                "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Open with KuickShow.desktop"
	$(INSTALL_DATA)   "Open with KView.desktop"                    "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Open with KView.desktop"
	$(INSTALL_DATA)   "Open with KWrite.desktop"                   "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Open with KWrite.desktop"
	$(INSTALL_DATA)   "Open with Opera.desktop"                    "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Open with Opera.desktop"
	$(INSTALL_DATA)   "Open with ShowImg.desktop"                  "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Open with ShowImg.desktop"
	$(INSTALL_DATA)   "Print file.desktop"                         "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Print file.desktop"
	$(INSTALL_DATA)   "Run within KDE.desktop"                     "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Run within KDE.desktop"
	$(INSTALL_DATA)   "Set as tiled wallpaper on desktop.desktop"  "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Set as tiled wallpaper on desktop.desktop"
	$(INSTALL_DATA)   "Set as wallpaper on desktop.desktop"        "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/Set as wallpaper on desktop.desktop"
	$(INSTALL_DATA)   "View in hexadecimal mode.desktop"           "$(DESTDIR)/$(kde_datadir)/ksquirrel/desktop/View in hexadecimal mode.desktop"

dist-hook:
	tar zxfv $(distdir)/desktops.tar.gz -C $(distdir)/
