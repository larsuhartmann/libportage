/*
 * Copyright (c) 2009 Lars Hartmann All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <archives.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <archive.h>
#include <archive_entry.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

struct lparchive {
     struct archive *archive;
     int fd;
};

#ifdef __cplusplus
extern "C" {
#endif

static inline int
lparchive_reopen(lparchive_t *handle);

extern lparchive_t *
lparchive_new(void)
{
     lparchive_t *archive;
     archive = malloc(sizeof(lparchive_t));

     return archive;
}

extern void
lparchive_init(lparchive_t *handle)
{
     handle->archive = archive_read_new();
     archive_read_support_compression_all(handle->archive);
     archive_read_support_format_all(handle->archive);
     handle->fd = 0;
}

extern void
lparchive_reset(lparchive_t *handle)
{
     archive_read_finish(handle->archive);
     lparchive_init(handle);
}

extern void
lparchive_destroy(lparchive_t *handle)
{
     archive_read_finish(handle->archive);
     free(handle);
}

extern int
lparchive_open_fd(lparchive_t *handle, int fd)
{
     handle->fd = fd;
     archive_read_open_fd(handle->archive, fd, 1024);
     return 0;
}

extern int
lparchive_open_path(lparchive_t *handle, const char *path)
{
     int fd;
     
     if ( (fd = open(path, O_RDONLY)) == -1)
          return -1;
     return lparchive_open_fd(handle, fd);
}

extern char **
lparchive_get_entry_names(struct lparchive *handle)
{
     char **r, **t;
     size_t size=64;
     unsigned int i;
     struct archive *archive = handle->archive;
     struct archive_entry *entry= NULL;

     if ( (r = malloc(sizeof(char *)*size)) == NULL )
          return NULL;

     for (i=0; archive_read_next_header(archive, &entry) == ARCHIVE_OK; ++i) {
          if ( i == size ) {
               size <<=1;
               if ( (t = realloc(r, sizeof(char *)*size)) == NULL ) {
                    free(r);
                    lparchive_reopen(handle);
                    return NULL;
               }
          }
          r[i] = strdup(archive_entry_pathname(entry));
          archive_read_data_skip(archive);
     }
     if ( size > i+1 ) {
          if ( (t = realloc(r, sizeof(char *)*(i+1))) == NULL ) {
               free(r);
               lparchive_reopen(handle);
               return NULL;
          }
          r = t;
     }
     r[i] = NULL;
     lparchive_reopen(handle);

     return r;
}

extern int
lparchive_extract(lparchive_t *handle, char *path)
{
     char cwd[1024];
     unsigned int i;
     struct archive_entry *entry;
     struct archive *archive = handle->archive;

     if ( getcwd(cwd, 1024) == NULL )
          return -1;

     if ( chdir(path) == -1 )
          return -1;

     for (i=0; archive_read_next_header(archive, &entry) == ARCHIVE_OK; ++i) {
          if ( archive_read_extract(archive, entry, 0) != ARCHIVE_OK )
               return -1;
     }
     if ( chdir(cwd) == -1 )
          return -1;
     
     return 0;
}

static inline int
lparchive_reopen(lparchive_t *handle)
{
     archive_read_finish(handle->archive);
     handle->archive = archive_read_new();
     archive_read_support_compression_all(handle->archive);
     archive_read_support_format_all(handle->archive);

     return lparchive_open_fd(handle, handle->fd);
}

#ifdef __cplusplus
extern }
#endif
