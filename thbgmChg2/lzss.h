/*
Touhou English translation patch collection
Copyright (C) 2005 Simon El駭

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

#ifndef BAFFE_LZSS_H
#define BAFFE_LZSS_H

#include <vector>
#include <memory>

bool lzss_decompress(unsigned char *src, int src_len, unsigned char *dest, int dest_len);
std::auto_ptr<std::vector<unsigned char> > lzss_compress(unsigned char *src, int src_len);

#endif
