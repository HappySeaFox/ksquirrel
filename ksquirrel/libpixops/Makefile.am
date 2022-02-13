noinst_LIBRARIES = libpixops.a

if USE_MMX
AM_CCASFLAGS = -mmmx
mmx_sources =				\
	have_mmx.S			\
	scale_line_22_33_mmx.S 		\
	composite_line_22_4a4_mmx.S 	\
	composite_line_color_22_4a4_mmx.S
endif

libpixops_a_SOURCES =  			\
	pixops.c			\
	pixops.h			\
	pixops-internal.h		\
	$(mmx_sources)

EXTRA_DIST =				\
	DETAILS				\
	README				\
	composite_line_22_4a4_mmx.S     \
	composite_line_color_22_4a4_mmx.S  \
	have_mmx.S  			\
	scale_line_22_33_mmx.S		\
	pixops.c  			\
	pixops.h  			\
	pixops-internal.h
