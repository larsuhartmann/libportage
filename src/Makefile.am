lib_LTLIBRARIES = libportage.la

libportage_la_SOURCES = liblpatom.c liblputil.c liblpxpak.c liblparchives.c   \
			liblpversion.c
libportage_la_LIBADD = ../replace/libreplace.la
libportage_la_LDFLAGS = -larchive

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/replace
METASOURCES = AUTO

# dev-targets
check-syntax:
	$(COMPILE) -o nul -Wall -c -S ${CHK_SOURCES}
