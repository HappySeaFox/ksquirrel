# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = ksquirrel.h sq_about.h sq_diroperator.h sq_filedetailview.h sq_fileiconview.h sq_menu_proc.h sq_options.ui.h sq_page.h sq_specialpage.h sq_splash.h sq_tray.h sq_treeview.h sq_version sq_version.h xdirs.h sq_editpicturedialog.h sq_resizewidget.ui.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ksquirrel.pot

KDE_ICON = ksquirrel

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = ksquirrel

# the application source, library search path, and link libraries
ksquirrel_SOURCES = main.cpp ksquirrel.cpp sq_diroperator.cpp sq_filedetailview.cpp sq_fileiconview.cpp sq_menu_proc.cpp sq_options.ui sq_page.cpp sq_specialpage.cpp sq_splash.cpp sq_tray.cpp sq_treeview.cpp sq_editpicturedialog.cpp sq_resizewidget.ui
ksquirrel_LDFLAGS = $(KDE_RPATH) $(all_libraries)
ksquirrel_LDADD = $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = ksquirrel.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/ksquirrel
shellrc_DATA = ksquirrelui.rc

