#!/bin/sh

mkdir result
rm result/*
../src/dia2code -t cpp package.dia -d result
if [[ ! -a result/pack.hpp ]] ; then
exit 1
fi ;
diff -pu pack.hpp result/pack.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp enum.dia -d result
if [[ ! -a result/enumeration.hpp ]] ; then
exit 1
fi ;
diff -pu enumeration.hpp result/enumeration.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp operations.dia -d result
if [[ ! -a result/operations.hpp ]] ; then
exit 1
fi ;
diff -pu operations.hpp result/operations.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm result/operations.hpp
../src/dia2code -t cpp operations.dia -d result -l COPYING
if [[ ! -a result/operations.hpp ]] ; then
exit 1
fi ;
diff -pu operations_COPYING.hpp result/operations.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp attributes.dia -d result
if [[ ! -a result/attributes.hpp ]] ; then
exit 1
fi ;
diff -pu attributes.hpp result/attributes.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp dependances.dia -d result
if [[ ! -a result/dependances.hpp ]] ; then
exit 1
fi ;
diff -pu dependances.hpp result/dependances.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp dependances_pack.dia -d result
if [[ ! -a result/ClassePack1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/ClassePack2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/ClassePack3.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/ClassePack4.hpp ]] ; then
exit 1
fi ;
diff -pu ClassePack1.hpp result/ClassePack1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu ClassePack2.hpp result/ClassePack2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu ClassePack3.hpp result/ClassePack3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu ClassePack4.hpp result/ClassePack4.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;


exit 0
