####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libimageedit_dlg.a

INCLUDES = $(all_includes)

libimageedit_dlg_a_METASOURCES = AUTO

libimageedit_dlg_a_SOURCES = sq_pushbutton.cpp sq_imagefilter.ui sq_label.cpp sq_printpanelbase.cpp sq_printpanelframe.cpp sq_printpanel.cpp sq_bcglabel.cpp sq_writeoption.ui sq_imagerotate.ui sq_imageresize.ui sq_imageprint.ui sq_imageeditsplash.ui sq_imageeditoptions.ui sq_imageconvert.ui sq_imagebcg.ui 


EXTRA_DIST = sq_imagebcg.ui sq_imageconvert.ui sq_imageeditoptions.ui sq_imageeditsplash.ui sq_imageprint.ui sq_imageresize.ui sq_imagerotate.ui sq_writeoption.ui sq_bcglabel.cpp sq_bcglabel.h sq_printpanel.cpp sq_printpanelframe.cpp sq_printpanelframe.h sq_printpanel.h sq_printpanelbase.cpp sq_printpanelbase.h sq_label.cpp sq_label.h sq_imagefilter.ui sq_pushbutton.cpp sq_pushbutton.h 

####### kdevelop will overwrite this part!!! (end)############

INCLUDES += -I.. -I../imageedit_overlay

messages:
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/ksquirrel.pot; \
	fi
