####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = ksquirrel

## INCLUDES were found outside kdevelop specific part

ksquirrel_SOURCES = sq_glselectionpainter.cpp sq_slideshowwidget.cpp sq_glwidget.cpp sq_glparts.cpp sq_codecsettings.cpp sq_filedialog.cpp sq_codecsettingsskeleton.ui sq_kipimanager.cpp sq_kipiinterface.cpp sq_progressbox.cpp sq_thumbnailloadjob.cpp sq_splashscreen.cpp sq_popupmenu.cpp sq_glinfo.ui sq_glwidget_helpers.cpp sq_imageloader.cpp sq_pluginsinfo.ui sq_iconloader.cpp sq_slideshow.ui sq_selectdeselectgroup.ui sq_thumbnailcachemaster.ui fmt_filters.cpp sq_errorstring.cpp sq_navigatordropmenu.cpp sq_fileiconviewbase.cpp sq_helpwidget.ui sq_glview.cpp sq_viewcache.ui sq_filethumbviewitem.cpp sq_progress.cpp sq_thumbnailsize.cpp sq_pixmapcache.cpp sq_filethumbview.cpp sq_dir.cpp sq_iconlistbox.cpp sq_iconlistitem.cpp sq_widgetstack.cpp sq_tray.cpp sq_options.ui sq_libraryhandler.cpp sq_imageproperties.ui sq_hloptions.cpp sq_glu.cpp sq_filters.ui sq_fileiconview.cpp sq_filedetailview.cpp sq_externaltools.ui sq_externaltool.cpp sq_diroperator.cpp sq_config.cpp sq_bookmarkowner.cpp sq_archivehandler.cpp ksquirrel.cpp main.cpp 
ksquirrel_LDADD   = ./sidebar/libsidebar.a ./imageedit/libimageedit.a $(LIB_KUTILS) $(LIB_KIO) $(ksquirrellibs_LIBS) -Llibpixops -lpixops -lkonq -lqui $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = sidebar imageedit 

EXTRA_DIST = main.cpp ksquirrel.cpp ksquirrel.h ksquirrel.desktop hi32-app-ksquirrel.png hi16-app-ksquirrel.png sq_about.h sq_archivehandler.cpp sq_archivehandler.h sq_bookmarkowner.cpp sq_bookmarkowner.h sq_config.cpp sq_config.h sq_diroperator.cpp sq_diroperator.h sq_externaltool.cpp sq_externaltool.h sq_externaltools.ui sq_filedetailview.cpp sq_filedetailview.h sq_fileiconview.cpp sq_fileiconview.h sq_filters.ui sq_glu.cpp sq_glu.h sq_hloptions.cpp sq_hloptions.h sq_imageproperties.ui sq_libraryhandler.cpp sq_libraryhandler.h sq_options.ui sq_tray.cpp sq_tray.h sq_widgetstack.cpp sq_widgetstack.h sq_iconlistitem.cpp sq_iconlistitem.h sq_iconlistbox.cpp sq_iconlistbox.h sq_dir.cpp sq_dir.h sq_filethumbview.cpp sq_filethumbview.h sq_pixmapcache.cpp sq_pixmapcache.h sq_thumbnailsize.cpp sq_thumbnailsize.h sq_progress.cpp sq_progress.h sq_thumbnailinfo.h sq_filethumbviewitem.cpp sq_filethumbviewitem.h sq_viewcache.ui sq_glview.cpp sq_glview.h sq_helpwidget.ui sq_fileiconviewbase.cpp sq_fileiconviewbase.h sq_navigatordropmenu.cpp sq_navigatordropmenu.h sq_errorstring.cpp sq_errorstring.h fmt_filters.cpp fmt_filters.h sq_thumbnailcachemaster.ui sq_glwidget_helpers.h sq_library.h sq_selectdeselectgroup.ui sq_slideshow.ui sq_iconloader.cpp sq_iconloader.h sq_pluginsinfo.ui sq_iconloader_pixmaps.h sq_imageloader.cpp sq_imageloader.h sq_glwidget_helpers.cpp file_broken.xpm sq_glinfo.ui sq_popupmenu.cpp sq_popupmenu.h sq_splashscreen.cpp sq_splashscreen.h sq_thumbnailloadjob.cpp sq_thumbnailloadjob.h sq_progressbox.cpp sq_progressbox.h sq_kipiinterface.cpp sq_kipiinterface.h sq_kipimanager.cpp sq_kipimanager.h sq_codecsettingsskeleton.ui sq_codecsettingsskeleton.ui.h sq_filedialog.cpp sq_filedialog.h sq_codecsettings.cpp sq_codecsettings.h sq_glparts.cpp sq_glparts.h sq_glwidget.cpp sq_glwidget.h sq_slideshowwidget.cpp sq_slideshowwidget.h sq_glselectionpainter.cpp sq_glselectionpainter.h 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Graphics/
	$(INSTALL_DATA) $(srcdir)/ksquirrel.desktop $(kde_appsdir)/Graphics/ksquirrel.desktop
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/hi32-app-ksquirrel.png $(kde_icondir)/hicolor/32x32/apps/ksquirrel.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/hi16-app-ksquirrel.png $(kde_icondir)/hicolor/16x16/apps/ksquirrel.png

uninstall-local:
	-rm -f $(kde_appsdir)/Graphics/ksquirrel.desktop
	-rm -f $(kde_icondir)/hicolor/32x32/apps/ksquirrel.png
	-rm -f $(kde_icondir)/hicolor/16x16/apps/ksquirrel.png

####### kdevelop will overwrite this part!!! (end)############

SUBDIRS  += libpixops
INCLUDES = $(all_includes) -I$(srcdir) -I$(top_srcdir)/ksquirrel/sidebar -I$(top_srcdir)/ksquirrel/imageedit
# ksquirrelpart

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

if SQ_HAVE_KIPI
AM_CXXFLAGS = ${libkipi_CFLAGS}
AM_LDFLAGS  = ${libkipi_LIBS}
endif

METASOURCES = AUTO

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
