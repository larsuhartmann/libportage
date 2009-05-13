#!/bin/sh

get_version()
{
    version=`git describe 2>&1`

    case "$version" in
        fatal:*) version=`date +%Y%m%d`;;
        *) ;;
    esac

    return
}

build_release()
{
    if ! autoreconf -i; then
        return 1
    fi
    if ! ./configure; then
        return 1
    fi
    if ! make dist dist-bzip2 dist-lzma; then
        return 1
    fi
    return 0
}

cleanup()
{
    mv configure.ac.bak configure.ac
    touch configure.ac
    make clean
}

get_version
sed -i.bak configure.ac -e "s/\[HEAD\]/\[$version\]/"

if ! build_release; then
    cleanup
    echo "#################"
    echo "# BUILD FAILED! #"
    echo "#################"

    exit 1
else
    cleanup
    echo "##################"
    echo "# BUILD SUCCEED! #"
    echo "##################"
    echo "Version: $version"
    echo "Distrib: libportage-$version.tar.*"

    exit 0
fi
