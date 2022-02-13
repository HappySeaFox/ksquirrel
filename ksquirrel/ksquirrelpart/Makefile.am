INCLUDES = $(all_includes) -I$(srcdir)

# These are not really libraries, but modules dynamically opened.
# So they should be installed in kde_module_dir, which is usually $kde_prefix/lib/kde3
kde_module_LTLIBRARIES = libksquirrelpart.la

AM_CXXFLAGS = -DKSQUIRREL_PART

if SQ_HAVE_KEXIF
AM_CXXFLAGS += $(libkexif_CFLAGS)
endif

AM_CCASFLAGS = -I$(srcdir) $(KSQUIRREL_ASM_DEFS)

libksquirrelpart_la_SOURCES = ksquirrelpart.cpp sq_glparts.cpp sq_glu.cpp sq_glwidget.cpp sq_glwidget_stuff.cpp sq_libraryhandler.cpp sq_downloader.cpp sq_iconloader.cpp fmt_filters.cpp sq_externaltool.cpp sq_config.cpp sq_filedialog.cpp sq_imagefilter.ui sq_imagebcg.ui sq_glselectionpainter.cpp sq_glwidget_helpers.cpp sq_label.cpp sq_bcglabel.cpp sq_popupmenu.cpp sq_codecsettingsskeleton.ui sq_codecsettings.cpp sq_imageproperties.ui sq_utils.cpp sq_helpwidget.ui sq_utils_asm_scale.S sq_utils_scale.cpp sq_diroperator.cpp sq_glview.cpp sq_errorstring.cpp
libksquirrelpart_la_LIBADD = $(LIB_KPARTS)
libksquirrelpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(ksquirrellibs_LIBS) $(LIB_KDEPRINT) -lqui -lkonq

if SQ_HAVE_KEXIF
libksquirrelpart_la_LDFLAGS += $(libkexif_LIBS)
endif

METASOURCES = AUTO

ksquirreldir = $(kde_datadir)/ksquirrelpart
ksquirrel_DATA = ksquirrelpart.rc

kde_services_DATA = ksquirrelpart.desktop
