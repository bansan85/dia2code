#!/bin/bash
#
# reconfig.sh
# Purpose:
#   Run this script when your automake/autoconf versions differ from the
#   ones expected by dia2code.

autoheader
aclocal -I m4
libtoolize
automake --add-missing --copy
autoconf
