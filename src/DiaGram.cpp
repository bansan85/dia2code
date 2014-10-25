/*
Copyright (C) 2014-2014

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "config.h"

#include "DiaGram.hpp"

DiaGram::DiaGram () :
  license ()
{
}


char *
DiaGram::getLicense ()
{
    return license.c_str ();
}


void
DiaGram::setLicense (char * lic)
{
    license.assign (lic);

    return;
}


DiaGram::~DiaGram ()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
