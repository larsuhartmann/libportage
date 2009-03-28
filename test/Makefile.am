METASOURCES = AUTO

if USE_VALGRIND
    TESTS_ENVIRONMENT=./valgrind-wrapper
endif

TESTS = 01_lputil_intlen 01_lputil_splitstr 02_lpatom_parse 03_lpxpak	\
04_lparchives

check_PROGRAMS = $(TESTS)

lp_objects = ../src/liblputil.o ../src/liblpatom.o ../src/liblpxpak.o	\
../src/liblparchives.o

01_lputil_intlen_SOURCES = 01_lputil_intlen.c
01_lputil_intlen_LDFLAGS = $(all_libraries)
01_lputil_intlen_LDADD = $(lp_objects)

01_lputil_splitstr_SOURCES = 01_lputil_splitstr.c 01_lputil_splitstr.txt
01_lputil_splitstr_LDFLAGS = $(all_libraries)
01_lputil_splitstr_LDADD = $(lp_objects)

02_lpatom_parse_SOURCES = 02_lpatom_parse.c 02_lpatom_parse.txt
02_lpatom_parse_LDFLAGS = $(all_libraries)
02_lpatom_parse_LDADD = $(lp_objects)

03_lpxpak_SOURCES = 03_lpxpak.c 03_lpxpak.tbz2
03_lpxpak_LDFLAGS = $(all_libraries)
03_lpxpak_LDADD = $(lp_objects)

04_lparchives_SOURCES = 04_lparchives.c 04_lparchives.tbz2 04_lparchives.txt
04_lparchives_LDFLAGS = $(all_libraries)
04_lparchives_LDADD = $(lp_objects)

AM_CPPFLAGS = -I$(top_srcdir)/include

EXTRA_DIST = valgrind-wrapper

# dev-targets
check-syntax:
	$(COMPILE) -o nul -Wall -c -S ${CHK_SOURCES}
