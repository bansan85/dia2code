#!/bin/bash
#
# reconfig.sh
# Purpose:
#   Run this script when your automake/autoconf versions differ from the
#   ones expected by dia2code.

autoheader
aclocal -I m4

# macOS has a Mach-O version of libtool installed. Thus MacPorts and Homebrew use the prefix 'g'.
case `uname` in Darwin*) glibtoolize --copy ;;
  *) libtoolize --copy ;; esac

automake --add-missing --copy
autoconf
