#!/bin/bash

autoheader
aclocal -I m4
libtoolize
automake --add-missing --copy
autoconf
