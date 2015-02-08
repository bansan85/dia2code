#!/bin/sh

generators="cpp cpp11 java"

if [[ ! -a ../src/dia2code ]] ; then
echo "Failed to found dia2code."
exit 1
fi ;
mkdir result
for file in *.dia
do
    for gen in $generators
    do
        rm -Rf result/*
        echo "dia2code -t $gen $file -d result"
        ../src/dia2code -t $gen $file -d result || exit 1
        diff -pu $file.$gen result > result.txt
        if [[ $? -eq 1 ]] ; then
            exit 1
        fi ;
        echo ""
    done
done

extras=("-1" "--buildtree")
for extra in ${extras[*]}
do
    for file in "association.dia" "package.dia" "dependances_pack.dia" "inheritance.dia"
    do
        for gen in $generators
        do
            rm -Rf result/*
            echo "dia2code -t $gen $file -d result $extra"
            ../src/dia2code -t $gen $file -d result $extra || exit 1
            diff -pu $file.$gen"$extra" result > result.txt
            if [[ $? -eq 1 ]] ; then
                exit 1
            fi ;
            echo ""
        done
    done
done

for gen in $generators
do
    rm -Rf result/*
    echo "dia2code -t $gen operations.dia -d result -l COPYING"
    ../src/dia2code -t $gen operations.dia -d result -l COPYING || exit 1
    diff -pu operations.dia.$gen"_COPYING" result > result.txt
    if [[ $? -eq 1 ]] ; then
        exit 1
    fi ;
    echo ""
done

exit 0
