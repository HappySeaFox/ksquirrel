####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = ksquirrel

## INCLUDES were found outside kdevelop specific part

ksquirrel_SOURCES = sq_bookmarkowner.cpp sq_dirlister.cpp sq_zfstream.cpp sq_cachehandler.cpp sq_filters.ui sq_config.cpp sq_glhelpwidget.cpp sq_externaltool.cpp sq_fileviewfilter.cpp sq_externaltools.ui sq_widgetstack.cpp sq_treeview.cpp sq_tray.cpp sq_specialpage.cpp sq_runprocess.cpp sq_options.ui sq_librarylistener.cpp sq_libraryhandler.cpp sq_librarieschanged.ui sq_glviewwidget.cpp sq_fileiconview.cpp sq_filedetailview.cpp sq_diroperator.cpp sq_application.cpp ksquirrel.cpp main.cpp 
ksquirrel_LDADD   = -lGLU -lkio $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = main.cpp ksquirrel.cpp ksquirrel.h ksquirrel.desktop lo32-app-ksquirrel.png lo16-app-ksquirrel.png defs.h sq_about.h sq_application.cpp sq_application.h sq_diroperator.cpp sq_diroperator.h sq_filedetailview.cpp sq_filedetailview.h sq_fileiconview.cpp sq_fileiconview.h sq_glviewwidget.cpp sq_glviewwidget.h sq_librarieschanged.ui sq_librarieschanged.ui.h sq_libraryhandler.cpp sq_libraryhandler.h sq_librarylistener.cpp sq_librarylistener.h sq_options.ui sq_options.ui.h sq_runprocess.cpp sq_runprocess.h sq_specialpage.cpp sq_specialpage.h sq_tray.cpp sq_tray.h sq_treeview.cpp sq_treeview.h sq_version sq_version.h sq_widgetstack.cpp sq_widgetstack.h sq_externaltools.ui sq_fileviewfilter.cpp sq_fileviewfilter.h sq_externaltool.cpp sq_externaltool.h sq_glhelpwidget.cpp sq_glhelpwidget.h sq_config.cpp sq_config.h sq_filters.ui sq_cachehandler.cpp sq_cachehandler.h sq_zfstream.cpp sq_zfstream.h sq_dirlister.cpp sq_dirlister.h view_gqview.png view_kuickshow.png view_squirrel.png view_winviewer.png view_xnview.png drag.png usual.png sq_bookmarkowner.cpp sq_bookmarkowner.h 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/ksquirrel.desktop $(kde_appsdir)/Applications/ksquirrel.desktop
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-ksquirrel.png $(kde_icondir)/locolor/32x32/apps/ksquirrel.png
	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/lo16-app-ksquirrel.png $(kde_icondir)/locolor/16x16/apps/ksquirrel.png
	$(mkinstalldirs) /usr/share/apps/ksquirrel/images/
	$(INSTALL_DATA) $(srcdir)/view_gqview.png /usr/share/apps/ksquirrel/images/view_gqview.png
	$(mkinstalldirs) /usr/share/apps/ksquirrel/images/
	$(INSTALL_DATA) $(srcdir)/view_kuickshow.png /usr/share/apps/ksquirrel/images/view_kuickshow.png
	$(mkinstalldirs) /usr/share/apps/ksquirrel/images/
	$(INSTALL_DATA) $(srcdir)/view_squirrel.png /usr/share/apps/ksquirrel/images/view_squirrel.png
	$(mkinstalldirs) /usr/share/apps/ksquirrel/images/
	$(INSTALL_DATA) $(srcdir)/view_winviewer.png /usr/share/apps/ksquirrel/images/view_winviewer.png
	$(mkinstalldirs) /usr/share/apps/ksquirrel/images/
	$(INSTALL_DATA) $(srcdir)/view_xnview.png /usr/share/apps/ksquirrel/images/view_xnview.png
	$(mkinstalldirs) /usr/share/apps/ksquirrel/cursors/
	$(INSTALL_DATA) $(srcdir)/drag.png /usr/share/apps/ksquirrel/cursors/drag.png
	$(mkinstalldirs) /usr/share/apps/ksquirrel/cursors/
	$(INSTALL_DATA) $(srcdir)/usual.png /usr/share/apps/ksquirrel/cursors/usual.png

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/ksquirrel.desktop
	-rm -f $(kde_icondir)/locolor/32x32/apps/ksquirrel.png
	-rm -f $(kde_icondir)/locolor/16x16/apps/ksquirrel.png
	-rm -f /usr/share/apps/ksquirrel/images/view_gqview.png
	-rm -f /usr/share/apps/ksquirrel/images/view_kuickshow.png
	-rm -f /usr/share/apps/ksquirrel/images/view_squirrel.png
	-rm -f /usr/share/apps/ksquirrel/images/view_winviewer.png
	-rm -f /usr/share/apps/ksquirrel/images/view_xnview.png
	-rm -f /usr/share/apps/ksquirrel/cursors/drag.png
	-rm -f /usr/share/apps/ksquirrel/cursors/usual.png

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

