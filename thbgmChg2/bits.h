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

#ifndef BAFFE_BITS_H
#define BAFFE_BITS_H

#include <vector>

class bitreader
{
public:
   bitreader(unsigned char *arr, int len): p(arr), end(arr + len), mask(0) {}

   // Med 4 byte (30 bitar) extra nollor behs aldrig l舅gdkontrollen.
   /*bool*/ unsigned char bit()
   {
      if (!mask)
      {
         if (p < end)
            byte = *p++;
         else
            byte = 0;
         mask = 128;
      }
      unsigned char ret = byte & mask;
      mask >>= 1;
      return ret;
   }

   int bits(int count)
   {
      int res = 0;
      int mask = (1 << count) >> 1;
      while (mask)
      {
         res |= (bit() ? mask: 0);
         mask >>= 1;
      }
      return res;
   }

private:
   unsigned char *p;
   unsigned char *end;
   unsigned char mask;
   unsigned char byte;
};

class bitwriter
{
public:
   bitwriter(std::vector<unsigned char> &vec): vec(vec), byte(0), mask(128) {}
   void bit(/*bool*/ int b)
   {
      byte |= (b ? mask : 0);
      mask >>= 1;
      if (!mask)
      {
         vec.push_back(byte);
         byte = 0;
         mask = 128;
      }
   }

   void bits(int count, int v)
   {
      int mask = (1 << count) >> 1;
      while (mask)
      {
         bit(v & mask);
         mask >>= 1;
      }
   }

   void finish()
   {
      if (mask != 128) vec.push_back(byte);
   }

private:
   std::vector<unsigned char> &vec;
   unsigned char byte;
   unsigned char mask;
};

#endif
