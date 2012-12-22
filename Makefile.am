## Process this file with automake to produce Makefile.in

SUBDIRS = src 

libghwpdocdir = ${prefix}/doc/libghwp
libghwpdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(libghwpdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(libghwpdocdir)
