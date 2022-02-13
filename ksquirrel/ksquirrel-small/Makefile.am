bin_PROGRAMS = ksquirrel-small

ksquirrel_small_SOURCES = sq_imageproperties.ui sq_helpwidget.ui main.cpp sq_hloptions.cpp sq_glview.cpp sq_glwidget.cpp sq_libraryhandler.cpp sq_librarylistener.cpp sq_config.cpp sq_glu.cpp fileio.cpp sq_errorstring.cpp sq_iconloader.cpp sq_glwidget_helpers.cpp

EXTRA_DIST = sq_imageproperties.ui.h sq_helpwidget.ui.h

ksquirrel_small_LDADD   = $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET) 

ksquirrel_small_LDFLAGS = $(all_libraries) $(KDE_RPATH)

AM_CPPFLAGS = -DSQ_SMALL

INCLUDES= $(all_includes) -I../..

METASOURCES = AUTO

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/ksquirrel.pot; \
	fi
