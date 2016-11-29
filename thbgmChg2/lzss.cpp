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
#include "stdafx.h"
#pragma warning(disable: 4786)
#include "lzss.h"
#include "bits.h"
//#include <iostream>

using namespace std;

const int dict_bits = 13;
const int dict_size = 8192; // 2 ** dict_bits
const int dict_str_size = 18;

bool lzss_decompress(unsigned char *src, int src_len, unsigned char *dest, int dest_len)
{
   unsigned char dict[dict_size];
   int dict_wpos = 1;
   int dest_idx = 0;

   bitreader b(src, src_len);

   while (true)
   {
      if (b.bit())
      {
         //cout << "1 byte literal" << endl;
         unsigned char byte = b.bits(8);
         if (dest_idx >= dest_len) return false;
         dest[dest_idx++] = byte;
         dict[dict_wpos++] = byte;
         dict_wpos %= dict_size;
      }
      else
      {
         int dict_rpos = b.bits(dict_bits);
         if (!dict_rpos) break;
         int dict_rcount = b.bits(4) + 3;
         //cout << dict_rcount << " bytes from dict pos " << dict_rpos << endl;
         for(int i = 0; i < dict_rcount; ++i)
         {
            unsigned char byte = dict[dict_rpos++];
            dict_rpos %= dict_size;
            if (dest_idx >= dest_len) return false;
            dest[dest_idx++] = byte;
            dict[dict_wpos++] = byte;
            dict_wpos %= dict_size;
         }
      }
   }
   return true;
}

// ========================================================================

struct node
{
   node(): parent(0), left(0), right(0) {}
   node(int parent, int left, int right): parent(parent), left(left), right(right) {}
   int &branch(bool b)
   {
      return b ? left : right;
   }
   int parent;
   int left;
   int right;
};

struct tree
{
   node nodes[dict_size + 1];

   tree(int root)
   {
      nodes[dict_size].right = root;
      nodes[root].parent = dict_size;
      nodes[root].right = 0;
      nodes[root].left = 0;
   }

   int root()
   {
      return nodes[dict_size].right;
   }

   void rename_node(int old_pos, int new_pos)
   {
      int p = nodes[old_pos].parent;
      if (nodes[p].left == old_pos)
         nodes[p].left = new_pos;
      else
         nodes[p].right = new_pos;
      nodes[new_pos] = nodes[old_pos];
      nodes[nodes[new_pos].left].parent = new_pos;
      nodes[nodes[new_pos].right].parent = new_pos;
      nodes[old_pos].parent = 0;   
   }

   int prev_node(int n)
   {
      n = nodes[n].left;
      while (nodes[n].right) n = nodes[n].right;
      return n;
   }

   void replace_node(int old_node, int new_node)
   {
      nodes[new_node].parent = nodes[old_node].parent;
      if (nodes[nodes[old_node].parent].right == old_node)
         nodes[nodes[old_node].parent].right = new_node;
      else
         nodes[nodes[old_node].parent].left = new_node;
      nodes[old_node].parent = 0;
   }

   void delete_node(int n)
   {
      if (!nodes[n].parent) return;
      if (!nodes[n].right)
         replace_node(n, nodes[n].left);
      else if (!nodes[n].left)
         replace_node(n, nodes[n].right);
      else
      {
         int n_prev = prev_node(n);
         delete_node(n_prev);
         rename_node(n, n_prev);
      }
   }

};

// ========================================================================

inline void find_best_match(tree &t, unsigned char *dictionary, int dict_pos, int &match_pos, int &match_len)
{
   match_len = 0;
   if (!dict_pos) return;
   int test_pos = t.root();
   int chr_diff;
   int len;

   while (true)
   {
      for(len = 0; len < dict_str_size; ++len)
      {
         chr_diff = (dictionary[(dict_pos + len) % dict_size] -
                     dictionary[(test_pos + len) % dict_size]);
         if (chr_diff)
            break;
      }
      if (len >= match_len)
      {
         match_len = len;
         match_pos = test_pos;
         if (len == dict_str_size)
         {
            t.rename_node(test_pos, dict_pos);
            return;
         }
      }

      int &branch = t.nodes[test_pos].branch(chr_diff < 0);
      if (!branch)
      {
         branch = dict_pos;
         t.nodes[dict_pos] = node(test_pos, 0, 0);
         return;
      }
      test_pos = branch;
   }
}

#if 1

auto_ptr<vector<unsigned char> > lzss_compress(unsigned char *src, int src_len)
{
   auto_ptr<vector<unsigned char> > dest(new vector<unsigned char>);
   bitwriter b(*dest);
   
   int src_pos = 0;
   int dict_pos = 1;

   unsigned char dictionary[dict_size];
   memset(dictionary, 0, sizeof dictionary);

   int avail_input;
   for(avail_input = 0; avail_input < dict_str_size; ++avail_input)
   {
      if (src_pos >= src_len) break;
      dictionary[dict_pos + avail_input] = src[src_pos++];
   }
   
   tree t(dict_pos);
   int match_len = 0;
   int match_pos;

   while (avail_input)
   {
      int used_input;
      if (match_len < 3)
      {
         b.bit(1);
         b.bits(8, dictionary[dict_pos]);
         used_input = 1;
      }
      else
      {
         b.bit(0);
         b.bits(dict_bits, match_pos);
         b.bits(4, match_len - 3);
         used_input = match_len;
      }

      for(int i = 0; i < used_input; ++i)
      {
         t.delete_node((dict_pos + dict_str_size) % dict_size);
         if (src_pos < src_len)
            dictionary[(dict_pos + dict_str_size) % dict_size] = src[src_pos++];
         else
            --avail_input;

         ++dict_pos;
         dict_pos %= dict_size;
         
         if (avail_input)
         {
            find_best_match(t, dictionary, dict_pos, match_pos, match_len);
            if (match_len > avail_input) match_len = avail_input;
         }
      }
   }
   
   // Not needed, zeroes are implied after end of file
   //b.bit(0);
   //b.bits(dict_bits, 0);
   b.finish();
   return dest;
}

#else

auto_ptr<vector<unsigned char> > lzss_compress(unsigned char *src, int src_len)
{
   auto_ptr<vector<unsigned char> > dest(new vector<unsigned char>);
   bitwriter b(*dest);
   for(int i = 0; i < src_len; ++i)
   {
      unsigned char byte = src[i];
      b.bit(1);
      b.bits(8, byte);
   }
   b.finish();
   return dest;
}

#endif
