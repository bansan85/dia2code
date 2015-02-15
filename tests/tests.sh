#!/bin/sh

generators="cpp cpp11 csharp java"
res="true"

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
        diff -pu $file.$gen result > $file.$gen.result.txt
        if [[ ! $? -eq 0 ]] ; then
            echo $file.$gen FAILED
            res="false"
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
            diff -pu $file.$gen"$extra" result > $file.$gen"$extra".result.txt
            if [[ ! $? -eq 0 ]] ; then
                echo $file.$gen"$extra" FAILED
                res="false"
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
    diff -pu operations.dia.$gen"_COPYING" result > operations.dia.$gen"_COPYING"result.txt
    if [[ ! $? -eq 0 ]] ; then
        echo operations.dia.$gen"_COPYING" FAILED
        res="false"
    fi ;
    echo ""
done

if [[ $res == "false" ]] ; then
    exit 1
fi;

exit 0
