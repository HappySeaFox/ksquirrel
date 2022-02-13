####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = ksquirrel

## INCLUDES were found outside kdevelop specific part

ksquirrel_SOURCES = sq_progress.cpp sq_thumbnailsize.cpp sq_thumbnailjob.cpp sq_pixmapcache.cpp sq_filethumbview.cpp sq_dir.cpp sq_iconlistbox.cpp sq_iconlistitem.cpp sq_widgetstack.cpp sq_treeview.cpp sq_tray.cpp sq_server.cpp sq_quickbrowser.cpp sq_options.ui sq_librarylistener.cpp sq_libraryhandler.cpp sq_librarieschanged.ui sq_imageproperties.ui sq_hloptions.cpp sq_glwidget.cpp sq_glview.cpp sq_glu.cpp sq_filters.ui sq_fileiconview.cpp sq_filedetailview.cpp sq_externaltools.ui sq_externaltool.cpp sq_diroperator.cpp sq_dirlister.cpp sq_decodingthread.cpp sq_config.cpp sq_bookmarkowner.cpp sq_archivehandler.cpp ksquirrel.cpp main.cpp 
ksquirrel_LDADD   = -lkio -lGL $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = main.cpp ksquirrel.cpp ksquirrel.h ksquirrel.desktop lo32-app-ksquirrel.png lo16-app-ksquirrel.png defs.h err.h sq_about.h sq_archivehandler.cpp sq_archivehandler.h sq_bookmarkowner.cpp sq_bookmarkowner.h sq_config.cpp sq_config.h sq_decodingthread.cpp sq_decodingthread.h sq_dirlister.cpp sq_dirlister.h sq_diroperator.cpp sq_diroperator.h sq_externaltool.cpp sq_externaltool.h sq_externaltools.ui sq_externaltools.ui.h sq_filedetailview.cpp sq_filedetailview.h sq_fileiconview.cpp sq_fileiconview.h sq_filters.ui sq_filters.ui.h sq_glu.cpp sq_glu.h sq_glview.cpp sq_glview.h sq_glwidget.cpp sq_glwidget.h sq_hloptions.cpp sq_hloptions.h sq_imageproperties.ui sq_imageproperties.ui.h sq_librarieschanged.ui sq_librarieschanged.ui.h sq_libraryhandler.cpp sq_libraryhandler.h sq_librarylistener.cpp sq_librarylistener.h sq_options.ui sq_options.ui.h sq_quickbrowser.cpp sq_quickbrowser.h sq_server.cpp sq_server.h sq_tray.cpp sq_tray.h sq_treeview.cpp sq_treeview.h sq_widgetstack.cpp sq_widgetstack.h sq_iconlistitem.cpp sq_iconlistitem.h sq_iconlistbox.cpp sq_iconlistbox.h sq_dir.cpp sq_dir.h sq_filethumbview.cpp sq_filethumbview.h sq_pixmapcache.cpp sq_pixmapcache.h sq_thumbnailjob.cpp sq_thumbnailjob.h sq_thumbnailsize.cpp sq_thumbnailsize.h sq_progress.cpp sq_progress.h 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/ksquirrel.desktop $(kde_appsdir)/Applications/ksquirrel.desktop
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-ksquirrel.png $(kde_icondir)/locolor/32x32/apps/ksquirrel.png
	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/lo16-app-ksquirrel.png $(kde_icondir)/locolor/16x16/apps/ksquirrel.png

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/ksquirrel.desktop
	-rm -f $(kde_icondir)/locolor/32x32/apps/ksquirrel.png
	-rm -f $(kde_icondir)/locolor/16x16/apps/ksquirrel.png

####### kdevelop will overwrite this part!!! (end)############
# These paths are KDE specific. Use them:
# kde_appsdir         Where your application's menu entry (.desktop) should go to.
# kde_icondir         Where your icon should go to - better use KDE_ICON.
# kde_sounddir        Where your sounds should go to.
# kde_htmldir         Where your docs should go to. (contains lang subdirs)
# kde_datadir         Where you install application data. (Use a subdir)
# kde_locale          Where translation files should go to. (contains lang subdirs)
# kde_cgidir          Where cgi-bin executables should go to.
# kde_confdir         Where config files should go to (system-wide ones with default values).
# kde_mimedir         Where mimetypes .desktop files should go to.
# kde_servicesdir     Where services .desktop files should go to.
# kde_servicetypesdir Where servicetypes .desktop files should go to.
# kde_toolbardir      Where general toolbar icons should go to (deprecated, use KDE_ICON).
# kde_wallpaperdir    Where general wallpapers should go to.
# kde_templatesdir    Where templates for the "New" menu (Konqueror/KDesktop) should go to.
# kde_bindir          Where executables should go to. Use bin_PROGRAMS or bin_SCRIPTS.
# kde_libdir          Where shared libraries should go to. Use lib_LTLIBRARIES.
# kde_moduledir       Where modules (e.g. parts) should go to. Use kde_module_LTLIBRARIES.
# kde_styledir        Where Qt/KDE widget styles should go to (new in KDE 3).
# kde_designerdir     Where Qt Designer plugins should go to (new in KDE 3).

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
ksquirrel_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# Uncomment the following two lines if you add a ui.rc file for your application to make use of
# KDE?s XML GUI builing
#rcdir = $(kde_datadir)/ksquirrel
#rc_DATA = ksquirrelui.rc

#WARNING: if you use a ui.rc file above, use:

# messages: rc.cpp

# instead of 

# messages:

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/ksquirrel.pot; \
	fi

