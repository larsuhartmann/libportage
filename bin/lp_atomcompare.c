/*
 * Copyright (c) 2009 Patrick Lauer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#include <stdio.h>
#include <atom.h>

int main(int argc, char** argv)
{
     lpatom_t *first;
     lpatom_t *second;
     int ret;

     first = lpatom_create();
     lpatom_init(first);
     if ( lpatom_parse(first, argv[1]) == -1 ) {
          printf("First argument is not a valid atom!\n");
          return 1;
     }
     second = lpatom_create();
     lpatom_init(second);
     if ( lpatom_parse(second, argv[2]) == -1 ) {
          printf("Second argument is not a valid atom!\n");
          return 1;
     }
     
     ret = lpatom_cmp(first, second);
     if ( ret == 0 )
          ret = 1;
     else if ( ret < 0 )
          ret = 0;
     else if ( ret > 0 )
          ret = 2;
     
     printf("%d\n", ret);
     return 0;
}
