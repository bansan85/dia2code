#!/bin/sh

mkdir result
rm result/*
../src/dia2code -t cpp package.dia -d result
if [[ ! -a result/pack.hpp ]] ; then
exit 1
fi ;
diff -pu result/pack.hpp pack.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp enum.dia -d result
if [[ ! -a result/enumeration.hpp ]] ; then
exit 1
fi ;
diff -pu result/enumeration.hpp enumeration.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp operations.dia -d result
if [[ ! -a result/operations.hpp ]] ; then
exit 1
fi ;
diff -pu result/operations.hpp operations.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;


exit 0
