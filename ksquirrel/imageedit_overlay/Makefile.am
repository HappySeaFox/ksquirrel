####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libimageedit_overlay.a

INCLUDES = $(all_includes)

libimageedit_overlay_a_METASOURCES = AUTO

libimageedit_overlay_a_SOURCES = sq_filter.cpp sq_rotater.cpp sq_resizer.cpp sq_printer.cpp sq_editbase.cpp sq_converter.cpp sq_contraster.cpp 


EXTRA_DIST = sq_contraster.cpp sq_contraster.h sq_converter.cpp sq_converter.h sq_editbase.cpp sq_editbase.h sq_imageedit.h sq_printer.cpp sq_printer.h sq_resizer.cpp sq_resizer.h sq_rotater.cpp sq_rotater.h sq_filter.cpp sq_filter.h 

####### kdevelop will overwrite this part!!! (end)############

INCLUDES += -I.. -I../imageedit_dlg

messages:
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/ksquirrel.pot; \
	fi
