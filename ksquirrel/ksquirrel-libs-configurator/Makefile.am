INCLUDES = $(all_includes) -I$(srcdir)

bin_PROGRAMS = ksquirrel-libs-configurator-real
bin_SCRIPTS  = ksquirrel-libs-configurator

ksquirrel_libs_configurator_real_SOURCES = main.cpp klc.ui
ksquirrel_libs_configurator_real_LDADD   = $(KDE_LDFLAGS) $(QT_LDFLAGS) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

AM_CXXFLAGS = $(ksquirrellibs_CFLAGS)

messages:
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/ksquirrel.pot; \
	fi
