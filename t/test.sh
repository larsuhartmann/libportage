#!/bin/sh
do_tests()
{
    cd "$i"
    # get no. of testfiles
    testno=`find -perm /u+x,g+x,o+x -iname '*.test'|wc -l`

    count=0
    printf "%s %d..%d" "$1" "$count" "$testno"
    # iterate over every file that ends with ".test"
    for j in ./*.test
    do
        # check if this is a executable file
        if [ -x "$j" ]
        then
            # increase counter
            count=$(( $count + 1 ))
            # overwrite status line
            printf "\r%s %d..%d" "$1" "$count" "$testno"
            # test if $j returned with exit status != 0
            # stdout and stderr of the test will be written to $j.out
            if ! ./"$j">"$j.out" 2>"$j.out"
            then
                # if a test fails
                echo " not ok"
                echo "output of $j:"
                # print out the stdout/stderr of the failing test
                cat "$j.out"
                rm "$j.out"
                cd ..
                exit 1
            fi
        fi
        # remove tempfile we used for storing stdout/stderr of the test
        rm "$j.out"
        echo " ok"
    done
    cd ..
}

# interate over all directories in current dir.
for i in *
do
    # check if this is a directory
    if test -d "$i"
    then
        # execute tests in directory $i
        do_tests $i
    fi
done
exit 0
