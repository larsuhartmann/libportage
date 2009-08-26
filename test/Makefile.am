METASOURCES = AUTO

TESTS = 01_lputil_intlen 01_lputil_int64len 01_lputil_splitstr	\
02_lpversion_parse 03_lpatom_parse 04_lpxpak 05_lparchives

check_PROGRAMS = $(TESTS)

01_lputil_intlen_SOURCES = 01_lputil_intlen.c
01_lputil_intlen_LDFLAGS = $(all_libraries)
01_lputil_intlen_LDADD = $(top_srcdir)/src/libportage.la

01_lputil_int64len_SOURCES = 01_lputil_int64len.c
01_lputil_int64len_LDFLAGS = $(all_libraries)
01_lputil_int64len_LDADD = $(top_srcdir)/src/libportage.la

01_lputil_splitstr_SOURCES = 01_lputil_splitstr.c 01_lputil_splitstr.txt
01_lputil_splitstr_LDFLAGS = $(all_libraries)
01_lputil_splitstr_LDADD = $(top_srcdir)/src/libportage.la

02_lpversion_parse_SOURCES = 02_lpversion_parse.c 02_lpversion_parse.txt
02_lpversion_parse_LDFLAGS = $(all_libraries)
02_lpversion_parse_LDADD = $(top_srcdir)/src/libportage.la

03_lpatom_parse_SOURCES = 03_lpatom_parse.c 03_lpatom_parse.txt
03_lpatom_parse_LDFLAGS = $(all_libraries)
03_lpatom_parse_LDADD = $(top_srcdir)/src/libportage.la

04_lpxpak_SOURCES = 04_lpxpak.c 04_lpxpak.tbz2
04_lpxpak_LDFLAGS = $(all_libraries)
04_lpxpak_LDADD = $(top_srcdir)/src/libportage.la

05_lparchives_SOURCES = 05_lparchives.c 05_lparchives.tbz2 05_lparchives.txt
05_lparchives_LDFLAGS = $(all_libraries)
05_lparchives_LDADD = $(top_srcdir)/src/libportage.la

AM_CPPFLAGS = -I$(top_srcdir)/include

# dev-targets
check-syntax:
	$(COMPILE) -o nul -Wall -c -S ${CHK_SOURCES}
