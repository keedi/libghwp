## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(LIBGHWP_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = libghwp.la


libghwp_la_SOURCES = \
	ghwp-document.c \
	ghwp-file.c \
	ghwp-page.c \
	ghwp-parse.c \
	ghwp-tags.c \
	gsf-input-stream.c

libghwp_la_LDFLAGS = 

libghwp_la_LIBADD = $(LIBGHWP_LIBS)

ghwpincludedir = $(includedir)/ghwp

ghwpinclude_HEADERS = \
	ghwp.h \
	ghwp-document.h \
	ghwp-file.h \
	ghwp-page.h \
	gsf-input-stream.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ghwp-0.1.pc

EXTRA_DIST = \
	ghwp-0.1.pc.in
