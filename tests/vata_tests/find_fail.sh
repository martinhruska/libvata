#!/bin/sh
# Start from root dir libvata with some automata paths and script
# will show correct and wrong results. At the end it writes number
# of correctly computed and number of failed tests.

success=0
fail=0
for file in "$@"
do
    if [[ $file == *result  ]]
    then
        continue
    fi

    echo $file
    correct_result=$(mktemp)
    test_result=$(mktemp)

    build/cli/vata -r bdd-td -o bdd=spec sim $file > $correct_result
    build/cli/vata -r expl sim $file > $test_result
    python tests/vata_tests/compare_simulations.py $correct_result $test_result
    if [ $? -eq "0" ] ;
    then
        success=$(( success+1 ))
    else
        fail=$(( fail+1 ))
    fi

    rm $correct_result $test_result
done

echo "Succeded: ", $success, ", Failed: ", $fail

