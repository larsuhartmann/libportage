#!/bin/sh

# Copyright (c) 2009 Lars Hartmann All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#    1. Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# get version string.
get_version()
{
    if [ ! "$RELEASE" ]; then
        RELEASE=`date +%Y%m%d`
    fi
    return
}

# set version in configure.ac
set_version()
{
    sed -i.bak configure.ac -e "s/\[HEAD\]/\[$1\]/"
}

# build release distribution archives
config_release()
{
    if ! autoreconf -i; then
        return 1
    fi
    if ! ./configure; then
        return 1
    fi
    return 0
}

# build gzip distribution
build_gzip()
{
    if ! make dist; then
        return 1
    fi
    return 0
}

# build bzip2 distribution
build_bzip()
{
    if ! make dist-bzip2; then
        return 1
    fi
    return 0
}

# build zip distribution
build_zip()
{
    if ! make dist-zip; then
        return 1
    fi
    return 0
}

# build lzma distribution
build_lzma()
{
    if ! make dist-lzma; then
        return 1
    fi
    return 0
}

# clean up the mess
cleanup()
{
    mv configure.ac.bak configure.ac
    touch configure.ac
    make clean
}

# print out help message
print_help()
{
    cat <<EOF
Usage: $0 [options]
Options:
  -r <version>  Set version to <version>
  -s <key-id>   GPG-Sign release with key <key-id>
  -b            make a bzip2 release
  -g            make a gzip release
  -z            make a zip release
  -l            make a lzma release
  -h            Display this information
EOF
}

#parse command line arguments
while getopts bghlr:s:z arg; do
    case "$arg" in
        s)      GPG_KEY="$OPTARG";;
        r)      RELEASE="$OPTARG";;
        b)      BZIP="t";;
        g)      GZIP="t";;
        l)      LZMA="t";;
        z)      ZIP="t";;
        h|?)    print_help; exit 0;;
    esac
done

get_version

if [ -n "$GPG_KEY" ]; then
    echo "GPG-Key: $GPG_KEY";
fi

if [ "$GZIP" = "t" ]||[ "$BZIP" = "t" ]||[ "$LZMA" = "t" ]||[ "$ZIP" = "t" ]; then
    set_version $RELEASE
    if ! config_release; then
        cleanup
        echo "an error has occured!"
        exit 1
    fi

    if [ "$GZIP" eq "true" ]; then
        if ! build_gzip; then
            cleanup
            echo "an error has occured!"
            exit 1
        fi
    fi
    if [ "$BZIP" eq "true" ]; then
        if ! build_bzip2; then
            cleanup
            echo "an error has occured!"
            exit 1
        fi
    fi
    if [ "$LZMA" eq "true" ]; then
        if ! build_lzma; then
            cleanup
            echo "an error has occured!"
            exit 1
        fi
    fi
    if [ "$ZIP" eq "true" ]; then
        if ! build_zip; then
            cleanup
            echo "an error has occured!"
            exit 1
        fi
    fi
    cleanup

    # sign packages if -s was given
    if [ -n "$GPG_KEY" ]; then
        for i in libportage-$RELEASE.*; do
            gpg -sau "$GPG_KEY" $i
        done
    fi

    echo ""
    echo "Release: $RELEASE"
    echo "Files:"
    for i in libportage-$RELEASE.tar.*; do
        echo "  $i"
    done
    
    exit 0
fi
print_help
exit 0
