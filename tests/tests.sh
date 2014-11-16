#!/bin/sh

mkdir result
../src/dia2code -t cpp package.dia -d result
diff result/pack.hpp pack.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
exit 0
